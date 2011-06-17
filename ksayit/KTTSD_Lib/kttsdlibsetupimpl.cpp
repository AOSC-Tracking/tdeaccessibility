//
// C++ Implementation: kttsdlibsetupimpl
//
// Description:
//
//
// Author: Robert Vogl <voglrobe@lapislazuli>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <stdio.h>

// TQt includes
#include <tqpushbutton.h>

// KDE includes
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

// App specific includes
#include "kttsdlibsetupimpl.h"

KTTSDlibSetupImpl::KTTSDlibSetupImpl(TQWidget *tqparent, const char *name)
 : KTTSDlibSetup(tqparent, name)
{
}


KTTSDlibSetupImpl::~KTTSDlibSetupImpl()
{
}


void KTTSDlibSetupImpl::slotLaunchControlcenter()
{
    kdDebug(100200) << "KTTSDlibSetupImpl::slotLaunchControlCenter()" << endl;

    // check if controllcenter module for KTTSD exists
    FILE *fp;
    char cmdresult[20];

    // if ( (fp = popen("kcmshell --list | grep kcmkttsmgr", "r")) != NULL){
    if ( (fp = popen("kcmshell --list | grep kcmkttsd", "r")) != NULL){
        fgets(cmdresult, 18, fp);
        pclose(fp);
    }
    if ( !TQCString(cmdresult).tqcontains("kcmkttsd") ){
            TQString error = i18n("Control Center Module for KTTSD not found.");
            KMessageBox::sorry(this, error, i18n("Problem"));
            return;
    }

    // invoke the Control Center Module
    KProcess *kcmproc = new KProcess();
    connect(kcmproc, TQT_SIGNAL(processExited(KProcess*)),
                    this, TQT_SLOT(slotKCMProcessExited(KProcess*)) );
    (*kcmproc) << "kcmshell";
    (*kcmproc) << "kcmkttsd";
    kcmproc->start(KProcess::NotifyOnExit);

    kcm_Button->setEnabled(false);
}


void KTTSDlibSetupImpl::slotKCMProcessExited(KProcess *p)
{
    kdDebug(100200) << "slotKCMProcessExited()" << endl;
    kcm_Button->setEnabled(true);
}



#include "kttsdlibsetupimpl.moc"
