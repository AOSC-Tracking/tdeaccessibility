# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/ksayit/KTTSD_Lib $(all_includes)

# these are the headers for your project
noinst_HEADERS = doctreeviewimpl.h effectstack.h fxpluginhandler.h \
		fxsetupimpl.h ksayit_fxplugin.h ksayitsystemtray.h ksayit_ttsplugin.h \
		ksayitviewimpl.h Types.h version.h voicesetupdlg.h ksayit.h parasaxparser.h saxhandler.h \
	ksayitbookmarkhandler.h contextmenuhandler.h docbookclasses.h docbookgenerator.h \
	docbookparser.h	

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/ksayit.pot

KDE_ICON = AUTO


#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = ksayit

# the application source, library search path, and link libraries
ksayit_SOURCES = doctreeviewimpl.cpp effectstack.cpp fxpluginhandler.cpp \
		fxsetupimpl.cpp ksayit.skel ksayitsystemtray.cpp ksayitviewimpl.cpp \
		main.cpp voicesetupdlg.cpp ksayit.cpp DocTreeView.ui fx_setup.ui KSayItView.ui \
		parasaxparser.cpp saxhandler.cpp ksayitbookmarkhandler.cpp contextmenuhandler.cpp docbookclasses.cpp \
		docbookgenerator.cpp docbookparser.cpp

ksayit_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(LIB_TQT) -lDCOP $(LIB_TDECORE) $(LIB_TDEUI) -ltdefx $(LIB_TDEIO) -ltdetexteditor -lartsflow -lartsflow_idl
ksayit_LDADD = $(top_builddir)/ksayit/KTTSD_Lib/libKTTSD_Lib.la $(LIB_TDEIO) \
	-lartskde -lsoundserver_idl -lartsflow -lartsflow_idl


# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/ksayit


shellrc_DATA =  ksayitui.rc ContextMenus.xml
xdg_apps_DATA = ksayit.desktop
include_HEADERS = ksayit_fxplugin.h
