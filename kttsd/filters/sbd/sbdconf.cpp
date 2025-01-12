/*
  Sentence Boundary Detection Filter Configuration class.
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
#include <tqfile.h>
#include <tqfileinfo.h>
#include <tqstring.h>
#include <tqhbox.h>
#include <tqlayout.h>
#include <tqdom.h>
#include <tqfile.h>
#include <tqradiobutton.h>

// KDE includes.
#include <tdeglobal.h>
#include <tdelocale.h>
#include <tdelistview.h>
#include <klineedit.h>
#include <kdialog.h>
#include <kdialogbase.h>
#include <kpushbutton.h>
#include <tdeconfig.h>
#include <kstandarddirs.h>
#include <kregexpeditorinterface.h>
#include <ktrader.h>
#include <tdeparts/componentfactory.h>
#include <tdefiledialog.h>

// KTTS includes.
#include "filterconf.h"

// SBD includes.
#include "sbdconf.h"
#include "sbdconf.moc"

/**
* Constructor 
*/
SbdConf::SbdConf( TQWidget *parent, const char *name, const TQStringList& /*args*/) :
    KttsFilterConf(parent, name)
{
    // kdDebug() << "SbdConf::SbdConf: Running" << endl;

    // Create configuration widget.
    TQVBoxLayout *layout = new TQVBoxLayout(this, KDialog::marginHint(),
        KDialog::spacingHint(), "SbdConfigWidgetLayout");
    layout->setAlignment (TQt::AlignTop);
    m_widget = new SbdConfWidget(this, "SbdConfigWidget");
    layout->addWidget(m_widget);

    // Determine if tdeutils Regular Expression Editor is installed.
    m_reEditorInstalled = !TDETrader::self()->query("KRegExpEditor/KRegExpEditor").isEmpty();

    m_widget->reButton->setEnabled( m_reEditorInstalled );
    if ( m_reEditorInstalled )
        connect( m_widget->reButton, TQ_SIGNAL(clicked()), this, TQ_SLOT(slotReButton_clicked()) );

    connect( m_widget->reLineEdit, TQ_SIGNAL(textChanged(const TQString&)),
         this, TQ_SLOT(configChanged()) );
    connect( m_widget->sbLineEdit, TQ_SIGNAL(textChanged(const TQString&)),
         this, TQ_SLOT(configChanged()) );
    connect( m_widget->nameLineEdit, TQ_SIGNAL(textChanged(const TQString&)),
         this, TQ_SLOT(configChanged()) );
    connect( m_widget->appIdLineEdit, TQ_SIGNAL(textChanged(const TQString&)),
         this, TQ_SLOT(configChanged()) );
    connect(m_widget->languageBrowseButton, TQ_SIGNAL(clicked()),
         this, TQ_SLOT(slotLanguageBrowseButton_clicked()));
    connect(m_widget->loadButton, TQ_SIGNAL(clicked()),
         this, TQ_SLOT(slotLoadButton_clicked()));
    connect(m_widget->saveButton, TQ_SIGNAL(clicked()),
         this, TQ_SLOT(slotSaveButton_clicked()));
    connect(m_widget->clearButton, TQ_SIGNAL(clicked()),
         this, TQ_SLOT(slotClearButton_clicked()));

    // Set up defaults.
    defaults();
}

