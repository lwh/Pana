/*
 * copyright (C) 2006 by Jeff Mitchell

 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef COLLECTIONSCANNER_DCOP_HANDLER_H
#define COLLECTIONSCANNER_DCOP_HANDLER_H

#include "collectionscannerdcopiface.h"

#include <qobject.h>

#include <kapplication.h>

class DcopCollectionScannerHandler : public QObject, virtual public CollectionScannerInterface
{
      Q_OBJECT

   public:
      DcopCollectionScannerHandler();

   signals:
      void pauseRequest();
      void unpauseRequest();

   public:
      virtual void pause();
      virtual void unpause();
};

#endif
