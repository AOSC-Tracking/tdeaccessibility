//
// C++ Interface: docbookparser
//
// Description:
//
//
// Author: Robert Vogl <voglrobe@lapislazuli>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DOCBOOKPARSER_H
#define DOCBOOKPARSER_H

// Qt includes
#include <tqdom.h>

// KDE includes

// App specific includes
#include "Types.h"
#include "contextmenuhandler.h"


class DocbookParser
{
public:
    // Constructor
    DocbookParser(ContextMenuHandler *cmHandler=0);

    // Destructor
    ~DocbookParser();
    
    void parseBook(const TQDomElement &element, ListViewInterface *item);
    
    int getIdCounter(){ return m_idCounter; };

private: // Methods
    // a selection of DocBook elements
    void parseBookInfo(const TQDomElement &element, ListViewInterface *item);
    void parseAuthorGroup(const TQDomElement &element, ListViewInterface *item);
    void parseAuthor(const TQDomElement &element, ListViewInterface *item);
    void parseKeywordSet(const TQDomElement &element, ListViewInterface *item);
    void parseAbstract(const TQDomElement &element, ListViewInterface *item);
    void parseChapter(const TQDomElement &element, ListViewInterface *item);
    void parseSect1(const TQDomElement &element, ListViewInterface *item);
    void parseSect2(const TQDomElement &element, ListViewInterface *item);
    void parseSect3(const TQDomElement &element, ListViewInterface *item);
    void parseSect4(const TQDomElement &element, ListViewInterface *item);
    void parseSect5(const TQDomElement &element, ListViewInterface *item);
    void parsePara(const TQDomElement &element, ListViewInterface *item);

    // Helpers
    TQString node2raw(TQDomNode node) const;
    
private: // Attributes
    ContextMenuHandler *m_contextmenuhandler;
    int m_idCounter;

};

#endif

 
