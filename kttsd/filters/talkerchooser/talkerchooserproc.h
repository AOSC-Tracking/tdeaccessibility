/*
  Generic Talker Chooser Filter Configuration class.
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

#ifndef _TALKERCHOOSERPROC_H_
#define _TALKERCHOOSERPROC_H_

// KTTS includes.
#include "filterproc.h"
#include "talkercode.h"

class TalkerChooserProc : virtual public KttsFilterProc
{
    TQ_OBJECT
  

public:
    /**
     * Constructor.
     */
    TalkerChooserProc( TQObject *parent, const char *name, const TQStringList &args = TQStringList() );

    /**
     * Destructor.
     */
    virtual ~TalkerChooserProc();

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
      * Returns True if the plugin supports asynchronous processing,
      * i.e., supports asyncConvert method.
      * @return                        True if this plugin supports asynchronous processing.
      *
      * If the plugin returns True, it must also implement @ref getState .
      * It must also emit @ref filteringFinished when filtering is completed.
      * If the plugin returns True, it must also implement @ref stopFiltering .
      * It must also emit @ref filteringStopped when filtering has been stopped.
      */
    virtual bool supportsAsync();

    /**
     * Convert input, returning output.  Runs synchronously.
     * @param inputText         Input text.
     * @param talkerCode        TalkerCode structure for the talker that KTTSD intends to
     *                          use for synthing the text.  Useful for extracting hints about
     *                          how to filter the text.  For example, languageCode.
     * @param appId             The DCOP appId of the application that queued the text.
     *                          Also useful for hints about how to do the filtering.
     */
    virtual TQString convert(const TQString& inputText, TalkerCode* talkerCode, const TQCString& appId);

private:

    TQString         m_re;
    TQStringList     m_appIdList;
    TalkerCode      m_chosenTalkerCode;
};

#endif      // _TALKERCHOOSERPROC_H_
