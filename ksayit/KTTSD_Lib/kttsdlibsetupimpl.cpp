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

KTTSDlibSetupImpl::KTTSDlibSetupImpl(TQWidget *parent, const char *name)
 : KTTSDlibSetup(parent, name)
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

    // if ( (fp = popen("tdecmshell --list | grep kcmkttsmgr", "r")) != NULL){
    if ( (fp = popen("tdecmshell --list | grep kcmkttsd", "r")) != NULL){
        fgets(cmdresult, 18, fp);
        pclose(fp);
    }
    if ( !TQCString(cmdresult).contains("kcmkttsd") ){
            TQString error = i18n("Control Center Module for KTTSD not found.");
            KMessageBox::sorry(this, error, i18n("Problem"));
            return;
    }

    // invoke the Control Center Module
    TDEProcess *kcmproc = new TDEProcess();
    connect(kcmproc, TQT_SIGNAL(processExited(TDEProcess*)),
                    this, TQT_SLOT(slotKCMProcessExited(TDEProcess*)) );
    (*kcmproc) << "tdecmshell";
    (*kcmproc) << "kcmkttsd";
    kcmproc->start(TDEProcess::NotifyOnExit);

    kcm_Button->setEnabled(false);
}


void KTTSDlibSetupImpl::slotKCMProcessExited(TDEProcess *p)
{
    kdDebug(100200) << "slotKCMProcessExited()" << endl;
    kcm_Button->setEnabled(true);
}



#include "kttsdlibsetupimpl.moc"
