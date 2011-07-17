#!/usr/bin/env ruby
# Proxy server for Last.fm and DAAP. Relays the stream from the server to localhost, and
# converts the protocol to http on the fly.
#
# (c) 2006 Paul Cifarelli <paul@cifarelli.net>
# (c) 2006 Mark Kretschmann <markey@web.de>
# (c) 2006 Michael Fellinger <manveru@weez-int.com>
# (c) 2006 Ian Monroe <ian@monroe.nu>
# (c) 2006 Martin Ellis <martin.ellis@kdemail.net>
# (c) 2006 Alexandre Oliveira <aleprj@gmail.net>
# (c) 2006 Tom Kaitchuck <tkaitchuck@comcast.net>
#
# License: GNU General Public License V2
# Pana  listens to stderr and recognizes these magic strings, do not remove them:
# "PANA_PROXY: startup", "PANA_PROXY: SYNC"


require 'socket'
require "uri"
$stdout.sync = true

class Proxy
  ENDL = "\r\n"

  def initialize( port, remote_url, engine, proxy )
    @engine = engine

    myputs( "running with port: #{port} and url: #{remote_url} and engine: #{engine}" )

    # Open the pana-facing socket
    # pana: the server port on the localhost to which the engine will connect.
    pana = TCPServer.new( port )
    myputs( "startup" )

    # panas: server socket for above.
    panas = pana.accept

    # uri: from pana, identifies the source of the music
    uri = URI.parse( remote_url )
    myputs("host " << uri.host << " ")
    myputs( port )

    # Now we have the source of the music, determine the HTTP request that
    # needs to be made to the remote server (or remote proxy).  It will
    # be of the form "GET ... HTTP/1.x".  It will include the
    # http://hostname/ part if, and only if, we're using a remote proxy.
    get = get_request( uri, !proxy.nil? )

    #Check for proxy
    begin
      proxy_uri = URI.parse( proxy )
      serv = TCPSocket.new( proxy_uri.host, proxy_uri.port )
    rescue
      serv = TCPSocket.new( uri.host, uri.port )
    end

    serv.sync = true
    myputs( "running with port: #{uri.port} and host: #{uri.host}" )

    # Read the GET request from the engine
    panas_get = panas.readline
    myputs( panas_get.inspect )

    myputs( get.inspect )
    myputs( "#{panas_get} but sending #{get}" )
    serv.puts( get )

    # Copy the HTTP REQUEST headers from the pana engine to the
    # remote server, and signal end of headers.
    myputs( "COPY from pana -> serv" )
    cp_to_empty_outward( panas, serv )
    safe_write( serv, "\r\n\r\n" )

    # Copy the HTTP RESPONSE headers from the server back to the
    # pana engine.
    myputs( "COPY from serv -> pana" )
    cp_to_empty_inward( serv, panas )

    if @engine == 'gst10-engine'
      3.times do
        myputs( "gst10-engine waiting for reconnect" )
        sleep 1
        break if panas.eof
      end
      panas = pana.accept
      safe_write( panas, "HTTP/1.0 200 OK\r\n\r\n" )
      panas.each_line do |data|
        myputs( data )
        data.chomp!
        break if data.empty?
      end
    end

    # Now stream the music!
    myputs( "Before cp_all()" )
    cp_all_inward( serv, panas )

    if @engine == 'helix-engine' && panas.eof
      myputs( "EOF Detected, reconnecting" )
      panas = pana.accept
      cp_all_inward( serv, panas )
    end
  end

  def safe_write( output, data )
    begin
        output.write data
    rescue
      myputs( "error from output.write, #{$!}" )
      myputs( $!.backtrace.inspect )
      break
    end
  end

  def cp_to_empty_outward( income, output )
    myputs "cp_to_empty_outward( income => #{income.inspect}, output => #{output.inspect}"
    income.each_line do |data|
      if data =~ /User-Agent: xine\/([0-9.]+)/
        version = $1.split(".").collect { |v| v.to_i }
        myputs("Found xine user agent version #{version.join(".")}")
        @xineworkaround = ( version[0] <= 1 && version[1] <= 1 && version[2] <= 2 )
      end
      myputs( data )
      data.chomp!
      safe_write( output, data )
      myputs( "data sent.")
      return if data.empty?
    end
  end

  def desync (data)
      if data.gsub!( "SYNC", "" )
        myputs( "SYNC" )
      end
  end

  def cp_to_empty_inward( income, output )
    myputs( "cp_to_empty_inward( income => #{income.inspect}, output => #{output.inspect}" )
    income.each_line do |data|
      myputs( data )
      safe_write( output, data )
      return if data.chomp == ""
    end
  end

  def cp_all_inward( income, output )
    myputs( "cp_all( income => #{income.inspect}, output => #{output.inspect}" )
    if self.is_a?( LastFM ) and @xineworkaround
      myputs( "Using buffer fill workaround." )
      filler = Array.new( 4096, 0 )
      safe_write( output, filler ) # HACK: Fill xine's buffer so that xine_open() won't block
    end
    if @engine == 'helix-engine'
      data = income.read( 1024 )
    else
      data = income.read( 4 )
    end
    desync( data )
    holdover = ""
    loop do
      begin
        safe_write( output, data )
      rescue
        myputs( "error from o.write, #{$!}" )
        break
      end
      newdata = income.read( 1024 )

      data = holdover + newdata[0..-5]
      holdover = newdata[-4..-1]
      desync( data )

      break if newdata == nil
    end
  end
end

class LastFM < Proxy
# Last.fm protocol:
# Stream consists of pure MP3 files concatenated, with the string "SYNC" in between, which
# marks a track change. The proxy notifies Amarok on track change.

  def get_request( remote_uri, via_proxy )
    # remote_uri - the URI of the stream we want
    # via_proxy - true iff we're going through another proxy
    if via_proxy then
      url = remote_uri.to_s
    else
      url = "#{remote_uri.path || '/'}?#{remote_uri.query}"
    end
    get = "GET #{url} HTTP/1.0" + ENDL
    get += "Host: #{remote_uri.host}:#{remote_uri.port}" + ENDL + ENDL
  end

end

class DaapProxy < Proxy
  def initialize( port, remote_url, engine, hash, request_id, proxy )
    @hash = hash
    @requestId = request_id
    super( port, remote_url, engine, proxy )
  end

  def get_request( remote_uri, via_proxy )
    # via_proxy ignored for now
    get = "GET #{remote_uri.path || '/'}?#{remote_uri.query} HTTP/1.0" + ENDL
    get += "Accept: */*" + ENDL
    get += "User-Agent: iTunes/4.6 (Windows; N)" + ENDL
    get += "Client-DAAP-Version: 3.0" + ENDL
    get += "Client-DAAP-Validation: #{@hash}" + ENDL
    get += "Client-DAAP-Access-Index: 2" + ENDL
    get += "Client-DAAP-Request-ID: #{@requestId}" + ENDL
    get += "Host: #{remote_uri.host}:#{remote_uri.port}" + ENDL + ENDL
    get
  end
end

def myputs( string )
   $stdout.puts( "PANA_PROXY: #{string}" )
end

begin
  myputs( ARGV )
  if( ARGV[0] == "--lastfm" ) then
    option, port, remote_url, engine, proxy = ARGV
    LastFM.new( port, remote_url, engine, proxy )
  else
    option, port, remote_url, engine, hash, request_id, proxy = ARGV
    DaapProxy.new( port, remote_url, engine, hash, request_id, proxy )
  end
rescue
  myputs( $!.to_s )
  myputs( $!.backtrace.inspect )
end

puts( "exiting" )
