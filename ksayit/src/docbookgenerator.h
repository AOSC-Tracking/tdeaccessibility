//
// C++ Interface: docbookgenerator
//
// Description:
//
//
// Author: Robert Vogl <voglrobe@lapislazuli>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DOCBOOKGENERATOR_H
#define DOCBOOKGENERATOR_H

// TQt includes
#include <tqtextstream.h>

// KDE includes

// App specific includes
#include "Types.h"

class DocbookGenerator
{
public:
    // Constructor
    DocbookGenerator();

    // Destructor
    ~DocbookGenerator();

    /**
     * Walks throug the entire treeview starting with given item and returns
     * returns a DocBook representation of its content.
     * \returns doc The content of the TreeView in DocBook format
     * \param item A pointer to the root item of the tree. Must be of type
     * \p RobDocument.
     */
    void writeBook(TQTextStream &doc, ListViewInterface *item);


private:
    void writeBookInfo(TQTextStream &doc, ListViewInterface *item);
    void writeTitleOfBook(TQTextStream &doc, ListViewInterface *item);
    void writeChapter(TQTextStream &doc, ListViewInterface *item);
    void writeKeywordSet(TQTextStream &doc, ListViewInterface *item);
    void writeKeyword(TQTextStream &doc, ListViewInterface *item);
    void writeAbstract(TQTextStream &doc, ListViewInterface *item);
    void writePara(TQTextStream &doc, ListViewInterface *item);
    void writeAuthorGroup(TQTextStream &doc, ListViewInterface *item);
    void writeAuthor(TQTextStream &doc, ListViewInterface *item);
    void writeDate(TQTextStream &doc, ListViewInterface *item);
    void writeReleaseInfo(TQTextStream &doc, ListViewInterface *item);
    void writeSect1(TQTextStream &doc, ListViewInterface *item);
    void writeSect2(TQTextStream &doc, ListViewInterface *item);
    void writeSect3(TQTextStream &doc, ListViewInterface *item);
    void writeSect4(TQTextStream &doc, ListViewInterface *item);
    void writeSect5(TQTextStream &doc, ListViewInterface *item);



};







#endif

