/*
  Filter Configuration class.
  This is the interface definition for text filter configuration dialogs.
  -------------------
  Copyright:
  (C) 2005 by Gary Cramblitt <garycramblitt@comcast.net>
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

// C++ library includes.
#include <stdlib.h>
#include <sys/param.h>

// TQt includes.
#include <tqfile.h>
#include <tqfileinfo.h>
#include <tqstring.h>

// KDE includes.
#include <tdeglobal.h>
#include <tdelocale.h>

// PluginConf includes.
#include "filterconf.h"
#include "filterconf.moc"

/**
* Constructor 
*/
KttsFilterConf::KttsFilterConf( TQWidget *parent, const char *name) : TQWidget(parent, name){
    // kdDebug() << "KttsFilterConf::KttsFilterConf: Running" << endl;
    TQString systemPath(getenv("PATH"));
    // kdDebug() << "Path is " << systemPath << endl;
    TDEGlobal::locale()->insertCatalogue("kttsd");
    m_path = TQStringList::split(":", systemPath);
}

/**
* Destructor.
*/
KttsFilterConf::~KttsFilterConf(){
    // kdDebug() << "KttsFilterConf::~KttsFilterConf: Running" << endl;
}

/**
* This method is invoked whenever the module should read its 
* configuration (most of the times from a config file) and update the 
* user interface. This happens when the user clicks the "Reset" button in 
* the control center, to undo all of his changes and restore the currently 
* valid settings.  Note that kttsmgr calls this when the plugin is
* loaded, so it not necessary to call it in your constructor.
* The plugin should read its configuration from the specified group
* in the specified config file.
* @param config      Pointer to a TDEConfig object.
* @param configGroup Call config->setGroup with this argument before
*                    loading your configuration.
*/
void KttsFilterConf::load(TDEConfig* /*config*/, const TQString& /*configGroup*/){
    // kdDebug() << "KttsFilterConf::load: Running" << endl;
}

/**
* This function gets called when the user wants to save the settings in 
* the user interface, updating the config files or wherever the 
* configuration is stored. The method is called when the user clicks "Apply" 
* or "Ok". The plugin should save its configuration in the specified
* group of the specified config file.
* @param config      Pointer to a TDEConfig object.
* @param configGroup Call config->setGroup with this argument before
*                    saving your configuration.
*/
void KttsFilterConf::save(TDEConfig* /*config*/, const TQString& /*configGroup*/){
    // kdDebug() << "KttsFilterConf::save: Running" << endl;
}

/** 
* This function is called to set the settings in the module to sensible
* default values. It gets called when hitting the "Default" button. The 
* default values should probably be the same as the ones the application 
* uses when started without a config file.  Note that defaults should
* be applied to the on-screen widgets; not to the config file.
*/
void KttsFilterConf::defaults(){
    // kdDebug() << "KttsFilterConf::defaults: Running" << endl;
}

/**
 * Indicates whether the plugin supports multiple instances.  Return
 * False if only one instance of the plugin can be configured.
 * @return            True if multiple instances are possible.
 */
bool KttsFilterConf::supportsMultiInstance() { return false; }

/**
 * Returns the name of the plugin.  Displayed in Filters tab of KTTSMgr.
 * If there can be more than one instance of a filter, it should return
 * a unique name for each instance.  The name should be translated for
 * the user if possible.  If the plugin is not correctly configured,
 * return an empty string.
 * @return           Filter instance name.
 */
TQString KttsFilterConf::userPlugInName() { return TQString(); }

/**
 * Returns True if this filter is a Sentence Boundary Detector.
 * @return          True if this filter is a SBD.
 */
bool KttsFilterConf::isSBD() { return false; }

/**
* Return the full path to any program in the $PATH environmental variable
* @param name        The name of the file to search for.
* @returns           The path to the file on success, a blank TQString
*                    if its not found.
*/
TQString KttsFilterConf::getLocation(const TQString &name) {
    // Iterate over the path and see if 'name' exists in it. Return the
    // full path to it if it does. Else return an empty TQString.
    if (TQFile::exists(name)) return name;
    // kdDebug() << "KttsFilterConf::getLocation: Searching for " << name << " in the path.." << endl;
    // kdDebug() << m_path << endl;
    for(TQStringList::iterator it = m_path.begin(); it != m_path.end(); ++it) {
        TQString fullName = *it;
        fullName += "/";
        fullName += name;
        // The user either has the directory of the file in the path...
        if(TQFile::exists(fullName)) {
            // kdDebug() << "KttsFilterConf:getLocation: " << fullName << endl;
            return fullName;
        }
        // ....Or the file itself
        else if(TQFileInfo(*it).baseName().append(TQString(".").append(TQFileInfo(*it).extension())) == name) {
            // kdDebug() << "KttsFilterConf:getLocation: " << fullName << endl;
            return fullName;
        }
    }
    return "";
}

/*static*/ TQString KttsFilterConf::realFilePath(const TQString &filename)
{
    char realpath_buffer[MAXPATHLEN + 1];
    memset(realpath_buffer, 0, MAXPATHLEN + 1);

    /* If the path contains symlinks, get the real name */
    if (realpath( TQFile::encodeName(filename).data(), realpath_buffer) != 0) {
        //succes, use result from realpath
        return TQFile::decodeName(realpath_buffer);
    }
    return filename;
}
