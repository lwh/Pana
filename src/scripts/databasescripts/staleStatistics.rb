#!/usr/bin/env ruby
#
# Ruby script to remove stale statistics in the database
# (c) 2005 Seb Ruiz <me@sebruiz.net>
# License: GNU General Public License V2

system("dcop", "pana", "playlist", "shortStatusMessage", "Removing stale entries from the database")

qresult = `dcop pana collection query "SELECT url FROM statistics;"`
result = qresult.split( "\n" )

i = 0

result.each do |url|
    unless FileTest.exist?( url )
        i = i + 1
        url.gsub!(/[']/, '\\\\\'')
        puts "Deleting: #{url}"
        system("dcop", "pana", "collection", "query", "DELETE FROM statistics WHERE url = '#{url}'")
    end
end

if i > 0
    system("dcop", "pana", "playlist", "popupMessage", "Removed #{i} stale entries from the database")
end
