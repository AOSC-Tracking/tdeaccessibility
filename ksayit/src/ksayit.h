/***************************************************************************
                          ksayit.h  -  description
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

#ifndef KSAYIT_H
#define KSAYIT_H
 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

using namespace std;

// include files for TQt
#include <tqstringlist.h>
#include <tqclipboard.h>

// include files for KDE 
#include <tdeapplication.h>
#include <tdemainwindow.h>
#include <tdeaccel.h>
#include <tdeaction.h>
#include <tdeactionclasses.h>
#include <kurl.h>
#include <dcopobject.h>
#include <kbookmarkmanager.h>
#include <kbookmarkmenu.h>

// App specific includes

// appended to $HOME
#define KSAYIT_BOOKMARKXMLFILE "/.ksayitbookmarks.xml"

// forward declaration of the KSayIt classes
class KSayItViewImpl;
class DocTreeViewImpl;
class KSayItSystemTray;
class FXPluginHandler;
class EffectStack;
class KTTSDLib;
class KSayItBookmarkHandler;

/**
  * The base class for KSayIt application windows. It sets up the main
  * window and reads the config file as well as providing a menubar, toolbar
  * and statusbar. An instance of KSayItView creates the center view.
  * KSayItApp reimplements the methods that TDEMainWindow provides for main window handling and supports
  * full session management as well as using TDEActions.
  * @see TDEMainWindow
  * @see TDEApplication
  * @see TDEConfig
  *
  * @author Robert Vogl
  */
class KSayItApp : public TDEMainWindow, public DCOPObject
{
  Q_OBJECT
//  
  K_DCOP

public:
    /** construtor of KSayItApp, calls all init functions to create the application.
     */
    KSayItApp(TQWidget* parent=0, const char* name=0, WFlags f=0,
        const TQCString &objID=0);
    ~KSayItApp();

k_dcop:
    /** DCOP interface for say operation
    \param msg The text to speak.
     */
    ASYNC dcopSayText(TQString msg);

    /** DCOP interface for stop operation
     */
    ASYNC dcopStop();
    
    /** DCOP interface to say DocBook file
     */
    ASYNC dcopSayXmlFile(KURL url);
     

signals:
    /** emitted when the Configuration was modified
     */
    void signalConfigChanged();
  
private slots:
    /** Puts the given text on the Job-Stack of KTTSD.
     * \param text The text to say.
     */
    void slotSetText(const TQString &text);
    
    /** Called from the ListView widget after one or many
     * 'setText'-Calls. Starts to say the text.
     */
    void slotSayNode(); 
    
    /** Called when a node in the treeview was clicked and
     * new content in the edit window must be shown.
     * \str The new content of the EditView.
     */    
    void slotTreeViewChanged(const TQString &str);
    
    /** save general Options like all bar positions and status as well as the geometry to
     * the configuration file.
     */
    void slotSaveOptions();
    
    /** closes the windows , then quits the application.
     */
    void slotFileQuit();

    /** changes the statusbar contents for the standard label permanently, used to indicate
     * current actions.
     * @param text the text that is displayed in the statusbar
     */
    void slotStatusMsg(const TQString &text);

    /** opens the Preferences-Dialog.
     */
    void slotPreferences();

    /** activated if "say" from tray was clicked.
     */
    void slotSayClipboard();

    /** activated if "say" from app-window was clicked.
     */
    void slotSayText();

    /** activated if "shut up" was clicked.
     */
    void slotStopActivated();

    /** activated if "pause" was clicked.
     */
    void slotPauseActivated();
    
    /** activated if "next sentence" was clicked.
     */
    void slotNextSentenceActivated();
    
    /** activated if "previous sentence" was clicked.
     */
    void slotPrevSentenceActivated();
    
    /** called if text processing was finished.
     */
    void slotFinished();

    /** called to enable the copy/cut action, if text is selected in the TextEdit-View
     \param enable Enable actions if true, disable actions if false.
     */
    void slotEnableCopyCut(bool enable);

    /** sets the Caption of the MainWindow */
    void slotSetCaption(const TQString &caption);

    /** called when the Text was changed.
     * \param empty True if the TextEdit view is empty.
     */
    void slotTextChanged(bool empty);
    
    /** enabled/disables the textedit window;
     * \param enable true=enabled, false=disabled
     */
    void slotEnableTextedit(bool enable);
    
    /** toggles the statusbar
     */
    void slotToggleStatusBar();

    /** Called when the ToolBar shall be configured by the user.
     */
    void slotConfigureToolbar();

    /** Called to add a bookmark.
     */
    void slotAddBookmark();
    
