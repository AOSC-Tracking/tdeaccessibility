/***************************************************************************
                          kmousetool.cpp  -  description
                             -------------------
    begin                : Sun Jan 20 23:27:58 PST 2002
    copyright            : (C) 2002-2003 by Jeff Roush
    email                : jeff@mousetool.com
    copyright            : (C) 2003 by Olaf Schmidt
    email                : ojschmidt@kde.org
    copyright            : (C) 2003 by Gunnar Schmi Dt
    email                : gunnar@schmi-dt.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "Xmd_kmousetool.h"
#include "kmousetool.h"
#include "kmousetool.moc"
#include <tdeconfig.h>
#include <X11/Intrinsic.h>     /* Intrinsics Definitions*/
#include <X11/StringDefs.h>    /* Standard Name-String definitions*/
#include <X11/extensions/xtestext1.h>    /* Standard Name-String definitions*/
#include <X11/extensions/XTest.h>    /* Standard Name-String definitions*/
#include <kdialog.h>
#include <tdelocale.h>
#include <tqpushbutton.h>
#include <tqpoint.h>
#include <tqnamespace.h>
#include <tdemessagebox.h>
#include <kaudioplayer.h>
#include <kstandarddirs.h>
#include <tqsound.h>
#include <tdeglobalsettings.h>
#include <kdebug.h>
#include <tqlayout.h>
#include <tqlineedit.h>
#include <tqcheckbox.h>
#include <ksystemtray.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>
#include <knuminput.h>
#include <tdepopupmenu.h>
#include <tdeaboutapplication.h>

//#include <arts/soundserver.h>
#include <netwm.h>

#include <iostream>

#include "mtstroke.h"

//using namespace Arts;

int currentXPosition;
int currentYPosition;

//using namespace std;

#undef long

/**
* Gnarly X functions
*/
void queryPointer(Window *pidRoot, int *pnRx, int *pnRy, int *pnX, int *pnY, unsigned int *puMask);
int CursorHasMoved(int minMovement);
void LeftClick();
void RightClick();
void DoubleClick();
void LeftDn();
void LeftUp();

// Declarations
Display *display;

//SimpleSoundServer sound_server(Reference("global:Arts_SimpleSoundServer"));


void KMouseTool::init_vars()
{
	continue_timer = 1;
	tick_count = 0;
	max_ticks = dwell_time+1;
	mouse_is_down = FALSE;

	loadOptions();

	// If the ~/.mousetool directory doesn't exist, create it
//	mSoundFileName = TQDir::homeDirPath();
	mSoundFileName = locate("appdata", "sounds/mousetool_tap.wav");
	mplayer = new KAudioPlayer(mSoundFileName);


	// find application file
	appfilename = locate("exe", "kmousetool");

	// find the user's autostart directory
	autostartdirname = TDEGlobalSettings::autostartPath();

	// SimpleSoundServer server(Reference("global:Arts_SimpleSoundServer"));
//	sound_server(Reference("global:Arts_SimpleSoundServer"));

	TQDesktopWidget *d = TQApplication::desktop();
	int w = d->width();
	int h = d->height();
	MTStroke::setUpperLeft(0,0);
	MTStroke::setUpperRight(w-1,0);
	MTStroke::setLowerLeft(0,h-1);
	MTStroke::setLowerRight(w-1,h-1);
}

void KMouseTool::resetSettings()
{
	cbDrag ->setChecked(smart_drag_on);
	cbStart->setChecked(isAutostart());
	cbClick->setChecked(playSound);
	cbStroke->setChecked(strokesEnabled);
	movementEdit->setValue(min_movement);
	dwellTimeEdit->setValue(dwell_time);
	dragTimeEdit->setValue(drag_time);
	settingsChanged();
}

void KMouseTool::setDefaultSettings()
{
	cbDrag ->setChecked(false);
	cbStart->setChecked(false);
	cbClick->setChecked(false);
	cbStroke->setChecked(false);
	movementEdit->setValue(5);
	dwellTimeEdit->setValue(5);
	dragTimeEdit->setValue(3);
	settingsChanged();
}


