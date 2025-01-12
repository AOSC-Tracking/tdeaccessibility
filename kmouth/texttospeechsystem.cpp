/***************************************************************************
                          texttospeechsystem.cpp  -  description
                             -------------------
    begin                : Son Sep 8 2002
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


#include "texttospeechsystem.h"
#include <tqregexp.h>
#include <tqtextcodec.h>
#include <tqptrlist.h>
#include <stdlib.h>

#include <tdeapplication.h>
#include <dcopclient.h>
#include <tdeconfig.h>

#include "speech.h"

TextToSpeechSystem::TextToSpeechSystem() {
   ttsCommand = "";
   stdIn = true;
   useKttsd = true;
   codec = Speech::Local; // local encoding;
   buildCodecList();
}

TextToSpeechSystem::~TextToSpeechSystem() {
   delete codecList;
}

bool kttsdSay (const TQString &text, const TQString &language) {
   DCOPClient *client = kapp->dcopClient();
   TQByteArray  data;
   TQCString replyType;
   TQByteArray  replyData;
   TQDataStream arg(data, IO_WriteOnly);
   arg << text << language;
   return client->call("kttsd", "KSpeech", "sayWarning(TQString,TQString)",
                       data, replyType, replyData, true);
}

void TextToSpeechSystem::speak (const TQString &text, const TQString &language) {
   if (text.length() > 0) {
      if (useKttsd) {
         if (kttsdSay(text, language))
            return;
      }

      if (codec < Speech::UseCodec)
         (new Speech())->speak(ttsCommand, stdIn, text, language, codec, 0);
      else
         (new Speech())->speak(ttsCommand, stdIn, text, language, Speech::UseCodec,
                               codecList->at (codec - Speech::UseCodec));
   }
}

void TextToSpeechSystem::readOptions (TDEConfig *config, const TQString &langGroup) {
  config->setGroup(langGroup);
  ttsCommand = config->readPathEntry("Command");
  stdIn = config->readBoolEntry("StdIn", true);
  useKttsd = config->readBoolEntry("useKttsd", true);

  TQString codecString = config->readEntry("Codec", "Local");
  if (codecString == "Local")
     codec = Speech::Local;
  else if (codecString == "Latin1")
     codec = Speech::Latin1;
  else if (codecString == "Unicode")
     codec = Speech::Unicode;
  else {
     codec = Speech::Local;
     for (uint i = 0; i < codecList->count(); i++ )
        if (codecString == codecList->at(i)->name())
           codec = Speech::UseCodec + i;
  }
}

void TextToSpeechSystem::saveOptions (TDEConfig *config, const TQString &langGroup) {
  config->setGroup(langGroup);
  config->writePathEntry("Command", ttsCommand);
  config->writeEntry("StdIn", stdIn);
  config->writeEntry("useKttsd", useKttsd);
  if (codec == Speech::Local)
     config->writeEntry("Codec", "Local");
  else if (codec == Speech::Latin1)
     config->writeEntry("Codec", "Latin1");
  else if (codec == Speech::Unicode)
     config->writeEntry("Codec", "Unicode");
  else config->writeEntry("Codec",
         codecList->at (codec-Speech::UseCodec)->name());

}

void TextToSpeechSystem::buildCodecList () {
   codecList = new TQPtrList<TQTextCodec>;
   TQTextCodec *codec;
   int i;
   for (i = 0; (codec = TQTextCodec::codecForIndex(i)); i++)
      codecList->append (codec);
}

#include "texttospeechsystem.moc"
