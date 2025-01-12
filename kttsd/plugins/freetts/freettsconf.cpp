/****************************************************************************
	Configuration widget and functions for FreeTTS (interactive) plug in
	-------------------
	Copyright : (C) 2004 Paul Giannaros
	-------------------
	Original author: Paul Giannaros <ceruleanblaze@gmail.com>
	Current Maintainer: Paul Giannaros <ceruleanblaze@gmail.com>
 ******************************************************************************/

/***************************************************************************
 *																					*
 *	 This program is free software; you can redistribute it and/or modify	*
 *	 it under the terms of the GNU General Public License as published by	*
 *	 the Free Software Foundation; version 2 of the License.				 *
 *																					 *
 ***************************************************************************/

// TQt includes. 
#include <tqlayout.h>
#include <tqlabel.h>
#include <tqstring.h>
#include <tqstringlist.h>
#include <tqfile.h>
#include <tqapplication.h>

// KDE includes.
#include <kdialog.h>
#include <tdetempfile.h>
#include <kstandarddirs.h>
#include <tdemessagebox.h>
#include <tdelocale.h>
#include <kprogress.h>

// KTTS includes.
#include <pluginconf.h>
#include <testplayer.h>

// FreeTTS includes.
#include "freettsconf.h"
#include "freettsconfigwidget.h"

/** Constructor */
FreeTTSConf::FreeTTSConf( TQWidget* parent, const char* name, const TQStringList&/*args*/) : 
	PlugInConf( parent, name ) {
	
	// kdDebug() << "FreeTTSConf::FreeTTSConf: Running" << endl;
	m_freettsProc = 0;
        m_progressDlg = 0;
	
	TQVBoxLayout *layout = new TQVBoxLayout(this, KDialog::marginHint(),
								KDialog::spacingHint(), "FreeTTSConfigWidgetLayout");
	layout->setAlignment (TQt::AlignTop);
	m_widget = new FreeTTSConfWidget(this, "FreeTTSConfigWidget");
	layout->addWidget(m_widget);
		
	defaults();
	
	connect(m_widget->freettsPath, TQ_SIGNAL(textChanged(const TQString&)),
		this, TQ_SLOT(configChanged()));
	connect(m_widget->freettsTest, TQ_SIGNAL(clicked()), this, TQ_SLOT(slotFreeTTSTest_clicked()));
}

/** Destructor */
FreeTTSConf::~FreeTTSConf() {
	// kdDebug() << "Running: FreeTTSConf::~FreeTTSConf()" << endl;
	if (!m_waveFile.isNull()) TQFile::remove(m_waveFile);
	delete m_freettsProc;
        delete m_progressDlg;
}

void FreeTTSConf::load(TDEConfig *config, const TQString &configGroup) {
	// kdDebug() << "FreeTTSConf::load: Running" << endl;

	config->setGroup(configGroup);
        TQString freeTTSJar = config->readEntry("FreeTTSJarPath", TQString());
        if (freeTTSJar.isEmpty())
        {
            config->setGroup("FreeTTS");
            freeTTSJar = config->readEntry("FreeTTSJarPath", TQString());
        }
	if (freeTTSJar.isEmpty())
	    freeTTSJar = getLocation("freetts.jar");
        m_widget->freettsPath->setURL(freeTTSJar);
	/// If freettsPath is still empty, then we couldn't find the file in the path.
}

void FreeTTSConf::save(TDEConfig *config, const TQString &configGroup){
	// kdDebug() << "FreeTTSConf::save: Running" << endl;

    config->setGroup("FreeTTS");
    config->writeEntry("FreeTTSJarPath",
        realFilePath(m_widget->freettsPath->url()));

    config->setGroup(configGroup);
    if(m_widget->freettsPath->url().isEmpty())
        KMessageBox::sorry(0, i18n("Unable to locate freetts.jar in your path.\nPlease specify the path to freetts.jar in the Properties tab before using TDE Text-to-Speech"), i18n("TDE Text-to-Speech"));
    config->writeEntry("FreeTTSJarPath",
        realFilePath(m_widget->freettsPath->url()));
}

void FreeTTSConf::defaults(){
	// kdDebug() << "Running: FreeTTSConf::defaults()" << endl;
	m_widget->freettsPath->setURL("");
}

void FreeTTSConf::setDesiredLanguage(const TQString &lang)
{
    m_languageCode = lang;
}

