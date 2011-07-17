/*
* copyright (C) 2011 Free Software Foundation, Inc.
*
* This file is part of Pana.
*
* Pana is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* Pana is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Pana. It can be found in the top-level source folder.
* If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef PANA_H
#define PANA_H

#include <qnamespace.h>
#include <qstring.h>

#include <kurl.h> // recursiveUrlExpand
#include <kprocio.h> //Pana::ProcIO
#include <kio/netaccess.h>
#include <kdeversion.h>

#include "pana_export.h"

class KActionCollection;
class KConfig;
class QColor;
class QDateTime;
class QEvent;
class QMutex;
class QPixmap;
class QWidget;
class DynamicMode;
class QListView;
class QListViewItem;
namespace KIO { class Job; }

namespace Pana
{
    const int VOLUME_MAX = 100;
    const int SCOPE_SIZE = 9; //= 2**9 = 512
    const int blue       = 0x202050;
    const int VOLUME_SENSITIVITY = 30; //for mouse wheels
    const int GUI_THREAD_ID = 0;

    extern QMutex globalDirsMutex; // defined in app.cpp

    namespace ColorScheme
    {
        ///eg. base of the Pana Player-window
        extern QColor Base; //Pana::blue
        ///eg. text in the Pana Player-window
        extern QColor Text; //Qt::white
        ///eg. background colour for Pana::PrettySliders
        extern QColor Background; //brighter blue
        ///eg. outline of slider widgets in Player-window
        extern QColor Foreground; //lighter blue
        ///eg. KListView alternative row color
        extern QColor AltBase; //grey toned base
    }

	/** The version of the playlist XML format. Increase when you break
		* backwards-compatibly. Pana note - this should NEVER happen for 1.4.x */
    inline QString xmlVersion() { return "2.4"; }

	/**
	* Convenience function to return the KApplication instance KConfig object
	* pre-set to a specific group.
	* @param group Will pre-set the KConfig object to this group.
	*/

    /* FIXME: This function can lead to very bizarre and hard to figure bugs.
              While we don't fix it properly, use it like this:
				  pana::config( Group )->readNumEntry( ... ) */
    KConfig *config( const QString &group = "General" ); //defined in app.cpp

    /**
     * @return the KActionCollection used by Pana
     * The KActionCollection is owned by the PlaylistWindow, so you must ensure
     * you don't try to use this before then, but we've taken steps to prevent
     * this eventuality - you should be safe.
     */
    KActionCollection *actionCollection(); //defined in app.cpp

    /**
     * An event handler that handles events in a generic Pana fashion. Mainly
     * useful for drops, i.e. offers the Pana popup for adding tracks to the
     * playlist. You shouldn't pass every event here, i.e. closeEvents will not be
     * handled as expected! Check the source in app.cpp if you want to see what
     * it can do.
     * @param recipient The object that received the event.
     * @param e The event you want handled in a generic fashion.
     * @return true if the event was handled.
     */
    bool genericEventHandler( QWidget *recipient, QEvent *e ); //defined in app.cpp

    /**
     * Invoke the external web browser set in the config.
     * @param url The URL to be opened in the browser.
     * @return True if the browser was started.
     */
    bool invokeBrowser( const QString& url ); //defined in app.cpp

    /**
     * Obtain a Pana PNG image as a QPixmap
     */
    QPixmap getPNG( const QString& /*fileName*/ ); //defined in app.cpp

    /**
     * Obtain a Pana JPG image as a QPixmap
     */
    QPixmap getJPG( const QString& /*fileName*/ ); //defined in app.cpp

    /**
     * The mainWindow is the playlistWindow or the playerWindow depending on
     * the configuration of Pana
     */
    QWidget *mainWindow(); //defined in app.cpp

    /**
     * Allocate one on the stack, and it'll set the busy cursor for you until it
     * is destroyed
     */
    class OverrideCursor { //defined in app.cpp
    public:
        OverrideCursor( Qt::CursorShape cursor = Qt::WaitCursor );
       ~OverrideCursor();
    };

    /**
     * For saving files to ~/.kde/share/apps/pana/directory
     * @param directory will be created if it doesn't exit, you MUST end the string
     * with /
     */
    LIBPANA_EXPORT QString saveLocation( const QString &directory = QString::null ); //defined in collectionreader.cpp

    KIO::Job *trashFiles( const KURL::List &files ); //defined in app.cpp

    /**
     * For recursively expanding the contents of a directory into a KURL::List
     * (playlists are ignored)
     */
    LIBPANA_EXPORT KURL::List recursiveUrlExpand( const KURL &url, int maxURLs = -1 ); //defined in playlistloader.cpp
    LIBPANA_EXPORT KURL::List recursiveUrlExpand( const KURL::List &urls, int maxURLs = -1 ); //defined in playlistloader.cpp

    QString verboseTimeSince( const QDateTime &datetime ); //defined in contextbrowser.cpp

    QString verboseTimeSince( uint time_t ); //defined in contextbrowser.cpp

    /**
     * Function that must be used when separating contextBrowser escaped urls
     */
    // defined in contextbrowser.cpp
    void albumArtistTrackFromUrl( QString url, QString &artist, QString &album, QString &detail );

    /**
     * @return the LOWERCASE file extension without the preceding '.', or "" if there is none
     */
    inline QString extension( const QString &fileName )
    {
        return fileName.contains( '.' ) ? fileName.mid( fileName.findRev( '.' ) + 1 ).lower() : "";
    }

    /** Transform url into a file url if possible */
    inline KURL mostLocalURL( const KURL &url )
    {
#if KDE_VERSION < KDE_MAKE_VERSION(3,5,0)
        return url;
#else
        return KIO::NetAccess::mostLocalURL( url, mainWindow() );
#endif
    }

    /**
     * @return the last directory in @param fileName
     */
    inline QString directory( const QString &fileName )
    {
        return fileName.section( '/', 0, -2 );
    }
	/** Due to xine-lib, we have to make KProcess close all fds, otherwise we get
		* "device is busy" messages
		* Used by Pana::ProcIO and Pana::Process, exploiting commSetupDoneC(), a
		* virtual method that happens to be called in the forked process
		* See bug #103750 for more information.
		*/

	//TODO ugly hack, fix KProcess for KDE 4.0
    void closeOpenFiles(int out, int in, int err); //defined in scriptmanager.cpp

    /**
    * Returns internal code for database type, DbConnection::sqlite, DbConnection::mysql, or DbConnection::postgresql
    * @param type either "SQLite", "MySQL", or "Postgresql".
    */
    int databaseTypeCode( const QString type ); //defined in configdialog.cpp

    void setUseScores( bool use ); //defined in app.cpp
    void setUseRatings( bool use );
    void setMoodbarPrefs( bool show, bool moodier, int alter, bool withMusic );

    bool repeatNone(); //defined in actionclasses.cpp
    bool repeatTrack();
    bool repeatAlbum();
    bool repeatPlaylist();
    bool randomOff();
    bool randomTracks();
    bool randomAlbums();
    bool favorNone();
    bool favorScores();
    bool favorRatings();
    bool favorLastPlay();
    bool entireAlbums(); //repeatAlbum() || randomAlbums()

    const DynamicMode *dynamicMode(); //defined in playlist.cpp

    QListViewItem* findItemByPath( QListView *view, QString path ); //defined in playlistbrowser.cpp
    QStringList splitPath( QString path ); //defined in playlistbrowser.cpp

    /**
     * Creates a copy of of the KURL instance, that doesn't have any QStrings sharing memory.
    **/
    KURL detachedKURL( const KURL &url ); //defined in metabundle.cpp

    /**
     * Maps the icon name to a system icon or custom Pana icon, depending on the settings.
     */
    LIBPANA_EXPORT QString icon( const QString& name ); //defined in iconloader.cpp

    /**
     * Removes accents from the string
     * @param path The original path.
     * @return The cleaned up path.
     */
    LIBPANA_EXPORT QString cleanPath( const QString &path ); //defined in app.cpp

    /**
     * Replaces all non-ASCII characters with '_'.
     * @param path The original path.
     * @return The ASCIIfied path.
     */
    LIBPANA_EXPORT QString asciiPath( const QString &path ); //defined in app.cpp

    /**
     * Transform path into one valid on VFAT file systems
     * @param path The original path.
     * @return The cleaned up path.
     */
    LIBPANA_EXPORT QString vfatPath( const QString &path ); //defined in app.cpp

    /**
     * Compare both strings from left to right and remove the common part from input
     * @param input the string that get's cleaned.
     * @param ref a reference to compare input with.
     * @return The cleaned up string.
     */
    LIBPANA_EXPORT QString decapitateString( const QString &input, const QString &ref );

    /*
     * Transform to be usable within HTML/HTML attributes
     * defined in contextbrowser.cpp
     */
    LIBPANA_EXPORT QString escapeHTML( const QString &s );
    LIBPANA_EXPORT QString escapeHTMLAttr( const QString &s );
    LIBPANA_EXPORT QString unescapeHTMLAttr( const QString &s );

    /* defined in scriptmanager.cpp */
    /**
     * Returns the proxy that should be used for a given URL.
     * @param url the url.
     * @return The url of the proxy, or a empty string if no proxy should be used.
     */
    QString proxyForUrl(const QString& url);

    /**
     * Returns the proxy that should be used for a given protocol.
     * @param protocol the protocol.
     * @return The url of the proxy, or a empty string if no proxy should be used.
     */
    QString proxyForProtocol(const QString& protocol);

    ////////////////////////////////////////////////////////////////////////////////
    // class Pana::ProcIO
    ////////////////////////////////////////////////////////////////////////////////
    /**
    * Due to xine-lib, we have to make KProcess close all fds, otherwise we get "device is busy" messages
    * Used by Pana::ProcIO and PanaProcess, exploiting commSetupDoneC(), a virtual method that
    * happens to be called in the forked process
    * See bug #103750 for more information.
    */
    class LIBPANA_EXPORT ProcIO : public KProcIO {
        public:
        ProcIO(); // ctor sets the textcodec to UTF-8, in scriptmanager.cpp
        virtual int commSetupDoneC() {
            const int i = KProcIO::commSetupDoneC();
            Pana::closeOpenFiles( KProcIO::out[0],KProcIO::in[0],KProcIO::err[0] );
            return i;
        };
    };

    ////////////////////////////////////////////////////////////////////////////////
    // class Pana::Process
    ////////////////////////////////////////////////////////////////////////////////
    /** Due to xine-lib, we have to make KProcess close all fds, otherwise we get "device is busy" messages
     * Used by Pana::ProcIO and Pana::Process, exploiting commSetupDoneC(), a virtual method that
     * happens to be called in the forked process
     * See bug #103750 for more information.
     */
    class LIBPANA_EXPORT Process : public KProcess {
        public:
        Process( QObject *parent = 0 ) : KProcess( parent ) {}
        virtual int commSetupDoneC() {
            const int i = KProcess::commSetupDoneC();
            Pana::closeOpenFiles(KProcess::out[0],KProcess::in[0], KProcess::err[0]);
            return i;
        };
    };


}


/**
 * Use this to const-iterate over QStringLists, if you like.
 * Watch out for the definition of last in the scope of your for.
 *
 *     QStringList strings;
 *     foreach( strings )
 *         debug() << *it << endl;
 */
#define foreach( x ) \
    for( QStringList::ConstIterator it = x.begin(), end = x.end(); it != end; ++it )

/**
 * You can use this for lists that aren't QStringLists.
 * Watch out for the definition of last in the scope of your for.
 *
 *     BundleList bundles;
 *     foreachType( BundleList, bundles )
 *         debug() << *it.url() << endl;
 */
#define foreachType( Type, x ) \
    for( Type::ConstIterator it = x.begin(), end = x.end(); it != end; ++it )

/**
 * Creates iterators of type @p Type.
 * Watch out for the definitions of last and end in your scope.
 *
 *     BundleList bundles;
 *     for( for_iterators( BundleList, bundles ); it != end; ++it )
 *         debug() << *it.url() << endl;
 */
#define for_iterators( Type, x ) \
    Type::ConstIterator it = x.begin(), end = x.end(), last = x.fromLast()


// Update this when necessary
#define APP_VERSION "1.4.18"

#endif
