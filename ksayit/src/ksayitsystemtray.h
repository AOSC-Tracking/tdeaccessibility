/***************************************************************************
                          ksayitsystemtray.h  -  description
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

#ifndef KSAYITSYSTEMTRAY_H
#define KSAYITSYSTEMTRAY_H

// QT includes
#include <tqwidget.h>
#include <tqpixmap.h>

// KDE includes
#include <tdeapplication.h>
#include <ksystemtray.h>
#include <tdepopupmenu.h>
#include <tdeaction.h>
#include <tdemenubar.h>
#include <khelpmenu.h>
#include <kiconloader.h>

// forward declarations
class State;

/**
  *@author Robert Vogl
  */
class KSayItSystemTray : public KSystemTray  {
   TQ_OBJECT
  

  friend class State;

public: 
    KSayItSystemTray(TQWidget *parent=0, const char *name=0);
    ~KSayItSystemTray();

signals:
  /** Signal emitted if configuration has been changed
   */
  void signalCallPreferences();

  /** Signal emitted if the Tray icon was clicked.
   */
  void signalSayClipboard();
  
  /** Signal emitted by the control actions
   */
  void signalSayActivated();

  void signalShutUpActivated();
    
  void signalPauseActivated();
  
  void signalNextActivated();
  
  void signalPrevActivated();
  
public slots:
  /** opens the Preferences-Dialog
   */
  void slotPreferences();

  /** activated if "say" was clicked
   */
  void slotSayActivated();

  /** activated if "shut up" was clicked
   */
  void slotStopActivated();

  /** activated if "pause" was clicked
   */
  void slotPauseActivated();
  
  /** activated if "next sentence" was clicked
   */
  void slotNextSentenceActivated();
  
  /** activated if "previous sentence" was clicked
   */
  void slotPrevSentenceActivated();

public:  
  /** Changes the state of the statemachine
   */
  void changeState(State *state);

  /** Enables/Diasabled the actions in the menu
   */
  void setActions(bool sayEnabled, bool pauseEnabled, bool shutupEnabled,
      bool nextEnabled, bool prevEnabled);
  
protected: // Methods
  /** Reimplementation from base class
   */
  void mousePressEvent(TQMouseEvent *);

  /** Reimplementation from base class
   */  
  void mouseReleaseEvent(TQMouseEvent *);

  /** KSystemTray default mouse handling
   */
  void normalMousePressEvent(TQMouseEvent *e);

  /** KSystemTray default mouse handling
   */
  void normalMouseReleaseEvent(TQMouseEvent *e);

  /** Called if the Tray icon was clicked. 
   */
  void sayClipboard();
    
private: // Methods
  /** initializes the TDEActions of the application */
  void initActions();

public:
  TDEAction *say;
  TDEAction *shutup;
  TDEAction *pause;
  TDEAction *next_sentence;
  TDEAction *prev_sentence;

private:
  TDEPopupMenu *menu;
  KHelpMenu *help;
  TDEAction *settings;
  TDEAction *help_about;
  TDEAction *help_kde;
  State *_state;
  
};




class State : public KSystemTray {
  TQ_OBJECT
  
public:
  State();
  ~State();

  virtual void mousePressEvent(KSayItSystemTray *caller, TQMouseEvent *e);
  virtual void mouseReleaseEvent(KSayItSystemTray *caller, TQMouseEvent *e);
  virtual void setContext(KSayItSystemTray *caller);

protected:
  void changeState(KSayItSystemTray *caller, State *state);
  void say(KSayItSystemTray *caller);  
  void mousePressEventCall(KSayItSystemTray *caller, TQMouseEvent *e);
  void mouseReleaseEventCall(KSayItSystemTray *caller, TQMouseEvent *e);
};




class StateWAIT : public State {

public:
  static StateWAIT* Instance();
  ~StateWAIT();

protected:
  StateWAIT();
  void mousePressEvent(KSayItSystemTray *caller, TQMouseEvent *e);
  void mouseReleaseEvent(KSayItSystemTray *caller, TQMouseEvent *e);
  void setContext(KSayItSystemTray *caller);

private:
  static StateWAIT *_instance;
  TQPixmap m_traypixmap;
};




class StateSAY : public State {

public:
  static StateSAY* Instance();
  ~StateSAY();

protected:
  StateSAY();
  void mousePressEvent(KSayItSystemTray *caller, TQMouseEvent *e);
  void mouseReleaseEvent(KSayItSystemTray *caller, TQMouseEvent *e);
  void setContext(KSayItSystemTray *caller);

private:
  static StateSAY *_instance;
  TQPixmap m_traypixmap;
};




class StateCLIPEMPTY : public State {

public:
  static StateCLIPEMPTY* Instance();
  ~StateCLIPEMPTY();

protected:
  StateCLIPEMPTY();
  void mousePressEvent(KSayItSystemTray *caller, TQMouseEvent *e);
  void mouseReleaseEvent(KSayItSystemTray *caller, TQMouseEvent *e);
  void setContext(KSayItSystemTray *caller);

private:
  static StateCLIPEMPTY *_instance;
  TQPixmap m_traypixmap;
};


#endif
