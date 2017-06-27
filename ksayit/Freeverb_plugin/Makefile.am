INCLUDES = -I$(top_srcdir)/ksayit/src $(all_includes) -I$(kde_includes)/tde -I$(kde_includes)/arts
METASOURCES = AUTO
kde_servicetypes_DATA = ksayit_libFreeverb_service.desktop
kde_services_DATA = ksayit_libFreeverb.desktop
fxpluginsdir = $(kde_libraries)/trinity
fxplugins_LTLIBRARIES = libFreeverb_plugin.la
libFreeverb_plugin_la_LDFLAGS = -avoid-version -module -no-undefined \
	$(all_libraries)
libFreeverb_plugin_la_LIBADD = $(LIB_TDECORE) $(LIB_TDEUI) $(LIB_QT) -lartskde -lartsflow -lartsflow_idl -lartsmoduleseffects -lsoundserver_idl \
	-lartsmodules
noinst_HEADERS = freeverbsetupimpl.h ksayitfreeverblib.h kdeexportfix.h
libFreeverb_plugin_la_SOURCES = freeverbsetupimpl.cpp freeverb_setup.ui \
	ksayitfreeverblib.cpp
