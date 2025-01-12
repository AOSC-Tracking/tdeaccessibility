/***************************************************************************
                          wordcompletionwidget.cpp  -  description
                             -------------------
    begin                : Tue Apr 29 2003
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

#include <tqlayout.h>
#include <tqlabel.h>
#include <tqcheckbox.h>
#include <tqlineedit.h>

#include <tdelistview.h>
#include <klineedit.h>
#include <kurlrequester.h>
#include <tdelocale.h>
#include <tdeglobal.h>
#include <kstandarddirs.h>
#include <tdeconfig.h>
#include <ksimpleconfig.h>
#include <tdefiledialog.h>
#include <tdeio/netaccess.h>
#include <tdemessagebox.h>

#include "dictionarycreationwizard.h"
#include "wordcompletionwidget.h"
#include "wordcompletion.h"
#include "klanguagebutton.h"

class DictionaryListItem : public TDEListViewItem {
public:
   DictionaryListItem (TQListView *parent, TQString filename, TQString name, TQString language, TQString languageCode)
   : TDEListViewItem (parent, name) {
      setFilename (filename);
      setLanguage (language, languageCode);
   };
   DictionaryListItem (TQListView *parent, TQString filename, TQString name, TQString languageCode)
   : TDEListViewItem (parent, name) {
      setFilename (filename);
      setLanguage (languageCode);
   };
   DictionaryListItem (TQListView *parent, TQListViewItem *after, TQString filename, TQString name, TQString languageCode)
   : TDEListViewItem (parent, after, name) {
      setFilename (filename);
      setLanguage (languageCode);
   };
   ~DictionaryListItem () {
   };

   TQString filename() {
      return myFilename;
   }

   TQString languageCode() {
      return myLanguageCode;
   }

   void setFilename(TQString filename) {
      myFilename = filename;
   }

   void setLanguage (TQString languageCode) {
      TQString filename = TDEGlobal::dirs()->findResource("locale",
			languageCode + TQString::fromLatin1("/entry.desktop"));

      KSimpleConfig entry(filename);
      entry.setGroup(TQString::fromLatin1("KCM Locale"));
      TQString name = entry.readEntry(TQString::fromLatin1("Name"), i18n("without name"));
      setLanguage (name + " (" + languageCode + ")", languageCode);
   }

   void setLanguage (TQString name, TQString languageCode) {
      myLanguageCode = languageCode;
      setText (1, name);
   }

private:
   TQString myFilename;
   TQString myLanguageCode;
};

/***************************************************************************/

WordCompletionWidget::WordCompletionWidget(TQWidget *parent, const char *name) : WordCompletionUI (parent, name) {
    dictionaryList->setSorting (-1); // no sorted list

    // Connect the signals from hte KCMKTTSDWidget to this class
    connect (addButton, TQ_SIGNAL (clicked()), this, TQ_SLOT(addDictionary()) );
    connect (deleteButton, TQ_SIGNAL (clicked()), this, TQ_SLOT (deleteDictionary()) );
    connect (moveUpButton, TQ_SIGNAL (clicked()), this, TQ_SLOT (moveUp()) );
    connect (moveDownButton, TQ_SIGNAL (clicked()), this, TQ_SLOT (moveDown()) );
    connect (exportButton, TQ_SIGNAL (clicked()), this, TQ_SLOT (exportDictionary()) );

    connect (dictionaryList, TQ_SIGNAL (selectionChanged()), this, TQ_SLOT (selectionChanged()) );
    connect (dictionaryName, TQ_SIGNAL (textChanged (const TQString &)), this, TQ_SLOT (nameChanged (const TQString &)) );
    connect (languageButton, TQ_SIGNAL (activated (int)), this, TQ_SLOT (languageSelected(int)) );

    // Object for the KCMKTTSD configuration
    config = new TDEConfig("kmouthrc");

    // Load the configuration from the file
    load();
}

/**
 * Destructor
 */
WordCompletionWidget::~WordCompletionWidget() {
    delete config;
}

/***************************************************************************/

void WordCompletionWidget::load() {
   dictionaryList->clear();

   // Set the group general for the configuration of kttsd itself (no plug ins)
   TQStringList groups = config->groupList();
   DictionaryListItem *last = 0;
   for (TQStringList::Iterator it = groups.begin(); it != groups.end(); ++it)
      if ((*it).startsWith ("Dictionary ")) {
         config->setGroup(*it);
         TQString languageTag = config->readEntry("Language");
         last = new DictionaryListItem (dictionaryList, last,
                                        config->readEntry("Filename"),
                                        config->readEntry("Name"),
                                        languageTag);
         if (!languageButton->containsTag(languageTag))
            languageButton->insertLanguage(languageTag, i18n("without name"), TQString::fromLatin1("l10n/"), TQString());
      }

   // Clean up disc space
   for (TQStringList::Iterator it = newDictionaryFiles.begin(); it != newDictionaryFiles.end(); ++it) {
      TQString filename = TDEGlobal::dirs()->findResource ("appdata", *it);
      if (!filename.isEmpty() && !filename.isNull())
         TQFile::remove (filename);
   }
   newDictionaryFiles.clear();
}

