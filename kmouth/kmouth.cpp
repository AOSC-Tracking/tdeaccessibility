/***************************************************************************
                          kmouth.cpp  -  description
                             -------------------
    begin                : Mon Aug 26 15:41:23 CEST 2002
    copyright            : (C) 2002 by Gunnar Schmi Dt
    email                : kmouth@schmi-dt.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for QT
#include <tqdir.h>
#include <tqpainter.h>

// include files for KDE
#include <kiconloader.h>
#include <tdemenubar.h>
#include <kstatusbar.h>
#include <tdelocale.h>
#include <tdeconfig.h>
#include <kstdaction.h>
#include <kprinter.h>
#include <tdepopupmenu.h>
#include <kstandarddirs.h>

// application specific includes
#include "kmouth.h"
#include "phraselist.h"
#include "phrasebook/phrasebook.h"
#include "phrasebook/phrasebookdialog.h"
#include "optionsdialog.h"
#include "configwizard.h"

#define ID_STATUS_MSG 1

KMouthApp::KMouthApp(TQWidget* , const char* name):TDEMainWindow(0, name)
{
   isConfigured = false;
   config=kapp->config();

   ///////////////////////////////////////////////////////////////////
   // call inits to invoke all other construction parts
   initStatusBar();
   initPhraseList();
   initActions();
   optionsDialog = new OptionsDialog(this);
   connect (optionsDialog, TQ_SIGNAL(configurationChanged ()),
            this, TQ_SLOT(slotConfigurationChanged ()));
   connect (optionsDialog, TQ_SIGNAL(configurationChanged ()),
            phraseList, TQ_SLOT(configureCompletion ()));

   phrases = new TDEActionCollection (this);

   readOptions();
   ConfigWizard *wizard = new ConfigWizard (this, "ConfigWizard", config);
   if (wizard->configurationNeeded ()) {
      if (wizard->requestConfiguration ()) {
         isConfigured = true;
         saveOptions();
         wizard->saveConfig (config);
         readOptions();
      }
      else
         isConfigured = false;
   }
   else
      isConfigured = true;
   delete wizard;

   if (isConfigured) {
      phraseList->configureCompletion();
   }

   ///////////////////////////////////////////////////////////////////
   // disable actions at startup
   fileSaveAs->setEnabled(false);
   filePrint->setEnabled(false);
}

KMouthApp::~KMouthApp()
{

}

bool KMouthApp::configured() {
   return isConfigured;
}

void KMouthApp::initActions() {
// The "File" menu
   fileOpen = new TDEAction(i18n("&Open as History..."), "phrasehistory_open", TDEStdAccel::open(), this, TQ_SLOT(slotFileOpen()), actionCollection(),"file_open");
   fileOpen->setStatusText(i18n("Opens an existing file as history"));
   fileOpen->setWhatsThis (i18n("Opens an existing file as history"));

   fileSaveAs = new TDEAction(i18n("Save &History As..."), "phrasehistory_save", TDEStdAccel::save(), this, TQ_SLOT(slotFileSaveAs()), actionCollection(),"file_save_as");
   fileSaveAs->setStatusText(i18n("Saves the actual history as..."));
   fileSaveAs->setWhatsThis (i18n("Saves the actual history as..."));

   filePrint = new TDEAction(i18n("&Print History..."), "phrasehistory_print", TDEStdAccel::print(), this, TQ_SLOT(slotFilePrint()), actionCollection(),"file_print");
   filePrint->setStatusText(i18n("Prints out the actual history"));
   filePrint->setWhatsThis (i18n("Prints out the actual history"));

   fileQuit = KStdAction::quit(this, TQ_SLOT(slotFileQuit()), actionCollection());
   fileQuit->setStatusText(i18n("Quits the application"));
   fileQuit->setWhatsThis (i18n("Quits the application"));

// The "Edit" menu
   editCut = KStdAction::cut(phraseList, TQ_SLOT(cut()), actionCollection());
   editCut->setStatusText(i18n("Cuts the selected section and puts it to the clipboard"));
   editCut->setWhatsThis (i18n("Cuts the selected section and puts it to the clipboard. If there is some text selected in the edit field it is placed it on the clipboard. Otherwise the selected sentences in the history (if any) are placed on the clipboard."));

   editCopy = KStdAction::copy(phraseList, TQ_SLOT(copy()), actionCollection());
   editCopy->setStatusText(i18n("Copies the selected section to the clipboard"));
   editCopy->setWhatsThis (i18n("Copies the selected section to the clipboard. If there is some text selected in the edit field it is copied to the clipboard. Otherwise the selected sentences in the history (if any) are copied to the clipboard."));

   editPaste = KStdAction::paste(phraseList, TQ_SLOT(paste()), actionCollection());
   editPaste->setStatusText(i18n("Pastes the clipboard contents to actual position"));
   editPaste->setWhatsThis (i18n("Pastes the clipboard contents at the current cursor position into the edit field."));

   editSpeak = new TDEAction (i18n("&Speak"), "speak", 0, phraseList, TQ_SLOT(speak()), actionCollection(),"edit_speak");
   editSpeak->setStatusText(i18n("Speaks the currently active sentence(s)"));
   editSpeak->setWhatsThis (i18n("Speaks the currently active sentence(s). If there is some text in the edit field it is spoken. Otherwise the selected sentences in the history (if any) are spoken."));

// The "Phrase book" menu
   phrasebookEdit = new TDEAction(i18n("&Edit..."), 0, 0, this, TQ_SLOT(slotEditPhrasebook()), actionCollection(),"phrasebook_edit");

// The "Options" menu
   viewMenuBar = KStdAction::showMenubar(this, TQ_SLOT(slotViewMenuBar()), actionCollection());
   viewToolBar = KStdAction::showToolbar(this, TQ_SLOT(slotViewToolBar()), actionCollection());
   viewToolBar->setStatusText(i18n("Enables/disables the toolbar"));
   viewToolBar->setWhatsThis (i18n("Enables/disables the toolbar"));

   viewPhrasebookBar = new TDEToggleAction (i18n("Show P&hrasebook Bar"), 0, 0, this, TQ_SLOT(slotViewPhrasebookBar()), actionCollection(), "showPhrasebookBar");
   viewPhrasebookBar->setStatusText(i18n("Enables/disables the phrasebook bar"));
   viewPhrasebookBar->setWhatsThis (i18n("Enables/disables the phrasebook bar"));

   viewStatusBar = KStdAction::showStatusbar(this, TQ_SLOT(slotViewStatusBar()), actionCollection());
   viewStatusBar->setStatusText(i18n("Enables/disables the statusbar"));
   viewStatusBar->setWhatsThis (i18n("Enables/disables the statusbar"));

   configureTTS = new TDEAction (i18n("&Configure KMouth..."), "configure", 0, this, TQ_SLOT(slotConfigureTTS()), actionCollection(), "configureTTS");
   configureTTS->setStatusText(i18n("Opens the configuration dialog"));
   configureTTS->setWhatsThis (i18n("Opens the configuration dialog"));

// The "Help" menu
   // The "Help" menu will automatically get created.

// The popup menu of the list of spoken sentences
   phraseListSpeak = new TDEAction (i18n("&Speak"),  "speak",  0, phraseList, TQ_SLOT(speakListSelection()),  actionCollection(), "phraselist_speak");
   phraseListSpeak->setStatusText(i18n("Speaks the currently selected phrases in the history"));
   phraseListSpeak->setWhatsThis (i18n("Speaks the currently selected phrases in the history"));

   phraseListRemove = new TDEAction (i18n("&Delete"), "edit-delete", 0, phraseList, TQ_SLOT(removeListSelection()), actionCollection(), "phraselist_remove");
   phraseListRemove->setStatusText(i18n("Deletes the currently selected phrases from the history"));
   phraseListRemove->setWhatsThis (i18n("Deletes the currently selected phrases from the history"));

   phraseListCut = new TDEAction (i18n("Cu&t"),   "edit-cut", 0, phraseList, TQ_SLOT(cutListSelection()),    actionCollection(), "phraselist_cut");
   phraseListCut->setStatusText(i18n("Cuts the currently selected phrases from the history and puts them to the clipboard"));
   phraseListCut->setWhatsThis (i18n("Cuts the currently selected phrases from the history and puts them to the clipboard"));

   phraseListCopy   = new TDEAction (i18n("&Copy"), "edit-copy", 0, phraseList, TQ_SLOT(copyListSelection()),   actionCollection(), "phraselist_copy");
   phraseListCut->setStatusText(i18n("Copies the currently selected phrases from the history to the clipboard"));
   phraseListCut->setWhatsThis (i18n("Copies the currently selected phrases from the history to the clipboard"));

   phraselistSelectAll = new TDEAction (i18n("Select &All Entries"), 0, 0, phraseList, TQ_SLOT(selectAllEntries()), actionCollection(),"phraselist_select_all");
   phraselistSelectAll->setStatusText(i18n("Selects all phrases in the history"));
   phraselistSelectAll->setWhatsThis (i18n("Selects all phrases in the history"));

   phraselistDeselectAll = new TDEAction (i18n("D&eselect All Entries"), 0, 0, phraseList, TQ_SLOT(deselectAllEntries()), actionCollection(),"phraselist_deselect_all");
   phraselistDeselectAll->setStatusText(i18n("Deselects all phrases in the history"));
   phraselistDeselectAll->setWhatsThis (i18n("Deselects all phrases in the history"));

// The popup menu of the edit field
   // The popup menu of the edit field will automatically get created.

   // use the absolute path to your kmouthui.rc file for testing purpose in createGUI();
   createGUI();
}

void KMouthApp::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  // STATUSBAR
  // TODO: add your own items you need for displaying current application status.
  statusBar()->insertItem(i18n("Ready."), ID_STATUS_MSG);
}

void KMouthApp::initPhraseList()
{
  ////////////////////////////////////////////////////////////////////
  // create the main widget here that is managed by KTMainWindow's view-region and
  // connect the widget to your document to display document contents.

   phraseList = new PhraseList(this);
   setCentralWidget(phraseList);
}

void KMouthApp::openDocumentFile(const KURL& url)
{
  slotStatusMsg(i18n("Opening file..."));

  phraseList->open (url);
  slotStatusMsg(i18n("Ready."));
}

void KMouthApp::saveOptions() {
   if (isConfigured) {
      config->setGroup("General Options");
      config->writeEntry("Geometry", size());
      config->writeEntry("Show Menubar", viewMenuBar->isChecked());
      config->writeEntry("Show Toolbar", viewToolBar->isChecked());
      config->writeEntry("Show Phrasebook Bar", viewPhrasebookBar->isChecked());
      config->writeEntry("Show Statusbar",viewStatusBar->isChecked());
      config->writeEntry("ToolBarPos", (int) toolBar("mainToolBar")->barPos());

      if (phraseList != 0)
         phraseList->saveCompletionOptions(config);
      optionsDialog->saveOptions(config);
      toolBar("mainToolBar")->saveSettings (config, "mainToolBar");
      toolBar("phrasebookBar")->saveSettings (config, "phrasebookBar");
   }
}


void KMouthApp::readOptions()
{
  config->setGroup("General Options");

  // bar status settings
  bool bViewMenubar = config->readBoolEntry("Show Menubar", true);
  viewMenuBar->setChecked(bViewMenubar);
  slotViewMenuBar();

  bool bViewToolbar = config->readBoolEntry("Show Toolbar", true);
  viewToolBar->setChecked(bViewToolbar);
  slotViewToolBar();

  bool bViewPhrasebookbar = config->readBoolEntry("Show Phrasebook Bar", true);
  viewPhrasebookBar->setChecked(bViewPhrasebookbar);

  bool bViewStatusbar = config->readBoolEntry("Show Statusbar", true);
  viewStatusBar->setChecked(bViewStatusbar);
  slotViewStatusBar();


  // bar position settings
  TDEToolBar::BarPosition toolBarPos;
  toolBarPos=(TDEToolBar::BarPosition) config->readNumEntry("ToolBarPos", TDEToolBar::Top);
  toolBar("mainToolBar")->setBarPos(toolBarPos);

  TQSize size=config->readSizeEntry("Geometry");
  if(!size.isEmpty())
  {
    resize(size);
  }

  optionsDialog->readOptions(config);

  toolBar("mainToolBar")->applySettings (config, "mainToolBar");
  toolBar("phrasebookBar")->applySettings (config, "phrasebookBar");

  TQString standardBook = TDEApplication::kApplication()->dirs()->findResource("appdata", "standard.phrasebook");
  if (!standardBook.isEmpty()) {
     PhraseBook book;
     book.open(KURL::fromPathOrURL( standardBook ));
     slotPhrasebookConfirmed(book);
  }
  if (phraseList != 0)
     phraseList->readCompletionOptions(config);
}

bool KMouthApp::queryClose()
{
  return true;
}

bool KMouthApp::queryExit()
{
  saveOptions();
  return true;
}

void KMouthApp::enableMenuEntries(bool existSelectedEntries, bool existDeselectedEntries) {
  bool existEntries = existSelectedEntries | existDeselectedEntries;
  fileSaveAs->setEnabled (existEntries);
  filePrint->setEnabled (existEntries);

  phraselistSelectAll->setEnabled (existDeselectedEntries);

  phraselistDeselectAll->setEnabled (existSelectedEntries);
  phraseListSpeak->setEnabled (existSelectedEntries);
  phraseListRemove->setEnabled (existSelectedEntries);
  phraseListCut->setEnabled (existSelectedEntries);
  phraseListCopy->setEnabled (existSelectedEntries);
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void KMouthApp::slotFileOpen() {
   slotStatusMsg(i18n("Opening file..."));

   phraseList->open();

   slotStatusMsg(i18n("Ready."));
}

void KMouthApp::slotFileSaveAs() {
   slotStatusMsg(i18n("Saving history with a new filename..."));

   phraseList->save();

   slotStatusMsg(i18n("Ready."));
}

void KMouthApp::slotFilePrint()
{
  slotStatusMsg(i18n("Printing..."));

  KPrinter printer;
  if (printer.setup(this))
  {
    phraseList->print(&printer);
  }

  slotStatusMsg(i18n("Ready."));
}

void KMouthApp::slotFileQuit()
{
  slotStatusMsg(i18n("Exiting..."));
  saveOptions();
  // close the first window, the list makes the next one the first again.
  // This ensures that queryClose() is called on each window to ask for closing
  TDEMainWindow* w;
  if (memberList)
  {
    for(w=memberList->first(); w!=0; w=memberList->first())
    {
      // only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
      // the window and the application stay open.
      if(!w->close())
         break;
      memberList->removeRef(w);
    }
  }
}

void KMouthApp::slotEditPhrasebook () {
   PhraseBookDialog *phraseBookDialog = PhraseBookDialog::get();
   // As we do not know whether the we are already connected to the slot,
   // we first disconnect and then connect again.
   disconnect (phraseBookDialog, TQ_SIGNAL(phrasebookConfirmed (PhraseBook &)),
               this, TQ_SLOT(slotPhrasebookConfirmed (PhraseBook &)));
   connect (phraseBookDialog, TQ_SIGNAL(phrasebookConfirmed (PhraseBook &)),
            this, TQ_SLOT(slotPhrasebookConfirmed (PhraseBook &)));

   // As we do not know whether the phrase book edit window is already open,
   // we first open and then raise it, so that it is surely the top window.
   phraseBookDialog->show();
   phraseBookDialog->raise();
}

void KMouthApp::slotViewMenuBar() {
   slotStatusMsg(i18n("Toggling menubar..."));

   if(!viewMenuBar->isChecked())
      menuBar()->hide();
   else
      menuBar()->show();

  slotStatusMsg(i18n("Ready."));
}

void KMouthApp::slotViewToolBar()
{
  slotStatusMsg(i18n("Toggling toolbar..."));
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off
  if(!viewToolBar->isChecked())
  {
    toolBar("mainToolBar")->hide();
  }
  else
  {
    toolBar("mainToolBar")->show();
  }

  slotStatusMsg(i18n("Ready."));
}

void KMouthApp::slotViewPhrasebookBar()
{
  slotStatusMsg(i18n("Toggling phrasebook bar..."));
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off
  if(!viewPhrasebookBar->isChecked())
  {
    toolBar("phrasebookBar")->hide();
  }
  else
  {
    toolBar("phrasebookBar")->show();
  }

  slotStatusMsg(i18n("Ready."));
}

void KMouthApp::slotViewStatusBar()
{
  slotStatusMsg(i18n("Toggle the statusbar..."));
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off
  if(!viewStatusBar->isChecked())
  {
    statusBar()->hide();
  }
  else
  {
    statusBar()->show();
  }

  slotStatusMsg(i18n("Ready."));
}

void KMouthApp::slotConfigureTTS() {
   phraseList->saveWordCompletion();
   optionsDialog->show();
}


void KMouthApp::slotStatusMsg(const TQString &text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message permanently
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MSG);
}

void KMouthApp::slotPhrasebookConfirmed (PhraseBook &book) {
   TQString name = "phrasebooks";
   TQPopupMenu *popup = (TQPopupMenu *)factory()->container(name, this);
   TDEToolBar *toolbar = toolBar ("phrasebookBar");

   TDEActionPtrList actions = phrases->actions ();
   TDEActionPtrList::iterator iter;
   for (iter = actions.begin(); iter != actions.end(); ++iter) {
      (*iter)->unplugAll();
   }
   delete phrases;

   phrases = new TDEActionCollection (this, actionCollection());
   book.addToGUI (popup, toolbar, phrases, this, TQ_SLOT(slotPhraseSelected (const TQString &)));

   TQString bookLocation = TDEApplication::kApplication()->dirs()->saveLocation ("appdata", "/");
   if (!bookLocation.isNull() && !bookLocation.isEmpty()) {
      book.save (KURL::fromPathOrURL( bookLocation + "standard.phrasebook" ));
   }
}

void KMouthApp::slotConfigurationChanged()
{
   optionsDialog->saveOptions (config);
}

void KMouthApp::slotPhraseSelected (const TQString &phrase) {
   phraseList->insert (phrase);
   if (optionsDialog->isSpeakImmediately())
      phraseList->speak ();
}

TextToSpeechSystem *KMouthApp::getTTSSystem() const {
   return optionsDialog->getTTSSystem();
}

#include "kmouth.moc"
