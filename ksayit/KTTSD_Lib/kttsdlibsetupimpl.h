//
// C++ Interface: kttsdsetupimpl
//
// Description: 
//
//
// Author: Robert Vogl <voglrobe@lapislazuli>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KTTSDSETUPIMPL_H
#define KTTSDSETUPIMPL_H

// TQt includes

// KDE includes
#include <kprocess.h>

// App specific includes
#include "KTTSDlibSetup.h"

/**
@author Robert Vogl
*/
class KTTSDlibSetupImpl : public KTTSDlibSetup
{
TQ_OBJECT
  
public:
    KTTSDlibSetupImpl(TQWidget *parent = 0, const char *name = 0);

    ~KTTSDlibSetupImpl();

protected slots:
	/** Connected with the button to launch the controlcenter.
	 */
	void slotLaunchControlcenter();

private slots:

	/** Called when the Control Center Module was closed.
	 */
	void slotKCMProcessExited(TDEProcess *p);
};

#endif
