/*
  Manages all the Talker (synth) plugins.
  -------------------
  Copyright:
  (C) 2004-2005 by Gary Cramblitt <garycramblitt@comcast.net>
  -------------------
  Original author: Gary Cramblitt <garycramblitt@comcast.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 ******************************************************************************/

// TQt includes.

// KDE includes.
#include <kdebug.h>
#include <tdeparts/componentfactory.h>
#include <ktrader.h>
#include <kstandarddirs.h>

// KTTS includes.
#include "pluginconf.h"
#include "talkermgr.h"
#include "threadedplugin.h"

/**
 * Constructor.
 */
TalkerMgr::TalkerMgr(TQObject *parent, const char *name) :
    TQObject( parent, name )
{
    m_loadedPlugIns.setAutoDelete(true);
}

/**
 * Destructor.
 */
TalkerMgr::~TalkerMgr()
{
    m_loadedPlugIns.clear();
}

/**
 * Load all the configured synth plugins,  populating loadedPlugIns structure.
 */
int TalkerMgr::loadPlugIns(TDEConfig* config)
{
    // kdDebug() << "Running: TalkerMgr::loadPlugIns()" << endl;
    int good = 0;
    int bad = 0;

    m_talkerToPlugInCache.clear();
    m_loadedPlugIns.clear();
    m_loadedTalkerCodes.clear();
    m_loadedTalkerIds.clear();

    config->setGroup("General");
    TQStringList talkerIDsList = config->readListEntry("TalkerIDs", ',');
    if (!talkerIDsList.isEmpty())
    {
        KLibFactory *factory;
        TQStringList::ConstIterator itEnd(talkerIDsList.constEnd());
        for( TQStringList::ConstIterator it = talkerIDsList.constBegin(); it != itEnd; ++it )
        {
            // kdDebug() << "Loading plugInProc for Talker ID " << *it << endl;

            // Talker ID.
            TQString talkerID = *it;

            // Set the group for the language we're loading
            config->setGroup("Talker_" + talkerID);

            // Get the DesktopEntryName of the plugin we will try to load.
            TQString desktopEntryName = config->readEntry("DesktopEntryName", TQString());

            // If a DesktopEntryName is not in the config file, it was configured before
            // we started using them, when we stored translated plugin names instead.
            // Try to convert the translated plugin name to a DesktopEntryName.
            // DesktopEntryNames are better because user can change their desktop language
            // and DesktopEntryName won't change.
            if (desktopEntryName.isEmpty())
            {
                TQString synthName = config->readEntry("PlugIn", TQString());
                // See if the translated name will untranslate.  If not, well, sorry.
                desktopEntryName = TalkerCode::TalkerNameToDesktopEntryName(synthName);
                // Record the DesktopEntryName from now on.
                if (!desktopEntryName.isEmpty()) config->writeEntry("DesktopEntryName", desktopEntryName);
            }

            // Get the talker code.
            TQString talkerCode = config->readEntry("TalkerCode", TQString());

            // Normalize the talker code.
            TQString fullLanguageCode;
            talkerCode = TalkerCode::normalizeTalkerCode(talkerCode, fullLanguageCode);

            // Find the KTTSD SynthPlugin.
            TDETrader::OfferList offers = TDETrader::self()->query(
                "KTTSD/SynthPlugin", TQString("DesktopEntryName == '%1'").arg(desktopEntryName));

            if(offers.count() > 1){
                ++bad;
                kdDebug() << "More than 1 plug in doesn't make any sense, well, let's use any" << endl;
            } else if(offers.count() < 1){
                ++bad;
                kdDebug() << "Less than 1 plug in, nothing can be done" << endl;
            } else {
                kdDebug() << "Loading " << offers[0]->library() << endl;
                factory = KLibLoader::self()->factory(offers[0]->library().latin1());
                if(factory){
                    PlugInProc *speech = 
                            KParts::ComponentFactory::createInstanceFromLibrary<PlugInProc>(
                            offers[0]->library().latin1(), this, offers[0]->library().latin1());
                    if(!speech){
                        kdDebug() << "Couldn't create the speech object from " << offers[0]->library() << endl;
                        ++bad;
                    } else {
                        if (speech->supportsAsync())
                        {
                            speech->init(config, "Talker_" + talkerID);
                            // kdDebug() << "Plug in " << desktopEntryName << " created successfully." << endl;
                            m_loadedPlugIns.append(speech);
                        } else {
                            // Synchronous plugins are run in a separate thread.
                            // Init will start the thread and it will immediately go to sleep.
                            TQString threadedPlugInName = TQString::fromLatin1("threaded") + desktopEntryName;
                            ThreadedPlugIn* speechThread = new ThreadedPlugIn(speech,
                                    this, threadedPlugInName.latin1());
                            speechThread->init(config, "Talker_" + talkerCode);
                            // kdDebug() << "Threaded Plug in " << desktopEntryName << " for language " <<  (*it).right((*it).length()-5) << " created succesfully." << endl;
                            m_loadedPlugIns.append(speechThread);
                        }
                        ++good;
                        m_loadedTalkerCodes.append(TalkerCode(talkerCode));
                        m_loadedTalkerIds.append(talkerID);
                    }
                } else {
                    kdDebug() << "Couldn't create the factory object from " << offers[0]->library() << endl;
                    ++bad;
                }
            }
        }
    }
    if(bad > 0){
        if(good == 0){
            // No plugin could be loaded.
            return -1;
        } else {
            // At least one plugin was loaded and one failed.
            return 0;
        }
    } else {
        if (good == 0)
            // No plugin could be loaded.
            return -1;
        else
            // All the plug in were loaded perfectly
            return 1;
    }
}

