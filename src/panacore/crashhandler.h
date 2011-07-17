/* Copyright (c) 2010 Free Software Foundation, Inc.
 * Copyright (C) 2005 Max Howell <max.howell@methylblue.com>
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CRASH_H
#define CRASH_H

#include <kcrash.h> //for main.cpp

namespace Pana
{
    /**
     * @author Max Howell
     * @short The Pana crash-handler
     *
     * I'm not entirely sure why this had to be inside a class, but it
     * wouldn't work otherwise *shrug*
     */
    class Crash
    {
    public:
        static void crashHandler( int signal );
    };
}

#endif
