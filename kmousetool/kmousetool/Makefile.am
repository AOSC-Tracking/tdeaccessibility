bin_PROGRAMS = kmousetool

kmousetool_SOURCES = kmousetoolui.ui mtstroke.cpp kmousetool.cpp main.cpp 
kmousetool_LDADD   = -lXtst $(LIB_TDEUI) $(LIB_TDECORE) $(LIB_TQT) -lXext -lX11 $(LIBSOCKET)

SUBDIRS = pics 

EXTRA_DIST = main.cpp version.h kmousetool.cpp kmousetool.h kmousetool.desktop mtstroke.cpp mtstroke.h mousetool_tap.wav 

xdg_apps_DATA = kmousetool.desktop

sounddatadir = $(kde_datadir)/kmousetool/sounds
sounddata_DATA = mousetool_tap.wav

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
kmousetool_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(LIB_TQT) -lDCOP $(LIB_TDECORE) $(LIB_TDEUI) -ltdefx $(LIB_TDEIO) -ltdetexteditor

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kmousetool.pot; \
	fi

