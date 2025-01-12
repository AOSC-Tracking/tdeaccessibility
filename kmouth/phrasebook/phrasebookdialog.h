/***************************************************************************
                          phrasebookdialog.h  -  description
                             -------------------
    begin                : Don Sep 19 2002
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

#ifndef PHRASEBOOKDIALOG_H
#define PHRASEBOOKDIALOG_H

#include <tqbutton.h>
#include <tqradiobutton.h>
#include <tqbuttongroup.h>
#include <tqlabel.h>
#include <tqlistview.h> 
#include <tdemainwindow.h>
#include <klineedit.h>
#include <kkeybutton.h>
#include <kurl.h>
#include "phrasebook.h"
#include "buttonboxui.h"

class TQListViewItem;
class PhraseTreeItem;
class PhraseTree;
class TQStringList;
class TQString;
class TDEListView;

struct StandardBook {
   TQString name;
   TQString path;
   TQString filename;
};
typedef TQValueList<StandardBook> StandardBookList;

/**The class PhraseTreeItem is an ListViewItem for either a phrase or a phrase book.
  *@author Gunnar Schmi Dt
  */

class CheckBookItem : public TQCheckListItem {
public:
   CheckBookItem (TQListViewItem *parent, TQListViewItem *last,
            const TQString &text, const TQString &name, const TQString &filename);
   CheckBookItem (TQListView *parent, TQListViewItem *last,
            const TQString &text, const TQString &name, const TQString &filename);
   ~CheckBookItem();

protected:
   virtual void activate ();
   virtual void stateChange (bool);
   virtual void propagateStateChange ();
   virtual void childChange (int numberDiff, int selDiff);

private:
   int numberOfBooks;
   int selectedBooks;
};

/**
 * This class represents a widget for configuring the initial phrasebook.
 * @author Gunnar Schmi Dt
 */

class InitialPhraseBookWidget : public TQWidget {
   TQ_OBJECT
  
public:
   InitialPhraseBookWidget(TQWidget *parent, const char *name);
   ~InitialPhraseBookWidget();

   void createBook();

private:
   /** initializes the list of standard phrase books */
   void initStandardPhraseBooks ();

   TDEListView *books;
};

/**
 * The class StandardPhraseBookInsertAction implements an Action for
 * inserting a standard phrase book.
 * @author Gunnar Schmi Dt
 */
class StandardPhraseBookInsertAction : public TDEAction {
   TQ_OBJECT
  
public:
   StandardPhraseBookInsertAction (const KURL &url, const TQString& name, const TQObject* receiver, const char* slot, TDEActionCollection* parent)
   : TDEAction (name, "phrasebook", 0, 0, 0, parent, 0) {
      this->url = url;
      connect (this, TQ_SIGNAL(slotActivated (const KURL &)), receiver, slot);
   };
   ~StandardPhraseBookInsertAction () {
   };

public slots:
   void slotActivated () {
      TDEAction::slotActivated();
      emit slotActivated (url);
   };

signals:
   void slotActivated (const KURL &url);

private:
   KURL url;
};

/**
 * This class represents a widget holding the buttons of the phrase book
 * edit window.
 * @author Gunnar Schmi Dt
 */
class ButtonBoxWidget : public ButtonBoxUI {
public:
   ButtonBoxWidget (TQWidget *parent = 0, const char *name = 0);
   ~ButtonBoxWidget ();

   KKeyButton *keyButton;
   TQButtonGroup *group;

protected:
   TQGridLayout *keyButtonPlaceLayout;
};

/**
 * The class PhraseBookDialog implements a dialog for editing phrase books.
 * @author Gunnar Schmi Dt
 */

class PhraseBookDialog : public TDEMainWindow {
   friend class InitialPhraseBookWidget;
   TQ_OBJECT
  
private:
   /** Constructor. It is private because this class implements the singleton
    * pattern. For creating the instance of the dialog, use the get() method.
    */
   PhraseBookDialog ();

   static TQString displayPath (TQString path);

public:
   /** Returns a pointer to the instance of this dialog. As a part off the
    * singleton pattern it will make sure that there is at most one instance
    * of the dialog at a given time.
    */
   static PhraseBookDialog *get();

   /** Destructor. */
   ~PhraseBookDialog();

   bool queryClose ();

public slots:
   void slotTextChanged (const TQString &s);
   void slotNoKey();
   void slotCustomKey();
   void capturedShortcut (const TDEShortcut& cut);
   
   void selectionChanged ();
   void contextMenuRequested(TQListViewItem *, const TQPoint &pos, int);

   void slotRemove ();
   void slotCut ();
   void slotCopy ();
   void slotPaste ();

   void slotAddPhrasebook ();
   void slotAddPhrase ();

   void slotSave ();
   void slotImportPhrasebook ();
   void slotImportPhrasebook (const KURL &url);
   void slotExportPhrasebook ();
   void slotPrint ();

   void slotDropped (TQDropEvent *e, TQListViewItem *parent, TQListViewItem *after);
   void slotMoved (TQListViewItem *item, TQListViewItem *, TQListViewItem *);

signals:
   void phrasebookConfirmed (PhraseBook &book);

private:
   static StandardBookList standardPhraseBooks ();

   void initGUI();
   /** initializes the TDEActions of the window */
   void initActions();
   /** initializes the list of standard phrase books */
   void initStandardPhraseBooks ();
   
   TQListViewItem *addBook (TQListViewItem *parent, TQListViewItem *after, PhraseBook *book);
   TQListViewItem *addBook (TQListViewItem *item, PhraseBook *book);
   
   void setShortcut (const TDEShortcut &cut);

   void _warning (const KKeySequence &cut, TQString sAction, TQString sTitle);
   
   bool isGlobalKeyPresent (const TDEShortcut& cut, bool warnUser);
   bool isPhraseKeyPresent (const TDEShortcut& cut, bool warnUser);
   bool isKeyPresent (const TDEShortcut& cut, bool warnUser);

   PhraseBook book;
   bool phrasebookChanged;

   PhraseTree *treeView;
   ButtonBoxWidget *buttonBox;

   TDEAction* fileNewPhrase;
   TDEAction* fileNewBook;
   TDEAction* fileSave;
   TDEAction* fileImport;
   TDEToolBarPopupAction* toolbarImport;
   TDEActionMenu* fileImportStandardBook;
   TDEAction* fileExport;
   TDEAction* filePrint;
   TDEAction* fileClose;
   TDEAction* editCut;
   TDEAction* editCopy;
   TDEAction* editPaste;
   TDEAction* editDelete;
};

#endif
