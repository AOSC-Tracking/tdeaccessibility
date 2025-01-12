/***************************************************************************
                          speech.cpp  -  description
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

#include "speech.h"
#include <tqstring.h>
#include <tqvaluelist.h>
#include <tqvaluestack.h>
#include <tqstringlist.h>
#include <tqregexp.h>
#include <tqtextcodec.h>
#include <tqfile.h>
#include <kdebug.h>

#include <tdeversion.h>
#ifdef KDE_IS_VERSION
#if KDE_IS_VERSION(3,2,0)
   #define macroExpander
   #include <kmacroexpander.h>
#endif
#endif

Speech::Speech() {
}

Speech::~Speech() {
}

TQString Speech::prepareCommand (TQString command, const TQString &text,
                          const TQString &filename, const TQString &language) {
#ifdef macroExpander
   TQMap<TQChar,TQString> map;
   map['t'] = text;
   map['f'] = filename;
   map['l'] = language;
   return KMacroExpander::expandMacrosShellQuote (command, map);
#else
   TQValueStack<bool> stack;  // saved isdoublequote values during parsing of braces
   bool issinglequote=false; // inside '...' ?
   bool isdoublequote=false; // inside "..." ?
   int noreplace=0; // nested braces when within ${...}
   TQString escText = KShellProcess::quote(text);

   // character sequences that change the state or need to be otherwise processed
   TQRegExp re_singlequote("('|%%|%t|%f|%l)");
   TQRegExp re_doublequote("(\"|\\\\|`|\\$\\(|\\$\\{|%%|%t|%f|%l)");
   TQRegExp re_noquote  ("('|\"|\\\\|`|\\$\\(|\\$\\{|\\(|\\{|\\)|\\}|%%|%t|%f|%l)");

   // parse the command:
   for (int i = re_noquote.search(command);
        i != -1;
        i = (issinglequote?re_singlequote.search(command,i)
            :isdoublequote?re_doublequote.search(command,i)
            :re_noquote.search(command,i))
       )
      // while there are character sequences that need to be processed
   {
      if ((command[i]=='(') || (command[i]=='{')) { // (...) or {...}
         // assert(isdoublequote == false)
         stack.push(isdoublequote);
         if (noreplace > 0)
            // count nested braces when within ${...}
            noreplace++;
         i++;
      }
      else if (command[i]=='$') { // $(...) or ${...}
         stack.push(isdoublequote);
         isdoublequote = false;
         if ((noreplace > 0) || (command[i+1]=='{'))
            // count nested braces when within ${...}
            noreplace++;
         i+=2;
      }
      else if ((command[i]==')') || (command[i]=='}')) {
         // $(...) or (...) or ${...} or {...}
         if (!stack.isEmpty())
            isdoublequote = stack.pop();
         else
            tqWarning("Parse error.");
         if (noreplace > 0)
            // count nested braces when within ${...}
            noreplace--;
         i++;
      }
      else if (command[i]=='\'') {
         issinglequote=!issinglequote;
         i++;
      }
      else if (command[i]=='"') {
         isdoublequote=!isdoublequote;
         i++;
      }
      else if (command[i]=='\\')
         i+=2;
      else if (command[i]=='`') {
         // Replace all `...` with safer $(...)
         command.replace (i, 1, "$(");
         TQRegExp re_backticks("(`|\\\\`|\\\\\\\\|\\\\\\$)");
         for (int i2=re_backticks.search(command,i+2);
              i2!=-1;
              i2=re_backticks.search(command,i2)
             )
         {
            if (command[i2] == '`') {
               command.replace (i2, 1, ")");
               i2=command.length(); // leave loop
            }
            else {
               // remove backslash and ignore following character
               command.remove (i2, 1);
               i2++;
            }
         }
         // Leave i unchanged! We need to process "$("
      }
      else if (noreplace > 0) { // do not replace macros within ${...}
         if (issinglequote)
            i+=re_singlequote.matchedLength();
         else if (isdoublequote)
            i+=re_doublequote.matchedLength();
         else
            i+=re_noquote.matchedLength();
      }
      else { // replace macro
         TQString match, v;

         // get match
         if (issinglequote)
            match=re_singlequote.cap();
         else if (isdoublequote)
            match=re_doublequote.cap();
         else
            match=re_noquote.cap();

         // substitute %variables
         if (match=="%t")
            v = escText;
         else if (match=="%f")
            v = filename;
         else if (match=="%%")
            v = "%";
         else if (match=="%l")
            v = language;

         // %variable inside of a quote?
         if (isdoublequote)
            v='"'+v+'"';
         else if (issinglequote)
            v="'"+v+"'";

         command.replace (i, match.length(), v);
         i+=v.length();
      }
   }
   return command;
#endif
}

void Speech::speak(TQString command, bool stdIn, const TQString &text, const TQString &language, int encoding, TQTextCodec *codec) {
   if (text.length () > 0) {
      // 1. prepare the text:
      // 1.a) encode the text
      TQTextStream ts (encText, IO_WriteOnly);
      if (encoding == Local)
         ts.setEncoding (TQTextStream::Locale);
      else if (encoding == Latin1)
         ts.setEncoding (TQTextStream::Latin1);
      else if (encoding == Unicode)
         ts.setEncoding (TQTextStream::Unicode);
      else
         ts.setCodec (codec);
      ts << text;

      // 1.b) create a temporary file for the text
      tempFile.setAutoDelete(true);
      TQTextStream* fs = tempFile.textStream();
      if (encoding == Local)
         fs->setEncoding (TQTextStream::Locale);
      else if (encoding == Latin1)
         fs->setEncoding (TQTextStream::Latin1);
      else if (encoding == Unicode)
         fs->setEncoding (TQTextStream::Unicode);
      else
         fs->setCodec (codec);
      *fs << text;
      *fs << endl;
      TQString filename = tempFile.file()->name();
      tempFile.close();

      // 2. prepare the command:
      command = prepareCommand (command, encText, filename, language);


      // 3. create a new process
      process << command;
      connect(&process, TQ_SIGNAL(processExited(TDEProcess *)), this, TQ_SLOT(processExited(TDEProcess *)));
      connect(&process, TQ_SIGNAL(wroteStdin(TDEProcess *)), this, TQ_SLOT(wroteStdin(TDEProcess *)));
      connect(&process, TQ_SIGNAL(receivedStdout(TDEProcess *, char *, int)), this, TQ_SLOT(receivedStdout(TDEProcess *, char *, int)));
      connect(&process, TQ_SIGNAL(receivedStderr(TDEProcess *, char *, int)), this, TQ_SLOT(receivedStderr(TDEProcess *, char *, int)));

      // 4. start the process
      if (stdIn) {
         process.start(TDEProcess::NotifyOnExit, TDEProcess::All);
         if (encText.size() > 0)
            process.writeStdin(encText, encText.size());
         else
            process.closeStdin();
      }
      else
         process.start(TDEProcess::NotifyOnExit, TDEProcess::AllOutput);
   }
}

void Speech::receivedStdout (TDEProcess *, char *buffer, int buflen) {
   kdDebug() << TQString::fromLatin1(buffer, buflen) + "\n";
}
void Speech::receivedStderr (TDEProcess *, char *buffer, int buflen) {
   kdDebug() << TQString::fromLatin1(buffer, buflen) + "\n";
}

void Speech::wroteStdin(TDEProcess *) {
   process.closeStdin();
}

void Speech::processExited(TDEProcess *) {
   delete this;
}

#include "speech.moc"
