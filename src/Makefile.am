bin_PROGRAMS = panaapp
panaapp_SOURCES = main.cpp
panaapp_CPPGLAGS =-I(top_srcdir)/src
panaapp_LDADD = \
	$(top_builddir)/src/panacore/libpanacore.la

lib_LTLIBRARIES = libpana.la
libpana_la_SOURCES = \
	Options1.ui \
	Options1.ui.h \
	Options2.ui \
	Options4.ui \
	Options5.ui \
	Options7.ui \
	Options8.ui \
	actionclasses.cpp

libpana_la_LIBADD =  \
	$(top_builddir)/src/panacore/libpanacore.la

libpana_la_LDFLAGS = \
	$(all_libraries)
	:$(KDE_RPATH)

INCLUDES = \
	-I$(top_builddir)/src/panacore \
	-I$(top_builddir)/src/magnatunebrowser \
	-I$(top_srcdir)/src/panacore \
	-I$(top_srcdir)/src/analyzers \
	-I$(top_srcdir)/src/plugin \
	-I$(top_srcdir)/src/statusbar \
	-I$(top_srcdir)/src/mediadevice \
	-I$(top_srcdir)/src/device \
	-I$(top_srcdir)/src \
	$(TAGLIB_CFLAGS) \
	$(sqlite_includes) \
	$(mysql_includes) \
	$(postgresql_includes) \
	$(all_includes)