void KMouseTool::timerEvent( TQTimerEvent * )
{
	if (!mousetool_is_running)
		return;

	if (!continue_timer) {
		this->killTimers();
		return;
	}

	max_ticks = dwell_time + drag_time;
	stroke.addPt(currentXPosition, currentYPosition);

	moving = moving?CursorHasMoved(1):CursorHasMoved(min_movement);
	if (moving) {
		if (mousetool_just_started) {
			mousetool_just_started = false;
			tick_count = max_ticks;
		}
		else
			tick_count = 0;
		return;
	}

	if (tick_count<max_ticks)
		tick_count++;


	// If the mouse has paused ...
	if (tick_count==dwell_time) {
		int strokeType = stroke.getStrokeType();

		// if we're dragging the mouse, ignore stroke type
		if (mouse_is_down) {
			normalClick();
			return;
		}

		if (!strokesEnabled) {
			normalClick();
			return;
		}
		if (strokeType == MTStroke::DontClick)
			return;
		if (strokeType==MTStroke::bumped_mouse)
			return;

		if (strokeType==MTStroke::RightClick || strokeType==MTStroke::UpperRightStroke)
			RightClick();
		else if (strokeType==MTStroke::DoubleClick || strokeType==MTStroke::LowerLeftStroke)
			DoubleClick();
		else
			normalClick();
	}
}

void KMouseTool::normalClick()
{
	if (smart_drag_on) {
		if (!mouse_is_down) {
			LeftDn();
			mouse_is_down = TRUE;
			tick_count = 0;
			playTickSound();
		}
		else if (mouse_is_down) {
			LeftUp();
			mouse_is_down = FALSE;
			tick_count = max_ticks;
		}
	}
	else {
		// not smart_drag_on
		LeftClick();
		playTickSound();
	}
}

// This function isn't happy yet.
void KMouseTool::playTickSound()
{
	if (!playSound)
	return;

	mplayer->play();
	// This is a bit slow.
//	KAudioPlayer::play(mSoundFileName);
	return;

	// the solution seems to be to use MCOP,
	// but SimpleSoundServer is not cooperating

	// When using the following code, make sure to link with
	// Put " -lsoundserver_idl" in the linker options in KDevelop.

//  SimpleSoundServer server = SimpleSoundServer::_fromString("global:Arts_SimpleSoundServer");
//	SimpleSoundServer *ss = new SimpleSoundServer(Reference("global:Arts_SimpleSoundServer"));
//	SimpleSoundServer sound_server2(Reference("global:Arts_SimpleSoundServer"));

//	if(sound_server.isNull())
//		return;
//	sound_server.play(mSoundFileName.latin1());

	// Another approach is to try using TQSound.
	// TQSound depends on Network Audio Server, which doesn't seem to work on my Debian Woody system.
	// I haven't spent much time working on it, though.
//	TQSound::play(mSoundFileName);
}

