//
// C++ Interface: kttsdlibtalker2
//
// Description: 
//
//
// Author: Robert Vogl <voglrobe@lapislazuli>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KTTSDLIBTALKER2_H
#define KTTSDLIBTALKER2_H

// TQt includes
#include <tqobject.h>
#include <tqcstring.h>

#include <kspeech_stub.h>
#include <kspeechsink.h>

// KDE includes
#include <tdeapplication.h>
#include <dcopclient.h>

/**
@author Robert Vogl
*/
class kttsdlibtalker2 : public TQObject, public KSpeech_stub, virtual public KSpeechSink
{
Q_OBJECT
  

signals:
    void signalTextStarted(const uint);
    void signalTextFinished(const uint);
    void signalTextStopped(const uint);

public:
    kttsdlibtalker2(TQObject *parent = 0, const char *name = 0);

    ~kttsdlibtalker2();
    
    /** Intializes the DCOP interface.
     */
    void KTTSD_init(TDEApplication *Appl);
    
    /** See \p KSpeech Class Reference. Returns the job number.
     */
    uint KTTSD_setText(const TQString &text, TQString lang);
    
    /** See \p KSpeech Class Reference.
     */
    void KTTSD_startText(uint jobNum);

    /** See \p KSpeech Class Reference.
     */
    void KTTSD_stopText(uint jobNum);
    
    /** See \p KSpeech Class Reference.
     */
    void KTTSD_pauseText(uint jobNum);
    
    /** See \p KSpeech Class Reference.
     */
    void KTTSD_resumeText(uint jobNum);
    
    /** See \p KSpeech Class Reference.
     */
    uint KTTSD_moveRelTextSentence(const int n, const uint jobNum = 0);
    
    /** See \p KSpeech Class Reference.
     */    
    int KTTSD_getTextJobState(uint jobNum);
    
    /** See \p KSpeech Class Reference.
     */    
    void KTTSD_removeText(uint jobNum);
    
    /** See \p KSpeech Class Reference.
     */    
    int KTTSD_getCurrentTextJob();


protected:
    ////////////////////////////////////////////
    // Slots for DCOP signals
    ////////////////////////////////////////////    
    ASYNC textStarted(const TQCString &appID, const uint jobNum);
    ASYNC textFinished(const TQCString &appID, const uint jobNum);
    ASYNC textStopped(const TQCString &appID, const uint jobNum);
    
private:
    TQCString m_objID;
    TDEApplication *m_Appl;
    DCOPClient *m_client;
    uint m_jobNum;
    

};

#endif
