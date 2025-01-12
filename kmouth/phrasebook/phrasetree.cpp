/***************************************************************************
                          phrasetree.cpp  -  description
                             -------------------
    begin                : Don Okt 24 2002
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

#include <tdelocale.h>
#include <tdeconfig.h>
#include <tdeaction.h>
#include <tdestdaccel.h>
#include <tdeshortcutlist.h>
#include <tdeactionshortcutlist.h>
#include <kiconloader.h>
#include <tdemessagebox.h>

#include "phrasetree.h"
#include "phrasebookdialog.h"
#include "phrasebook.h"

PhraseTreeItem::PhraseTreeItem (TQListView *parent, TQListViewItem *after, TQString phrase, TDEShortcut shortcut, TQPixmap icon)
   : TDEListViewItem (parent, after, phrase)
{
   isPhraseValue = true;
   cutValue = shortcut;
   setText(1, cutValue.toString());
   setPixmap(0, icon);
   setExpandable (false);
}

PhraseTreeItem::PhraseTreeItem (TQListViewItem *parent, TQListViewItem *after, TQString phrase, TDEShortcut shortcut, TQPixmap icon)
   : TDEListViewItem (parent, after, phrase)
{
   isPhraseValue = true;
   cutValue = shortcut;
   setText(1, cutValue.toString());
   setPixmap(0, icon);
   setExpandable (false);
}
PhraseTreeItem::PhraseTreeItem (TQListView *parent, TQListViewItem *after, TQString name, TQPixmap icon)
   : TDEListViewItem (parent, after, name)
{
   isPhraseValue = false;
   setPixmap(0, icon);
   setExpandable (true);
}
PhraseTreeItem::PhraseTreeItem (TQListViewItem *parent, TQListViewItem *after, TQString name, TQPixmap icon)
   : TDEListViewItem (parent, after, name)
{
   isPhraseValue = false;
   setPixmap(0, icon);
   setExpandable (true);
}
bool PhraseTreeItem::isPhrase () {
   return isPhraseValue;
}
TDEShortcut PhraseTreeItem::cut () {
   return cutValue;
}
void PhraseTreeItem::setCut (TDEShortcut cut) {
   cutValue = cut;
   setText(1, cut.toString());
}

// ***************************************************************************

PhraseTree::PhraseTree (TQWidget *parent, const char *name)
   : TDEListView (parent, name)
{
   phrasebook_open   = TDEGlobal::iconLoader()->loadIcon("phrasebook",        TDEIcon::Small);
   phrasebook_closed = TDEGlobal::iconLoader()->loadIcon("phrasebook_closed", TDEIcon::Small);
   phrase            = TDEGlobal::iconLoader()->loadIcon("phrase",            TDEIcon::Small);

   connect (this, TQ_SIGNAL(expanded (TQListViewItem *)), this, TQ_SLOT(itemExpanded (TQListViewItem *)));
   connect (this, TQ_SIGNAL(collapsed (TQListViewItem *)), this, TQ_SLOT(itemCollapsed (TQListViewItem *)));
}

PhraseTree::~PhraseTree (){
}

namespace PhraseTreePrivate {
   TQListViewItem *prevSibling (TQListViewItem *item) {
      TQListViewItem *parent = item->parent();
      TQListViewItem *above  = item->itemAbove();

      if (above == parent)
         return 0;

      while (above->parent() != parent)
         above = above->parent();

      return above;
   }

   bool findAbovePosition (TQListViewItem *item,
                           TQListViewItem **newParent,
                           TQListViewItem **newAbove)
   {
      if (item == 0)
         return false;

      TQListViewItem *parent = item->parent();
      TQListViewItem *above  = item->itemAbove();

      if (above == 0)
         return false;
      else if (above == parent) {
         *newParent = parent->parent();
         *newAbove  = prevSibling (parent);
         return true;
      }
      else if (above->parent() == parent) {
         *newParent = parent;
         *newAbove  = prevSibling (above);
         return true;
      }
      else {
         while (above->parent()->parent() != parent)
            above = above->parent();
         *newParent = above->parent();
         *newAbove  = above;
         return true;
      }
   }

   bool findBelowPosition (TQListViewItem *item,
                           TQListViewItem **newParent,
                           TQListViewItem **newAbove)
   {
      if (item == 0)
         return false;

      TQListViewItem *parent = item->parent();
      TQListViewItem *below  = item->nextSibling();

      if (parent == 0 && below == 0)
         return false;
      else if (parent != 0 && below == 0) {
         *newParent = parent->parent();
         *newAbove  = parent;
         return true;
      }
      else if (below->isOpen()) {
         *newParent = below;
         *newAbove  = 0;
         return true;
      }
      else {
         *newParent = parent;
         *newAbove  = below;
         return true;
      }
   }

   bool findRightPosition (TQListViewItem *item,
                           TQListViewItem **newParent,
                           TQListViewItem **newAbove)
   {
      if (item == 0)
         return false;

      TQListViewItem *above = prevSibling (item);

      if (above == 0)
         return false;
      else if (((PhraseTreeItem *)above)->isPhrase())
         return false;
      else {
         above->setOpen(true);
         *newParent = above;
         *newAbove = 0;
         above = (*newParent)->firstChild();
         while (above != 0) {
            *newAbove = above;
            above = above->nextSibling();
         }
         return true;
      }
   }

   bool findLeftPosition (TQListViewItem *item,
                          TQListViewItem **newParent,
                          TQListViewItem **newAbove)
   {
      if (item == 0)
         return false;

      TQListViewItem *parent = item->parent();

      if (parent == 0)
         return false;
      else {
         *newParent = parent->parent();
         *newAbove  = parent;
         return true;
      }
   }
}

void PhraseTree::moveItem (TQListViewItem *item,
                           TQListViewItem *parent,
                           TQListViewItem *above)
{
   if (item != 0) {
      if (item->parent() == 0)
         takeItem (item);
      else
         item->parent()->takeItem (item);

      if (parent == 0)
         insertItem (item);
      else
         parent->insertItem (item);

      item->moveItem(above);
   }
}

bool PhraseTree::hasSelectedItems() {
   TQListViewItem *i = firstChild();
   if ( !i )
       return false;
   int level = 0;
   do {
      if (i->isSelected())
         return true;

      if (i->firstChild() != 0) {
         i = i->firstChild();
         level++;
      }
      else {
         while ((i != 0) && (i->nextSibling() == 0)) {
            i = i->parent();
            level--;
         }
         if (i != 0)
            i = i->nextSibling();
      }
   }
   while (i != 0);

   return false;
}

void PhraseTree::deleteSelectedItems() {
   TQListViewItem *i = firstChild();
   if ( !i )
       return;
   TQListViewItem *deleteItem = 0;
   do {
      if (i->isSelected())
         deleteItem = i;

      if ((i->firstChild() != 0) && (!i->isSelected())) {
         i = i->firstChild();
      }
      else {
         while ((i != 0) && (i->nextSibling() == 0)) {
            i = i->parent();
         }
         if (i != 0)
            i = i->nextSibling();
      }
      if (deleteItem != 0) {
         delete deleteItem;
         deleteItem = 0;
      }
   }
   while (i != 0);
}

void PhraseTree::keyPressEvent (TQKeyEvent *e) {
   if ((e->state() & TQt::KeyButtonMask) == TQt::AltButton) {
      if (e->key() == TQt::Key_Up) {
         TQListViewItem *item = currentItem();
         if ((item != 0) && (item->isSelected())) {
            TQListViewItem *parent;
            TQListViewItem *above;

            if (PhraseTreePrivate::findAbovePosition (item, &parent, &above)) {
               moveItem(item, parent, above);
               setCurrentItem (item);
               item->setSelected(true);
            }
         }
         e->accept();
         return;
      }
      else if (e->key() == TQt::Key_Down) {
         TQListViewItem *item = currentItem();
         if ((item != 0) && (item->isSelected())) {
            TQListViewItem *parent;
            TQListViewItem *above;

            if (PhraseTreePrivate::findBelowPosition (item, &parent, &above)) {
               moveItem(item, parent, above);
               setCurrentItem (item);
               item->setSelected(true);
            }
         }
         e->accept();
         return;
      }
      else if (e->key() == TQt::Key_Left) {
         TQListViewItem *item = currentItem();
         if ((item != 0) && (item->isSelected())) {
            TQListViewItem *parent;
            TQListViewItem *above;

            if (PhraseTreePrivate::findLeftPosition (item, &parent, &above)) {
               moveItem(item, parent, above);
               setCurrentItem (item);
               item->setSelected(true);
            }
         }
         e->accept();
         return;
      }
      else if (e->key() == TQt::Key_Right) {
         TQListViewItem *item = currentItem();
         if ((item != 0) && (item->isSelected())) {
            TQListViewItem *parent;
            TQListViewItem *above;

            if (PhraseTreePrivate::findRightPosition (item, &parent, &above)) {
               moveItem(item, parent, above);
               setCurrentItem (item);
               item->setSelected(true);
            }
         }
         e->accept();
         return;
      }
   }
   TDEListView::keyPressEvent(e);
}

PhraseTreeItem *PhraseTree::insertPhrase (TQListViewItem *parent, TQListViewItem *after, TQString phrase, TQString shortcut) {
   TDEShortcut cut = TDEShortcut(shortcut);
   if (isKeyPresent (cut, 0, false))
      cut = TDEShortcut(TQString());

   if (parent == 0)
      return new PhraseTreeItem (this, after, phrase, cut, this->phrase);
   else
      return new PhraseTreeItem (parent, after, phrase, cut, this->phrase);
}

PhraseTreeItem *PhraseTree::insertBook (TQListViewItem *parent, TQListViewItem *after, TQString name) {
   if (parent == 0)
      return new PhraseTreeItem (this, after, name, phrasebook_closed);
   else
      return new PhraseTreeItem (parent, after, name, phrasebook_closed);
}

TQListViewItem *PhraseTree::addBook (TQListViewItem *parent, TQListViewItem *after, PhraseBook *book) {
   TQListViewItem *last = after;
   int level = 0;
   PhraseBookEntryList::iterator it;
   for (it = book->begin(); it != book->end(); ++it) {
      int newLevel = (*it).getLevel();
      while (level < newLevel) {
         parent = insertBook(parent, last, "");
         last = 0;
         level++;
      }
      while (level > newLevel) {
         last = parent;
         if (parent != 0)
            parent = parent->parent();
         level--;
      }

      if ((*it).isPhrase()) {
         Phrase phrase = (*it).getPhrase();
         last = insertPhrase (parent, last, phrase.getPhrase(), phrase.getShortcut());
      }
      else {
         Phrase phrase = (*it).getPhrase();
         parent = insertBook(parent, last, phrase.getPhrase());
         last = 0;
         level++;
      }
   }
   while (level > 0) {
      last = parent;
      if (parent != 0)
         parent = parent->parent();
      level--;
   }
   return last;
}

void PhraseTree::fillBook (PhraseBook *book, bool respectSelection) {
   TQListViewItem *i = firstChild();
   int level = 0;
   if ( !i )
       return;
   do {
      if (i->isSelected() || !respectSelection || level > 0) {
         PhraseTreeItem *it = (PhraseTreeItem *)i;
         Phrase phrase(it->text(0), it->cut().toStringInternal());
         *book += PhraseBookEntry(phrase, level, it->isPhrase());
      }

      if (i->firstChild() != 0) {
         if (i->isSelected() || !respectSelection || level > 0)
            level++;
         i = i->firstChild();
      }
      else {
         while ((i != 0) && (i->nextSibling() == 0)) {
            i = i->parent();
            if (level > 0)
               level--;
         }
         if (i != 0)
            i = i->nextSibling();
      }
   }
   while (i != 0);
}

TQDragObject *PhraseTree::dragObject () {
   return dragObject (true);
}

TQDragObject *PhraseTree::dragObject (bool isDependent) {
   PhraseBook book;
   fillBook (&book, true);
   if (isDependent)
      return new PhraseBookDrag(&book, viewport());
   return new PhraseBookDrag(&book);
}

bool PhraseTree::acceptDrag (TQDropEvent* event) const {
   if (TDEListView::acceptDrag (event))
      return true;
   else
      return PhraseBookDrag::canDecode(event);
}

// Returns iSeq index if cut2 has a sequence of equal or higher priority
// to a sequence in cut, else -1
static int keyConflict (const TDEShortcut& cut, const TDEShortcut& cut2) {
   for (uint iSeq = 0; iSeq < cut.count(); iSeq++) {
      for (uint iSeq2 = 0; iSeq2 <= iSeq && iSeq2 < cut2.count(); iSeq2++) {
         if (cut.seq(iSeq) == cut2.seq(iSeq2))
            return iSeq;
       }
   }
   return -1;
}

void PhraseTree::_warning (const KKeySequence& cut, TQString sAction, TQString sTitle) {
   sAction = sAction.stripWhiteSpace();

   TQString s =
       i18n("The '%1' key combination has already been allocated "
       "to %2.\n"
       "Please choose a unique key combination.").
       arg(cut.toString()).arg(sAction);

   KMessageBox::sorry( this, s, sTitle );
}

bool PhraseTree::isStdAccelPresent (const TDEShortcut& cut, bool warnUser) {
   for (uint iSeq = 0; iSeq < cut.count(); iSeq++) {
      const KKeySequence& seq = cut.seq(iSeq);

      TDEStdAccel::StdAccel id = TDEStdAccel::findStdAccel( seq );
      if( id != TDEStdAccel::AccelNone
          && keyConflict (cut, TDEStdAccel::shortcut(id)) > -1)
      {
         if (warnUser)
            _warning (cut.seq(iSeq),
                      i18n("the standard \"%1\" action").arg(TDEStdAccel::label(id)),
                      i18n("Conflict with Standard Application Shortcut"));
         return true;
      }
   }
   return false;
}

bool PhraseTree::isGlobalKeyPresent (const TDEShortcut& cut, bool warnUser) {
   TQMap<TQString, TQString> mapEntry = TDEGlobal::config()->entryMap ("Global Shortcuts");
   TQMap<TQString, TQString>::Iterator it;
   for (it = mapEntry.begin(); it != mapEntry.end(); ++it) {
      int iSeq = keyConflict (cut, TDEShortcut(*it));
      if (iSeq > -1) {
         if (warnUser)
            _warning (cut.seq(iSeq),
                      i18n("the global \"%1\" action").arg(it.key()),
                      i18n("Conflict with Global Shortcuts"));
         return true;
      }
   }
   return false;
}

bool PhraseTree::isPhraseKeyPresent (const TDEShortcut& cut, PhraseTreeItem* cutItem, bool warnUser) {
   for (TQListViewItemIterator it(this); it.current(); ++it) {
      PhraseTreeItem* item = dynamic_cast<PhraseTreeItem*>(it.current());
      if ((item != 0) && (item != cutItem)) {
         int iSeq = keyConflict (cut, item->cut());
         if (iSeq > -1) {
            if (warnUser)
               _warning (cut.seq(iSeq),
                         i18n("an other phrase"),
                         i18n("Key Conflict"));
            return true;
         }
      }
   }
   return false;
}

bool PhraseTree::isKeyPresent (const TDEShortcut& cut, PhraseTreeItem* cutItem, bool warnUser) {
   if (isStdAccelPresent (cut, warnUser))
      return true;

   if (isGlobalKeyPresent (cut, warnUser))
      return true;

   if (isPhraseKeyPresent (cut, cutItem, warnUser))
      return true;

   return false;
}

void PhraseTree::itemExpanded (TQListViewItem *item) {
   PhraseTreeItem *i = (PhraseTreeItem *)item;
   if (!i->isPhrase())
      i->setPixmap(0, phrasebook_open);
}

void PhraseTree::itemCollapsed (TQListViewItem *item) {
   PhraseTreeItem *i = (PhraseTreeItem *)item;
   if (!i->isPhrase())
      i->setPixmap(0, phrasebook_closed);
}

// ***************************************************************************

#include "phrasetree.moc"
