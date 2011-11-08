/***************************************************************************
                          ksayit.cpp  -  description
                             -------------------
    begin                : Son Aug 10 13:26:57 EDT 2003
    copyright            : (C) 2003 by Robert Vogl
    email                : voglrobe@saphir
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdlib.h> // getenv, system

// include files for QT
#include <tqdir.h>
#include <tqapplication.h>
#include <tqdoctwindow.h>
#include <tqfileinfo.h>
#include <tqfile.h>

// include files for KDE
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdialogbase.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <kpopupmenu.h>
#include <kstatusbar.h>
#include <kconfig.h>
#include <kstdaction.h>
#include <kedittoolbar.h>
#include <kkeydialog.h>
#include <kurl.h>
#include <kstandarddirs.h>

// application specific includes
#include "ksayit.h"
#include "ksayitviewimpl.h"
#include "voicesetupdlg.h"
#include "ksayitsystemtray.h"
#include "effectstack.h"
#include "fxpluginhandler.h"
#include "kttsdlib.h"
#include "doctreeviewimpl.h"
#include "ksayitbookmarkhandler.h"

// #define KSAYITUI "/home/voglrobe/Projekte/ksayit/ksayit/ksayitui.rc"
#define KSAYITUI "ksayitui.rc"
#define ID_STATUS_MSG 1

KSayItApp::KSayItApp(TQWidget* parent, const char* name, WFlags f,
    const TQCString &objID)
  : KMainWindow(parent, name, f), DCOPObject(objID)
{
    config    = NULL;
    view      = NULL;
    tray      = NULL;
    es        = NULL;
    treeview  = NULL;
    bkManager = NULL;
    bkHandler = NULL;
    bkMenu    = NULL;
    m_currentBookmarkFile = TQString();
  
    config=kapp->config();
    m_kttslib = new KTTSDLib(TQT_TQOBJECT(this), "KTTSD-Library", kapp);
    
    initView();
    initStatusBar();
    initActions();
    initBookmarkManager( i18n("Untitled") );
    
    createGUI(KSAYITUI);
    setAutoSaveSettings("MainWindow");     
    
    readOptions();
    
    // connect TTS plugin handler to this object
    connect(m_kttslib, TQT_SIGNAL(signalFinished()),
            TQT_TQOBJECT(this), TQT_SLOT(slotTTSFinished()) );
    
    // only used if library thinks it has to call the preferences dialog.
    // e.g. when it detects a bad configuration.
    connect(m_kttslib, TQT_SIGNAL(signalCallPreferences()),
            TQT_TQOBJECT(this), TQT_SLOT(slotPreferences()) );
    
    // init Clipboard
    cb = TQApplication::tqclipboard();
    connect(cb, TQT_SIGNAL(dataChanged()), TQT_TQOBJECT(this), TQT_SLOT(slotClipboardChanged()) );
    
    // create SystemTray object
    tray = new KSayItSystemTray(this, "system_tray");
    connect(tray, TQT_SIGNAL(signalCallPreferences()), TQT_TQOBJECT(this), TQT_SLOT(slotPreferences()) );
    connect(tray, TQT_SIGNAL(signalSayActivated()), TQT_TQOBJECT(this), TQT_SLOT(slotSayText()) );
    connect(tray, TQT_SIGNAL(signalSayClipboard()), TQT_TQOBJECT(this), TQT_SLOT(slotSayClipboard()) );
    connect(tray, TQT_SIGNAL(signalShutUpActivated()), TQT_TQOBJECT(this), TQT_SLOT(slotStopActivated()) );
    connect(tray, TQT_SIGNAL(signalPauseActivated()), TQT_TQOBJECT(this), TQT_SLOT(slotPauseActivated()) );
    connect(tray, TQT_SIGNAL(signalNextActivated()), TQT_TQOBJECT(this), TQT_SLOT(slotNextSentenceActivated()) );
    connect(tray, TQT_SIGNAL(signalPrevActivated()), TQT_TQOBJECT(this), TQT_SLOT(slotPrevSentenceActivated()) );
    tray->show();
    tray->setEnabled(true);
    
    // create the FX plugin handler
    m_fxpluginhandler = new FXPluginHandler(TQT_TQOBJECT(this), "fxpluginhandler", config);
    m_fxpluginhandler->searchPlugins();
    
    // create object for effectstack  
    es = new EffectStack(m_fxpluginhandler, config);
    
    // emit signalConfigChanged();
    m_kttslib->reloadConfiguration();
    m_fxpluginhandler ->readConfiguration();
    
    // init docview
    // treeview->clear();
    // treeview->createEmptyDocument();
    slotClear();  
}

KSayItApp::~KSayItApp()
{
  // delete kapp->mainWidget();
  delete m_kttslib;
  delete tray;
  delete m_fxpluginhandler;
  delete es;
  delete view;
  if (bkHandler)
      delete bkHandler;
  if (bkMenu)
      delete bkMenu;
}

void KSayItApp::initActions()
{
  // Standard-Actions
  open   = KStdAction::open(TQT_TQOBJECT(this), TQT_SLOT(slotFileOpen()), actionCollection());
  save   = KStdAction::save(TQT_TQOBJECT(this), TQT_SLOT(slotFileSave()), actionCollection());
  saveAs = KStdAction::saveAs(TQT_TQOBJECT(this), TQT_SLOT(slotFileSaveAs()), actionCollection());
  KStdAction::quit(TQT_TQOBJECT(this), TQT_SLOT(slotFileQuit()), actionCollection());
  KStdAction::close(TQT_TQOBJECT(this), TQT_SLOT(slotCloseMainWindow()), actionCollection());
  cut   = KStdAction::cut(TQT_TQOBJECT(view), TQT_SLOT(slotCut()), actionCollection());
  copy  = KStdAction::copy(TQT_TQOBJECT(view), TQT_SLOT(slotCopy()), actionCollection());
  paste = KStdAction::paste(TQT_TQOBJECT(view), TQT_SLOT(slotPaste()), actionCollection());
  preferences = KStdAction::preferences(TQT_TQOBJECT(this), TQT_SLOT(slotPreferences()), actionCollection());
  KStdAction::keyBindings(TQT_TQOBJECT(this), TQT_SLOT(slotEditKeys()), actionCollection());
  KStdAction::configureToolbars(TQT_TQOBJECT(this), TQT_SLOT(slotConfigureToolbar()), actionCollection());
  KStdAction::showToolbar("mainToolBar", actionCollection());
  statusBarAction = KStdAction::showStatusbar(TQT_TQOBJECT(this), TQT_SLOT(slotToggleStatusBar()), actionCollection());

  // User defined actions
  say = new KAction (i18n("Say"),
              TQt::Key_F9,
              TQT_TQOBJECT(this), TQT_SLOT (slotSayText()),
              actionCollection(),
              "say_it");

  pause = new KAction (i18n("Pause"),
              TQt::Key_Pause,
              TQT_TQOBJECT(this), TQT_SLOT (slotPauseActivated()),
              actionCollection(),
              "pause");
  
  shutup = new KAction (i18n("Shut Up"),
              TQt::Key_F10,
              TQT_TQOBJECT(this), TQT_SLOT (slotStopActivated()),
              actionCollection(),
              "shut_up");
              
  next_sentence = new KAction (i18n("Next Sentence"),
              TQt::Key_Next,
              TQT_TQOBJECT(this), TQT_SLOT (slotNextSentenceActivated()),
              actionCollection(),
              "next_sentence");


  prev_sentence = new KAction (i18n("Previous Sentence"),
              TQt::Key_Prior,
              TQT_TQOBJECT(this), TQT_SLOT(slotPrevSentenceActivated()),
              actionCollection(),
              "prev_sentence");
  
  clear = new KAction (i18n("Clear"),
              TQt::Key_F12,
              TQT_TQOBJECT(this), TQT_SLOT(slotClear()),
              actionCollection(),
              "clear");
  
  edit = new KToggleAction( i18n("Edit Text"),
             0,
             TQT_TQOBJECT(this), TQT_SLOT(slotEditToggled()),
             actionCollection(),
             "edittext");
             
  bookmarkmenu = new KActionMenu( i18n("Bookmarks"),
             "bookmark", // icon
             actionCollection(),
             "bookmarks"); // name
             bookmarkmenu->setDelayed(false);
  
  // default disable/enable
  save         ->setEnabled(false);
  saveAs       ->setEnabled(false);
  copy         ->setEnabled(false);
  cut          ->setEnabled(false);
  paste        ->setEnabled(false);
  say          ->setEnabled(false);
  pause        ->setEnabled(false);
  shutup       ->setEnabled(false);
  next_sentence->setEnabled(false);
  prev_sentence->setEnabled(false);
  preferences  ->setEnabled(true);
  edit         ->setChecked(false);
  bookmarkmenu ->setEnabled(false);
  m_ap_saying = false;
  m_ap_paused = false;
  m_textview_empty = true;
}


void KSayItApp::initBookmarkManager(const TQString &filename)
{
    TQString bkFile = getBookmarkDir( filename );
    if ( bkFile.isNull() )
        return;
    
    m_currentBookmarkFile = bkFile;        
    bkManager = KBookmarkManager::managerForFile( bkFile, false );
    
    // create BookmarkHandler object
    if ( bkHandler )
        delete bkHandler;
    bkHandler = new KSayItBookmarkHandler(bkManager, this);
    
    // create Bookmarkmenu
    KPopupMenu *bkPopupMenu = bookmarkmenu->popupMenu(); 
    if ( bkMenu )
        delete bkMenu;
    bkMenu = new KBookmarkMenu(bkManager, bkHandler, bkPopupMenu, 0, true );   
}


void KSayItApp::slotNotifyBookmarkHandler(const TQString &ID, const TQString &title)
{
    if ( bkHandler )
        bkHandler->notifyBookmarkHandler(ID, title);
}


void KSayItApp::slotDeleteBookmark(const TQString &url, const TQString &title)
{
    kdDebug(100200) << "KSayItApp::slotDeleteBookmark(" << url << ")" << endl;
    if ( bkHandler )
        bkHandler->deleteBookmark( url, title );
}


TQString KSayItApp::getBookmarkDir(const TQString &filename)
{
    TQString bkRelPath = "ksayit/ksayit_bookmarks/" + filename + ".bookmarks";
    TQString bkFile;   
    bkFile = locateLocal("data", bkRelPath);
    return bkFile;
}


void KSayItApp::slotSetBookmarkFilename(const TQString &newname)
{
    kdDebug(100200) << "KSayItApp::slotSetBookmarkFilename(" << newname << ")" << endl;
    
    initBookmarkManager(newname);
}


void KSayItApp::slotChangeBookmarkFilename(const TQString &newname)
{
    kdDebug(100200) << "KSayItApp::slotChangeBookmarkFilename(" << newname << ")" << endl;
    
    if ( m_currentBookmarkFile.isNull() )
        return;
        
    TQString newbkFile = getBookmarkDir(newname);
    if ( newbkFile.isNull() )
        return;    
    
    // copy old bookmarkfile to new file
    if ( m_currentBookmarkFile != newbkFile ){
        if ( TQFile::exists(m_currentBookmarkFile) ){
            TQString command = TQString("cp %1 %2").tqarg(m_currentBookmarkFile).tqarg(newbkFile);
            system( command.ascii() );
        }
        // install new BookmarkHandler based on the new file
        initBookmarkManager(newname);
    }
}


TQString KSayItApp::setItemByBookmark( const TQString &ID, const TQString &title )
{
    TQString result;
    result = treeview->selectItemByID( ID, title );
    return result;
}


void KSayItApp::initStatusBar()
{
  // STATUSBAR
  statusBar()->insertItem(i18n("Ready."), ID_STATUS_MSG, 1);
  statusBar()->setItemAlignment(ID_STATUS_MSG, TQt::AlignLeft);
}


void KSayItApp::initView()
{ 
  // create the main widget
  view = new KSayItViewImpl(this);
  view->setMinimumSize(view->tqsizeHint());
  setCentralWidget(view);
  // connections
  connect( view, TQT_SIGNAL(signalEnableCopyCut(bool)),
    TQT_TQOBJECT(this), TQT_SLOT(slotEnableCopyCut(bool)));
  connect( view, TQT_SIGNAL(signalShowtqStatus(const TQString &)),
    TQT_TQOBJECT(this), TQT_SLOT(slotStatusMsg(const TQString &)));
  connect( view, TQT_SIGNAL(signalSetCaption(const TQString &)),
    TQT_TQOBJECT(this), TQT_SLOT(slotSetCaption(const TQString &)));
  connect( view, TQT_SIGNAL(signalTextChanged(bool)),
    TQT_TQOBJECT(this), TQT_SLOT(slotTextChanged(bool)) );
  
  // DockWindow
  TQDockWindow *docview = new TQDockWindow(TQDockWindow::InDock, this, "docview");
  treeview = new DocTreeViewImpl(docview, "treeview");
  treeview->enableContextMenus( true );
  // treeview->clear();
  docview->setWidget(treeview);
  docview->setResizeEnabled(true);
  this->setDockEnabled(docview, TQt::DockTop, false);
  this->setDockEnabled(docview, TQt::DockBottom, false);
  this->moveDockWindow(docview, TQt::DockLeft);
  connect( treeview, TQT_SIGNAL(signalContentChanged(const TQString&)),
    TQT_TQOBJECT(this), TQT_SLOT(slotTreeViewChanged(const TQString&)) );
  connect( treeview, TQT_SIGNAL(signalSetText(const TQString&)),
    TQT_TQOBJECT(this), TQT_SLOT(slotSetText(const TQString&)) );
  connect( treeview, TQT_SIGNAL(signalAllNodesProcessed()),
    TQT_TQOBJECT(this), TQT_SLOT(slotSayNode()) );
  connect( treeview, TQT_SIGNAL(signalEnableTextedit(bool)),
    TQT_TQOBJECT(this), TQT_SLOT(slotEnableTextedit(bool)) );
  connect( treeview, TQT_SIGNAL(signalNotifyBookmarkManager(const TQString&, const TQString&)),
    TQT_TQOBJECT(this), TQT_SLOT(slotNotifyBookmarkHandler(const TQString&, const TQString&)) );
  connect( treeview, TQT_SIGNAL(signalSetBookmarkFilename(const TQString&)),
    TQT_TQOBJECT(this), TQT_SLOT(slotSetBookmarkFilename(const TQString&)) );
  connect( treeview, TQT_SIGNAL(signalChangeBookmarkFilename(const TQString&)),
    TQT_TQOBJECT(this), TQT_SLOT(slotChangeBookmarkFilename(const TQString&)) );                     
  connect( treeview, TQT_SIGNAL(signalDeleteBookmark(const TQString&, const TQString&)),
    TQT_TQOBJECT(this), TQT_SLOT(slotDeleteBookmark(const TQString&, const TQString&)) );
}


void KSayItApp::readOptions()
{
  applyMainWindowSettings( config, "MainWindow" );
  statusBarAction->setChecked(!statusBar()->isHidden());
}

void KSayItApp::closeEvent(TQCloseEvent *ce)
{
  ce->ignore();
  if ( !this->isVisible() ){
    slotFileQuit();
  } else {
    this->hide();
  }
}


/////////////////////////////////////////////////////////////////////
// DCOP FUNCTIONS
/////////////////////////////////////////////////////////////////////

ASYNC KSayItApp::dcopSayText(TQString msg)
{
  slotSetText( msg );
  sayActivated();
}

ASYNC KSayItApp::dcopStop()
{
  slotStopActivated();
}


ASYNC KSayItApp::dcopSayXmlFile(KURL url)
{
    slotStatusMsg(i18n("Say XML file..."));
    m_enableChangeNotifications = false;
   
    try{
        if ( !url.isEmpty() ){
            resetView();    
            treeview->openFile( url );
            setActions(ACTIONS::PLAY | ACTIONS::FFWD);
            save  ->setEnabled(false);
            saveAs->setEnabled(true);              
            edit->setChecked(false);
            treeview->setEditMode( false );
        }
    }
    catch( TQString err ){
        KMessageBox::error(this, err);
        save  ->setEnabled(false);
        saveAs->setEnabled(true);              
    }
    
    m_enableChangeNotifications = true;
    
    slotSayText();
}


/////////////////////////////////////////////////////////////////////
// TQT_SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void KSayItApp::slotEditToggled()
{
    if ( edit->isChecked() ){
        treeview->setEditMode( true );
        slotStatusMsg(i18n("Edit Mode ON."));
    } else {
        treeview->setEditMode( false );
        slotStatusMsg(i18n("Edit Mode OFF."));
    }
}


void KSayItApp::slotSaveOptions()
{
  saveMainWindowSettings( config, "MainWindow" );
  config->sync();
}


void KSayItApp::slotConfigureToolbar()
{
  saveMainWindowSettings( config, "MainWindow" );
  KEditToolbar dlg(actionCollection(), KSAYITUI);
  connect(&dlg,TQT_SIGNAL(newToolbarConfig()),this,TQT_SLOT(slotNewToolbarConfig()));
  if (dlg.exec())
  {
     createGUI(KSAYITUI);
  }
}


void KSayItApp::slotNewToolbarConfig()
{
  applyMainWindowSettings( config, "MainWindow" );
}


void KSayItApp::slotFileOpen()
{
    slotStatusMsg(i18n("Open file..."));
    m_enableChangeNotifications = false;
   
    // open filerequester
    TQString fn;
    TQString usershome( getenv("HOME") );
    KURL url = KFileDialog::getOpenURL(usershome, TQString(), this, i18n("File to Speak") );
    try{
        if ( !url.isEmpty() ){
            resetView();    
            treeview->openFile( url );
            setActions(ACTIONS::PLAY | ACTIONS::FFWD);
            save  ->setEnabled(false);
            saveAs->setEnabled(true);              
            edit->setChecked(false);
            treeview->setEditMode( false );
            bookmarkmenu->setEnabled(true);
        }
    }
    catch( TQString err ){
        KMessageBox::error(this, err);
        save  ->setEnabled(false);
        saveAs->setEnabled(true);
        bookmarkmenu->setEnabled(false);              
    }
  
  m_enableChangeNotifications = true;
}


void KSayItApp::slotFileSave()
{
   slotStatusMsg(i18n("Save file..."));
   try {
         treeview->saveFile();
         save  ->setEnabled(false);
         saveAs->setEnabled(true);  
   }   
   catch ( TQString err )
   {
      KMessageBox::error(this, err);
      save  ->setEnabled(false);
      saveAs->setEnabled(true);  
   }
}


void KSayItApp::slotFileSaveAs()
{
  slotStatusMsg(i18n("Save file as..."));
   try {
         treeview->saveFileAs();
         save  ->setEnabled(true);
         saveAs->setEnabled(true);
         bookmarkmenu->setEnabled(true);  
   }   
   catch ( TQString err )
   {
      KMessageBox::error(this, err);
      save  ->setEnabled(true);
      saveAs->setEnabled(true);
      bookmarkmenu->setEnabled(false);  
   }
}


void KSayItApp::slotEnableCopyCut(bool enable)
{
  copy->setEnabled(enable);
  cut ->setEnabled(enable);
}


void KSayItApp::slotEditKeys()
{
  KKeyDialog::configure( actionCollection() ); //, xmlFile());
}


void KSayItApp::slotFileQuit()
{
  slotStatusMsg(i18n("Exiting..."));
  int really = KMessageBox::questionYesNo( this, i18n("Do you really want to quit?"), TQString(), KStdGuiItem::quit(), KStdGuiItem::cancel() );
  if (really == KMessageBox::Yes){
     slotSaveOptions();
     kapp->quit();
  } else {
     slotStatusMsg(i18n("Ready."));
  }  
}


void KSayItApp::slotCloseMainWindow()
{
  this->hide();
}


void KSayItApp::slotToggleStatusBar()
{
  slotStatusMsg(i18n("Toggle the statusbar..."));
  //turn Statusbar on or off
  if(statusBar()->isVisible())
    statusBar()->hide();
  else
    statusBar()->show();

  slotStatusMsg(i18n("Ready."));
}


void KSayItApp::slotStatusMsg(const TQString &text)
{
  // change status message permanently
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MSG);
}


void KSayItApp::slotSetCaption(const TQString &caption)
{
  this->setCaption(caption);
}


void KSayItApp::slotPreferences()
{
  // create Preferences object and open it
  VoiceSetupDlg *prefs = new VoiceSetupDlg(this, "preferences", i18n("Setup"), true,
                      config,
                      m_fxpluginhandler,
                      m_kttslib);

  if (prefs->exec() == TQDialog::Accepted){
    delete prefs;
    m_kttslib->reloadConfiguration();
    m_fxpluginhandler ->readConfiguration();
  } else {
    delete prefs;
  }
}


void KSayItApp::slotClipboardChanged()
{
  if ( cb->text().isEmpty() ){
    tray->changeState( StateCLIPEMPTY::Instance() );
    paste->setEnabled(false);
    return;
  }

  if ( cb->data(TQClipboard::Clipboard)->provides("text/plain") ){
      tray->changeState( StateWAIT::Instance() );
      paste->setEnabled(true);
  } else {
      tray->changeState( StateCLIPEMPTY::Instance() );
      paste->setEnabled(false);
  }  
}


void KSayItApp::slotTextChanged(bool empty)
{
    m_textview_empty = empty;
    
    // do nothing if change notifications from the Textedit are
    // currently not welcome.     
    if ( !m_enableChangeNotifications )
        return; 
    
    // do nothing if say is in progress
    if ( m_ap_saying )
        return;
        
    kdDebug(100200) << "KSayItApp::slotTextChanged( " << empty << " )" << endl;
    
    // Update actions
    if ( empty ){
        setActions(0);    
    } else {
        setActions(ACTIONS::PLAY);
    }
    // save  ->setEnabled(!empty);
    saveAs->setEnabled(!empty);
    
    // update TreeView-Item
    treeview->setNodeContent( view->getText() );    
    
}


void KSayItApp::slotClear()
{
    m_enableChangeNotifications = false;
    // TextEdit clear
    view->textClear();

    // TreeView clear
    treeview->clear();
    treeview->createEmptyDocument();
    treeview->setEditMode( true );
    edit->setChecked(true);
    slotStatusMsg(i18n("Edit Mode ON."));

    // set actions
    save  ->setEnabled(false);
    saveAs->setEnabled(false);
    setActions(0);
    bookmarkmenu->setEnabled(false);
    m_enableChangeNotifications = true;
}


void KSayItApp::slotSayClipboard()
{
    // get Text from clipboard
    slotStatusMsg(i18n("speaking Clipboard..."));
    setActions(ACTIONS::PAUSE | ACTIONS::STOP | ACTIONS::FFWD | ACTIONS::FREV);
    tray->changeState( StateSAY::Instance() );
    
    clip = cb->text();
    // sayActivated( clip.stripWhiteSpace() );
    // set text and say it
    slotSetText( clip.stripWhiteSpace() );
    sayActivated();
}


void KSayItApp::slotSayText()
{
    // say Text from text-window
    setActions(ACTIONS::PAUSE | ACTIONS::STOP | ACTIONS::FFWD | ACTIONS::FREV);
    tray->changeState( StateSAY::Instance() );
    
    slotStatusMsg(i18n("synthesizing..."));

    if ( m_ap_paused ){
        m_ap_paused = false;
        m_kttslib->resume();
    } else {
        es->loadEffects();
        treeview->sayActiveNodeAndChilds();
    }
}


void KSayItApp::slotSayNode()
{
    sayActivated();
}


void KSayItApp::slotSetText(const TQString &text)
{
    // also invoked by the treeview for the each node
    if ( text.isNull() )
        return;
    
    // put text on the stack of KTTS     
    m_kttslib->setText( text );
}

void KSayItApp::sayActivated()
{  
    kdDebug(100200) << "KSayItApp::sayActivated()" << endl;
    
    preferences->setEnabled(false);
    m_ap_saying = true;
        
    try {
        m_kttslib->sayText();
    }
    catch (TQString err){
        kdDebug(100200) << "Exception catched in KSayItApp::sayActivated()" << endl;
        KMessageBox::sorry(0, err, i18n("Unable to speak text"));
        slotFinished();
    }
}


void KSayItApp::slotTTSFinished()
{
    kdDebug(100200) << "KSayItApp::slotTTSFinished()" << endl;
    es->unloadEffects();
    slotFinished();
}


void KSayItApp::slotFinished()
{
    kdDebug(100200) << "KSayItApp::slotFinished()" << endl; 
    
    // disable/enable actions
    m_ap_paused = false;
    
    if ( m_textview_empty ){
        setActions(0);
    } else {
        setActions(ACTIONS::PLAY);
    }
    tray->changeState( StateWAIT::Instance() );
    preferences->setEnabled(true);
    m_ap_saying = false;
    slotStatusMsg(i18n("Ready."));
}


void KSayItApp::slotStopActivated()
{
    m_kttslib->stop();
    // slotFinished();    
}


void KSayItApp::slotPauseActivated()
{
    kdDebug(100200) << "KSayItApp::slotPauseActivated()" << endl;
    
    m_kttslib->pause();
    m_ap_paused = true;
    setActions(ACTIONS::PLAY | ACTIONS::STOP);
    tray->changeState( StateSAY::Instance() );
    slotStatusMsg(i18n("Paused..."));
}


void KSayItApp::slotNextSentenceActivated()
{
    kdDebug(100200) << "KSayItApp::slotNextSentenceActivated()" << endl;
    m_kttslib->ffwd();
}
    

void KSayItApp::slotPrevSentenceActivated()
{
    kdDebug(100200) << "KSayItApp::slotPrevSentenceActivated()" << endl;
    m_kttslib->frev();
}


void KSayItApp::setActions(int actions)
{
    // ACTIONS::PLAY | ACTIONS::STOP  | ACTIONS::PAUSE | ACTIONS::FFWD | ACTIONS::FREV;
    
    // Get the mask of supported actions from the plugin.
    int mask = m_kttslib->getActions();
    kdDebug(100200) << TQString("KSayItApp:PSA: %1").tqarg(mask, 0, 2) << endl;
    
    // disable actions not supported by the plugin
    int ma = actions & mask;
    
    if (ma & ACTIONS::PLAY){
        say->setEnabled(true);
    } else {
        say->setEnabled(false);
    }
    if (ma & ACTIONS::STOP){
        shutup->setEnabled(true);
    } else {
        shutup->setEnabled(false);
    }
    if (ma & ACTIONS::PAUSE){
        pause->setEnabled(true);
    } else {
        pause->setEnabled(false);
    }
    if (ma & ACTIONS::FFWD){
        next_sentence->setEnabled(true);
    } else {
        next_sentence->setEnabled(false);
    }
    if (ma & ACTIONS::FREV){
        prev_sentence->setEnabled(true);
    } else {
        prev_sentence->setEnabled(false);
    }
    
    tray->setActions( say->isEnabled(), pause->isEnabled(), shutup->isEnabled(),
        next_sentence->isEnabled(), prev_sentence->isEnabled() );
}


void KSayItApp::slotTreeViewChanged(const TQString &str)
{
    kdDebug(100200) << "KSayItApp::slotTreeViewChanged()" << endl;
    
    if ( !m_ap_saying )
        setActions(ACTIONS::PLAY);
    
    m_enableChangeNotifications = false;
    view->textClear();
    view->setText(str);
    m_enableChangeNotifications = true;
}


void KSayItApp::slotEnableTextedit(bool enable)
{
    kdDebug(100200) << "KSayItApp::slotEnableTextedit( " << enable << " )" << endl;
    
    view->enableTextedit( enable );
}


void KSayItApp::resetView()
{
    m_enableChangeNotifications = false;
    // TextEdit clear
    view->textClear();

    // TreeView clear
    treeview->clear();

    // set actions
    save  ->setEnabled(false);
    saveAs->setEnabled(false);
    setActions(0);
    m_enableChangeNotifications = true;    
}


void KSayItApp::slotAddBookmark()
{
    kdDebug(100200) << "KSayItApp::slotAddBookmark()" << endl;


}


void KSayItApp::slotEditBookmarks()
{
    kdDebug(100200) << "KSayItApp::slotEditBookmark()" << endl;

    
}



#include "ksayit.moc"
