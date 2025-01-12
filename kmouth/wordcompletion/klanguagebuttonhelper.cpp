/*
 * klanguagebuttonhelper.cpp - Methods that help filling the KLanguageButton with data.
 *
 * Copyright (c) 2003 Gunnar Schmi Dt <gunnar@schmi-dt.de>
 *
 * Requires the TQt widget libraries, available at no cost at
 * http://www.trolltech.com/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "klanguagebuttonhelper.h"

#include <tdeglobal.h>
#include <tdelocale.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include "klanguagebutton.h"

TQString languageName (TQString languageCode) {
   TQString filename = TDEGlobal::dirs()->findResource("locale",
			languageCode + TQString::fromLatin1("/entry.desktop"));
      
   KSimpleConfig entry(filename);
   entry.setGroup(TQString::fromLatin1("KCM Locale"));
   return entry.readEntry(TQString::fromLatin1("Name"), i18n("without name"));
}

void loadLanguageList(KLanguageButton *combo)
// This function was taken from tdebase/kcontrol/tdm/tdm-appear.cpp
{
  TQStringList langlist = TDEGlobal::dirs()->findAllResources("locale",
			TQString::fromLatin1("*/entry.desktop"));
  langlist.sort();
  for ( TQStringList::ConstIterator it = langlist.begin();
	it != langlist.end(); ++it )
  {
    TQString fpath = (*it).left((*it).length() - 14);
    int index = fpath.findRev('/');
    TQString nid = fpath.mid(index + 1);

    KSimpleConfig entry(*it);
    entry.setGroup(TQString::fromLatin1("KCM Locale"));
    TQString name = entry.readEntry(TQString::fromLatin1("Name"), i18n("without name"));
    combo->insertLanguage(nid, name, TQString::fromLatin1("l10n/"), TQString());
  }
  
  if (TDEGlobal::locale())
     combo->setCurrentItem (TDEGlobal::locale()->language());
  else
     combo->setCurrentItem (TDELocale::defaultLanguage());
}
