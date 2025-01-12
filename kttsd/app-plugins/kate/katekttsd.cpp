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

// TQt includes.
#include <tqmessagebox.h>
#include <dcopclient.h>
#include <tqtimer.h>

// KDE includes.
#include <tdetexteditor/editinterface.h>
#include <tdetexteditor/selectioninterface.h>

#include <tdeaction.h>
#include <tdelocale.h>
#include <kstandarddirs.h>
#include <tdeapplication.h>
#include <kgenericfactory.h>

// KateKttsdPlugin includes.
#include "katekttsd.h"
#include "katekttsd.moc"

K_EXPORT_COMPONENT_FACTORY( tdetexteditor_kttsd, KGenericFactory<KateKttsdPlugin>( "tdetexteditor_kttsd" ) )

KateKttsdPlugin::KateKttsdPlugin( TQObject *parent, const char* name, const TQStringList& )
    : KTextEditor::Plugin ( (KTextEditor::Document*) parent, name )
{
}

KateKttsdPlugin::~KateKttsdPlugin()
{
}


void KateKttsdPlugin::addView(KTextEditor::View *view)
{
    KateKttsdPluginView *nview = new KateKttsdPluginView (view, "KTTSD Plugin");
    m_views.append (nview);
}

void KateKttsdPlugin::removeView(KTextEditor::View *view)
{
    for (uint z=0; z < m_views.count(); ++z)
        if (m_views.at(z)->parentClient() == view)
    {
        KateKttsdPluginView *nview = m_views.at(z);
        m_views.remove (nview);
        delete nview;
    }
    TDEGlobal::locale()->removeCatalogue("kttsd");
}


KateKttsdPluginView::KateKttsdPluginView( KTextEditor::View *view, const char *name )
    : TQObject( view, name ),
    KXMLGUIClient( view )
{
    view->insertChildClient( this );
    setInstance( KGenericFactory<KateKttsdPlugin>::instance() );
    TDEGlobal::locale()->insertCatalogue("kttsd");
    (void) new TDEAction( i18n("Speak Text"), "kttsd", 0, this, TQ_SLOT(slotReadOut()), actionCollection(), "tools_kttsd" );
    setXMLFile( "tdetexteditor_kttsdui.rc" );
}

void KateKttsdPluginView::slotReadOut()
{
    KTextEditor::View *v = (KTextEditor::View*)parent();
    KTextEditor::SelectionInterface *si = KTextEditor::selectionInterface( v->document() );
    TQString text;

    if ( si->hasSelection() )
      text = si->selection();
    else {
        KTextEditor::EditInterface *ei = KTextEditor::editInterface( v->document() );
        text = ei->text();
    }

    DCOPClient *client = kapp->dcopClient();
    // If KTTSD not running, start it.
    if (!client->isApplicationRegistered("kttsd"))
    {
        TQString error;
        if (kapp->startServiceByDesktopName("kttsd", TQStringList(), &error))
            TQMessageBox::warning(0, i18n( "Starting KTTSD Failed"), error );
    }
    TQByteArray  data;
    TQByteArray  data2;
    TQCString    replyType;
    TQByteArray  replyData;
    TQDataStream arg(data, IO_WriteOnly);
    arg << text << "";
    if ( !client->call("kttsd", "KSpeech", "setText(TQString,TQString)",
                       data, replyType, replyData, true) )
       TQMessageBox::warning( 0, i18n( "DCOP Call Failed" ),
                                 i18n( "The DCOP call setText failed." ));
    TQDataStream arg2(data2, IO_WriteOnly);

    arg2 << 0;
    if ( !client->call("kttsd", "KSpeech", "startText(uint)",
                       data2, replyType, replyData, true) )
       TQMessageBox::warning( 0, i18n( "DCOP Call Failed" ),
                                i18n( "The DCOP call startText failed." ));
}

