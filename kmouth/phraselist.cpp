/***************************************************************************
                          phraselist.cpp  -  description
                             -------------------
    begin                : Mit Sep 11 2002
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

// include files for TQt
#include <tqprinter.h>
#include <tqpainter.h>
#include <tqlayout.h>
#include <tqwhatsthis.h>
#include <tqpopupmenu.h>
#include <tqclipboard.h>

// include files for KDE
#include <tdelistbox.h>
#include <klineedit.h>
#include <kaudioplayer.h>
#include <kcursor.h>
#include <kiconloader.h>
#include <tdelocale.h>
#include <tdefiledialog.h>
#include <kcombobox.h>
#include <tdemessagebox.h>

#include <stdlib.h>

// application specific includes
#include "phraselistitem.h"
#include "phraselist.h"
#include "phraseedit.h"
#include "kmouth.h"
#include "texttospeechsystem.h"
#include "phrasebook/phrasebook.h"
#include "wordcompletion/wordcompletion.h"

PhraseList::PhraseList(TQWidget *parent, const char *name) : TQWidget(parent,name) {
   isInSlot = false;
   setBackgroundMode(PaletteBase);
   TQVBoxLayout *layout = new TQVBoxLayout (this);

   listBox = new TDEListBox (this);
   listBox->setFocusPolicy(TQWidget::NoFocus);
   listBox->setSelectionMode (TQListBox::Extended);
   TQWhatsThis::add (listBox, i18n("This list contains the history of spoken sentences. You can select sentences and press the speak button for re-speaking."));
   layout->addWidget(listBox);

   TQHBoxLayout *rowLayout = new TQHBoxLayout ();
   layout->addLayout(rowLayout);

   completion = new WordCompletion();

   dictionaryCombo = new KComboBox (this, "Dictionary Combo");
   configureCompletionCombo(completion->wordLists());
   rowLayout->addWidget(dictionaryCombo);

   lineEdit = new PhraseEdit ("", this);
   lineEdit->setFocusPolicy(TQWidget::StrongFocus);
   lineEdit->setFrame(true);
   lineEdit->setEchoMode(TQLineEdit::Normal);
   lineEdit->setCompletionObject (completion);
   lineEdit->setAutoDeleteCompletionObject(true);
   TQWhatsThis::add (lineEdit, i18n("Into this edit field you can type a phrase. Click on the speak button in order to speak the entered phrase."));
   rowLayout->addWidget(lineEdit);
   lineEdit->setFocus();

   TQIconSet icon = TDEGlobal::iconLoader()->loadIconSet("speak", TDEIcon::Small);
   speakButton = new TQPushButton (icon, i18n("&Speak"), this);
   speakButton->setFocusPolicy(TQWidget::NoFocus);
   speakButton->setAutoDefault(false);
   TQWhatsThis::add (speakButton, i18n("Speaks the currently active sentence(s). If there is some text in the edit field it is spoken. Otherwise the selected sentences in the history (if any) are spoken."));
   rowLayout->addWidget(speakButton);

   connect(dictionaryCombo, TQ_SIGNAL (activated (const TQString &)), completion, TQ_SLOT (setWordList(const TQString &)));
   connect(completion, TQ_SIGNAL (wordListsChanged (const TQStringList &)), this, TQ_SLOT (configureCompletionCombo (const TQStringList &)));
   connect(listBox,  TQ_SIGNAL(selectionChanged()), TQ_SLOT(selectionChanged()));
   connect(listBox,  TQ_SIGNAL(contextMenuRequested (TQListBoxItem *, const TQPoint &)), TQ_SLOT(contextMenuRequested (TQListBoxItem *, const TQPoint &)));
   connect(lineEdit, TQ_SIGNAL(returnPressed(const TQString &)), TQ_SLOT(lineEntered(const TQString &)));
   connect(lineEdit, TQ_SIGNAL(textChanged  (const TQString &)), TQ_SLOT(textChanged(const TQString &)));
   connect(speakButton, TQ_SIGNAL( clicked ()), TQ_SLOT(speak()));
}

PhraseList::~PhraseList() {
   delete speakButton;
   delete listBox;
   delete lineEdit;
}

void PhraseList::print(KPrinter *pPrinter) {
   PhraseBook book;
   for (TQListBoxItem *item = listBox->firstItem(); item != 0; item = item->next()) {
      book += PhraseBookEntry(Phrase(item->text()));
   }
   
   book.print (pPrinter);
}

TQStringList PhraseList::getListSelection() {
   TQStringList res = TQStringList();

   for (TQListBoxItem *item = listBox->firstItem(); item != 0; item = item->next()) {
      if (item->isSelected())
         res += item->text();
   }
   
   return res;
}

bool PhraseList::existListSelection() {
   for (TQListBoxItem *item = listBox->firstItem(); item != 0; item = item->next()) {
      if (item->isSelected())
         return true;
   }

   return false;
}

bool PhraseList::existEditSelection() {
   return lineEdit->hasSelectedText();
}

void PhraseList::enableMenuEntries() {
   bool deselected = false;
   bool selected = false;
   for (TQListBoxItem *item = listBox->firstItem(); item != 0; item = item->next()) {
      if (item->isSelected())
         selected = true;
      else
         deselected = true;
   }
   KMouthApp *theApp=(KMouthApp *) parentWidget();
   theApp->enableMenuEntries (selected, deselected);
}

void PhraseList::configureCompletion() {
   completion->configure();
}

void PhraseList::configureCompletionCombo(const TQStringList &list) {
   TQString current = completion->currentWordList();
   dictionaryCombo->clear();
   if (list.isEmpty())
      dictionaryCombo->hide();
   else if (list.count() == 1) {
      dictionaryCombo->insertStringList (list);
      dictionaryCombo->setCurrentItem (0);
      dictionaryCombo->hide();
   }
   else {
      dictionaryCombo->insertStringList (list);
      dictionaryCombo->show();
   
      TQStringList::ConstIterator it;
      int i = 0;
      for (it = list.begin(), i = 0; it != list.end(); ++it, ++i) {
         if (current == *it) {
            dictionaryCombo->setCurrentItem (i);
            return;
         }
      }
   }
}

void PhraseList::saveCompletionOptions(TDEConfig *config) {
   config->setGroup("General Options");
   config->writeEntry("Show speak button", speakButton->isVisible() || !lineEdit->isVisible());
      
   config->setGroup("Completion");
   config->writeEntry("Mode", static_cast<int>(lineEdit->completionMode()));
   config->writeEntry("List", completion->currentWordList());
}

void PhraseList::readCompletionOptions(TDEConfig *config) {
   config->setGroup("General Options");
   if (!config->readBoolEntry("Show speak button", true))
      speakButton->hide();

   if (config->hasGroup ("Completion")) {
      config->setGroup("Completion");
      int mode = config->readNumEntry ("Mode", TDEGlobalSettings::completionMode());
      lineEdit->setCompletionMode (static_cast<TDEGlobalSettings::Completion>(mode));

      TQString current = config->readEntry ("List", TQString());
      TQStringList list = completion->wordLists();
      TQStringList::ConstIterator it;
      int i = 0;
      for (it = list.begin(), i = 0; it != list.end(); ++it, ++i) {
         if (current == *it) {
            dictionaryCombo->setCurrentItem (i);
            return;
         }
      }
   }
}

void PhraseList::saveWordCompletion () {
   completion->save();
}


void PhraseList::selectAllEntries () {
   listBox->selectAll (true);
}

void PhraseList::deselectAllEntries () {
   listBox->selectAll (false);
}

void PhraseList::speak () {
   TQString phrase = lineEdit->text();
   if (phrase.isNull() || phrase.isEmpty())
      speakListSelection();
   else {
      insertIntoPhraseList (phrase, true);
      speakPhrase (phrase);
   }
}

void PhraseList::cut() {
   if (lineEdit->hasSelectedText())
      lineEdit->cut();
   else
      cutListSelection();
}

void PhraseList::copy() {
   if (lineEdit->hasSelectedText())
      lineEdit->copy();
   else
      copyListSelection();
}

void PhraseList::paste() {
   lineEdit->paste();
}

void PhraseList::insert (const TQString &s) {
   setEditLineText(s);
}

void PhraseList::speakListSelection () {
   speakPhrase(getListSelection().join ("\n"));
}

void PhraseList::removeListSelection () {
   TQListBoxItem *next;
   for (TQListBoxItem *item = listBox->firstItem(); item != 0; item = next) {
      next = item->next();
      
      if (item->isSelected()) {
         listBox->removeItem(listBox->index(item));
      }
   }
   enableMenuEntries ();
}

void PhraseList::cutListSelection () {
   copyListSelection ();
   removeListSelection ();
}

void PhraseList::copyListSelection () {
   TQApplication::clipboard()->setText (getListSelection().join ("\n"));
}

void PhraseList::lineEntered (const TQString &phrase) {
   if (phrase.isNull() || phrase.isEmpty())
      speakListSelection();
   else {
      insertIntoPhraseList (phrase, true);
      speakPhrase (phrase);
   }
}

void PhraseList::speakPhrase (const TQString &phrase) {
   TQApplication::setOverrideCursor (KCursor::WaitCursor, false);
   KMouthApp *theApp=(KMouthApp *) parentWidget();
   TQString language = completion->languageOfWordList (completion->currentWordList());
   theApp->getTTSSystem()->speak(phrase, language);
   TQApplication::restoreOverrideCursor ();
}

void PhraseList::insertIntoPhraseList (const TQString &phrase, bool clearEditLine) {
   int lastLine = listBox->count() - 1;
   if ((lastLine == -1) || (phrase != listBox->text(lastLine))) {
      listBox->insertItem(new PhraseListItem(phrase));
      if (clearEditLine)
         completion->addSentence (phrase);
   }

   if (clearEditLine) {
      lineEdit->selectAll();
      line = "";
   }
   enableMenuEntries ();
}

void PhraseList::contextMenuRequested (TQListBoxItem *, const TQPoint &pos) {
   TQString name;
   if (existListSelection())
      name = "phraselist_selection_popup";
   else
      name = "phraselist_popup";
   
   KMouthApp *theApp=(KMouthApp *) parentWidget();
   KXMLGUIFactory *factory = theApp->factory();
   TQPopupMenu *popup = (TQPopupMenu *)factory->container(name,theApp);
   if (popup != 0) {
      popup->popup(pos, 0);
   }
}

void PhraseList::textChanged (const TQString &s) {
   if (!isInSlot) {
      isInSlot = true;
      line = s;
      listBox->setCurrentItem (listBox->count() - 1);
      listBox->clearSelection ();
      isInSlot = false;
   }
}

void PhraseList::selectionChanged () {
   if (!isInSlot) {
      isInSlot = true;
      
      TQStringList sel = getListSelection();

      if (sel.empty())
         setEditLineText(line);
      else if (sel.count() == 1)
         setEditLineText(sel.first());
      else {
         setEditLineText("");
      }
      isInSlot = false;
   }
   enableMenuEntries ();
}

void PhraseList::setEditLineText(const TQString &s) {
   lineEdit->end(false);
   while (!(lineEdit->text().isNull() || lineEdit->text().isEmpty()))
      lineEdit->backspace();
   lineEdit->insert(s);
}

void PhraseList::keyPressEvent (TQKeyEvent *e) {
   if (e->key() == TQt::Key_Up) {
      bool selected = false;
      for (TQListBoxItem *item = listBox->firstItem(); item != 0; item = item->next()) {
         if (item->isSelected()) {
            selected = true;
         }
      }
      
      if (!selected) {
         listBox->setCurrentItem (listBox->count() - 1);
         listBox->setSelected (listBox->count() - 1, true);
         listBox->ensureCurrentVisible ();
      }
      else {
         int curr = listBox->currentItem();
         
         if (curr == -1) {
            isInSlot = true;
            listBox->clearSelection();
            isInSlot = false;
            curr = listBox->count() - 1;
            listBox->setCurrentItem (curr);
            listBox->setSelected (curr, true);
            listBox->ensureCurrentVisible ();
         }
         else if (curr != 0) {
            isInSlot = true;
            listBox->clearSelection();
            isInSlot = false;
            listBox->setCurrentItem (curr - 1);
            listBox->setSelected (curr - 1, true);
            listBox->ensureCurrentVisible ();
         }
      }
      
      e->accept();
   }
   else if (e->key() == TQt::Key_Down) {
      bool selected = false;
      for (TQListBoxItem *item = listBox->firstItem(); item != 0; item = item->next()) {
         if (item->isSelected()) {
            selected = true;
         }
      }
      
      if (selected) {
         int curr = listBox->currentItem();
      
         if (curr == (int)listBox->count() - 1) {
            listBox->clearSelection();
         }
         else if (curr != -1) {
            isInSlot = true;
            listBox->clearSelection();
            isInSlot = false;
            listBox->setCurrentItem (curr + 1);
            listBox->setSelected (curr + 1, true);
            listBox->ensureCurrentVisible ();
         }
      }
      e->accept();
   }
   else if ((e->state() & TQt::KeyButtonMask) == TQt::ControlButton) {
      if (e->key() == TQt::Key_C) {
         copy();
         e->accept();
      }
      else if (e->key() == TQt::Key_X) {
         cut();
         e->accept();
      }
   }
   else
      e->ignore();
}

void PhraseList::save () {
   // We want to save a history of spoken sentences here. However, as
   // the class PhraseBook does already provide a method for saving
   // phrase books in both the phrase book format and plain text file
   // format we use that method here.
   
   PhraseBook book;
   for (TQListBoxItem *item = listBox->firstItem(); item != 0; item = item->next()) {
      book += PhraseBookEntry(Phrase(item->text()));
   }

   KURL url;
   if (book.save (this, i18n("Save As"), url, false) == -1)
      KMessageBox::sorry(this,i18n("There was an error saving file\n%1").arg( url.url() ));
}

void PhraseList::open () {
   KURL url=KFileDialog::getOpenURL(TQString(),
        i18n("*|All Files\n*.phrasebook|Phrase Books (*.phrasebook)\n*.txt|Plain Text Files (*.txt)"), this, i18n("Open File as History"));

   if(!url.isEmpty())
      open (url);
}

void PhraseList::open (KURL url) {
   // We want to open a history of spoken sentences here. However, as
   // the class PhraseBook does already provide a method for opening
   // both phrase books and plain text files we use that method here.

   PhraseBook book;
   if (book.open (url)) {
      // convert PhraseBookEntryList -> TQStringList
      TQStringList list = book.toStringList();
      listBox->clear();
      TQStringList::iterator it;
      for (it = list.begin(); it != list.end(); ++it)
         insertIntoPhraseList (*it, false);
   }
   else
      KMessageBox::sorry(this,i18n("There was an error loading file\n%1").arg( url.url() ));
}

#include "phraselist.moc"
