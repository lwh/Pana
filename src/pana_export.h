// Copyright:  See COPYING file that comes with this distribution

#ifndef _PANA_EXPORT_H_
#define _PANA_EXPORT_H_

#include <config.h>

#ifdef __KDE_HAVE_GCC_VISIBILITY
#define LIBPANA_NO_EXPORT __attribute__ ((visibility("hidden")))
#define LIBPANA_EXPORT __attribute__ ((visibility("default")))
#else
#define LIBPANA_NO_EXPORT
#define LIBPANA_EXPORT
#endif
 
#endif

