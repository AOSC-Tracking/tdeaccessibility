//
// C++ Implementation: fxpluginhandler
//
// Description: 
//
//
// Author: Robert Vogl <voglrobe@lapislazuli>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

// QT includes
#include <tqdir.h>

// KDE includes
#include <kdebug.h>
#include <kapplication.h>
#include <ktrader.h>
#include <klibloader.h>
#include <kservice.h>

// App specific includes
#include "fxpluginhandler.h"
#include "ksayit_fxplugin.h"
#include "ksayit_ttsplugin.h"


FXPluginHandler::FXPluginHandler(TQObject *tqparent, const char *name, KConfig *config)
 : TQObject(tqparent, name), m_config(config)
{
    m_mapPluginList.clear();
    m_lstActivePlugins.clear();
}


FXPluginHandler::~FXPluginHandler()
{
    TQMap<TQString, fx_struct>::Iterator mit;
     
    for (mit=m_mapPluginList.begin(); mit!=m_mapPluginList.end(); ++mit){
        if ( (*mit).p != 0 ){
            delete (*mit).p;
        }
    }
}


void FXPluginHandler::searchPlugins()
{  
    kdDebug(100200) << "FXPluginHandler::searchPlugins()" << endl;
    
    TQStringList sRegistered;
    fx_struct plugin;
    
    sRegistered.clear();
    m_mapPluginList.clear();
    
    KTrader::OfferList offers = KTrader::self()->query("KSayIt/FXPlugin");
    KLibFactory *factory = NULL;
    TQString library = TQString();
    TQString name = TQString();
    
    KTrader::OfferList::Iterator it=offers.begin();
    for ( ;it!=offers.end(); ++it ){
        KService::Ptr ptr = (*it);
        library = ptr->library();
        name    = ptr->name();
        factory = KLibLoader::self()->factory( library.latin1() );
        if ( factory ){
            kdDebug(100200) << "FXPluginHandler::searchPlugins(): Plugin factory found." << endl;
            // register found plugin
            if ( !sRegistered.tqcontains( TQString(name) )){
                sRegistered.append( TQString(name) );
                plugin.name    = name;
                plugin.library = library;
                plugin.description = TQString();
                plugin.EffectID = 0;
                plugin.p = NULL;
                m_mapPluginList[plugin.name] = plugin; // update Plugin Map
                kdDebug(100200) << "FXPluginHandler::searchPlugins(): " << plugin.name << " registered." << endl;
            }            
        }
    }
}


void FXPluginHandler::readConfiguration()
{
    kdDebug(100200) << "+++ entering FXPluginHandler::readConfiguration()" << endl;
    
    fx_struct plugin;
    TQMap<TQString, fx_struct>::Iterator mit;
    TQStringList::Iterator lit;
    
    // unload all plugins and destroy the effect objects
    lit = m_lstActivePlugins.begin();
    while ( lit != m_lstActivePlugins.end() ){
        mit = m_mapPluginList.tqfind( *lit );
        if ( mit!=m_mapPluginList.end() ){
            plugin = *mit;
            if ( (plugin.p != NULL) && (plugin.EffectID == 0) ){
                delete plugin.p;
                plugin.p = NULL;
                m_mapPluginList[plugin.name] = plugin; // update Plugin Map
                lit = m_lstActivePlugins.remove( lit );                           
            } else {
                lit++;
            }
        }
    }
    
    // load active plugins as given in config file
    m_config->setGroup("Effect Stack Configuration");
    TQStringList conf_active = m_config->readListEntry("Activated");
    KLibFactory *factory = NULL;   
    
    for (lit=conf_active.begin(); lit!=conf_active.end(); ++lit){ // for all in config
        mit = m_mapPluginList.tqfind(*lit);
        if( mit!=m_mapPluginList.end() ){
            // plugin found in list of registered plugins 
            plugin = *mit;
            // load plugin            
            factory = KLibLoader::self()->factory( (plugin.library).latin1() );
            if ( factory ){
                plugin.p = static_cast<FXPlugin*>( factory->create( (TQObject*)this, (plugin.name).latin1() ) );
                if ( plugin.p ){ // Plugin found
                    plugin.p->setApplication( KApplication::kApplication() );
                    plugin.description = plugin.p->getDescription_KS();
                    // append to list of active plugins
                    m_lstActivePlugins.append( plugin.name );
                    m_mapPluginList[plugin.name] = plugin; // update Plugin Map
                    kdDebug(100200) << "FXPluginHandler: FX-Plugin " << plugin.name << " active." << endl;                       
                }
            }
        }            
    }
}


