INCLUDES = \
	-I$(top_srcdir)/kttsd/libkttsd -I$(top_builddir)/kttsd/libkttsd \
	$(all_includes)

METASOURCES = AUTO

kde_module_LTLIBRARIES = libkttsd_fliteplugin.la

libkttsd_fliteplugin_la_SOURCES = \
        fliteconfwidget.ui \
        fliteconf.cpp \
        fliteproc.cpp \
        fliteplugin.cpp 
libkttsd_fliteplugin_la_LDFLAGS = $(KDE_PLUGIN) $(all_libraries)
libkttsd_fliteplugin_la_LIBADD = $(top_builddir)/kttsd/libkttsd/libkttsd.la

services_DATA = kttsd_fliteplugin.desktop
servicesdir = $(kde_servicesdir)

noinst_HEADERS =  fliteproc.h fliteconf.h fliteconfwidget.h
