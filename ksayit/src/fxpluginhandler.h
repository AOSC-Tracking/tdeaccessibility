//
// C++ Interface: fxpluginhandler
//
// Description: 
//
//
// Author: Robert Vogl <voglrobe@lapislazuli>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FXPLUGINHANDLER_H
#define FXPLUGINHANDLER_H

// QT includes
#include <tqobject.h>
#include <tqlibrary.h>
#include <tqstring.h>
#include <tqstringlist.h>
#include <tqmap.h>
// #include <tqvaluelist.h>

// KDE includes
#include <tdeconfig.h>
#include <arts/kartsserver.h>
#include <arts/artsflow.h>

using namespace Arts;

// App specific includes
#include "ksayit_fxplugin.h"

/**
@author Robert Vogl
*/
/** Internal structure of the effect plugin
 */
typedef struct {
    TQString name; // KService::Ptr->name()
    TQString library; // KService::Ptr->library()
    TQString description;
    long EffectID; // !=0, if effect is on the effect stack.
    FXPlugin *p; // factory->create(...)
} fx_struct;


class FXPluginHandler : public TQObject
{
TQ_OBJECT
  
public:
    FXPluginHandler(TQObject *parent = 0, const char *name = 0, TDEConfig *config=0);
    ~FXPluginHandler();

public: // Methods  
    /** Loads the activated plugins.
     */
    void readConfiguration();

    /** Loads any available plugin from the directories containing KSayIt plugins,
     *  fills the plugin_struct with the corresponding data and unloads the plugin.
     */
    void searchPlugins();
    
    /** Shows the GUI to setup the effect named name
    \param pname The name of the effect to configure.
     */ 
    void showEffectGUI(const TQString &pname);
    
    /** Activates the named effect
    \param pname Name of the effect to activate.
    \param server A pointer to the soundserver instance.
    \param fx_stack A pointer to the effect stack of the soundserver.
     */
    void activateEffect(const TQString &pname,
                        KArtsServer *server,
                        StereoEffectStack *fx_stack);

    /** Removes all effects from the soundservers effect stack.
    \param fx_stack A pointer to the effect stack of the soundserver.
     */
    void deactivateEffects(StereoEffectStack *fx_stack);
    
    /** Returns a list of all available plugins.
    \param pluginlist A reference to a TQStringList that contains the available plugins.
     */
    void getPlugins(TQStringList &pluginlist); 

private: // Attributes
    TDEConfig *m_config;
    TQMap<TQString, fx_struct> m_mapPluginList; // holds all plugins found on the system
    // TQMap<TQString, fx_struct> m_mapActivePlugins; // holds the active effects
    TQStringList m_lstActivePlugins;
          

};



#endif
