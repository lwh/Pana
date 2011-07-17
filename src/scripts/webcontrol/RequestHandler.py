#!/usr/bin/env python
# -*- coding: Latin-1 -*-

"""
 This scripts starts a small http-server that serves the current-playlist as
 HTML. Start it and point your browser to http://localhost:4773/

 Author: André Kelpe fs111 at web dot de
       : Jonas Drewsen kde at xspect dot dk

 License: GPL

"""
import SimpleHTTPServer
import BaseHTTPServer
from Playlist import Playlist
import Globals

import time

# necessary for <= python2.2 that cannot handle "infds" in var
import string

PLIST = None

# keep track of request ids in order to not repeat
# requests if user refreshes his browser
REQ_IDS = {}

#
# Holding current PanaStatus. A bunch of init_XXX functions in
# order to begin dcop requests as early as possible to avoid too
# much latency
#
class PanaStatus:

    EngineEmpty = 1
    EngineIdle = 2
    EnginePause = 3
    EnginePlay = 4

    allowControl = 0
    publish = 0
    playState = -1

    dcop_isplaying = None
    dcop_volume = None
    dcop_trackcurrentindex = None
    dcop_trackcurrenttime = None
    dcop_tracktotaltime = None
    
    def __init__(self):
        self.controls_enabled = 0
        self.time_left = None

        self.dcop_isplaying = Globals.PlayerDcop("isPlaying")
        self.dcop_volume = Globals.PlayerDcop("getVolume")
        self.dcop_trackcurrentindex = Globals.PlaylistDcop("getActiveIndex")
        self.dcop_trackcurrenttime = Globals.PlayerDcop("trackCurrentTime")
        self.dcop_tracktotaltime = Globals.PlayerDcop("trackTotalTime")
        
    def isPlaying(self):
        if self.playState != -1:
            res = self.playState == self.EnginePlay
        else:
            res = string.find(self.dcop_isplaying.result(), "true") >= 0
            if res:
                self.playState = self.EnginePlay
            else:
                self.playState = self.EnginePause
        
        return res

    def getActiveIndex(self):
        return int(self.dcop_trackcurrentindex.result())

    def getVolume(self):
        return int(self.dcop_volume.result())

    def timeLeft(self):
        cur =   int(self.dcop_trackcurrenttime.result())
        total = int(self.dcop_tracktotaltime.result())
        return total - cur

    def controlsEnabled(self):
        return self.allowControl

class RequestHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):
    """We need our own 'RequestHandler, to handle the requests, that arrive at
    our server."""
    
    def _panaPlay(self):
        PanaStatus.playState = PanaStatus.EnginePlay
        Globals._dcopCallPlayer("play")

    def _panaPause(self):
        PanaStatus.playState = PanaStatus.EnginePause
        Globals._dcopCallPlayer("pause")

    def _panaNext(self):
        Globals._dcopCallPlayer("next")

    def _panaGoto(self,index):
        PanaStatus.playState = PanaStatus.EnginePlay
        PanaStatus.currentTrackIndex = int(index)
        Globals._dcopCallPlaylistArg("playByIndex",index)

    def _panaPrev(self):
        Globals._dcopCallPlayer("prev")

    def _panaStop(self):
        Globals._dcopCallPlayer("stop")
        PanaStatus.playState = PanaStatus.EngineStop
        
    def _panaSetVolume(self, val):
        Globals._dcopCallPlayerArg("setVolume",val)

    def _parseQueryVars(self):
        querystr = self.path.split("?")

        qmap = {}

        if len(querystr) <= 1:
            return qmap

        queries = querystr[-1].split("&");

        for query in queries:
            var = query.split("=")
            if len(var) != 2:
                continue
            qmap[var[0]] = var[1]

        return qmap

    def _handleAction(self, qmap):
        global REQ_IDS

        # get the sessions last reqid
        try:
            req_id = REQ_IDS[qmap["sesid"]]
        except:
            return 0
        
        # abort a request that has already been completed
        # probably a refresh from the users browser
        if qmap.has_key("reqid") and req_id == int(qmap["reqid"]):
            return 0

        if qmap.has_key("action"):
            a = qmap["action"]
            if a == "stop":
                self._panaStop()
            elif a == "play":
                self._panaPlay()
            elif a == "pause":
                self._panaPause()
            elif a == "prev":
                self._panaPrev()
            elif a == "next":
                self._panaNext()
            elif a == "goto":
                self._panaGoto(qmap["value"])
            elif a == "setvolume":
                self._panaSetVolume(qmap["value"])
        return 1
    
    def _panaStatus(self):
        status = PanaStatus()
        return status
        
    def _sendFile(self, path):
        # only allow doc root dir access
        elem = self.path.split("/")
        if len(elem):
            path = elem[-1]
            f = open(Globals.EXEC_PATH + "/" + path, 'r')
            self.copyfile(f, self.wfile)

    def do_HEAD(self):
        """Serve a HEAD request."""
        RequestHandler.extensions_map.update({
            '': 'application/octet-stream', # Default
            '.png': 'image/png',
            '.js': 'text/plain',
            '.css': 'text/plain'
            })
        f = self.send_head()
        if f:
            f.close()

    def _getSessionInfo(self, qmap):
        # get the sessions last reqid
        last_req_id = 0
        session_id = None
        if qmap.has_key("sesid"):
            session_id = qmap["sesid"]
            if REQ_IDS.has_key(session_id):
                last_req_id = REQ_IDS[session_id]
            else:
                REQ_IDS[session_id] = last_req_id
        else:
            # Create a session 
            session_id = str(time.time())
            REQ_IDS[session_id] = last_req_id
        return session_id, last_req_id
   
    def do_GET(self):
        """Overwrite the do_GET-method of the super class."""
        RequestHandler.extensions_map.update({
            '': 'application/octet-stream', # Default
            '.png': 'image/png',
            '.js': 'text/plain',
            '.css': 'text/plain'
            })

        global REQ_IDS

        qmap = self._parseQueryVars()
        session_id, last_req_id = self._getSessionInfo(qmap)
            
        if PanaStatus.allowControl and self._handleAction(qmap):
            last_req_id = last_req_id + 1
            REQ_IDS[session_id] = last_req_id
        
        newreqid = last_req_id + 1
        
        #
        # Surely there must be a better way that this:)
        #
        self.send_response(200)
        if string.find(self.path, ".png") >= 0:
            self.send_header("content-type","image/png")
            self.end_headers()
            self._sendFile(self.path)
        elif string.find(self.path, ".js") >= 0:
            self.send_header("content-type","text/plain")
            self.end_headers()
            self._sendFile(self.path)
        elif string.find(self.path, ".css") >= 0:
            self.send_header("content-type","text/css")
            self.end_headers()
            self._sendFile(self.path)
        else:
            status = self._panaStatus()
            status.dcop_volume.init()
            status.dcop_trackcurrenttime.init()
            status.dcop_tracktotaltime.init()
            self.send_header("content-type","text/html")
            self.send_header("Cache-Control","no-cache")
            self.end_headers()
            status.reqid = newreqid
            status.sesid = session_id
            self.wfile.write(PLIST.toHtml(status))

def main():
    """main is the starting-point for our script."""
    global PLIST
    PLIST = Playlist()
    srv = BaseHTTPServer.HTTPServer(('',Globals.PORT),RequestHandler)
    srv.serve_forever()


if __name__ == "__main__":
    main()

