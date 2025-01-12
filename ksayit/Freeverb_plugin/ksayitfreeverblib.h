//
// C++ Interface: ksayitfreeverblib
//
// Description: 
//
//
// Author: Robert Vogl <voglrobe@lapislazuli>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KSAYITFREEVERBLIB_H
#define KSAYITFREEVERBLIB_H

// QT includes
#include <tqobject.h>
#include <tqstringlist.h>

// KDE includes
#include <ksimpleconfig.h>
#include <tdeapplication.h>
#include <klibloader.h>
#include <kinstance.h>

#include "ksayit_fxplugin.h"

class FreeverbPluginFactory : public KLibFactory
{
TQ_OBJECT
  
public:
    // Consturctor
    FreeverbPluginFactory(TQObject *parent=0, const char* name=0);
    
    // Destructor
    virtual ~FreeverbPluginFactory(){ delete p_instance; };
    
    TQObject* createObject(TQObject *parent=0, const char* name=0, 
            const char* className="TQObject", 
            const TQStringList &args=TQStringList());        

private:
    static TDEInstance* p_instance;

};





class FreeverbPlugin : public FXPlugin
{
TQ_OBJECT
  
public: 
    // Constructor
    FreeverbPlugin(TQObject *parent=0, const char* name=0); //, TDEApplication *Appl=0);
    
    // Destructor
    ~FreeverbPlugin();    
    
    /** sets the Main application object
    */
    void setApplication(TDEApplication *Appl);
    
    /** returns the Name of the Plugin
    */
    TQString getName_KS() const;
    
    /** returns a description of the plugin
    */
    TQString getDescription_KS() const;
    
    /** shows the GUI to configure the plugin
    */
    bool showGUI_KS();
    
    /** activate the effect
    */
    long activate_KS(KArtsServer *server,
                             StereoEffectStack *fx_stack) const;
    
    /** deactivates the effect
    */
    bool deactivate_KS(StereoEffectStack *fx_stack,
                               long EffectID ) const;
                               

private:
    TDEApplication *m_Appl;
    KSimpleConfig *m_config;

};
    


#endif