/**
* Destructor.
*/
SbdConf::~SbdConf(){
    // kdDebug() << "SbdConf::~SbdConf: Running" << endl;
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
void SbdConf::load(TDEConfig* config, const TQString& configGroup){
    // kdDebug() << "SbdConf::load: Running" << endl;
    config->setGroup( configGroup );
    m_widget->nameLineEdit->setText( 
        config->readEntry("UserFilterName", m_widget->nameLineEdit->text()) );
    m_widget->reLineEdit->setText(
        config->readEntry("SentenceDelimiterRegExp", m_widget->reLineEdit->text()) );
    m_widget->sbLineEdit->setText(
        config->readEntry("SentenceBoundary", m_widget->sbLineEdit->text()) );
    TQStringList langCodeList = config->readListEntry("LanguageCodes");
    if (!langCodeList.isEmpty())
        m_languageCodeList = langCodeList;
    TQString language = "";
    for ( uint ndx=0; ndx < m_languageCodeList.count(); ++ndx)
    {
        if (!language.isEmpty()) language += ",";
        language += TDEGlobal::locale()->twoAlphaToLanguageName(m_languageCodeList[ndx]);
    }
    m_widget->languageLineEdit->setText(language);
    m_widget->appIdLineEdit->setText(
            config->readEntry("AppID", m_widget->appIdLineEdit->text()) );
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
void SbdConf::save(TDEConfig* config, const TQString& configGroup){
    // kdDebug() << "SbdConf::save: Running" << endl;
    config->setGroup( configGroup );
    config->writeEntry("UserFilterName", m_widget->nameLineEdit->text() );
    config->writeEntry("SentenceDelimiterRegExp", m_widget->reLineEdit->text() );
    config->writeEntry("SentenceBoundary", m_widget->sbLineEdit->text() );
    config->writeEntry("LanguageCodes", m_languageCodeList );
    config->writeEntry("AppID", m_widget->appIdLineEdit->text().replace(" ", "") );
}

/** 
* This function is called to set the settings in the module to sensible
* default values. It gets called when hitting the "Default" button. The 
* default values should probably be the same as the ones the application 
* uses when started without a config file.  Note that defaults should
* be applied to the on-screen widgets; not to the config file.
*/
void SbdConf::defaults(){
    // kdDebug() << "SbdConf::defaults: Running" << endl;
    m_widget->nameLineEdit->setText( i18n("Standard Sentence Boundary Detector") );
    m_widget->reLineEdit->setText( "([\\.\\?\\!\\:\\;])(\\s|$|(\\n *\\n))" );
    m_widget->sbLineEdit->setText( "\\1\\t" );
    m_languageCodeList.clear();
    m_widget->languageLineEdit->setText( "" );
    m_widget->appIdLineEdit->setText( "" );
    // kdDebug() << "SbdConf::defaults: Exiting" << endl;
}

/**
 * Indicates whether the plugin supports multiple instances.  Return
 * False if only one instance of the plugin can be configured.
 * @return            True if multiple instances are possible.
 */
bool SbdConf::supportsMultiInstance() { return true; }

/**
 * Returns the name of the plugin.  Displayed in Filters tab of KTTSMgr.
 * If there can be more than one instance of a filter, it should return
 * a unique name for each instance.  The name should be translated for
 * the user if possible.  If the plugin is not correctly configured,
 * return an empty string.
 * @return          Filter instance name.
 */
TQString SbdConf::userPlugInName()
{
    if ( m_widget->reLineEdit->text().isEmpty() )
        return TQString();
    else
        return m_widget->nameLineEdit->text();
}

/**
 * Returns True if this filter is a Sentence Boundary Detector.
 * @return          True if this filter is a SBD.
 */
bool SbdConf::isSBD() { return true; }

void SbdConf::slotReButton_clicked()
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
            configChanged();
        }
        delete editorDialog;
    } else return;
}

