/***************************************************************************
                          Types.h  -  description
                             -------------------
    begin                : Fr Nov 21 2003
    copyright            : (C) 2003 by voglrobe
    email                : voglrobe@saphir
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TYPES_H
#define TYPES_H

// #include <vector>

// QT includes
#include <tqvaluelist.h>
#include <tqmap.h>

// KDE includes
#include <tdelistview.h>

typedef TQValueList<int> ParameterList_type;
typedef TQMap<TQString, ParameterList_type> EffectConfig_type;

namespace KSayItGlobal {
    static const int item_initial_id = 100;
    
    enum {
        XMLCONTEXTNAME=0,
        ISEDITABLE,
        MAXLINES,
        SUMMARY,
        RAWDATA,
        RTFHEADER,
        RTFDATA,
        SPEAKERHEADER,
        SPEAKERDATA
    };
    
}

/**
This abstract class is the interface to access the items of the TreeView.
@author Robert Vogl
*/
class ListViewInterface : public TDEListViewItem
{
public:
    ListViewInterface(ListViewInterface *parent=0, TQString label=TQString());
    ListViewInterface(ListViewInterface *parent=0, ListViewInterface *after=0, TQString label=TQString());
    ListViewInterface(TDEListView *lv=0, TQString label=TQString());

    // ~ListViewItemInterface();
    
    /**
     * Stores data to the item referenced by the given index.
     * \param index The index is defined by the following enum:
     * \param data The data to store.
     * \returns <tt>true</tt> if the operation was successfully performed,
     * in all other cases <tt>false</tt>.
     */
    virtual bool setValue( unsigned int index, TQVariant data ) = 0;
    
    /**
     * Requests data from the item referenced by the index.
     * \param index See <tt>setValue()</tt>.
     * \returns The requested data as a <tt>TQVariant</tt> or a invalid
     * <tt>TQVariant()</tt> if the data was not available.
     */
    virtual TQVariant getValue( unsigned int index ) const = 0;

};


#endif
