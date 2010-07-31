//
// C++ Interface: doctreeviewimpl
//
// Description:
//
//
// Author: Robert Vogl <voglrobe@lapislazuli>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DOCTREEVIEWIMPL_H
#define DOCTREEVIEWIMPL_H

// Qt includes
#include <tqstring.h>

// KDE includes
#include <kurl.h>
#include <klistview.h>
#include <kpopupmenu.h>

// App specific includes
#include "Types.h"
#include "DocTreeView.h"

// forward declarations
class ParaSaxParser;
class ContextMenuHandler;


/**
This class implements the TreeView object.\n
It takes a XML/DocBook file, parses it and builds up a hierarchical
\p TQListView structure corresponding to the structure of the document.
The objective of this class is to maintain the document and provide
all required methods to access or manipulate its content.
@author Robert Vogl
*/
class DocTreeViewImpl : public DocTreeView
{
public:
    DocTreeViewImpl(TQWidget* parent=0, const char* name=0, WFlags fl=0);

    ~DocTreeViewImpl();

    /**
     * Tries to open and parse the given file. If the file is not identified as a
     * valid XML/DocBook file, it will be packed as a \p CDATA block into a \p para element
     * of a simple DocBook'ish XML file.
     * \param url The URL of the file.
     */
    void openFile(const KURL &url);

    /**
     * Saves the document to a XML file.
     */
    void saveFile();

    /**
     * Saves the document to a XML file and allows the user to define a name.
     */
    void saveFileAs();

    /**
     * Cleans up the interal document data structures.
     */
    void clear();

    /**
     * Creates an empty XML document that conatins one editable DocBook \p Paragraph.
     * Usually called after clean() and initial startup.
     */
    void createEmptyDocument();

    /**
     * Stores the given text but does not make the new content
     * persistent.
     * \param text The content of the TextEdit view.
     */
    void setNodeContent(TQString &text);

    /**
     * Starting from the current selected node it travels recursively
     * through all childs and collects the content of all corresponding
     * DOM nodes. After sending the text to the TTS system the speech synthesis
     * process will be started.
     */
    void sayActiveNodeAndChilds();

    /**
     * Signals a stop-talking-request from the user.
     */
    void stop();

    /**
     * Controls the Edit Mode.
     * \param mode If true, Edit Mode ON,\n if false, Edit Mode OFF.
     */
    void setEditMode(bool mode);
    
    /**
     * Sets the item active designated by the ID.
     * \param ID The ID (column 3) of the TreeView item.
     * \returns <tt>TQString::null</tt> if the operation was successfull,\n
     * an error message, if the item was not found.
     */
    TQString selectItemByID(const TQString &ID, const TQString title);
    
    /**
     * Enables/disables the contextmenus
     */
    void enableContextMenus( bool enabled );

public slots:
    void slotRenameItem();  // reimplemented from base classes
    void slotDeleteItem();
    void slotNewBookInfo();
    void slotNewChapter();
    void slotNewKeywordSet();
    void slotNewKeyword();
    void slotNewAbstract();
    void slotNewAuthorGroup();
    void slotNewAuthor();
    void slotNewDate();
    void slotNewReleaseInfo();
    void slotNewTitle();
    void slotNewParagraph();
    void slotNewSection_1();
    void slotNewSection_2();
    void slotNewSection_3();
    void slotNewSection_4();
    void slotNewSection_5();
    
protected slots:
    /**
     * Reimplemented from base class.
     * Called whenever the active node has been changed.\n
     * Makes the former selected item persistent (maybe it has
     * been changed).\n
     * Displays the content of the selected Node either in RTF or
     * plain text format (depending on the Edit Mode flag, childs
     * present or not).\n
     * Informs the BookmarkManager about the new item.
     *
     * \param item The new active item of the TreeView.
     */
    void slotItemClicked(TQListViewItem *item);
    
    /**
     * Reimplemented from base class.
     * Called when the right button was pressed.
     * \param item The new active item of the TreeView.
     */
    void slotRightButtonPressed(TQListViewItem *item, const TQPoint &pos, int);
    
    /**
     * Reimplemented from TQWidget class.
     */
    // void contextMenuEvent(TQContextMenuEvent *e);

private: // Methods
    void recursiveTextCollector(ListViewInterface* item, TQTextStream &msg, bool header=true);
    void recursiveSayNodes(ListViewInterface* item);
    void recursiveBuildItemIdentifier(ListViewInterface* item, TQTextStream &idstring);
    void makeCurrentNodePersistent();
    int newIndexFirstChild();
    int newIndexLastChild();
    void findHighestIndex(ListViewInterface* item, int &index);
    TQString getItemTitle( ListViewInterface *item);
    void makeToSingleLine( TQString &content );    

        
private: // Attributes
    ContextMenuHandler *m_contextmenuhandler;
    KPopupMenu *m_contextmenu; 
    ListViewInterface *m_rootItem;
    ListViewInterface *m_currentItem;
    TQString m_changedContent;
    int m_idCounter;
    bool m_stopped;
    KURL m_url;
    ParaSaxParser *m_parasaxparser;
    bool m_editMode;
};





#endif
