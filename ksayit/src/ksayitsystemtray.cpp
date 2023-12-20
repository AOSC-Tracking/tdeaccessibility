/***************************************************************************
                          ksayitsystemtray.cpp  -  description
                             -------------------
    begin                : Die Sep 2 2003
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
#include <iostream>

// QT includes
#include <tqtranslator.h>

// KDE includes
#include <tdeglobal.h>
#include <tdelocale.h>

// App specific includes
#include "ksayitsystemtray.h"

KSayItSystemTray::KSayItSystemTray(TQWidget *parent, const char *name)
 : KSystemTray(parent,name)
{
  initActions();
  changeState( StateCLIPEMPTY::Instance() );
}

KSayItSystemTray::~KSayItSystemTray()
{
  delete StateWAIT::Instance();
  delete StateSAY::Instance();
  delete StateCLIPEMPTY::Instance();
}

void KSayItSystemTray::initActions()
{
  // Context-menu
  menu = this->contextMenu();
  help = new KHelpMenu(this, kapp->aboutData(), false, actionCollection());
  // Standard actions
  settings = KStdAction::preferences(this, TQT_SLOT(slotPreferences()), actionCollection());
  help_about = KStdAction::aboutApp(help, TQT_SLOT(aboutApplication()), actionCollection());
  help_kde = KStdAction::aboutKDE(help, TQT_SLOT(aboutKDE()), actionCollection());

  // User defined actions
  say = new TDEAction(i18n("Say"),
              "media-playback-start",
              0,
              this, TQT_SLOT (slotSayActivated()),
              actionCollection(),
              "say_it");
  
  shutup = new TDEAction(i18n("Shut Up"),
              "media-playback-stop",
              0,
              this, TQT_SLOT (slotStopActivated()),
              actionCollection(),
              "shut_up");
  
  pause = new TDEAction (i18n("Pause"),
              "media-playback-pause",
              0,
              this, TQT_SLOT (slotPauseActivated()),
              actionCollection(),
              "pause");

  next_sentence = new TDEAction (i18n("Next Sentence"),
              "2rightarrow",
              0,
              this, TQT_SLOT (slotNextSentenceActivated()),
              actionCollection(),
              "next_sentence");


  prev_sentence = new TDEAction (i18n("Previous Sentence"),
              "2leftarrow",
              0,
              this, TQT_SLOT(slotPrevSentenceActivated()),
              actionCollection(),
              "prev_sentence");

  // Actions -> Context-menu
  settings->plug(menu); menu->insertSeparator();
  say->plug(menu);
  shutup->plug(menu);
  pause->plug(menu);
  next_sentence->plug(menu);
  prev_sentence->plug(menu); menu->insertSeparator();
  help_about->plug(menu);
  help_kde->plug(menu);

  // default enables/disables
  say          ->setEnabled(false);
  shutup       ->setEnabled(false);
  pause        ->setEnabled(false);
  next_sentence->setEnabled(false);
  prev_sentence->setEnabled(false);
}


void KSayItSystemTray::slotPreferences()
{
  // call Preferences
  emit signalCallPreferences();
}

void KSayItSystemTray::mousePressEvent(TQMouseEvent *me)
{
  _state->mousePressEvent(this, me);
}

void KSayItSystemTray::mouseReleaseEvent(TQMouseEvent *me)
{
  _state->mouseReleaseEvent(this, me);
}

void KSayItSystemTray::slotSayActivated()
{
  // start to say content of clipboard
  emit signalSayActivated();
}

void KSayItSystemTray::slotStopActivated()
{
  // stop saying
  emit signalShutUpActivated();
}

void KSayItSystemTray::slotPauseActivated()
{
  // pause saying
  emit signalPauseActivated();
}

void KSayItSystemTray::slotNextSentenceActivated()
{
  // next sentence
  emit signalNextActivated();
} 
  
void KSayItSystemTray::slotPrevSentenceActivated()
{
  // previous sentence
  emit signalPrevActivated();
}


void KSayItSystemTray::changeState(State *state)
{
  _state = state;
  _state->setContext(this);
}

void KSayItSystemTray::setActions(bool sayEnabled, bool pauseEnabled, bool shutupEnabled,
      bool nextEnabled, bool prevEnabled)
{
  say          ->setEnabled(sayEnabled);
  pause        ->setEnabled(pauseEnabled);
  shutup       ->setEnabled(shutupEnabled);
  next_sentence->setEnabled(nextEnabled);
  prev_sentence->setEnabled(prevEnabled);
}


void KSayItSystemTray::normalMousePressEvent(TQMouseEvent *e)
{
  KSystemTray::mousePressEvent(e);
}

void KSayItSystemTray::normalMouseReleaseEvent(TQMouseEvent *e)
{
  KSystemTray::mouseReleaseEvent(e);
}

void KSayItSystemTray::sayClipboard()
{
    emit signalSayClipboard();
}

  


////////////////////////////////////////////
State::State(){
}
State::~State(){
}

void State::mousePressEvent(KSayItSystemTray *caller, TQMouseEvent *e)
{
  // reimplemented by subclasses
}

void State::mouseReleaseEvent(KSayItSystemTray *caller, TQMouseEvent *e)
{
  // reimplemented by subclasses
}

void State::setContext(KSayItSystemTray *caller)
{
  // reimplemented by subclasses
}

void State::changeState(KSayItSystemTray *caller, State *state)
{
  caller->changeState(state);  
}

void State::say(KSayItSystemTray *caller)
{
  caller->sayClipboard();
}

void State::mousePressEventCall(KSayItSystemTray *caller, TQMouseEvent *e)
{
  caller->normalMousePressEvent(e);
}

void State::mouseReleaseEventCall(KSayItSystemTray *caller, TQMouseEvent *e)
{
  caller->normalMouseReleaseEvent(e);
}


////////////////////////////////////////////
StateWAIT::StateWAIT(){
  m_traypixmap = TDEGlobal::iconLoader()->loadIcon("ksayit", TDEIcon::Toolbar);
}
StateWAIT::~StateWAIT(){
}
StateWAIT* StateWAIT::_instance = 0;

StateWAIT* StateWAIT::Instance()
{
  if (_instance == 0){
    _instance = new StateWAIT();
  }
  return _instance;
}

void StateWAIT::setContext(KSayItSystemTray *caller)
{
  caller->setPixmap( m_traypixmap );
}

void StateWAIT::mousePressEvent(KSayItSystemTray *caller, TQMouseEvent *e)
{
  if (e->button()==TQt::LeftButton){ // left Mouse-button pressed
    TQWidget::mousePressEvent(e); // do nothing (see mouseReleaseEvent)
  } else {
    mousePressEventCall(caller, e); // normal mouse-handling
  }
}

void StateWAIT::mouseReleaseEvent(KSayItSystemTray *caller, TQMouseEvent *e)
{
  if (e->button()==TQt::LeftButton){ // left Mouse-button released
    say(caller);
  } else {
    mouseReleaseEventCall(caller, e); // normal mouse-handling
  }
}



////////////////////////////////////////////
StateSAY::StateSAY(){
  m_traypixmap = TDEGlobal::iconLoader()->loadIcon("ksayit_talking", TDEIcon::Toolbar);
}
StateSAY::~StateSAY(){
}
StateSAY* StateSAY::_instance = 0;

StateSAY* StateSAY::Instance()
{
  if (_instance == 0){
    _instance = new StateSAY();
  }
  return _instance;
}

void StateSAY::setContext(KSayItSystemTray *caller)
{
  caller->setPixmap( m_traypixmap );
}

void StateSAY::mousePressEvent(KSayItSystemTray *caller, TQMouseEvent *e)
{
  if (e->button()==TQt::LeftButton){ // left Mouse-button pressed
    TQWidget::mousePressEvent(e); // do nothing (see mouseReleaseEvent)
  } else {
    mousePressEventCall(caller, e); // normal mouse-handling
  }
}

void StateSAY::mouseReleaseEvent(KSayItSystemTray *caller, TQMouseEvent *e)
{
  if (e->button()==TQt::LeftButton){ // left Mouse-button released
    TQWidget::mouseReleaseEvent(e); // do nothing (see mouseReleaseEvent)
  } else {
    mouseReleaseEventCall(caller, e); // normal mouse-handling
  }
}




////////////////////////////////////////////
StateCLIPEMPTY::StateCLIPEMPTY(){
  m_traypixmap = TDEGlobal::iconLoader()->loadIcon("ksayit_clipempty", TDEIcon::Toolbar);
}
StateCLIPEMPTY::~StateCLIPEMPTY(){
}
StateCLIPEMPTY* StateCLIPEMPTY::_instance = 0;

StateCLIPEMPTY* StateCLIPEMPTY::Instance()
{
  if (_instance == 0){
    _instance = new StateCLIPEMPTY();
  }
  return _instance;
}

void StateCLIPEMPTY::setContext(KSayItSystemTray *caller)
{
  caller->setPixmap( m_traypixmap );
}

void StateCLIPEMPTY::mousePressEvent(KSayItSystemTray *caller, TQMouseEvent *e)
{
  if (e->button()==TQt::LeftButton){ // left Mouse-button pressed
    TQWidget::mousePressEvent(e); // do nothing (see mouseReleaseEvent)
  } else {
    mousePressEventCall(caller, e); // normal mouse-handling
  }
}

void StateCLIPEMPTY::mouseReleaseEvent(KSayItSystemTray *caller, TQMouseEvent *e)
{
  if (e->button()==TQt::LeftButton){ // left Mouse-button released
    TQWidget::mouseReleaseEvent(e); // do nothing (see mouseReleaseEvent)
  } else {
    mouseReleaseEventCall(caller, e); // normal mouse-handling
  }
}


#include "ksayitsystemtray.moc"