/**
 * Get a list of the talkers configured in KTTS.
 * @return               A TQStringList of fully-specified talker codes, one
 *                       for each talker user has configured.
 */
TQStringList TalkerMgr::getTalkers()
{
    TQStringList talkerList;
    for (int ndx = 0; ndx < int(m_loadedPlugIns.count()); ++ndx)
    {
        talkerList.append(m_loadedTalkerCodes[ndx].getTalkerCode());
    }
    return talkerList;
}

/**
 * Returns a list of all the loaded plugins.
 */
TQPtrList<PlugInProc> TalkerMgr::getLoadedPlugIns()
{
    return m_loadedPlugIns;
}

/**
 * Given a talker code, returns pointer to the closest matching plugin.
 * @param talker          The talker (language) code.
 * @return                Index to m_loadedPlugins array of Talkers.
 *
 * If a plugin has not been loaded to match the talker, returns the default
 * plugin.
 */
int TalkerMgr::talkerToPluginIndex(const TQString& talker) const
{
    // kdDebug() << "TalkerMgr::talkerToPluginIndex: matching talker " << talker << " to closest matching plugin." << endl;
    // If we have a cached match, return that.
    if (m_talkerToPlugInCache.contains(talker))
        return m_talkerToPlugInCache[talker];
    else
    {
        int winner = TalkerCode::findClosestMatchingTalker(m_loadedTalkerCodes, talker, true);
        m_talkerToPlugInCache[talker] = winner;
        return winner;
    }
}

/**
 * Given a talker code, returns pointer to the closest matching plugin.
 * @param talker          The talker (language) code.
 * @return                Pointer to closest matching plugin.
 *
 * If a plugin has not been loaded to match the talker, returns the default
 * plugin.
 *
 * TODO: When picking a talker, %KTTSD will automatically determine if text contains
 * markup and pick a talker that supports that markup, if available.  This
 * overrides all other attributes, i.e, it is treated as an automatic "top priority"
 * attribute.
 */
PlugInProc* TalkerMgr::talkerToPlugin(const TQString& talker) const
{
    int talkerNdx = talkerToPluginIndex(talker);
    return const_cast< TQPtrList<PlugInProc>* >(&m_loadedPlugIns)->at(talkerNdx);
}

/**
 * Given a talker code, returns the parsed TalkerCode of the closest matching Talker.
 * @param talker          The talker (language) code.
 * @return                Parsed TalkerCode structure.
 *
 * If a plugin has not been loaded to match the talker, returns the default
 * plugin.
 *
 * The returned TalkerCode is a copy and should be destroyed by caller.
 *
 * TODO: When picking a talker, %KTTSD will automatically determine if text contains
 * markup and pick a talker that supports that markup, if available.  This
 * overrides all other attributes, i.e, it is treated as an automatic "top priority"
 * attribute.
 */
TalkerCode* TalkerMgr::talkerToTalkerCode(const TQString& talker)
{
    int talkerNdx = talkerToPluginIndex(talker);
    return new TalkerCode(&m_loadedTalkerCodes[talkerNdx]);
}

/**
 * Given a Talker Code, returns the Talker ID of the talker that would speak
 * a text job with that Talker Code.
 * @param talkerCode     Talker Code.
 * @return               Talker ID of the talker that would speak the text job.
 */
TQString TalkerMgr::talkerCodeToTalkerId(const TQString& talkerCode)
{
    int talkerNdx = talkerToPluginIndex(talkerCode);
    return m_loadedTalkerIds[talkerNdx];
}

/**
 * Get the user's default talker.
 * @return               A fully-specified talker code.
 *
 * @see talkers
 * @see getTalkers
 */
