/***************************************************** vim:set ts=4 sw=4 sts=4:
  Generic Talker Chooser Filter Configuration class.
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

// TQt includes.
#include <tqstring.h>
#include <tqhbox.h>
#include <tqlayout.h>

// KDE includes.
#include <klocale.h>
#include <klineedit.h>
#include <kdialog.h>
#include <kdialogbase.h>
#include <kcombobox.h>
#include <kpushbutton.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kregexpeditorinterface.h>
#include <ktrader.h>
#include <kparts/componentfactory.h>
#include <kfiledialog.h>

// KTTS includes.

// TalkerChooser includes.
#include "talkerchooserconf.h"
#include "talkerchooserconf.moc"

/**
* Constructor 
*/
TalkerChooserConf::TalkerChooserConf( TQWidget *parent, const char *name, const TQStringList& /*args*/) :
    KttsFilterConf(parent, name)
{
    // kdDebug() << "TalkerChooserConf::TalkerChooserConf: Running" << endl;

    // Create configuration widget.
    TQVBoxLayout *layout = new TQVBoxLayout(this, KDialog::marginHint(),
        KDialog::spacingHint(), "TalkerChooserConfigWidgetLayout");
    layout->setAlignment (TQt::AlignTop);
    m_widget = new TalkerChooserConfWidget(this, "TalkerChooserConfigWidget");
    layout->addWidget(m_widget);

    // Determine if tdeutils Regular Expression Editor is installed.
    m_reEditorInstalled = !KTrader::self()->query("KRegExpEditor/KRegExpEditor").isEmpty();
    m_widget->reEditorButton->setEnabled(m_reEditorInstalled);

    connect(m_widget->nameLineEdit, TQT_SIGNAL(textChanged(const TQString&)),
            this, TQT_SLOT(configChanged()));
    connect(m_widget->reLineEdit, TQT_SIGNAL(textChanged(const TQString&)),
            this, TQT_SLOT(configChanged()));
    connect(m_widget->reEditorButton, TQT_SIGNAL(clicked()),
            this, TQT_SLOT(slotReEditorButton_clicked()));
    connect(m_widget->appIdLineEdit, TQT_SIGNAL(textChanged(const TQString&)),
            this, TQT_SLOT(configChanged()));
    connect(m_widget->talkerButton, TQT_SIGNAL(clicked()),
            this, TQT_SLOT(slotTalkerButton_clicked()));

    connect(m_widget->loadButton, TQT_SIGNAL(clicked()),
            this, TQT_SLOT(slotLoadButton_clicked()));
    connect(m_widget->saveButton, TQT_SIGNAL(clicked()),
            this, TQT_SLOT(slotSaveButton_clicked()));
    connect(m_widget->clearButton, TQT_SIGNAL(clicked()),
            this, TQT_SLOT(slotClearButton_clicked()));

    // Set up defaults.
    defaults();
}

