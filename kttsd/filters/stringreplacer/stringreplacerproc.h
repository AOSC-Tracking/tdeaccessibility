/*
  Generic String Replacement Filter Processing class.
  This is the interface definition for text filters.
  -------------------
  Copyright:
  (C) 2005 by Gary Cramblitt <garycramblitt@comcast.net>
  -------------------
  Original author: Gary Cramblitt <garycramblitt@comcast.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 ******************************************************************************/

#ifndef _STRINGREPLACERPROC_H_
#define _STRINGREPLACERPROC_H_

// TQt includes.
#include <tqobject.h>
#include <tqtextstream.h>
#include <tqvaluelist.h>
#include <tqregexp.h>
#include <tqstringlist.h>

// KTTS includes.
#include "filterproc.h"

class StringReplacerProc : virtual public KttsFilterProc
{
    TQ_OBJECT
  

public:
    /**
     * Constructor.
     */
    StringReplacerProc( TQObject *parent, const char *name, const TQStringList &args = TQStringList() );

    /**
     * Destructor.
     */
    virtual ~StringReplacerProc();

    /**
     * Initialize the filter.
     * @param config          Settings object.
     * @param configGroup     Settings Group.
     * @return                False if filter is not ready to filter.
     *
     * Note: The parameters are for reading from kttsdrc file.  Plugins may wish to maintain
     * separate configuration files of their own.
     */
    virtual bool init(TDEConfig *config, const TQString &configGroup);

    /**
     * Convert input, returning output.
     * @param inputText         Input text.
     * @param talkerCode        TalkerCode structure for the talker that KTTSD intends to
     *                          use for synthing the text.  Useful for extracting hints about
     *                          how to filter the text.  For example, languageCode.
     * @param appId             The DCOP appId of the application that queued the text.
     *                          Also useful for hints about how to do the filtering.
     */
    virtual TQString convert(const TQString& inputText, TalkerCode* talkerCode, const TQCString& appId);

    /**
     * Did this filter do anything?  If the filter returns the input as output
     * unmolested, it should return False when this method is called.
     */
    virtual bool wasModified();

private:
    // Language codes supported by the filter.
    TQStringList m_languageCodeList;
    // If not empty, apply filter only to apps containing one or more of these strings.
    TQStringList m_appIdList;

    // List of regular expressions to match.
    TQValueList<TQRegExp> m_matchList;
    // List of match case flag
    TQValueList<TQString> m_caseList;
    // List of substitutions to replace matches.
    TQValueList<TQString> m_substList;
    // True if this filter did anything to the text.
    bool m_wasModified;
};

#endif      // _STRINGREPLACERPROC_H_
