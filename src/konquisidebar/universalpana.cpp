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

#include "universalpana.h"
#include "panadcopiface_stub.h"


#include <qlabel.h>
#include <kinstance.h>
#include <klocale.h>
#include <qstring.h>
#include <qwidget.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <qlayout.h>
#include <qtimer.h>
#include <dcopclient.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kiconloader.h>
#include <qdatetime.h>
#include <qfileinfo.h>
#include <ktoolbar.h>
#include <kapplication.h>
#include <qslider.h>
#include <kdebug.h>
#include <kurldrag.h>
#include <khtmlview.h>

#define HTML_FILE KGlobal::dirs()->saveLocation( "data", "pana/", true ) + "contextbrowser.html"

panaWidget::panaWidget( QWidget * parent, const char * name, WFlags f )
                 : QVBox(parent, name, f)
{
    setAcceptDrops(true);
}
void panaWidget::dragEnterEvent(QDragEnterEvent* event)
{
    event->accept( KURLDrag::canDecode(event) );
}

void panaWidget::dropEvent(QDropEvent* event)
{
    KURL::List urlList;
    if( KURLDrag::decode(event, urlList) )
    {
        KURL::List::iterator it;
        KURL::List::iterator end( urlList.end() );
        for (it = urlList.begin(); it != end; ++it )
            emit emitURL(*it);
    }
}

bool panaWidget::eventFilter( QObject *, QEvent *e )
{
    if(e->type() < QEvent::DragEnter || e->type() > QEvent::Drop )
        return false;
    QApplication::sendEvent(this, e);
    return true;
}

UniversalPana::UniversalPana(KInstance *inst,QObject *parent,QWidget *widgetParent, QString &desktopName, const char* name):
                   KonqSidebarPlugin(inst,parent,widgetParent,desktopName,name)
{
    KGlobal::iconLoader()->addAppDir( "pana" );
    widget = new panaWidget( widgetParent );
//    widgetParent->resize(580,300);
    KToolBar *topBar = new KToolBar( widget, "Topbar" );
    topBar->setIconSize(16);
    topBar->insertButton( "today",    0, SIGNAL( clicked() ), this, SLOT( currentTrack() ) );
    topBar->insertButton( "document", 0, SIGNAL( clicked() ), this, SLOT( lyrics() ) );
    topBar->insertButton( "personal", 0, SIGNAL( clicked() ), this, SLOT( wiki() ) );

    browser = new KHTMLPart(widget, "widget-browser");
//browser=new KHTMLPart(widget);
    kdDebug() << "parentPart() << " << browser->parentPart() << endl;
    browser->setDNDEnabled( true );
    browser->setEncoding( "utf8", true );
    updateBrowser( HTML_FILE );
    browser->view()->installEventFilter( widget );
    panaDCOP = new DCOPClient();
    panaDCOP->attach();

    playerStub   = new PanaPlayerInterface_stub( panaDCOP, "pana", "player");
    playlistStub = new PanaPlaylistInterface_stub( panaDCOP, "pana", "playlist");
    contextStub = new PanaContextBrowserInterface_stub (panaDCOP, "pana", "contextbrowser");

    KToolBar* toolBar=new KToolBar(widget, "PlayerControls");

    toolBar->setIconSize(16);
    toolBar->insertButton( "player_start",0, SIGNAL( clicked() ), this, SLOT( sendPrev() ) );
    toolBar->insertButton( "player_play", 0, SIGNAL( clicked() ), this, SLOT( sendPlay() ) );
    toolBar->insertButton( "player_pause",0, SIGNAL( clicked() ), this, SLOT( sendPause() ) );
    toolBar->insertButton( "player_stop", 0, SIGNAL( clicked() ), this, SLOT( sendStop() ) );
    toolBar->insertButton( "player_end",  0, SIGNAL( clicked() ), this, SLOT( sendNext() ) );

    toolBar->insertSeparator();
    toolBar->insertButton( "arts",        0, SIGNAL( clicked() ), this, SLOT( sendMute() ) );

    vol_slider = new QSlider(0,100,1,0,Qt::Horizontal, toolBar,"volume");
    vol_slider->setLineStep(2);

    connect(vol_slider, SIGNAL( valueChanged(int) ), this, SLOT(volChanged(int ) ) );
    toolBar->insertWidget(1,2, vol_slider);

    fileInfo  = new QFileInfo(HTML_FILE);
    QTimer *t = new QTimer( this );

    connect( t, SIGNAL(timeout()), SLOT(updateStatus() ) );
    t->start( 2000, false );
    kdDebug() << "Connecting widget signal" << endl;

    connect( widget,                      SIGNAL( emitURL( const KURL &)),
             this,                        SLOT( openURLRequest( const KURL &) ) );
    connect( browser->browserExtension(), SIGNAL( openURLRequest( const KURL &, const KParts::URLArgs & ) ),
             this,                        SLOT( openURLRequest( const KURL & ) ) );
    widget->show();
}


UniversalPana::~UniversalPana()
{
    delete fileInfo;
}


