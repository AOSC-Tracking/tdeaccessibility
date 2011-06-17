/***************************************************************************
                          kmag.h  -  description
                             -------------------
    begin                : Mon Feb 12 23:45:41 EST 2001
    copyright            : (C) 2001 by Sarang Lakare
    email                : sarang@users.sourceforge.net
    copyright            : (C) 2003-2004 by Olaf Schmidt
    email                : ojschmidt@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef KMAG_H
#define KMAG_H


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "kmagzoomview.h"

#include <vector>
// include files for TQt
#include <tqstringlist.h>

// include files for KDE
#include <kapplication.h>
#include <kmainwindow.h>
#include <kdockwidget.h>
#include <kaction.h>
#include <kaccel.h>
#include <knuminput.h>

/**
  * The base class for Kmag application windows. It sets up the main
  * window and reads the config file as well as providing a menubar, toolbar
  * and statusbar. An instance of KmagView creates your center view, which is connected
  * to the window's Doc object.
  * KmagApp reimplements the methods that KMainWindow provides for main window handling and supports
  * full session management as well as using KActions.
  * @see KMainWindow
  * @see KApplication
  * @see KConfig
  *
  * @author Source Framework Automatically Generated by KDevelop, (c) The KDevelop Team.
  * @version KDevelop version 1.2 code generation
  */
class KmagApp : public KMainWindow
{
  Q_OBJECT
  TQ_OBJECT

  public:
    /**
     * Construtor of KmagApp, calls all init functions to create the application.
     */
    KmagApp(TQWidget* tqparent=0, const char* name=0);

    /// Default destructor
    ~KmagApp();



  protected:
    /** save general Options like all bar positions and status as well as the tqgeometry and the recent file list to the configuration
     * file
     */
    void saveOptions();
    /** read general Options again and initialize all variables like the recent file list
     */
    void readOptions();
    /** initializes the KActions of the application */
    void initActions();

    /** creates the centerwidget of the KTMainWindow instance and sets it as the view
     */
    void initView();

    /// Initialize all connections
    void initConnections();

    virtual bool queryClose();

    /** queryExit is called by KTMainWindow when the last window of the application is going to be closed during the closeEvent().
     * Against the default implementation that just returns true, this calls saveOptions() to save the settings of the last window's
     * properties.
     * @see KTMainWindow#queryExit
     * @see KTMainWindow#closeEvent
     */
    virtual bool queryExit();

  public slots:
    /** open a new application window by creating a new instance of KmagApp */
    void slotFileNewWindow();

    /** print the actual file */
    void slotFilePrint();
    
    void slotFileQuit();

    /** put the marked text/object into the clipboard
     */
    void copyToClipBoard();

    /// Toggle the refreshing of the window
    void slotToggleRefresh();

    void slotModeFollowMouse();
    void slotModeSelWin();
    void slotModeEdgeTop();
    void slotModeEdgeLeft();
    void slotModeEdgeRight();
    void slotModeEdgeBottom();

    /// Zooms in
    void zoomIn();

    /// Zooms out
    void zoomOut();

    /// Save the zoomed image
    void saveZoomPixmap();

    /// Sets the zoom index to index
    void setZoomIndex(int index);

    /// Sets the invertation mode
    void slotToggleInvert();

    /// Sets the rotation index to index
    void setRotationIndex(int index);

    /// Sets the fps index to index
    void setFPSIndex(int index);

    /// Shows/hides the mouse cursor
    void showMouseCursor(bool show);

    void slotShowMenu();
    void slotShowMainToolBar();
    void slotShowViewToolBar();
    void slotShowSettingsToolBar();

    void slotToggleHideCursor();

    /// Opens shortcut key configuration dialogue
    void slotConfKeys();

    /// Called when toolbar config is updated
    void slotNewToolbarConfig();

    /// Called when "configure toolbar" is clicked
    void slotEditToolbars();

    void contextMenu(TQPoint pos);

  signals:
    /// This signal is raised whenever the index into the zoom array is changed
    void updateZoomIndex(int);

    /// This signal is raised whenever the zoom value changes
    void updateZoomValue(float);

    /// This signal is raised whenever the index into the rotation array is changed
    void updateRotationIndex(int);

    /// This signal is raised whenever the rotation value changes
    void updateRotationValue(int);

    /// This signal is raised whenever the index into the fps array is changed
    void updateFPSIndex(int);

    /// This signal is raised whenever the fps value changes
    void updateFPSValue(float);

  private:

    void setEdgeMode (KToggleAction *mode);
    void unsetEdgeMode (KToggleAction *mode);

    int edgesize;

    /// the configuration object of the application
    KConfig *config;

    // KAction pointers to enable/disable actions
    KAction *fileNewWindow, *m_pSnapshot, *m_pCopy, *m_keyConf, *m_toolConf;
    KAction *m_pPrint;
    KAction *m_pZoomIn;
    KAction *m_pZoomOut;
    KAction *m_pQuit;
    KAction *refreshSwitch;
    KToggleAction *m_pInvert, *m_pShowMenu, *m_pShowMainToolBar, *m_pShowViewToolBar, *m_pShowSettingsToolBar;
    KSelectAction *m_pZoomBox, *m_pRotationBox, *m_pFPSBox;

    /// zoom slider
    KIntNumInput *m_zoomSlider;

    /// Current index into the zoomArray
    unsigned int m_zoomIndex;

    /// Current index into the rotationArray
    unsigned int m_rotationIndex;

    /// Current index into the fpsArray
    unsigned int m_fpsIndex;

    TQStringList zoomArrayString;
    std::vector<float> zoomArray;

    TQStringList rotationArrayString;
    std::vector<int> rotationArray;

    TQStringList fpsArrayString;
    std::vector<float> fpsArray;

  KMagZoomView* m_zoomView;
  KToggleAction *m_hideCursor;
  KRadioAction *m_modeFollowMouse, *m_modeSelWin;
  KRadioAction *m_modeEdgeTop, *m_modeEdgeLeft, *m_modeEdgeRight, *m_modeEdgeBottom;

  /// Stores the non-zero cursor type to be used
  unsigned int m_mouseCursorType;

  unsigned int m_defaultMouseCursorType;
};


#endif // KMAG_H
