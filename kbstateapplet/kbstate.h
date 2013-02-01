/*
 *  Copyright (c) 2004 Gunnar Schmi Dt <gunnar@schmi-dt.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#ifndef __kbstate_h__
#define __kbstate_h__

#include <kpanelapplet.h> 
#include <tqpushbutton.h>
#include <tqptrlist.h>
#include <tqtimer.h>

extern "C"
{
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
}
     
class TQLabel;      
class TQGridLayout;
class TDEPopupMenu;

class StatusIcon : public TQPushButton {
   Q_OBJECT
  
public:
   StatusIcon (const TQString &text, TQWidget *parent, const char *name=0);
   ~StatusIcon ();

   TQSize minimumSizeHint () const;
};
typedef TQPtrList<StatusIcon> IconList;

class TimeoutIcon : public StatusIcon {
   Q_OBJECT
  
public:
   TimeoutIcon (TDEInstance *instance, const TQString &text,
					 const TQString &featurename,
					 TQWidget *parent, const char *name=0);
   ~TimeoutIcon ();

   void update ();
   void setGlyth (const TQString &glyth);
   void setImage (const TQString &name, int timeout = 0);

   void drawButton (TQPainter *p);

private:
   TQString glyth;
	TQString iconname;
	TQString featurename;
   TQPixmap pixmap;
   TQPixmap image;
   TQTimer timer;
   TDEInstance *instance;
   
private slots:
   void timeout ();
};

class KeyIcon : public StatusIcon {
   Q_OBJECT
  
public:
   KeyIcon (int keyId, TDEInstance *instance,
            TQWidget *parent, const char *name=0);
   ~KeyIcon ();
   void setState (bool latched, bool locked);
   void drawButton (TQPainter *p);
   void updateImages ();

signals:
   void stateChangeRequest (KeyIcon *source, bool latched, bool locked);

protected:
   void resizeEvent(TQResizeEvent*);

private slots:
   void clickedSlot();

private:
   TQPixmap locked;
   TQPixmap latched;
   TQPixmap unlatched;
   bool    isLatched;
   bool    isLocked;
   bool    tristate;
   int     keyId;
   TDEInstance *instance;
};

class MouseIcon : public StatusIcon {
	Q_OBJECT
  
	public:
		MouseIcon (TDEInstance *instance, TQWidget *parent, const char *name=0);
		~MouseIcon ();
		void setState (int state);
		void setActiveKey (int activekey);
		void drawButton (TQPainter *p);
		void updateImages ();

	protected:
		void resizeEvent(TQResizeEvent*);

	private:
		TQPixmap mouse;
		TQPixmap leftSelected;
		TQPixmap middleSelected;
		TQPixmap rightSelected;
		TQPixmap leftDot;
		TQPixmap middleDot;
		TQPixmap rightDot;
		TQPixmap leftDotSelected;
		TQPixmap middleDotSelected;
		TQPixmap rightDotSelected;
		int state, activekey;
		TDEInstance *instance;
};

class KbStateApplet : public KPanelApplet {
   Q_OBJECT
  

public:
   KbStateApplet(const TQString& configFile, Type t = Normal, int actions = 0,
              TQWidget *parent = 0, const char *name = 0);
   ~KbStateApplet();

   int widthForHeight(int height) const;
   int heightForWidth(int width) const;

protected:
    void mousePressEvent(TQMouseEvent *e);
    void timerEvent(TQTimerEvent*);
    void resizeEvent(TQResizeEvent*);
    bool x11Event (XEvent *);

public slots:
   void about();
   void configureAccessX();
   void configureMouse();
   void configureKeyboard();
   void toggleModifier ();
   void toggleLockkeys ();
	void toggleMouse ();
	void toggleAccessX ();
	void paletteChanged();

	void toggleFillSpace ();

private slots:
	void setIconDim (int size);
   void stateChangeRequest (KeyIcon *source, bool latched, bool locked);

private:
   void loadConfig();
   void saveConfig();
   void layout();
   
   int xkb_base_event_type;

   KeyIcon *(icons[8]);
   IconList modifiers;
   IconList lockkeys;
	TimeoutIcon *sticky;
	TimeoutIcon *slow;
	TimeoutIcon *bounce;
   MouseIcon *mouse;
   
	int state;
	unsigned int accessxFeatures;
   int size;
   
   void initMasks();

   void buildPopupMenu();
   void updateMenu();
   TDEPopupMenu *popup;
   TDEPopupMenu *sizePopup;
   TDEPopupMenu *showPopup;
	int modifierItem, lockkeysItem, mouseItem, accessxItem;
	int fillSpaceItem;
	bool showModifiers, showLockkeys, showMouse, showAccessX;
	bool fillSpace;

	TDEInstance *instance;
	XkbDescPtr xkb;
};

#endif // __kbstate_h__