void WordCompletionWidget::save() {
   TQStringList groups = config->groupList();
   for (TQStringList::Iterator it = groups.begin(); it != groups.end(); ++it)
      if ((*it).startsWith ("Dictionary "))
         config->deleteGroup (*it);

   int number = 0;
   TQListViewItemIterator it(dictionaryList);
   while (it.current()) {
      DictionaryListItem *item = dynamic_cast<DictionaryListItem*>(it.current());
      if (item != 0) {
         config->setGroup(TQString("Dictionary %1").arg(number));
         config->writeEntry ("Filename", item->filename());
         config->writeEntry ("Name",     item->text (0));
         config->writeEntry ("Language", item->languageCode());
         number++;
      }
      ++it;
   }
   config->sync();

   // Clean up disc space
   for (TQStringList::Iterator it = removedDictionaryFiles.begin(); it != removedDictionaryFiles.end(); ++it) {
      TQString filename = TDEGlobal::dirs()->findResource ("appdata", *it);
      if (!filename.isEmpty() && !filename.isNull())
         TQFile::remove (filename);
   }
   removedDictionaryFiles.clear();
}

/***************************************************************************/

void WordCompletionWidget::addDictionary() {
   TQStringList dictionaryNames;
   TQStringList dictionaryFiles;
   TQStringList dictionaryLanguages;
   TQListViewItemIterator it(dictionaryList);
   while (it.current()) {
      DictionaryListItem *item = dynamic_cast<DictionaryListItem*>(it.current());
      if (item != 0) {
         dictionaryNames += item->text (0);
         dictionaryFiles += item->filename();
         dictionaryLanguages += item->languageCode();
      }
      ++it;
   }
   DictionaryCreationWizard *wizard = new DictionaryCreationWizard (this, "Dictionary creation wizard", dictionaryNames, dictionaryFiles, dictionaryLanguages);
   if (wizard->exec() == TQDialog::Accepted) {
      TQString filename = wizard->createDictionary();
      newDictionaryFiles += filename;
      TQString languageTag = wizard->language();
      if (!languageButton->containsTag(languageTag)) {
         languageButton->insertLanguage(languageTag, i18n("without name"), TQString::fromLatin1("l10n/"), TQString());
      }
      TDEListViewItem *item = new DictionaryListItem (dictionaryList,
                      filename, wizard->name(), languageTag);
      dictionaryList->setSelected(item, true);
   }
   delete wizard;
}

void WordCompletionWidget::deleteDictionary() {
   DictionaryListItem *item = dynamic_cast<DictionaryListItem*>(dictionaryList->selectedItem ());

   if (item != 0) {
      removedDictionaryFiles += item->filename();
      delete item;
   }
}

void WordCompletionWidget::moveUp() {
   TQListViewItem *item = dictionaryList->selectedItem ();

   if (item != 0) {
      TQListViewItem *above = item->itemAbove();

      if (above != 0) {
         above->moveItem (item);
      }
   }
}

void WordCompletionWidget::moveDown() {
   TQListViewItem *item = dictionaryList->selectedItem ();

   if (item != 0) {
      TQListViewItem *next = item->itemBelow();

      if (next != 0) {
         item->moveItem (next);
      }
   }
}

void WordCompletionWidget::exportDictionary() {
   DictionaryListItem *item = dynamic_cast<DictionaryListItem*>(dictionaryList->selectedItem ());

   if (item != 0) {
      KURL url = KFileDialog::getSaveURL(TQString(), TQString(), this, i18n("Export Dictionary"));
      if (url.isEmpty() || !url.isValid())
         return;

      if (TDEIO::NetAccess::exists(url, false, this)) {
         if (KMessageBox::warningContinueCancel(0,TQString("<qt>%1</qt>").arg(i18n("The file %1 already exists. "
                                                          "Do you want to overwrite it?").arg(url.url())),i18n("File Exists"),i18n("&Overwrite"))==KMessageBox::Cancel) {
            return;
         }
      }
      KURL src;
      src.setPath( TDEGlobal::dirs()->findResource ("appdata", item->filename()) );
      TDEIO::NetAccess::copy (src, url, this);
   }
}

void WordCompletionWidget::selectionChanged() {
   DictionaryListItem *item = dynamic_cast<DictionaryListItem*>(dictionaryList->selectedItem ());

   if (item != 0) {
      deleteButton->setEnabled(true);
      moveUpButton->setEnabled(true);
      moveDownButton->setEnabled(true);
      exportButton->setEnabled(true);
      selectedDictionaryDetails->setEnabled(true);
      languageLabel->setEnabled(true);
      dictionaryNameLabel->setEnabled(true);
      dictionaryName->setEnabled(true);
      languageButton->setEnabled(true);

      dictionaryName->setText(item->text(0));
      languageButton->setCurrentItem(item->languageCode());
   }
   else {
      deleteButton->setEnabled(false);
      moveUpButton->setEnabled(false);
      moveDownButton->setEnabled(false);
      exportButton->setEnabled(false);
      selectedDictionaryDetails->setEnabled(false);
      languageLabel->setEnabled(false);
      dictionaryNameLabel->setEnabled(false);
      dictionaryName->setEnabled(false);
      languageButton->setEnabled(false);

      dictionaryName->setText("");
      languageButton->setText("");
   }
}

void WordCompletionWidget::nameChanged (const TQString &text) {
   TQListViewItem *item = dictionaryList->selectedItem ();

   if (item != 0) {
      TQString old = item->text(0);

      if (old != text) {
         item->setText(0, text);
         emit changed(true);
      }
   }
}

void WordCompletionWidget::languageSelected (int) {
   DictionaryListItem *item = dynamic_cast<DictionaryListItem*>(dictionaryList->selectedItem ());

   if (item != 0) {
      TQString old = item->text(1);
      TQString text = languageButton->currentTag();

      if (old != text) {
         item->setLanguage(languageButton->text(), text);
         emit changed(true);
      }
   }
}

#include "wordcompletionwidget.moc"
