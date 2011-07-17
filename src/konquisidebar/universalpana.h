/***************************************************************************
 *   Copyright (C) 2004 by Marco Gulino                                    *
 *   marco@Paganini                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef KONQUERORSIDEBAR_H
#define KONQUERORSIDEBAR_H

#include <khtml_part.h>
#include <konqsidebarplugin.h>
#include <dcopclient.h>
#include <qslider.h>
#include <qvbox.h>
#include <khtmlview.h>
#include <kurl.h>
#include "panadcopiface_stub.h"

/**
@author Marco Gulino
*/

class universalpanawidget;
class DCOPClient;
class QFileInfo;
class QDateTime;

class panaWidget : public QVBox
{
Q_OBJECT
public:
    panaWidget( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );

protected:
    virtual void dragEnterEvent ( QDragEnterEvent * );
    virtual void dropEvent(QDropEvent*);
    bool eventFilter( QObject *o, QEvent *e );

signals:
    void emitURL( const KURL &);
};


class UniversalPana : public KonqSidebarPlugin
{
Q_OBJECT
public:
    UniversalPana(KInstance *inst,QObject *parent,QWidget *widgetParent, QString &desktopName, const char* name=0);

    ~UniversalPana();

   virtual QWidget *getWidget(){return (QWidget*)widget;}
   virtual void *provides(const QString &) {return 0;}
   virtual void handleURL(const KURL& /*url*/) {}
   QString getCurrentPlaying();
   void showIntroduction();

private:
   panaWidget* widget;
   KHTMLPart* browser;
   QString panaPlaying;
   DCOPClient* panaDCOP;
   QFileInfo* fileInfo;
   QDateTime fileDT;
   QSlider* vol_slider;
   PanaPlayerInterface_stub *playerStub;
   PanaPlaylistInterface_stub *playlistStub;
   PanaContextBrowserInterface_stub *contextStub;

public slots:
    void updateBrowser(const QString&);
    void updateStatus();
    void sendPrev() { checkForPana(); playerStub->prev(); }
    void sendPlay() { checkForPana(); playerStub->play(); }
    void sendPause() { checkForPana(); playerStub->pause(); }
    void sendStop() { checkForPana(); playerStub->stop(); }
    void sendNext() { checkForPana(); playerStub->next(); }
    void sendMute() { checkForPana(); playerStub->mute(); }
    void volChanged(int vol);
    void openURLRequest( const KURL & );
    void checkForPana();
    void noPanaRunning();
    void runPana();
    void lyrics() { contextStub->showLyrics(); }
    void currentTrack() { contextStub->showCurrentTrack(); }
    void wiki() { contextStub->showWiki(); }
};

#endif
