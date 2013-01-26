# Allow build from within tdebase or tdeaccessibility.
INCLUDES    = \
	-I$(top_srcdir)/kate/interfaces \
	-I$(kde_includes)/kate \
	$(all_includes)

METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = tdetexteditor_kttsd.la

tdetexteditor_kttsd_la_SOURCES = katekttsd.cpp
tdetexteditor_kttsd_la_LIBADD  = -ltdetexteditor
tdetexteditor_kttsd_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kttsddatadir = $(kde_datadir)/tdetexteditor_kttsd
kttsddata_DATA = tdetexteditor_kttsdui.rc

kde_services_DATA = tdetexteditor_kttsd.desktop