void FXPluginHandler::showEffectGUI(const TQString &pname)
{
    kdDebug(100200) << "FXPluginHandler::showEffectGUI(): " << pname << endl;
    
    TQMap<TQString, fx_struct>::Iterator mit;
    fx_struct plugin;
    
    // find plugin with name==pname in list and show its GUI
    mit = m_mapPluginList.tqfind(pname);
    if ( mit != m_mapPluginList.end() ){
        plugin = *mit;
        if ( plugin.p != NULL ){ // plugin loaded
            plugin.p->showGUI_KS();
            return;
        } else {
            kdDebug(100200) << "Effect not active yet. Loading..." << endl;
            KLibFactory *factory = KLibLoader::self()->factory( (plugin.library).latin1() );
            if ( factory ){
                plugin.p = static_cast<FXPlugin*>( factory->create( (TQObject*)this, (plugin.name).latin1() ) );
                if ( plugin.p ){ // Plugin found
                    plugin.p->setApplication( KApplication::kApplication() );
                    plugin.description = plugin.p->getDescription_KS();
                    // append to list of active plugins
                    m_lstActivePlugins.append( plugin.name );
                    m_mapPluginList[plugin.name] = plugin; // update Plugin Map
                    kdDebug(100200) << "FXPluginHandler: FX-Plugin " << plugin.name << " active." << endl;                       
                    plugin.p->showGUI_KS();
                    return;
                }
            }
        }        
    }
 }


void FXPluginHandler::getPlugins(TQStringList &pluginlist)
{
    // kdDebug(100200) << "FXPluginHandler::getPlugins" << endl;
    
    pluginlist.clear();
    TQString sPname;
    
    TQMap<TQString, fx_struct>::Iterator mit;
    for (mit=m_mapPluginList.begin(); mit!=m_mapPluginList.end(); ++mit){
        sPname = (*mit).name; //testing
        pluginlist.append( sPname );    
    }
} 


void FXPluginHandler::activateEffect(const TQString &pname,
                                   KArtsServer *server,
                                   StereoEffectStack *fx_stack)
{
    kdDebug(100200) << "FXPluginHandler::activateEffect: " << pname << endl;
    
    TQMap<TQString, fx_struct>::Iterator mit;
    fx_struct plugin;
    
    // find plugin with name==pname
    mit = m_mapPluginList.tqfind(pname);
    if ( mit!=m_mapPluginList.end() ){
        plugin = *mit;
        if ( plugin.p != NULL ){
            plugin.EffectID = plugin.p->activate_KS(server, fx_stack);
            m_mapPluginList[plugin.name] = plugin; // update Plugin Map                
        } else {
            kdDebug(100200) << "Effect " << pname << " not found or inactive." << endl;    
        }
    }
}


void FXPluginHandler::deactivateEffects(StereoEffectStack *fx_stack)
{
    kdDebug(100200) << "FXPluginHandler::deactivateEffects." << endl;
    
    TQMap<TQString, fx_struct>::Iterator mit;
    fx_struct plugin;
      
    for (mit=m_mapPluginList.begin(); mit!=m_mapPluginList.end(); ++mit){
        if ( mit != m_mapPluginList.end() ){
            plugin = *mit;
            if ( (plugin.EffectID != 0) && (plugin.p != NULL) ){
                // effect active and on stack
                plugin.p->deactivate_KS(fx_stack, plugin.EffectID);
                kdDebug(100200) << plugin.name << " with Effect ID " << plugin.EffectID << " removed." << endl;
                plugin.EffectID = 0;
                m_mapPluginList[plugin.name] = plugin; // update Plugin Map
            }
        }
    }    
}





#include "fxpluginhandler.moc"