void SbdConf::slotLanguageBrowseButton_clicked()
{
    // Create a  TQHBox to host TDEListView.
    TQHBox* hBox = new TQHBox(m_widget, "SelectLanguage_hbox");
    // Create a TDEListView and fill with all known languages.
    TDEListView* langLView = new TDEListView(hBox, "SelectLanguage_lview");
    langLView->addColumn(i18n("Language"));
    langLView->addColumn(i18n("Code"));
    langLView->setSelectionMode(TQListView::Extended);
    TQStringList allLocales = TDEGlobal::locale()->allLanguagesTwoAlpha();
    TQString locale;
    TQString languageCode;
    TQString countryCode;
    TQString charSet;
    TQString language;
    // Blank line so user can select no language.
    TQListViewItem* item = new TDEListViewItem(langLView, "", "");
    if (m_languageCodeList.isEmpty()) item->setSelected(true);
    const int allLocalesCount = allLocales.count();
    for (int ndx=0; ndx < allLocalesCount; ++ndx)
    {
        locale = allLocales[ndx];
        TDEGlobal::locale()->splitLocale(locale, languageCode, countryCode, charSet);
        language = TDEGlobal::locale()->twoAlphaToLanguageName(languageCode);
        if (!countryCode.isEmpty()) language +=
            " (" + TDEGlobal::locale()->twoAlphaToCountryName(countryCode)+")";
        TQListViewItem* item = new TDEListViewItem(langLView, language, locale);
        if (m_languageCodeList.contains(locale)) item->setSelected(true);
    }
    // Sort by language.
    langLView->setSorting(0);
    langLView->sort();
    // Display the box in a dialog.
    KDialogBase* dlg = new KDialogBase(
            KDialogBase::Swallow,
    i18n("Select Languages"),
    KDialogBase::Help|KDialogBase::Ok|KDialogBase::Cancel,
    KDialogBase::Cancel,
    m_widget,
    "SelectLanguage_dlg",
    true,
    true);
    dlg->setMainWidget(hBox);
    dlg->setHelp("", "kttsd");
    dlg->setInitialSize(TQSize(300, 500), false);
    int dlgResult = dlg->exec();
    languageCode = TQString();
    if (dlgResult == TQDialog::Accepted)
    {
        m_languageCodeList.clear();
        TQListViewItem* item = langLView->firstChild();
        while (item)
        {
            if (item->isSelected()) m_languageCodeList += item->text(1);
            item = item->nextSibling();
        }
    }
    delete dlg;
    // TODO: Also delete TDEListView and TQHBox?
    if (dlgResult != TQDialog::Accepted) return;
    language = "";
    for ( uint ndx=0; ndx < m_languageCodeList.count(); ++ndx)
    {
        if (!language.isEmpty()) language += ",";
        language += TDEGlobal::locale()->twoAlphaToLanguageName(m_languageCodeList[ndx]);
    }
    m_widget->languageLineEdit->setText(language);
    configChanged();
}

void SbdConf::slotLoadButton_clicked()
{
    // TQString dataDir = TDEGlobal::dirs()->resourceDirs("data").last() + "/kttsd/stringreplacer/";
    TQString dataDir = TDEGlobal::dirs()->findAllResources("data", "kttsd/sbd/").last();
    TQString filename = KFileDialog::getOpenFileName(
        dataDir,
        "*rc|SBD Config (*rc)",
        m_widget,
        "sbd_loadfile");
    if ( filename.isEmpty() ) return;
    TDEConfig* cfg = new TDEConfig( filename, true, false, 0 );
    load( cfg, "Filter" );
    delete cfg;
    configChanged();
}

void SbdConf::slotSaveButton_clicked()
{
    TQString filename = KFileDialog::getSaveFileName(
        TDEGlobal::dirs()->saveLocation( "data" ,"kttsd/sbd/", false ),
        "*rc|SBD Config (*rc)",
        m_widget,
        "sbd_savefile");
    if ( filename.isEmpty() ) return;
    TDEConfig* cfg = new TDEConfig( filename, false, false, 0 );
    save( cfg, "Filter" );
    delete cfg;
}

void SbdConf::slotClearButton_clicked()
{
    m_widget->nameLineEdit->setText( TQString() );
    m_widget->reLineEdit->setText( TQString() );
    m_widget->sbLineEdit->setText( TQString() );
    m_languageCodeList.clear();
    m_widget->languageLineEdit->setText( TQString() );
    m_widget->appIdLineEdit->setText( TQString() );
    configChanged();
}
