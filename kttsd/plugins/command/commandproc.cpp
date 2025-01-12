/*
  Main speaking functions for the Command Plug in
  -------------------
  Copyright : (C) 2002 by Gunnar Schmi Dt and 2004 by Gary Cramblitt
  -------------------
  Original author: Gunnar Schmi Dt <kmouth@schmi-dt.de>
  Current Maintainer: Gary Cramblitt <garycramblitt@comcast.net>
 ******************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 ***************************************************************************/

// TQt includes.
#include <tqfile.h>
#include <tqstring.h>
#include <tqvaluelist.h>
#include <tqstringlist.h>
#include <tqregexp.h>
#include <tqtextcodec.h>
#include <tqvaluestack.h>

// KDE includes.
#include <kdebug.h>
#include <tdeconfig.h>
#include <kprocess.h>
#include <tdetempfile.h>
#include <kstandarddirs.h>

// KTTS includes.
#include <pluginproc.h>

// Command Plugin includes.
#include "commandproc.h"
#include "commandproc.moc"

/** Constructor */
CommandProc::CommandProc( TQObject* parent, const char* name, const TQStringList& /*args*/) : 
    PlugInProc( parent, name )
{
    kdDebug() << "CommandProc::CommandProc: Running" << endl;
    m_commandProc = 0;
    m_state = psIdle;
    m_stdin = true;
    m_supportsSynth = false;
    m_waitingStop = false;
}

/** Destructor */
CommandProc::~CommandProc()
{
    kdDebug() << "CommandProc::~CommandProc: Running" << endl;
    if (m_commandProc)
    {
        if (m_commandProc->isRunning()) m_commandProc->kill();
        delete m_commandProc;
        // Don't delete synth file.  That is responsibility of caller.
        if (!m_textFilename.isNull()) TQFile::remove(m_textFilename);
    }
}

/** Initialize */
bool CommandProc::init(TDEConfig *config, const TQString &configGroup){
    kdDebug() << "CommandProc::init: Initializing plug in: Command " << endl;

    config->setGroup(configGroup);
    m_ttsCommand = config->readEntry("Command", "cat -");
    m_stdin = config->readBoolEntry("StdIn", true);
    m_language = config->readEntry("LanguageCode", "en");

    // Support separate synthesis if the TTS command contains %w macro.
    m_supportsSynth = (m_ttsCommand.contains("%w"));

    TQString codecString = config->readEntry("Codec", "Local");
    m_codec = codecNameToCodec(codecString);
    kdDebug() << "CommandProc::init: Initialized with command: " << m_ttsCommand << " codec: " << codecString << endl;
    return true;
}

/** 
* Say a text.  Synthesize and audibilize it.
* @param text                    The text to be spoken.
*
* If the plugin supports asynchronous operation, it should return immediately.
*/
void CommandProc::sayText(const TQString &text)
{
    synth(text, TQString(),
        m_ttsCommand, m_stdin, m_codec, m_language);
}

/**
* Synthesize text into an audio file, but do not send to the audio device.
* @param text                    The text to be synthesized.
* @param suggestedFilename       Full pathname of file to create.  The plugin
*                                may ignore this parameter and choose its own
*                                filename.  KTTSD will query the generated
*                                filename using getFilename().
*
* If the plugin supports asynchronous operation, it should return immediately.
*/
void CommandProc::synthText(const TQString& text, const TQString& suggestedFilename)
{
    synth(text, suggestedFilename,
        m_ttsCommand, m_stdin, m_codec, m_language);
}

