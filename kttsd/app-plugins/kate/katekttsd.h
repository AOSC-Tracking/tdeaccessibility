/***************************************************************************
  A KTextEditor (Kate Part) plugin for speaking text.

  Copyright:
  (C) 2003-2004 by Olaf Schmidt <ojschmidt@kde.org>
  (C) 2005 by Gary Cramblitt <garycramblitt@comcast.net>

  Original Author: Olaf Schmidt <ojschmidt@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _KATEKTTSD_H_
#define _KATEKTTSD_H_

#include <tdetexteditor/plugin.h>
#include <tdetexteditor/view.h>
#include <kxmlguiclient.h>
#include <tqobject.h>

class KateKttsdPlugin : public KTextEditor::Plugin, public KTextEditor::PluginViewInterface
{
    TQ_OBJECT
  

    public:
        KateKttsdPlugin( TQObject *parent = 0,
                         const char* name = 0,
                         const TQStringList &args = TQStringList() );
        virtual ~KateKttsdPlugin();

        void addView (KTextEditor::View *view);
        void removeView (KTextEditor::View *view);

    private:
        TQPtrList<class KateKttsdPluginView> m_views;
};

class KateKttsdPluginView : public TQObject, public KXMLGUIClient
{
    TQ_OBJECT
  

    public:
        KateKttsdPluginView( KTextEditor::View *view, const char *name=0 );
        ~KateKttsdPluginView() {};

    public slots:
        void slotReadOut();
};

#endif      // _KATEKTTSD_H_
