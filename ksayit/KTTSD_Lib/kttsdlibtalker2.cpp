//
// C++ Implementation: kttsdlibtalker2
//
// Description: 
//
//
// Author: Robert Vogl <voglrobe@lapislazuli>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <unistd.h> //sleep

// TQt includes
#include <tqstring.h>

// KDE includes
#include <kdebug.h>

// App specific includes
#include "kttsdlibtalker2.h"

kttsdlibtalker2::kttsdlibtalker2(TQObject *tqparent, const char *name)
    : TQObject(tqparent, name), DCOPStub("kttsd", "KSpeech")
{
    m_client = NULL;

    connectDCOPSignal("kttsd", "KSpeech",
        "textStarted(TQCString, uint)",
        "textStarted(TQCString, uint)", false);    
    
    connectDCOPSignal("kttsd", "KSpeech",
        "textFinished(TQCString, uint)",
        "textFinished(TQCString, uint)", false);    
    
    connectDCOPSignal("kttsd", "KSpeech",
        "textStopped(TQCString, uint)",
        "textStopped(TQCString, uint)", false);    
    
}


kttsdlibtalker2::~kttsdlibtalker2()
{
}


void kttsdlibtalker2::KTTSD_init(KApplication *Appl)
{
    kdDebug(100200) << "kttsdlibtalker2::KTTSD_init()" << endl;

    m_client = Appl->dcopClient();
    if ( !m_client->isApplicationRegistered("kttsd")){
        TQString error;
        if ( Appl->startServiceByDesktopName("kttsd", TQStringList(), &error) ){
            // throw(error);
	    kdDebug() << "Starting KTTSD failed with message " << error << endl;
        }
    }
    m_objID = m_client->appId();  
    
    kdDebug(100200) << "    sucessfully registered. objID: " << m_objID << endl; 
}


uint kttsdlibtalker2::KTTSD_setText(const TQString &text, TQString lang)
{
    kdDebug(100200) << "kttsdlibtalker2::KTTSD_setText()" << endl;
    uint jobNum = setText(text, lang);
    return jobNum;
}


void kttsdlibtalker2::KTTSD_startText(uint jobNum)
{
    kdDebug(100200) << "kttsdlibtalker2::KTTSD_startText()" << endl;
    startText(jobNum);
}


void kttsdlibtalker2::KTTSD_stopText(uint jobNum)
{
    kdDebug(100200) << "kttsdlibtalker2::KTTSD_stopText()" << endl;
    stopText(jobNum);
}


void kttsdlibtalker2::KTTSD_pauseText(uint jobNum)
{
    kdDebug(100200) << "kttsdlibtalker2::KTTSD_pauseText()" << endl;
    pauseText(jobNum);
}


void kttsdlibtalker2::KTTSD_resumeText(uint jobNum)
{
    kdDebug(100200) << "kttsdlibtalker2::KTTSD_resumeText()" << endl;
    resumeText(jobNum);
}


uint kttsdlibtalker2::KTTSD_moveRelTextSentence(const int n, const uint jobNum)
{
    kdDebug(100200) << "kttsdlibtalker2::KTTSD_moveRelTextSentence()" << endl;
    uint seqNum = moveRelTextSentence(n, jobNum);
    return seqNum;
}


void kttsdlibtalker2::KTTSD_removeText(uint jobNum)
{
    kdDebug(100200) << "kttsdlibtalker2::KTTSD_removeText()" << endl;
    removeText(jobNum);
}


int kttsdlibtalker2::KTTSD_getTextJobState(uint jobNum)
{

    return 0;
}


int kttsdlibtalker2::KTTSD_getCurrentTextJob()
{

    return 0;
}



////////////////////////////////////////////
// DCOP signals
////////////////////////////////////////////    

ASYNC kttsdlibtalker2::textStarted(const TQCString &appID,
        const uint jobNum)
{
    kdDebug(100200) << "*** kttsdlibtalker2::textStarted()" << endl;
    if ( appID != m_objID )
        return;
    emit signalTextStarted(jobNum);
}


ASYNC kttsdlibtalker2::textFinished(const TQCString &appID,
        const uint jobNum)
{
    kdDebug(100200) << "*** kttsdlibtalker2::textFinished()" << endl;
    if ( appID != m_objID )
        return;
    emit signalTextFinished(jobNum);
}


ASYNC kttsdlibtalker2::textStopped(const TQCString &appID,
        const uint jobNum)
{
    kdDebug(100200) << "*** kttsdlibtalker2::textStopped()" << endl;
    if ( appID != m_objID )
        return;
    emit signalTextStopped(jobNum);
}

#include "kttsdlibtalker2.moc"
