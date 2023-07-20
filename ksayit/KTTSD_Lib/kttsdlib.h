//
// C++ Interface: kttsdplugin
//
// Description: 
//
//
// Author: Robert Vogl <voglrobe@lapislazuli>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KTTSDLIB_H
#define KTTSDLIB_H

// stl
#include <queue>
using namespace std;

// QT includes
#include <tqobject.h>
#include <tqwidget.h>
#include <tqframe.h>

#include <kdemacros.h>
#include "kdeexportfix.h"

// KDE includes
#include <tdeapplication.h>

// App specific includes


namespace TTS {
    enum {AUDIOFILE=1};
}

namespace ACTIONS {
    enum {PLAY=1, STOP=2, PAUSE=4, FFWD=8, FREV=16};
}


// forward declarations
class kttsdlibtalker2;

/**
@author Robert Vogl
*/
class KDE_EXPORT KTTSDLib : public TQObject
{
TQ_OBJECT

signals:
    /** Emitted when the plugin has been finished speeking.
     */
    void signalFinished();
    
public slots:
    /** Called from the talker on textFinished().
     * \param job The number of the finished job.
     */
    void slotTextFinished(const uint job);
    
    /** Called from the talker on textStopped().
     * \param job The number of the stopped job.
     */
    void slotTextStopped(const uint job);
    
    /** Called from the talker on textStarted().
     * \param job The number of the stopped job.
     */
    void slotTextStarted(const uint job);
    
public:
    KTTSDLib(TQObject *parent=0, const char *name=0, TDEApplication *Appl=0);

    ~KTTSDLib();

    /** Reimplementations from the base class.
     */
    TQString getName() const;
    
    TQString getDescription() const;
    
    int getActions();
    
    const TQWidget* getGUI(TQFrame *frame);
       
    void reloadConfiguration();
    
    bool saveWasClicked() const;
    
    void setText(const TQString &text);
    
    void sayText();
    
    int getStatus() const;
    
    void stop();
    
    void pause();
    
    void resume();
    
    void ffwd();
    
    void frev();

private slots:
    void removeAllJobsFromList();  
    
private:
    TDEApplication *m_Appl;
    kttsdlibtalker2 *m_talker;
    uint m_curJobNum;
    typedef queue<uint> jobListType;
    jobListType jobList;

};

#endif
