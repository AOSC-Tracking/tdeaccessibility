/***************************************************************************
                          configwizard.h  -  description
                             -------------------
    begin                : Mit Nov 20 2002
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


#ifndef CONFIGWIZARD_H
#define CONFIGWIZARD_H

#include <tqwidget.h>

#include <kwizard.h>

class TDEConfig;
class TextToSpeechConfigurationWidget;
class InitialPhraseBookWidget;
class CompletionWizardWidget;

/**The class ConfigWizard is used when the user starts KMouth for the first
 * time. It asks the user to provide a first set of configuration data.
 *@author Gunnar Schmi Dt
 */

class ConfigWizard : public KWizard  {
   Q_OBJECT
  
public:
   ConfigWizard (TQWidget *parent, const char *name, TDEConfig *config);
   ~ConfigWizard();

   bool configurationNeeded ();
   bool requestConfiguration ();
   void saveConfig (TDEConfig *config);

protected:
   void help();

private:
   void initCommandPage (TDEConfig *config);
   void initBookPage();
   void initCompletion (TDEConfig *config);

   TextToSpeechConfigurationWidget *commandWidget;
   InitialPhraseBookWidget *bookWidget;
   CompletionWizardWidget *completionWidget;
};

#endif