KMouseTool::KMouseTool(TQWidget *parent, const char *name) : KMouseToolUI(parent, name)
{
	init_vars();
	resetSettings();

	connect(movementEdit, TQ_SIGNAL(valueChanged(int)), this, TQ_SLOT(settingsChanged()));
	connect(dwellTimeEdit, TQ_SIGNAL(valueChanged(int)), this, TQ_SLOT(settingsChanged()));
	connect(dragTimeEdit, TQ_SIGNAL(valueChanged(int)), this, TQ_SLOT(settingsChanged()));
	connect(cbDrag, TQ_SIGNAL(stateChanged(int)), this, TQ_SLOT(settingsChanged()));
	connect(cbStroke, TQ_SIGNAL(stateChanged(int)), this, TQ_SLOT(settingsChanged()));
	connect(cbClick, TQ_SIGNAL(stateChanged(int)), this, TQ_SLOT(settingsChanged()));
	connect(cbStart, TQ_SIGNAL(stateChanged(int)), this, TQ_SLOT(settingsChanged()));

	connect(buttonStartStop, TQ_SIGNAL(clicked()), this, TQ_SLOT(startStopSelected()));
	buttonDefault->setGuiItem(KStdGuiItem::defaults());
	connect(buttonDefault, TQ_SIGNAL(clicked()), this, TQ_SLOT(defaultSelected()));
	connect(buttonReset, TQ_SIGNAL(clicked()), this, TQ_SLOT(resetSelected()));
	buttonApply->setGuiItem(KStdGuiItem::apply());
	connect(buttonApply, TQ_SIGNAL(clicked()), this, TQ_SLOT(applySelected()));
	buttonHelp->setGuiItem(KStdGuiItem::help());
	connect(buttonHelp, TQ_SIGNAL(clicked()), this, TQ_SLOT(helpSelected()));
	buttonClose->setGuiItem(KStdGuiItem::close());
	connect(buttonClose, TQ_SIGNAL(clicked()), this, TQ_SLOT(closeSelected()));
#if TDE_VERSION >= TDE_MAKE_VERSION (3,1,90)
	buttonQuit->setGuiItem(KStdGuiItem::quit());
#endif // KDE 3.2
	connect(buttonQuit, TQ_SIGNAL(clicked()), this, TQ_SLOT(quitSelected()));

	// When we first start, it's nice to not click immediately.
	// So, tell MT we're just starting
	mousetool_just_started = true;

	startTimer(100);
	trayIcon = new KMouseToolTray (this, "systemTrayIcon");
	updateStartStopText ();
	connect(trayIcon, TQ_SIGNAL(startStopSelected()), this, TQ_SLOT(startStopSelected()));
	connect(trayIcon, TQ_SIGNAL(configureSelected()), this, TQ_SLOT(configureSelected()));
	connect(trayIcon, TQ_SIGNAL(aboutSelected()), this, TQ_SLOT(aboutSelected()));
	connect(trayIcon, TQ_SIGNAL(helpSelected()), this, TQ_SLOT(helpSelected()));
	connect(trayIcon, TQ_SIGNAL(quitSelected()), this, TQ_SLOT(quitSelected()));

	aboutDlg = new TDEAboutApplication (0, "KMouseToolDlg", false);
}

KMouseTool::~KMouseTool()
{
}

// *********************************************************
// Raw X functions
// Begin mouse monitoring and event generation code.
// these should be moved to a separate file.
void LeftClick()
{
	XTestFakeButtonEvent(display, 1, TRUE, 0);
	XTestFakeButtonEvent(display, 1, FALSE, 0);
}

void DoubleClick()
{
	XTestFakeButtonEvent(display, 1, TRUE, 0);
	XTestFakeButtonEvent(display, 1, FALSE, 100);
	XTestFakeButtonEvent(display, 1, TRUE, 200);
	XTestFakeButtonEvent(display, 1, FALSE, 300);
}

void RightClick()
{
	XTestFakeButtonEvent(display, 3, TRUE, 0);
	XTestFakeButtonEvent(display, 3, FALSE, 0);
}


void LeftDn()
{
	XTestFakeButtonEvent(display, 1, TRUE, 0);
}


void LeftUp()
{
	XTestFakeButtonEvent(display, 1, FALSE, 0);
}


void queryPointer(Window *pidRoot, int *pnRx, int *pnRy, int *pnX, int *pnY, unsigned int *puMask)
{
	Window id2, idRoot;
	int screen_num;

	screen_num = DefaultScreen(display);
	idRoot = RootWindow(display,screen_num);
	XQueryPointer(display, idRoot, &idRoot, &id2, pnRx, pnRy, pnX, pnY, puMask);

	*pidRoot = idRoot;
}

// return 1 if cursor has moved, 0 otherwise
int CursorHasMoved (int minMovement)
{
	static int nOldRootX = -1;
	static int nOldRootY = -1;

//	XEvent evButtonEvent;
	Window idRootWin;
	int nRootX, nRootY, nWinX, nWinY;
	unsigned int uintMask;

	queryPointer(&idRootWin, &nRootX, &nRootY, &nWinX, &nWinY, &uintMask);

	int nRes = 0;
	if ((nRootX>nOldRootX?nRootX-nOldRootX:nOldRootX-nRootX) >= minMovement)
		nRes = 1;
	if ((nRootY>nOldRootY?nRootY-nOldRootY:nOldRootY-nRootY) >= minMovement)
		nRes = 1;

	currentXPosition = nRootX;
	currentYPosition = nRootY;

	if (nRes) {
		nOldRootX = nRootX;
		nOldRootY = nRootY;
	}

	return nRes;
}
// End mouse monitoring and event creation code