TQString TalkerMgr::userDefaultTalker() const
{
    return m_loadedTalkerCodes[0].getTalkerCode();
}

/**
 * Determine whether the currently-configured speech plugin supports a speech markup language.
 * @param talker         Code for the talker to do the speaking.  Example "en".
 *                       If NULL, defaults to the user's default talker.
 * @param markupType     The kttsd code for the desired speech markup language.
 * @return               True if the plugin currently configured for the indicated
 *                       talker supports the indicated speech markup language.
 * @see kttsdMarkupType
 */
bool TalkerMgr::supportsMarkup(const TQString& talker, const uint /*markupType*/) const
{
    kdDebug() << "TalkerMgr::supportsMarkup: Testing talker " << talker << endl;
    TQString matchingTalker = talker;
    if (matchingTalker.isEmpty()) matchingTalker = userDefaultTalker();
    PlugInProc* plugin = talkerToPlugin(matchingTalker);
    return ( plugin->getSsmlXsltFilename() !=
            TDEGlobal::dirs()->resourceDirs("data").last() + "kttsd/xslt/SSMLtoPlainText.xsl");
}

bool TalkerMgr::autoconfigureTalker(const TQString& langCode, TDEConfig* config)
{
    // Not yet implemented.
    // return false;

    TQString languageCode = langCode;

    // Get last TalkerID from config.
    TQStringList talkerIDsList = config->readListEntry("TalkerIDs", ',');
    int lastTalkerID = 0;
    for (uint talkerIdNdx = 0; talkerIdNdx < talkerIDsList.count(); ++talkerIdNdx)
    {
        int id = talkerIDsList[talkerIdNdx].toInt();
        if (id > lastTalkerID) lastTalkerID = id;
    }

    // Assign a new Talker ID for the talker.  Wraps around to 1.
    TQString talkerID = TQString::number(lastTalkerID + 1);

    // Query for all the KTTSD SynthPlugins.
    TDETrader::OfferList offers = TDETrader::self()->query("KTTSD/SynthPlugin");

    // Iterate thru the possible plug ins.
    for(unsigned int i=0; i < offers.count() ; ++i)
    {
        // See if this plugin supports the desired language.
        TQStringList languageCodes = offers[i]->property("X-TDE-Languages").toStringList();
        if (languageCodes.contains(languageCode))
        {
            TQString desktopEntryName = offers[i]->desktopEntryName();

            // Load the plugin.
            KLibFactory *factory = KLibLoader::self()->factory(offers[0]->library().latin1());
            if (factory)
            {
                // If the factory is created successfully, instantiate the PlugInConf class for the
                // specific plug in to get the plug in configuration object.
                PlugInConf* loadedTalkerPlugIn =
                    KParts::ComponentFactory::createInstanceFromLibrary<PlugInConf>(
                        offers[0]->library().latin1(), NULL, offers[0]->library().latin1());
                if (loadedTalkerPlugIn)
                {
                    // Give plugin the language code and permit plugin to autoconfigure itself.
                    loadedTalkerPlugIn->setDesiredLanguage(languageCode);
                    loadedTalkerPlugIn->load(config, TQString("Talker_")+talkerID);

                    // If plugin was able to configure itself, it returns a full talker code.
                    TQString talkerCode = loadedTalkerPlugIn->getTalkerCode();

                    if (!talkerCode.isEmpty())
                    {
                        // Erase extraneous Talker configuration entries that might be there.
                        config->deleteGroup(TQString("Talker_")+talkerID);

                        // Let plugin save its configuration.
                        config->setGroup(TQString("Talker_")+talkerID);
                        loadedTalkerPlugIn->save(config, TQString("Talker_"+talkerID));

                        // Record configuration data.
                        config->setGroup(TQString("Talker_")+talkerID);
                        config->writeEntry("DesktopEntryName", desktopEntryName);
                        talkerCode = TalkerCode::normalizeTalkerCode(talkerCode, languageCode);
                        config->writeEntry("TalkerCode", talkerCode);

                        // Add TalkerID to configured list.
                        talkerIDsList.append(talkerID);
                        config->setGroup("General");
                        config->writeEntry("TalkerIDs", talkerIDsList.join(","));
                        config->sync();

                        // TODO: Now that we have modified the config, need a way to inform
                        // other apps, including KTTSMgr.  As this routine is likely called
                        // when KTTSMgr is not running, is not a serious problem.

                        // Success!
                        delete loadedTalkerPlugIn;
                        return true;
                    }

                    // Plugin no longer needed.
                    delete loadedTalkerPlugIn;
                }
            }
        }
    }

    return false;
}
