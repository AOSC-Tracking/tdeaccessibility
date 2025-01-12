/*
  A KPart to display running jobs in KTTSD and permit user to stop, rewind,
  advance, change Talker, etc. 
  -------------------
  Copyright : (C) 2004 by Gary Cramblitt <garycramblitt@comcast.net>
  -------------------
  Current Maintainer: Gary Cramblitt <garycramblitt@comcast.net>
 ******************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 ***************************************************************************/

#ifndef _KTTSJOBMGRPART_H_
#define _KTTSJOBMGRPART_H_

// KDE includes.
#include <tdeparts/browserextension.h>
#include <klibloader.h>

// KTTS includes.
#include "kspeech_stub.h"
#include "kspeechsink.h"

class TDEAboutData;
class TDEInstance;
class KttsJobMgrBrowserExtension;
class TDEListView;
class TQListViewItem;
class TQVBox;
class KTextEdit;

class KttsJobMgrFactory : public KLibFactory
{
    TQ_OBJECT
  
public:
    KttsJobMgrFactory() {};
    virtual ~KttsJobMgrFactory();

    virtual TQObject* createObject(TQObject* parent = 0, const char* name = 0,
                            const char* classname = "TQObject",
                            const TQStringList &args = TQStringList());

    static TDEInstance *instance();
    static TDEAboutData *aboutData();

private:
    static TDEInstance *s_instance;
};

