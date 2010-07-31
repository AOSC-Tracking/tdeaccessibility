//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONTEXTMENUHANDLER_H
#define CONTEXTMENUHANDLER_H

// Qt includes
#include <tqobject.h>
#include <tqstring.h>
#include <tqxml.h>

// KDE includes
#include <kaction.h>
#include <kpopupmenu.h>

//forward declarations
class ContextMenuHandler;
class ListViewInterface;

/**
@author Robert Vogl
*/
class ContextActionHandler : public TQXmlDefaultHandler 
{
public:
    ContextActionHandler(ContextMenuHandler *menuhandler = 0);
    ~ContextActionHandler();
    
    // Reimplementations from base class
    bool startElement( const TQString &namespaceURI,
            const TQString &localName,
            const TQString &qName,
            const TQXmlAttributes &atts );    
    
    // Reimplementations from base class
    bool endElement( const TQString &namespaceURI,
            const TQString &localName,
            const TQString &qName );
            
    // Reimplementations from base class
    bool characters( const TQString &ch );

    void setSearchID( const TQString xmlID );
    
private:
    ContextMenuHandler *m_menuhandler;
    TQString m_subName;
    TQString m_actionName;
    TQString m_qty;
    KPopupMenu *m_popup;
    bool m_hit;
    TQString m_searchID;
};


/**
@author Robert Vogl
*/
class ContextMenuHandler : public QObject
{
Q_OBJECT
public:
    ContextMenuHandler(TQObject *parent = 0, const char *name = 0);

    ~ContextMenuHandler();

    /**
     * \param item The selected TreeView item.
     * \returns A pointer to the popup-menu.
     */
    KPopupMenu* getPopupMenu(ListViewInterface *item);
    
    /**
     * \returns A pointer to the Submenu as a child of the parent
     * \p parent.
     */
    KPopupMenu* SubMenuFactory(KPopupMenu *parent);
    
    /**
     * Creates an action sufficiant to the given name.
     * It checks if the action is allowed or not and returns
     * a disabled or enabled action.
     * \param actionName The name of the action as defined in the
     * namespace \p ContextMenu.xml.
     * \param qty The maximum allowed quantity of the same action for the
     * given element.
     * \returns A Pointer to the action. 
     */
    KAction* ActionFactory( const TQString &actionName, const TQString &qty );
    
    /**
     * Creates a Popup context menu for the given item and stores a
     * pointer to it in \p m_MenuList.
     */
    void registerPopupMenu(ListViewInterface *item);
    
    /**
     * Sets wether the content of the item is editable or not.
     */
    void setItemEditable( bool editable );
    
    /**
     * Sets the number of lines a editable item can hold.
     * -1 = unlimited.
     */
    void setItemMaxlines( const TQString &maxlines ); 

private: // Methods
    void initActions();
    bool parseXmlFile(const TQString &xmlID);

public: // Attributes
    KPopupMenu* m_popupmenu;

private:
    // Basic actions
    KAction* renameItem;
    KAction* deleteItem;
    // Actions to create new items
    KAction* newBookInfo;
    KAction* newChapter;
    KAction* newKeywordSet;
    KAction* newKeyword;
    KAction* newAbstract;
    KAction* newAuthorGroup;
    KAction* newAuthor;
    KAction* newDate;
    KAction* newReleaseInfo;
    KAction* newTitle;
    KAction* newParagraph;
    KAction* newSection_1;
    KAction* newSection_2;
    KAction* newSection_3;
    KAction* newSection_4;
    KAction* newSection_5;

    // mixed stuff
    TQString m_XmlFilePath;
    ListViewInterface* m_item;
    TQObject* m_DocTreeView;

};

#endif