/**
* Say or Synthesize text.
* @param inputText               The text that shall be spoken
* @param suggestedFilename       If not Null, synthesize only to this filename, otherwise
*                                synthesize and audibilize the text.
* @param userCmd                 The program that shall be executed for speaking
* @param stdIn                   True if the program shall recieve its data via standard input
* @param codec                   The TQTextCodec if encoding==UseCodec
* @param language                The language code (used for the %l macro)
*/
void CommandProc::synth(const TQString& inputText, const TQString& suggestedFilename,
    const TQString& userCmd, bool stdIn, TQTextCodec *codec, TQString& language)
{
    if (m_commandProc)
    {
        if (m_commandProc->isRunning()) m_commandProc->kill();
        delete m_commandProc;
        m_commandProc = 0;
        m_synthFilename = TQString();
        if (!m_textFilename.isNull()) TQFile::remove(m_textFilename);
        m_textFilename = TQString();
    }
    TQString command = userCmd;
    TQString text = inputText.stripWhiteSpace();
    if (text.isEmpty()) return;
     // 1. prepare the text:
     // 1.a) encode the text
    text += "\n";
    TQCString encodedText;
    if (codec)
        encodedText = codec->fromUnicode(text);
    else
        encodedText = text.latin1();  // Should not happen, but just in case.

    // 1.b) quote the text as one parameter
    TQString escText = KShellProcess::quote(text);

    // 1.c) create a temporary file for the text, if %f macro is used.
    if (command.contains("%f"))
    {
        KTempFile tempFile(locateLocal("tmp", "commandplugin-"), ".txt");
        TQTextStream* fs = tempFile.textStream();
        fs->setCodec(codec);
        *fs << text;
        *fs << endl;
        m_textFilename = tempFile.file()->name();
        tempFile.close();
    } else m_textFilename = TQString();

    // 2. replace variables with values
    TQValueStack<bool> stack;
    bool issinglequote=false;
    bool isdoublequote=false;
    int noreplace=0;
    TQRegExp re_noquote("(\"|'|\\\\|`|\\$\\(|\\$\\{|\\(|\\{|\\)|\\}|%%|%t|%f|%l|%w)");
    TQRegExp re_singlequote("('|%%|%t|%f|%l|%w)");
    TQRegExp re_doublequote("(\"|\\\\|`|\\$\\(|\\$\\{|%%|%t|%f|%l|%w)");

    for	( int i = re_noquote.search(command);
        i != -1;
        i = (issinglequote?re_singlequote.search(command,i)
            :isdoublequote?re_doublequote.search(command,i)
            :re_noquote.search(command,i))
    )
    {
        if ((command[i]=='(') || (command[i]=='{')) // (...) or {...}
        {
            // assert(isdoublequote == false)
            stack.push(isdoublequote);
            if (noreplace > 0)
                // count nested braces when within ${...}
                noreplace++;
            i++;
        }
        else if (command[i]=='$')
        {
            stack.push(isdoublequote);
            isdoublequote = false;
            if ((noreplace > 0) || (command[i+1]=='{'))
                // count nested braces when within ${...}
                noreplace++;
            i+=2;
        }
        else if ((command[i]==')') || (command[i]=='}'))
            // $(...) or (...) or ${...} or {...}
        {
            if (!stack.isEmpty())
                isdoublequote = stack.pop();
            else
                tqWarning("Parse error.");
            if (noreplace > 0)
                // count nested braces when within ${...}
                noreplace--;
            i++;
        }
        else if (command[i]=='\'')
        {
            issinglequote=!issinglequote;
            i++;
        }
        else if (command[i]=='"')
        {
            isdoublequote=!isdoublequote;
            i++;
        }
        else if (command[i]=='\\')
            i+=2;
        else if (command[i]=='`')
        {
            // Replace all `...` with safer $(...)
            command.replace (i, 1, "$(");
            TQRegExp re_backticks("(`|\\\\`|\\\\\\\\|\\\\\\$)");
            for (	int i2=re_backticks.search(command,i+2);
                i2!=-1;
                i2=re_backticks.search(command,i2)
            )
            {
                if (command[i2] == '`')
                {
                    command.replace (i2, 1, ")");
                    i2=command.length(); // leave loop
                }
                else
                {   // remove backslash and ignore following character
                    command.remove (i2, 1);
                    i2++;
                }
            }
            // Leave i unchanged! We need to process "$("
        }
        else if (noreplace == 0) // do not replace macros within ${...}
        {
            TQString match, v;

            // get match
            if (issinglequote)
                match=re_singlequote.cap();
            else if (isdoublequote)
                match=re_doublequote.cap();
            else
                match=re_noquote.cap();

            // substitue %variables
            if (match=="%%")
                v="%";
            else if (match=="%t")
                v=escText;
            else if (match=="%f")
                v=m_textFilename;
            else if (match=="%l")
                v=language;
            else if (match=="%w")
                v = suggestedFilename;

            // %variable inside of a quote?
            if (isdoublequote)
                v='"'+v+'"';
            else if (issinglequote)
                v="'"+v+"'";

            command.replace (i, match.length(), v);
            i+=v.length();
        }
        else
        {
            if (issinglequote)
                i+=re_singlequote.matchedLength();
            else if (isdoublequote)
                i+=re_doublequote.matchedLength();
            else
                i+=re_noquote.matchedLength();
        }
    }

    // 3. create a new process
    kdDebug() << "CommandProc::synth: running command: " << command << endl;
    m_commandProc = new TDEProcess;
    m_commandProc->setUseShell(true);
    m_commandProc->setEnvironment("LANG", language + "." + codec->mimeName());
    m_commandProc->setEnvironment("LC_CTYPE", language + "." + codec->mimeName());
    *m_commandProc << command;
    connect(m_commandProc, TQ_SIGNAL(processExited(TDEProcess*)),
        this, TQ_SLOT(slotProcessExited(TDEProcess*)));
    connect(m_commandProc, TQ_SIGNAL(receivedStdout(TDEProcess*, char*, int)),
        this, TQ_SLOT(slotReceivedStdout(TDEProcess*, char*, int)));
    connect(m_commandProc, TQ_SIGNAL(receivedStderr(TDEProcess*, char*, int)),
        this, TQ_SLOT(slotReceivedStderr(TDEProcess*, char*, int)));
    connect(m_commandProc, TQ_SIGNAL(wroteStdin(TDEProcess*)),
        this, TQ_SLOT(slotWroteStdin(TDEProcess* )));

    // 4. start the process

    if (suggestedFilename.isNull())
        m_state = psSaying;
    else
    {
        m_synthFilename = suggestedFilename;
        m_state = psSynthing;
    }
    if (stdIn) {
        m_commandProc->start(TDEProcess::NotifyOnExit, TDEProcess::All);
        if (encodedText.length() > 0)
            m_commandProc->writeStdin(encodedText, encodedText.length());
        else
            m_commandProc->closeStdin();
    }
    else
        m_commandProc->start(TDEProcess::NotifyOnExit, TDEProcess::AllOutput);
}