class KttsJobMgrPart: 
    public KParts::ReadOnlyPart,
    public KSpeech_stub,
    virtual public KSpeechSink
{
    TQ_OBJECT
  
public:
    KttsJobMgrPart(TQWidget *parent, const char *name);
    virtual ~KttsJobMgrPart();

protected:
    virtual bool openFile();
    virtual bool closeURL();

    /** DCOP Methods connected to DCOP Signals emitted by KTTSD. */

    /**
    * This signal is emitted when KTTSD starts or restarts after a call to reinit.
    */
    ASYNC kttsdStarted();
    /**
    * This signal is emitted when the speech engine/plugin encounters a marker in the text.
    * @param appId          DCOP application ID of the application that queued the text.
    * @param markerName     The name of the marker seen.
    * @see markers
    */
    ASYNC markerSeen(const TQCString& appId, const TQString& markerName);
    /**
    * This signal is emitted whenever a sentence begins speaking.
    * @param appId          DCOP application ID of the application that queued the text.
    * @param jobNum         Job number of the text job.
    * @param seq            Sequence number of the text.
    * @see getTextCount
    */
    ASYNC sentenceStarted(const TQCString& appId, const uint jobNum, const uint seq);
    /**
    * This signal is emitted when a sentence has finished speaking.
    * @param appId          DCOP application ID of the application that queued the text.
    * @param jobNum         Job number of the text job.
    * @param seq            Sequence number of the text.
    * @see getTextCount
    */
    ASYNC sentenceFinished(const TQCString& appId, const uint jobNum, const uint seq);

    /**
    * This signal is emitted whenever a new text job is added to the queue.
    * @param appId          The DCOP senderId of the application that created the job.  NULL if kttsd.
    * @param jobNum         Job number of the text job.
    */
    ASYNC textSet(const TQCString& appId, const uint jobNum);

    /**
    * This signal is emitted whenever a new part is appended to a text job.
    * @param appId          The DCOP senderId of the application that created the job.
    * @param jobNum         Job number of the text job.
    * @param partNum        Part number of the new part.  Parts are numbered starting
    *                       at 1.
    */
    ASYNC textAppended(const TQCString& appId, const uint jobNum, const int partNum);

    /**
    * This signal is emitted whenever speaking of a text job begins.
    * @param appId          The DCOP senderId of the application that created the job.  NULL if kttsd.
    * @param jobNum         Job number of the text job.
    */
    ASYNC textStarted(const TQCString& appId, const uint jobNum);
    /**
    * This signal is emitted whenever a text job is finished.  The job has
    * been marked for deletion from the queue and will be deleted when another
    * job reaches the Finished state. (Only one job in the text queue may be
    * in state Finished at one time.)  If @ref startText or @ref resumeText is
    * called before the job is deleted, it will remain in the queue for speaking.
    * @param appId          The DCOP senderId of the application that created the job.  NULL if kttsd.
    * @param jobNum         Job number of the text job.
    */
    ASYNC textFinished(const TQCString& appId, const uint jobNum);
    /**
    * This signal is emitted whenever a speaking text job stops speaking.
    * @param appId          The DCOP senderId of the application that created the job.  NULL if kttsd.
    * @param jobNum         Job number of the text job.
    */
    ASYNC textStopped(const TQCString& appId, const uint jobNum);
    /**
    * This signal is emitted whenever a speaking text job is paused.
    * @param appId          The DCOP senderId of the application that created the job.  NULL if kttsd.
    * @param jobNum         Job number of the text job.
    */
    ASYNC textPaused(const TQCString& appId, const uint jobNum);
    /**
    * This signal is emitted when a text job, that was previously paused, resumes speaking.
    * @param appId          The DCOP senderId of the application that created the job.  NULL if kttsd.
    * @param jobNum         Job number of the text job.
    */
    ASYNC textResumed(const TQCString& appId, const uint jobNum);
    /**
    * This signal is emitted whenever a text job is deleted from the queue.
    * The job is no longer in the queue when this signal is emitted.
    * @param appId          The DCOP senderId of the application that created the job.  NULL if kttsd.
    * @param jobNum         Job number of the text job.
    */
    ASYNC textRemoved(const TQCString& appId, const uint jobNum);

private slots:
    /**
    * This slot is connected to the Job List View selectionChanged signal.
    */
    void slot_selectionChanged(TQListViewItem* item);
    /**
    * Slots connected to buttons.
    */
    void slot_job_hold();
    void slot_job_resume();
    void slot_job_restart();
    void slot_job_remove();
    void slot_job_move();
    void slot_job_change_talker();
    void slot_speak_clipboard();
    void slot_speak_file();
    void slot_refresh();
    void slot_job_prev_par();
    void slot_job_prev_sen();
    void slot_job_next_sen();
    void slot_job_next_par();

private:
    /**
    * @enum JobListViewColumn
    * Columns in the Job List View.
    */
    enum JobListViewColumn
    {
        jlvcJobNum = 0,               /**< Job Number. */
        jlvcOwner = 1,                /**< AppId of job owner */
        jlvcTalkerID = 2,             /**< Job Talker ID */
        jlvcState = 3,                /**< Job State */
        jlvcPosition = 4,             /**< Current sentence of job. */
        jlvcSentences = 5,            /**< Number of sentences in job. */
        jlvcPartNum = 6,              /**< Current part of the job. */
        jlvcPartCount = 7             /**< Number of parts in job. */
    };

    /**
    * Convert a KTTSD job state integer into a display string.
    * @param state          KTTSD job state
    * @return               Display string for the state.
    */
    TQString stateToStr(int state);

    /**
    * Get the Job Number of the currently-selected job in the Job List View.
    * @return               Job Number of currently-selected job.
    *                       0 if no currently-selected job.
    */
    uint getCurrentJobNum();

    /**
    * Get the number of parts in the currently-selected job in the Job List View.
    * @return               Number of parts in currently-selected job.
    *                       0 if no currently-selected job.
    */
    int getCurrentJobPartCount();

    /**
    * Given a Job Number, returns the Job List View item containing the job.
    * @param jobNum         Job Number.
    * @return               TQListViewItem containing the job or 0 if not found.
    */
    TQListViewItem* findItemByJobNum(const uint jobNum);

    /**
    * Enables or disables all the job-related buttons.
    * @param enable        True to enable the job-related butons.  False to disable.
    */
    void enableJobActions(bool enable);

    /**
    * Enables or disables all the job part-related buttons.
    * @param enable        True to enable the job par-related butons.  False to disable.
    */
    void enableJobPartActions(bool enable);

    /**
    * Refresh display of a single job in the JobListView.
    * @param jobNum         Job Number.
    */
    void refreshJob(uint jobNum);

    /**
    * Fill the Job List View.
    */
    void refreshJobListView();

    /**
    * If nothing selected in Job List View and list not empty, select top item.
    * If nothing selected and list is empty, disable job buttons.
    */
    void autoSelectInJobListView();

    /**
    * Return the Talker ID corresponding to a Talker Code, retrieving from cached list if present.
    * @param talkerCode    Talker Code.
    * @return              Talker ID.
    */
    TQString cachedTalkerCodeToTalkerID(const TQString& talkerCode);

    /**
    * Job ListView.
    */
    TDEListView* m_jobListView;
    KttsJobMgrBrowserExtension *m_extension;

    /**
    * Current sentence box.
    */
    KTextEdit* m_currentSentence;

    /**
    * Box containing buttons.
    */
    TQVBox* m_buttonBox;

    /**
    * This flag is set to True whenever we want to select the next job that
    * is announced in a textSet signal.
    */
    bool m_selectOnTextSet;

    /**
    * Cache mapping Talker Codes to Talker IDs.
    */
    TQMap<TQString,TQString> m_talkerCodesToTalkerIDs;
};

class KttsJobMgrBrowserExtension : public KParts::BrowserExtension
{
    TQ_OBJECT
  
    friend class KttsJobMgrPart;
public:
    KttsJobMgrBrowserExtension(KttsJobMgrPart *parent);
    virtual ~KttsJobMgrBrowserExtension();
};

#endif    // _KTTSJOBMGRPART_H_

