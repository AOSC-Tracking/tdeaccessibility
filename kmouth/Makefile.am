bin_PROGRAMS = kmouth

kmouth_SOURCES = preferencesui.ui texttospeechconfigurationui.ui texttospeechconfigurationwidget.cpp optionsdialog.cpp configwizard.cpp phraseedit.cpp phraselist.cpp speech.cpp texttospeechsystem.cpp phraselistitem.cpp kmouth.cpp main.cpp 
kmouth_LDADD   = ./phrasebook/libphrasebook.a ./wordcompletion/libwordcompletion.a -ltdeprint $(LIB_TDEFILE) $(LIB_TDEUI) $(LIB_TDECORE) $(LIB_TQT) $(LIBSOCKET)

SUBDIRS = icons phrasebook wordcompletion books . 

EXTRA_DIST = main.cpp version.h kmouth.cpp kmouth.h kmouthui.rc kmouth.desktop phraselistitem.cpp phraselistitem.h texttospeechsystem.cpp texttospeechsystem.h speech.cpp speech.h phraselist.cpp phraselist.h phraseedit.cpp phraseedit.h kmouthrc configwizard.cpp configwizard.h optionsdialog.cpp optionsdialog.h texttospeechconfigurationwidget.h texttospeechconfigurationwidget.cpp texttospeechconfigurationui.ui preferencesui.ui 

kde_conf_DATA = kmouthrc

xdg_apps_DATA = kmouth.desktop

# set the include path for X, qt and KDE
INCLUDES= -Iphrasebook -Iwordcompletion $(all_includes)

METASOURCES = AUTO

# the library search path. 
kmouth_LDFLAGS = $(all_libraries) -ltdespell $(KDE_RPATH) $(LIB_TQT) -lDCOP $(LIB_TDECORE) $(LIB_TDEUI) -ltdefx $(LIB_TDEIO) -ltdetexteditor

rcdir = $(kde_datadir)/kmouth
rc_DATA = kmouthui.rc

messages: rc.cpp
	$(EXTRACTRC) *.rc *.ui */*.ui >> ./rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kmouth.pot; \
	fi