/**
* Get the generated audio filename from synthText.
* @return                        Name of the audio file the plugin generated.
*                                Null if no such file.
*
* The plugin must not re-use the filename.
*/
TQString CommandProc::getFilename()
{
    kdDebug() << "CommandProc::getFilename: returning " << m_synthFilename << endl;
    return m_synthFilename;
}

/**
* Stop current operation (saying or synthesizing text).
* Important: This function may be called from a thread different from the
* one that called sayText or synthText.
* If the plugin cannot stop an in-progress @ref sayText or
* @ref synthText operation, it must not block waiting for it to complete.
* Instead, return immediately.
*
* If a plugin returns before the operation has actually been stopped,
* the plugin must emit the @ref stopped signal when the operation has
* actually stopped.
*
* The plugin should change to the psIdle state after stopping the
* operation.
*/
void CommandProc::stopText(){
    kdDebug() << "CommandProc::stopText: Running" << endl;
    if (m_commandProc)
    {
        if (m_commandProc->isRunning())
        {
            kdDebug() << "CommandProc::stopText: killing Command." << endl;
            m_waitingStop = true;
            m_commandProc->kill();
        } else m_state = psIdle;
    }else m_state = psIdle;
    kdDebug() << "CommandProc::stopText: Command stopped." << endl;
}

void CommandProc::slotProcessExited(TDEProcess*)
{
    kdDebug() << "CommandProc:slotProcessExited: Command process has exited." << endl;
    pluginState prevState = m_state;
    if (m_waitingStop)
    {
        m_waitingStop = false;
        m_state = psIdle;
        emit stopped();
    } else {
        m_state = psFinished;
        if (prevState == psSaying)
            emit sayFinished();
        else
            if (prevState == psSynthing)
                emit synthFinished();
    }
}

void CommandProc::slotReceivedStdout(TDEProcess*, char* buffer, int buflen)
{
    TQString buf = TQString::fromLatin1(buffer, buflen);
    kdDebug() << "CommandProc::slotReceivedStdout: Received output from Command: " << buf << endl;
}

void CommandProc::slotReceivedStderr(TDEProcess*, char* buffer, int buflen)
{
    TQString buf = TQString::fromLatin1(buffer, buflen);
    kdDebug() << "CommandProc::slotReceivedStderr: Received error from Command: " << buf << endl;
}

void CommandProc::slotWroteStdin(TDEProcess*)
{
    kdDebug() << "CommandProc::slotWroteStdin: closing Stdin" << endl;
    m_commandProc->closeStdin();
}

/**
* Return the current state of the plugin.
* This function only makes sense in asynchronous mode.
* @return                        The pluginState of the plugin.
*
* @see pluginState
*/
pluginState CommandProc::getState() { return m_state; }

/**
* Acknowledges a finished state and resets the plugin state to psIdle.
*
* If the plugin is not in state psFinished, nothing happens.
* The plugin may use this call to do any post-processing cleanup,
* for example, blanking the stored filename (but do not delete the file).
* Calling program should call getFilename prior to ackFinished.
*/
void CommandProc::ackFinished()
{
    if (m_state == psFinished)
    {
        m_state = psIdle;
        m_synthFilename = TQString();
        if (!m_textFilename.isNull()) TQFile::remove(m_textFilename);
        m_textFilename = TQString();
    }
}

/**
* Returns True if the plugin supports asynchronous processing,
* i.e., returns immediately from sayText or synthText.
* @return                        True if this plugin supports asynchronous processing.
*
* If the plugin returns True, it must also implement @ref getState .
* It must also emit @ref sayFinished or @ref synthFinished signals when
* saying or synthesis is completed.
*/
bool CommandProc::supportsAsync() { return true; }

/**
* Returns True if the plugin supports synthText method,
* i.e., is able to synthesize text to a sound file without
* audibilizing the text.
* @return                        True if this plugin supports synthText method.
*/
bool CommandProc::supportsSynth() { return m_supportsSynth; }
