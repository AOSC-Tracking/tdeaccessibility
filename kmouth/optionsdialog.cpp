/***************************************************************************
                          optionsdialog.cpp  -  description
                             -------------------
    begin                : Don Nov 21 2002
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
#include <tqwhatsthis.h>
#include <tqgrid.h>
#include <tqtabwidget.h>
#include <tqpixmap.h>
#include <tqfile.h>

#include <kcombobox.h>
#include <tdelocale.h>
#include <tdeconfig.h>
#include <tdeglobal.h>
#include <kiconloader.h>
#include <tdecmodule.h>
#include <klibloader.h>
#include <tdeparts/componentfactory.h>

#include "optionsdialog.h"
#include "wordcompletion/wordcompletionwidget.h"

#include "texttospeechconfigurationwidget.h"
#include "speech.h"

PreferencesWidget::PreferencesWidget (TQWidget *parent, const char *name)
   : PreferencesUI (parent, name)
{
   speakCombo->setCurrentItem (1);
   speak = false;

   closeCombo->setCurrentItem (2);
   save = 2;
}

PreferencesWidget::~PreferencesWidget() {
}

void PreferencesWidget::cancel() {
   if (speak)
      speakCombo->setCurrentItem (0);
   else
      speakCombo->setCurrentItem (1);
   closeCombo->setCurrentItem (save);
}

void PreferencesWidget::ok() {
   speak = speakCombo->currentItem () == 0;
   save  = closeCombo->currentItem ();
}

void PreferencesWidget::readOptions (TDEConfig *config) {
   config->setGroup("Preferences");
   if (config->hasKey("AutomaticSpeak"))
      if (config->readEntry ("AutomaticSpeak") == "Yes")
         speak = true;
      else
         speak = false;
   else
      speak = false;

   config->setGroup("Notification Messages");
   if (config->hasKey("AutomaticSave"))
      if (config->readEntry ("AutomaticSave") == "Yes")
         save = 0;
      else
         save = 1;
   else
      save = 2;

   if (speak)
      speakCombo->setCurrentItem (0);
   else
      speakCombo->setCurrentItem (1);
   closeCombo->setCurrentItem (save);
}

void PreferencesWidget::saveOptions (TDEConfig *config) {
   config->setGroup("Preferences");
   if (speak)
      config->writeEntry ("AutomaticSpeak", "Yes");
   else
      config->writeEntry ("AutomaticSpeak", "No");

   config->setGroup("Notification Messages");
   if (save == 0)
      config->writeEntry ("AutomaticSave", "Yes");
   else if (save == 1)
      config->writeEntry ("AutomaticSave", "No");
   else
      config->deleteEntry ("AutomaticSave");
}

bool PreferencesWidget::isSpeakImmediately () {
   return speak;
}

/***************************************************************************/

OptionsDialog::OptionsDialog (TQWidget *parent)
   : KDialogBase(IconList, i18n("Configuration"), Ok|Apply|Cancel|Help, Ok,
                  parent, "configuration", false, true)
{
   setHelp ("config-dialog");

   TQPixmap iconGeneral = TDEGlobal::iconLoader()->loadIcon("configure", TDEIcon::NoGroup, TDEIcon::SizeMedium);
   TQGrid *pageGeneral = addGridPage (1, TQt::Horizontal, i18n("General Options"), TQString(), iconGeneral);
   
   tabCtl = new TQTabWidget (pageGeneral, "general");

   behaviourWidget = new PreferencesWidget (tabCtl, "prefPage");
   behaviourWidget->layout()->setMargin(KDialog::marginHint());
   tabCtl->addTab (behaviourWidget, i18n("&Preferences"));
   
   commandWidget = new TextToSpeechConfigurationWidget (tabCtl, "ttsTab");
   commandWidget->layout()->setMargin(KDialog::marginHint());
   tabCtl->addTab (commandWidget, i18n("&Text-to-Speech"));
   
   TQPixmap iconCompletion = TDEGlobal::iconLoader()->loadIcon("input-keyboard", TDEIcon::NoGroup, TDEIcon::SizeMedium);
   TQGrid *pageCompletion = addGridPage (1, TQt::Horizontal, i18n("Word Completion"), TQString(), iconCompletion);
   completionWidget = new WordCompletionWidget(pageCompletion, "Word Completion widget");

   kttsd = loadKttsd();
   if (kttsd != 0) {
      TQPixmap iconKttsd = TDEGlobal::iconLoader()->loadIcon("multimedia", TDEIcon::NoGroup, TDEIcon::SizeMedium);
      TQGrid *pageKttsd = addGridPage (1, TQt::Horizontal, i18n("KTTSD Speech Service"),
                                      i18n("TDE Text-to-Speech Daemon Configuration"), iconKttsd);
      
      kttsd->reparent(pageKttsd,0,TQPoint(0,0),true);
   }
}

OptionsDialog::~OptionsDialog() {
   unloadKttsd();
}

void OptionsDialog::slotCancel() {
   KDialogBase::slotCancel();
   commandWidget->cancel();
   behaviourWidget->cancel();
   completionWidget->load();
   if (kttsd != 0)
      kttsd->load ();
}

void OptionsDialog::slotOk() {
   KDialogBase::slotOk();
   commandWidget->ok();
   behaviourWidget->ok();
   completionWidget->save();
   emit configurationChanged();
   if (kttsd != 0)
      kttsd->save ();
   
}

void OptionsDialog::slotApply() {
   KDialogBase::slotApply();
   commandWidget->ok();
   behaviourWidget->ok();
   completionWidget->save();
   emit configurationChanged();
   if (kttsd != 0)
      kttsd->save ();
}

TextToSpeechSystem *OptionsDialog::getTTSSystem() const {
   return commandWidget->getTTSSystem();
}

void OptionsDialog::readOptions (TDEConfig *config) {
   commandWidget->readOptions (config, "TTS System");
   behaviourWidget->readOptions (config);
}

void OptionsDialog::saveOptions (TDEConfig *config) {
   commandWidget->saveOptions (config, "TTS System");
   behaviourWidget->saveOptions (config);
   config->sync();
}

bool OptionsDialog::isSpeakImmediately () {
   return behaviourWidget->isSpeakImmediately ();
}

TDECModule *OptionsDialog::loadKttsd () {
   KLibLoader *loader = KLibLoader::self();

   TQString libname = "kcm_kttsd";
   KLibrary *lib = loader->library(TQFile::encodeName(libname));

   if (lib == 0) {
      libname = "libkcm_kttsd";
      lib = loader->library(TQFile::encodeName("libkcm_kttsd"));
   }

   if (lib != 0) {
      TQString initSym("init_");
      initSym += libname;

      if (lib->hasSymbol(TQFile::encodeName(initSym))) {
         // Reuse "lib" instead of letting createInstanceFromLibrary recreate it
         KLibFactory *factory = lib->factory();
         if (factory != 0) {
            TDECModule *module = KParts::ComponentFactory::createInstanceFromFactory<TDECModule> (factory);
            if (module)
                return module;
         }
      }

      lib->unload();
   }
   return 0;
}

void OptionsDialog::unloadKttsd () {
  KLibLoader *loader = KLibLoader::self();
  loader->unloadLibrary(TQFile::encodeName("libkcm_kttsd"));
  loader->unloadLibrary(TQFile::encodeName("kcm_kttsd"));
}

#include "optionsdialog.moc"