    /** Called to delete a bookmark
     */
    void slotDeleteBookmark(const TQString &url, const TQString &title);
    
    /** Called to edit bookmarks.
     */
    void slotEditBookmarks();
    
    /** Called from slotConfigureToolbar() if we have a new ToolBar-configuration.
     */
    void slotNewToolbarConfig();
    
    /** called whenever the content of the clipboard changes.
     *  Checks if the content of the Clipboard is of type text/plain.
     *  Disables and enables the SystemTray-object.
     */
    void slotClipboardChanged();
    
    /** Deletes the content of the Textview.
     */
    void slotClear();

    /** Hides the MainWindow
     */
    void slotCloseMainWindow();

    /** Dialog to edit the Key associations
     */
    void slotEditKeys();

    /** Open file action
     */
    void slotFileOpen();

    /** Save file
     */
    void slotFileSave();

    /** Save file as
     */
    void slotFileSaveAs();
    
    /** Called, when the TTS Library has finished processing.
     */
    void slotTTSFinished();
    
    /** Edit mode switched on/off.
     */
    void slotEditToggled();
    
    /**
     * Catches the signal from the TreeView object
     * and forwards it to the BookmarkHandler object.
     */
    void slotNotifyBookmarkHandler(const TQString &ID, const TQString &title);
        
    /**
     * Copies the content of the current bookmark file to the
     * file designated by \p newname and install a new
     * BookmarkHandler.
     * \param newname The name of the new bookmark file.
     */
    void slotChangeBookmarkFilename(const TQString &newname);    

    /** Loads bookmark file corresponding to the given file.
     * \param newname The name of the new file.
     */
    void slotSetBookmarkFilename(const TQString &newname);    

public:   
    /**
     * Selects the item with the given ID in the TreeView.
     * \param ID The ID of the item to select.
     * \param title The title of the bookmark.
     * \returns <tt>TQString()</tt>, if the operation was successfull,
     * an error message, if the ID was not found.
     */
    TQString setItemByBookmark( const TQString &ID, const TQString &title );
    
private: // Methods
    /** read general Options again and initialize all variables.
     */
    void readOptions();

    /** initializes the TDEActions of the application */
    void initActions();

    /** sets up the statusbar for the main window by initialzing a statuslabel.
     */
    void initStatusBar();

    /**
     */
    void initBookmarkManager(const TQString &filename);
    
    /** creates the centerwidget of the KTMainWindow instance and sets it as the view.
     */
    void initView();
    
    /** resets the editor. Do not create an empty document.
     */
    void resetView();

    /** start to say the text on the KTTSD's Job-Stack.
     */
    void sayActivated();

    /** reimplemented from baseclass
     */
    virtual void closeEvent(TQCloseEvent *ce);
    
    /** disable/enable Actions.
     * \param actions An OR'ed integer of enum ACTIONS. A '1' is supposed
     * as 'Action enabled'.
     */
    void setActions(int actions);
    
    /**
     * Returns the absolute pathname of the given bookmarkfile.
     * This is the first KDE resource directory \p (share/apps) in which
     * the user has write permissions. The relative path is
     * \p ksayit/ksayit_bookmarks/.
     * \param filename The name of the bookmark file.
     * \returns The absolute pathname of the given bookmark file.
     * Returns \p TQString() if the operation was not successfull.
     */
    TQString getBookmarkDir(const TQString &filename);
    
    
private:
    TDEConfig *config;
    KSayItViewImpl *view;
    KSayItSystemTray *tray;
    EffectStack *es;
    DocTreeViewImpl *treeview;
    KBookmarkManager *bkManager;
    KSayItBookmarkHandler *bkHandler;
    KBookmarkMenu *bkMenu;

    // TDEAction pointers
    TDEToggleAction *statusBarAction;
    TDEAction *say;
    TDEAction *pause;
    TDEAction *shutup;
    TDEAction *next_sentence;
    TDEAction *prev_sentence;
    TDEAction *clear;
    TDEAction *copy;
    TDEAction *cut;
    TDEAction *paste;
    TDEAction *open;
    TDEAction *save;
    TDEAction *saveAs;
    TDEAction *preferences;
    TDEToggleAction *edit;
    TDEActionMenu *bookmarkmenu;
    
    // Misc stuff
    TQClipboard *cb;
    TQString clip;
    KTTSDLib *m_kttslib;
    FXPluginHandler *m_fxpluginhandler;
    bool m_ap_saying;
    bool m_ap_paused;
    bool m_textview_empty;
    bool m_enableChangeNotifications;
    TQString m_currentBookmarkFile;

};
 
#endif // KSAYIT_H
