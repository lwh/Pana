/***************************************************************************
 * copyright            : (C) 2006 Ian Monroe <ian@monroe.nu>              *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef PANA_DAAPSERVER_H
#define PANA_DAAPSERVER_H

#include <daapclient.h>

namespace DNSSD {
    class PublicService;
}

class DaapServer : public QObject
{
    Q_OBJECT

    public:
        DaapServer(QObject* parent, char* name);
        ~DaapServer();
    public slots:
        void readSql();
    private:
        KProcIO* m_server;
        #ifdef DNSSD_SUPPORT
            DNSSD::PublicService* m_service;
        #endif
};

#endif /* PANA_DAAPSERVER_H */