/**
* Destructor.
*/
TalkerChooserConf::~TalkerChooserConf(){
    // kdDebug() << "TalkerChooserConf::~TalkerChooserConf: Running" << endl;
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
void TalkerChooserConf::load(TDEConfig* config, const TQString& configGroup){
    // kdDebug() << "TalkerChooserConf::load: Running" << endl;
    config->setGroup( configGroup );
    m_widget->nameLineEdit->setText( config->readEntry( "UserFilterName", m_widget->nameLineEdit->text() ) );
    m_widget->reLineEdit->setText(
            config->readEntry("MatchRegExp", m_widget->reLineEdit->text()) );
    m_widget->appIdLineEdit->setText(
            config->readEntry("AppIDs", m_widget->appIdLineEdit->text()) );

    m_talkerCode = TalkerCode(config->readEntry("TalkerCode"), false);
    // Legacy settings.
    TQString s = config->readEntry( "LanguageCode" );
    if (!s.isEmpty()) m_talkerCode.setFullLanguageCode(s);
    s = config->readEntry( "SynthInName" );
    if (!s.isEmpty()) m_talkerCode.setPlugInName(s);
    s = config->readEntry( "Gender" );
    if (!s.isEmpty()) m_talkerCode.setGender(s);
    s = config->readEntry( "Volume" );
    if (!s.isEmpty()) m_talkerCode.setVolume(s);
    s = config->readEntry( "Rate" );
    if (!s.isEmpty()) m_talkerCode.setRate(s);

    m_widget->talkerLineEdit->setText(m_talkerCode.getTranslatedDescription());
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
void TalkerChooserConf::save(TDEConfig* config, const TQString& configGroup){
    // kdDebug() << "TalkerChooserConf::save: Running" << endl;
    config->setGroup( configGroup );
    config->writeEntry( "UserFilterName", m_widget->nameLineEdit->text() );
    config->writeEntry( "MatchRegExp", m_widget->reLineEdit->text() );
    config->writeEntry( "AppIDs", m_widget->appIdLineEdit->text().replace(" ", "") );
    config->writeEntry( "TalkerCode", m_talkerCode.getTalkerCode());
}

/** 
* This function is called to set the settings in the module to sensible
* default values. It gets called when hitting the "Default" button. The 
* default values should probably be the same as the ones the application 
* uses when started without a config file.  Note that defaults should
* be applied to the on-screen widgets; not to the config file.
*/
void TalkerChooserConf::defaults(){
    // kdDebug() << "TalkerChooserConf::defaults: Running" << endl;
    // Default name.
    m_widget->nameLineEdit->setText( i18n("Talker Chooser") );
    // Default regular expression is blank.
    m_widget->reLineEdit->setText( "" );
    // Default App ID is blank.
    m_widget->appIdLineEdit->setText( "" );
    // Default to using default Talker.
    m_talkerCode = TalkerCode( TQString(), false );
    m_widget->talkerLineEdit->setText( m_talkerCode.getTranslatedDescription() );
}

/**
 * Indicates whether the plugin supports multiple instances.  Return
 * False if only one instance of the plugin can be configured.
 * @return            True if multiple instances are possible.
 */
bool TalkerChooserConf::supportsMultiInstance() { return true; }

/**
 * Returns the name of the plugin.  Displayed in Filters tab of KTTSMgr.
 * If there can be more than one instance of a filter, it should return
 * a unique name for each instance.  The name should be TalkerCode::translated for
 * the user if possible.  If the plugin is not correctly configured,
 * return an empty string.
 * @return          Filter instance name.
 */
TQString TalkerChooserConf::userPlugInName()
{
    if (m_widget->talkerLineEdit->text().isEmpty()) return TQString();
    if (m_widget->appIdLineEdit->text().isEmpty() &&
        m_widget->reLineEdit->text().isEmpty()) return TQString();
    TQString instName = m_widget->nameLineEdit->text();
    if (instName.isEmpty()) return TQString();
    return instName;
}

void TalkerChooserConf::slotReEditorButton_clicked()
{
    // Show Regular Expression Editor dialog if it is installed.
    if ( !m_reEditorInstalled ) return;
    TQDialog *editorDialog = 
        KParts::ComponentFactory::createInstanceFromQuery<TQDialog>( "KRegExpEditor/KRegExpEditor" );
    if ( editorDialog )
    {
        // tdeutils was installed, so the dialog was found.  Fetch the editor interface.
        KRegExpEditorInterface *reEditor =
            static_cast<KRegExpEditorInterface *>(editorDialog->tqt_cast( "KRegExpEditorInterface" ) );
        Q_ASSERT( reEditor ); // This should not fail!// now use the editor.
        reEditor->setRegExp( m_widget->reLineEdit->text() );
        int dlgResult = editorDialog->exec();
        if ( dlgResult == TQDialog::Accepted )
        {
            TQString re = reEditor->regExp();
            m_widget->reLineEdit->setText( re );
        }
        delete editorDialog;
    } else return;
}

void TalkerChooserConf::slotTalkerButton_clicked()
{
    TQString talkerCode = m_talkerCode.getTalkerCode();
    SelectTalkerDlg dlg( m_widget, "selecttalkerdialog", i18n("Select Talker"), talkerCode, true );
    int dlgResult = dlg.exec();
    if ( dlgResult != KDialogBase::Accepted ) return;
    m_talkerCode = TalkerCode( dlg.getSelectedTalkerCode(), false );
    m_widget->talkerLineEdit->setText( m_talkerCode.getTranslatedDescription() );
    configChanged();
}

void TalkerChooserConf::slotLoadButton_clicked()
{
    TQString dataDir = TDEGlobal::dirs()->findAllResources("data", "kttsd/talkerchooser/").last();
    TQString filename = KFileDialog::getOpenFileName(
        dataDir,
        "*rc|Talker Chooser Config (*rc)",
        m_widget,
        "talkerchooser_loadfile");
    if ( filename.isEmpty() ) return;
    TDEConfig* cfg = new TDEConfig( filename, true, false, 0 );
    load( cfg, "Filter" );
    delete cfg;
    configChanged();
}

void TalkerChooserConf::slotSaveButton_clicked()
{
    TQString filename = KFileDialog::getSaveFileName(
        TDEGlobal::dirs()->saveLocation( "data" ,"kttsd/talkerchooser/", false ),
       "*rc|Talker Chooser Config (*rc)",
        m_widget,
        "talkerchooser_savefile");
    if ( filename.isEmpty() ) return;
    TDEConfig* cfg = new TDEConfig( filename, false, false, 0 );
    save( cfg, "Filter" );
    delete cfg;
}

void TalkerChooserConf::slotClearButton_clicked()
{
    m_widget->nameLineEdit->setText( TQString() );
    m_widget->reLineEdit->setText( TQString() );
    m_widget->appIdLineEdit->setText( TQString() );
    m_talkerCode = TalkerCode( TQString(), false );
    m_widget->talkerLineEdit->setText( m_talkerCode.getTranslatedDescription() );
    configChanged();
}