#include "universalpana.moc"

#if ! KDE_IS_VERSION(3,4,0)
#define KDE_EXPORT __attribute__ ((visibility("default")))
#endif

// FIXME: is this referenced from anywhere ??!

extern "C"
{
    KDE_EXPORT void* create_konqsidebar_universalpana(KInstance *instance,QObject *par,QWidget *widp,QString &desktopname,const char *name)
    {
        KGlobal::locale()->insertCatalogue( "pana" );
        return new UniversalPana(instance,par,widp,desktopname,name);
    }
}

// FIXME: Is this referenced from anywhere ??!
extern "C"
{
    KDE_EXPORT bool add_konqsidebar_universalpana(QString* fn, QString* param, QMap<QString,QString> *map)
        {
        Q_UNUSED(param);

        map->insert ("Type", "Link");
        map->insert ("URL", "");
        map->insert ("Icon", "pana");
        map->insert ("Name", i18n ("Pana"));
        map->insert ("Open", "true");
        map->insert ("X-KDE-KonqSidebarModule","konqsidebar_universalpana");
        fn->setLatin1 ("pana.desktop");
        return true;
    }
}


/*!
    \fn UniversalPana::updateBrowser()
 */
void UniversalPana::updateBrowser(const QString& file)
{
    if (! (QFile::exists(file) ) )
    {
        showIntroduction();
        return;
    }
    QString text;
    QFile f_file(file);
    if( f_file.open(IO_ReadOnly) )
    {
        QTextStream stream( &f_file );
        stream.setEncoding( QTextStream::UnicodeUTF8 );
        QString line;
        while ( !stream.atEnd() ) {
            line = stream.readLine(); // line of text excluding '\n'
             text += QString("\n") + line;
        } f_file.close();
            text=text.replace("<img id='current_box-largecover-image' ", "<img id='current_box-largecover-image' width=70 height=70 ");
        browser->begin();
        browser->write(text);
        browser->end();
    } else
    browser->openURL(file);
}


/*!
    \fn UniversalPana::updateStatus()
 */
void UniversalPana::updateStatus()
{
    checkForPana();
    vol_slider->setValue( playerStub->getVolume() );
    fileInfo->refresh();
    if( fileInfo->lastModified() != fileDT )
    {
        updateBrowser( HTML_FILE );
        fileDT=fileInfo->lastModified();
    }
}


/*!
    \fn UniversalPana::getCurrentPlaying()
 */
QString UniversalPana::getCurrentPlaying()
{
    return playerStub->nowPlaying();
}


/*!
    \fn UniversalPana::openURLRequest( const KURL &url )
 */
void UniversalPana::openURLRequest( const KURL &url )
{
   if( ! url.isValid() ) return;
   if (url.url() == "run:pana") {
        runPana();
        return;
   }
   checkForPana();
   playlistStub->playMedia(url);
}


/*!
    \fn UniversalPana::checkForPana()
 */
void UniversalPana::checkForPana()
{
    if(!panaDCOP->isApplicationRegistered("pana"))
        noPanaRunning();
}


void UniversalPana::noPanaRunning() {
    QString  m_HTMLSource="";
    m_HTMLSource.append(
            "<html>"
            "<div id='introduction_box' class='box'>"
                "<div id='introduction_box-header' class='box-header'>"
                    "<span id='introduction_box-header-title' class='box-header-title'>"
                    + i18n( "Pana is not running!" ) +
                    "</span>"
                "</div>"
                "<div id='introduction_box-body' class='box-body'>"
                    "<p>" +
                    i18n( "To run Pana, just click on the link below: "
                        ) +
                    "</p>"
                    "<a href='run:pana' class='button'>" + i18n( "Run Pana..." ) + "</a>"
                "</div>"
            "</div>"
            "</html>"
                       );
    browser->begin();
    browser->write( m_HTMLSource );
    browser->end();
}

void UniversalPana::runPana() {
    KApplication::kdeinitExecWait("pana");
}



void UniversalPana::volChanged(int vol)
{
    checkForPana();
    playerStub->setVolume(vol);
}

void UniversalPana::showIntroduction()
{
    QString  m_HTMLSource="";
    m_HTMLSource.append(
            "<html>"
            "<div id='introduction_box' class='box'>"
                "<div id='introduction_box-header' class='box-header'>"
                    "<span id='introduction_box-header-title' class='box-header-title'>"
                    + i18n( "Hello Pana user!" ) +
                    "</span>"
                "</div>"
                "<div id='introduction_box-body' class='box-body'>"
                    "<p>" +
                    i18n( "This is the Context Browser: "
                          "it shows you contextual information about the currently playing track. "
                          "In order to use this feature of Pana, you need to build a Collection."
                        ) +
                    "</p>"
                    "<a href='show:collectionSetup' class='button'>" + i18n( "Build Collection..." ) + "</a>"
                "</div>"
            "</div>"
            "</html>"
                       );
kdDebug() << m_HTMLSource << endl;
    browser->begin();
    browser->write( m_HTMLSource );
    browser->end();
}