bool KMouseTool::isAutostart()
{
	TQString sym = autostartdirname;
	sym += "kmousetool";			// sym is now full path to symlink
	TQFileInfo fi(sym);

	return fi.exists();
}

void KMouseTool::setAutostart (bool start)
{
	TQString sym = autostartdirname;
	sym += "kmousetool";			// sym is now full path to symlink
	TQFileInfo fi(sym);
	TQString cmd;

	if (start) {
		if (!fi.exists())  			// if it doesn't exist, make it
		cmd.sprintf("ln -s %s %s", appfilename.latin1(), autostartdirname.latin1());
	}
	else {
	if (fi.exists()) 			// if it exists, delete it
		cmd.sprintf("rm -f %s", sym.latin1());
	}
	system(cmd.ascii());
}

bool KMouseTool::applySettings()
{
	int drag, dwell;

	dwell = dwellTimeEdit->value();
	drag = dragTimeEdit->value() ;

	// The drag time must be less than the dwell time
	if ( dwell < drag) {
		KMessageBox::sorry(this, i18n("The drag time must be less than or equal to the dwell time."), i18n("Invalid Value"));
		return false;
	}

	// if we got here, we must be okay.
	min_movement   = movementEdit->value();
	smart_drag_on  = cbDrag->isChecked();
	playSound      = cbClick->isChecked();
	strokesEnabled = cbStroke->isChecked();
	setAutostart (cbStart->isChecked());

	dwell_time = dwell;
	drag_time  = drag;
	tick_count = max_ticks;

	saveOptions();
	settingsChanged();
	return true;
}

// Save options to kmousetoolrc file
void KMouseTool::loadOptions()
{
	kapp->config()->setGroup("UserOptions");

	playSound = kapp->config()->readBoolEntry("AudibleClick",false);
	smart_drag_on = kapp->config()->readBoolEntry("SmartDrag",false);

	dwell_time = kapp->config()->readNumEntry("DwellTime",5);
	drag_time = kapp->config()->readNumEntry("DragTime",3);
	min_movement = kapp->config()->readNumEntry("Movement",5);
	strokesEnabled = kapp->config()->readBoolEntry("strokesEnabled",false);

	TQPoint p;
	int x = kapp->config()->readNumEntry("x",0);
	int y = kapp->config()->readNumEntry("y",0);
	p.setX(x);
	p.setY(y);
	move(p);

	mousetool_is_running = kapp->config()->readBoolEntry("MouseToolIsRunning",false);
	display = XOpenDisplay(NULL);
}

// Save options to kmousetoolrc file
void KMouseTool::saveOptions()
{
	TQPoint p = pos();
	int x = p.x();
	int y = p.y();

	kapp->config()->setGroup("ProgramOptions");
	kapp->config()->writeEntry("Version", KMOUSETOOL_VERSION);
	kapp->config()->setGroup("UserOptions");
	kapp->config()->writeEntry("x", x);
	kapp->config()->writeEntry("y", y);
	kapp->config()->writeEntry("strokesEnabled", strokesEnabled);
	kapp->config()->writeEntry("IsMinimized", isHidden());
	kapp->config()->writeEntry("DwellTime", dwell_time);
	kapp->config()->writeEntry("DragTime", drag_time);
	kapp->config()->writeEntry("Movement", min_movement);
	kapp->config()->writeEntry("SmartDrag", smart_drag_on);
	kapp->config()->writeEntry("AudibleClick", playSound);
	kapp->config()->writeEntry("MouseToolIsRunning", mousetool_is_running);
	kapp->config()->sync();
}

void KMouseTool::updateStartStopText()
{
	if (mousetool_is_running)
		buttonStartStop->setText(i18n("&Stop"));
	else
		buttonStartStop->setText(i18n("&Start"));
	trayIcon->updateStartStopText(mousetool_is_running);
}

bool KMouseTool::newSettings()
{
	return ((dwell_time != dwellTimeEdit->value()) ||
		(drag_time != dragTimeEdit->value()) ||
		(min_movement != movementEdit->value()) ||
		(smart_drag_on != cbDrag->isChecked()) ||
		(playSound != cbClick->isChecked()) ||
		(strokesEnabled != cbStroke->isChecked()) ||
		(isAutostart() != cbStart->isChecked()));
}

