// Author: Mark Kretschmann (C) Copyright 2004
// Copyright: See COPYING file that comes with this distribution

#ifndef PANA_PLUGIN_H
#define PANA_PLUGIN_H

#include <config.h>
#include "pana_export.h"

#define PANA_EXPORT_PLUGIN( classname ) \
    extern "C" { \
          KDE_EXPORT Pana::Plugin* create_plugin() { return new classname; } \
    }

#include <qmap.h>
#include <qstring.h>

class QWidget;

namespace Pana
{
    class PluginConfig;

    class LIBPANA_EXPORT Plugin
    {
        public:
            virtual ~Plugin();

            /**
             * TODO @param parent you must parent the widget to parent
             * @return the configure widget for your plugin, create it on the heap!
             */
             //TODO rename configureWidget( QWidget *parent )
            virtual PluginConfig* configure() const { return 0; }

            void addPluginProperty( const QString& key, const QString& value );
            QString pluginProperty( const QString& key );
            bool hasPluginProperty( const QString& key );

        protected:
            Plugin();

        private:
            QMap<QString, QString> m_properties;
    };

} //namespace Pana


#endif /* PANA_PLUGIN_H */