TQString FreeTTSConf::getTalkerCode()
{
    TQString freeTTSJar = realFilePath(m_widget->freettsPath->url());
    if (!freeTTSJar.isEmpty())
    {
        if (!getLocation(freeTTSJar).isEmpty())
        {
            return TQString(
                    "<voice lang=\"%1\" name=\"%2\" gender=\"%3\" />"
                    "<prosody volume=\"%4\" rate=\"%5\" />"
                    "<kttsd synthesizer=\"%6\" />")
                    .arg(m_languageCode)
                    .arg("fixed")
                    .arg("neutral")
                    .arg("medium")
                    .arg("medium")
                    .arg("FreeTTS");
        }
    }
    return TQString();
}

// TQString FreeTTSConf::getLocation(const TQString &name) {
// 	/// Iterate over the path and see if 'name' exists in it. Return the
// 	/// full path to it if it does. Else return an empty TQString.
// 	kdDebug() << "FreeTTSConf::getLocation: Searching for " << name << " in the path... " << endl;
// 	kdDebug() << m_path << endl;
// 	for(TQStringList::iterator it = m_path.begin(); it != m_path.end(); ++it) {
// 		TQString fullName = *it;
// 		fullName += "/";
// 		fullName += name;
// 		/// The user either has the directory of the file in the path...
// 		if(TQFile::exists(fullName)) {
// 			return fullName;
// 			kdDebug() << fullName << endl;
// 		}
// 		/// ....Or the file itself
// 		else if(TQFileInfo(*it).baseName().append(TQString(".").append(TQFileInfo(*it).extension())) == name) {
// 			return fullName;
// 			kdDebug() << fullName << endl;
// 		}
// 	}
// 	return "";
// }


void FreeTTSConf::slotFreeTTSTest_clicked()
{
	// kdDebug() << "FreeTTSConf::slotFreeTTSTest_clicked(): Running" << endl;
        // If currently synthesizing, stop it.
	if (m_freettsProc)
		m_freettsProc->stopText();
	else
        {
		m_freettsProc = new FreeTTSProc();
                connect (m_freettsProc, TQ_SIGNAL(stopped()), this, TQ_SLOT(slotSynthStopped()));
        }
        // Create a temp file name for the wave file.
	KTempFile tempFile (locateLocal("tmp", "freettsplugin-"), ".wav");
	TQString tmpWaveFile = tempFile.file()->name();
	tempFile.close();

    // Get test message in the language of the voice.
    TQString testMsg = testMessage(m_languageCode);

        // Tell user to wait.
        m_progressDlg = new KProgressDialog(m_widget, "kttsmgr_freetts_testdlg",
            i18n("Testing"),
            i18n("Testing."),
            true);
        m_progressDlg->progressBar()->hide();
        m_progressDlg->setAllowCancel(true);

	// I think FreeTTS only officialy supports English, but if anyone knows of someone
	// whos built up a different language lexicon and has it working with FreeTTS gimme an email at ceruleanblaze@gmail.com
        connect (m_freettsProc, TQ_SIGNAL(synthFinished()), this, TQ_SLOT(slotSynthFinished()));
        m_freettsProc->synth(
            testMsg,
            tmpWaveFile,
            realFilePath(m_widget->freettsPath->url()));

        // Display progress dialog modally.  Processing continues when plugin signals synthFinished,
        // or if user clicks Cancel button.
        m_progressDlg->exec();
        disconnect (m_freettsProc, TQ_SIGNAL(synthFinished()), this, TQ_SLOT(slotSynthFinished()));
        if (m_progressDlg->wasCancelled()) m_freettsProc->stopText();
        delete m_progressDlg;
        m_progressDlg = 0;
}

void FreeTTSConf::slotSynthFinished()
{
    // If user canceled, progress dialog is gone, so exit.
    if (!m_progressDlg)
    {
        m_freettsProc->ackFinished();
        return;
    }
    // Hide the Cancel button so user can't cancel in the middle of playback.
    m_progressDlg->showCancelButton(false);
    // Get new wavefile name.
    m_waveFile = m_freettsProc->getFilename();
    // Tell synth we're done.
    m_freettsProc->ackFinished();
    // Play the wave file (possibly adjusting its Speed).
    // Player object deletes the wave file when done.
    if (m_player) m_player->play(m_waveFile);
    TQFile::remove(m_waveFile);
    m_waveFile = TQString();
    if (m_progressDlg) m_progressDlg->close();
}

void FreeTTSConf::slotSynthStopped()
{
    // Clean up after canceling test.
    TQString filename = m_freettsProc->getFilename();
    if (!filename.isNull()) TQFile::remove(filename);
}

#include "freettsconf.moc"