bool KMouseTool::defaultSettings()
{
	return ((5 == dwellTimeEdit->value()) &&
		(3 == dragTimeEdit->value()) &&
		(5 == movementEdit->value()) &&
		!cbDrag->isChecked() &&
		!cbClick->isChecked() &&
		!cbStroke->isChecked() &&
		!cbStart->isChecked());
}

/******** SLOTS **********/

// Value or state changed
void KMouseTool::settingsChanged ()
{
	buttonReset->setEnabled (newSettings());
	buttonApply->setEnabled (newSettings());
	buttonDefault->setDisabled (defaultSettings());
}

// Buttons within the dialog
void KMouseTool::startStopSelected()
{
	mousetool_is_running = !mousetool_is_running;
	updateStartStopText();
}

void KMouseTool::defaultSelected()
{
	setDefaultSettings();
}

void KMouseTool::resetSelected()
{
	resetSettings();
}

void KMouseTool::applySelected()
{
	applySettings();
}

// Buttons at the bottom of the dialog
void KMouseTool::helpSelected()
{
	kapp->invokeHelp();
}

void KMouseTool::closeSelected()
{
	if (newSettings())
	{
		int answer = KMessageBox::questionYesNoCancel (this,
			i18n("There are unsaved changes in the active module.\nDo you want to apply the changes before closing the configuration window or discard the changes?"),
			i18n("Closing Configuration Window"),
			KStdGuiItem::apply(), KStdGuiItem::discard(), "AutomaticSave");
		if (answer == KMessageBox::Yes)
			applySettings();
		else if (answer == KMessageBox::No)
			resetSettings();
		if (answer != KMessageBox::Cancel)
			hide();
	}
	else
		hide();
}

void KMouseTool::quitSelected()
{
	if (newSettings())
	{
		int answer = KMessageBox::questionYesNoCancel (this,
			i18n("There are unsaved changes in the active module.\nDo you want to apply the changes before quitting KMousetool or discard the changes?"),
			i18n("Quitting KMousetool"),
			KStdGuiItem::apply(), KStdGuiItem::discard(), "AutomaticSave");
		if (answer == KMessageBox::Yes)
			applySettings();
		if (answer != KMessageBox::Cancel)
		{
			saveOptions();
			kapp->quit();
		}
	}
	else
	{
		saveOptions();
		kapp->quit();
	}
}

// Menu functions
void KMouseTool::configureSelected()
{
	show();
	raise();
	setActiveWindow();
}

void KMouseTool::aboutSelected()
{
	aboutDlg->show();
}



KMouseToolTray::KMouseToolTray (TQWidget *parent, const char *name) : KSystemTray (parent, name)
{
	startStopId = contextMenu()->insertItem (i18n("&Start"), this, TQ_SIGNAL(startStopSelected()));
	contextMenu()->insertSeparator();
	contextMenu()->insertItem (TDEGlobal::iconLoader()->loadIcon("configure", TDEIcon::Small),
	                           i18n("&Configure KMouseTool..."), this, TQ_SIGNAL(configureSelected()));
	contextMenu()->insertSeparator();
	contextMenu()->insertItem (TDEGlobal::iconLoader()->loadIcon("contents", TDEIcon::Small),
	                           i18n("KMousetool &Handbook"), this, TQ_SIGNAL(helpSelected()));
	contextMenu()->insertItem (TDEGlobal::iconLoader()->loadIcon("kmousetool", TDEIcon::Small),
	                           i18n("&About KMouseTool"), this, TQ_SIGNAL(aboutSelected()));
}

KMouseToolTray::~KMouseToolTray() {
}

void KMouseToolTray::updateStartStopText(bool mousetool_is_running)
{
	TQPixmap icon;

	if (mousetool_is_running) {
		contextMenu()->changeItem (startStopId, i18n("&Stop"));
		icon = TDEGlobal::iconLoader()->loadIcon("kmousetool_on", TDEIcon::Small);
	}
	else {
		contextMenu()->changeItem (startStopId, i18n("&Start"));
		icon = TDEGlobal::iconLoader()->loadIcon("kmousetool_off", TDEIcon::Small);
	}
	setPixmap (icon);
	show();
}
