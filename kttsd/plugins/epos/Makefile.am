INCLUDES = \
        -I$(top_srcdir)/kttsd/libkttsd -I$(top_builddir)/kttsd/libkttsd \
        $(all_includes)

METASOURCES = AUTO

kde_module_LTLIBRARIES = libkttsd_eposplugin.la

libkttsd_eposplugin_la_SOURCES = \
        eposconfwidget.ui \
        eposconf.cpp \
        eposproc.cpp \
        eposplugin.cpp 
libkttsd_eposplugin_la_LDFLAGS = $(KDE_PLUGIN) $(all_libraries)
libkttsd_eposplugin_la_LIBADD = $(top_builddir)/kttsd/libkttsd/libkttsd.la

services_DATA = kttsd_eposplugin.desktop
servicesdir = $(kde_servicesdir)

noinst_HEADERS =  eposproc.h eposconf.h eposconfwidget.h
