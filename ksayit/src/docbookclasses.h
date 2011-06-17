//
// C++ Interface: docbookclasses.h
//
// Description: 
//
//
// Author: Robert Vogl <voglrobe@lapislazuli>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DOCBOOKCLASSES_H
#define DOCBOOKCLASSES_H

// TQt includes
#include <tqstring.h>

#include <tqvariant.h>
#include <tqvaluelist.h>

// KDE includes
#include <klistview.h>
 
// App specific includes
#include "Types.h"

// forward declarations
class TQStringList;

//
// Classes for Roberts elements
//
/**
 Represents the root item of the document, therefore it expects
 a \p TQListView object as tqparent.\n
 Structural item only (no references to a DOM node).
 @author Robert Vogl
 */
class RobDocument : public ListViewInterface
{
public:
    /**
     * Constructor
     * \param lv The ListView tqparent.
     * \param label The label is shown in column 0 of the TreeView and is also
     * used as bookmark title.
     */
    RobDocument(KListView *lv=0, TQString label=TQString());
    ~RobDocument();

    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:
    TQValueList<TQVariant> m_valueList;
};


//
// Classes for Docbook elements
//
/**
 Represents the DocBook \p Overview element of the document.\n
 Structural item only (no references to a DOM node).
 @author Robert Vogl
 */
class Overview : public ListViewInterface
{
public:
    Overview(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~Overview();

    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:
    TQValueList<TQVariant> m_valueList;
};


/**
 Represents the DocBook \p Date element of the document.\n
 @author Robert Vogl
 */
class Date : public ListViewInterface
{
public:
    Date(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~Date();

    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:
    // Pointer to the associated Data
    TQValueList<TQVariant> m_valueList;
};


/**
 Represents the DocBook \p ReleasInfo element of the document.\n
 @author Robert Vogl
 */
class ReleaseInfo : public ListViewInterface
{
public:
    ReleaseInfo(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~ReleaseInfo();

    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:
    // Pointer to the associated Data
    TQValueList<TQVariant> m_valueList;
};


/**
 Represents the DocBook \p AuthorGroup element of the document.\n
 Structural item only (no references to a DOM node).
 @author Robert Vogl
 */
class AuthorGroup : public ListViewInterface
{
public:
    AuthorGroup(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~AuthorGroup();
    
    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:
    TQValueList<TQVariant> m_valueList;
};


/**
 Represents the DocBook \p Author element and its Subelements
 \p Firstname and \p Surname of the document.\n
 @author Robert Vogl
 */
class Author : public ListViewInterface
{
public:
    Author(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~Author();

    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:
    TQValueList<TQVariant> m_valueList;
};


/**
 Represents the DocBook \p KeywordSet element of the document.\n
 Structural item only (no references to a DOM node).
 @author Robert Vogl
 */
class KeywordSet : public ListViewInterface
{
public:
    KeywordSet(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~KeywordSet();

    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:
    TQValueList<TQVariant> m_valueList;
};


/**
 Represents the DocBook \p Keyword element of the document.\n
 @author Robert Vogl
 */
class Keyword : public ListViewInterface
{
public:
    Keyword(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~Keyword();

    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:    
    TQValueList<TQVariant> m_valueList;
};


/**
 Represents the DocBook \p Abstract element of the document.\n
 Structural item only (no references to a DOM node).
 @author Robert Vogl
 */
class Abstract : public ListViewInterface
{
public:
    Abstract(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~Abstract();
    
    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:
    TQValueList<TQVariant> m_valueList;
};


/**
 Represents a DocBook Paragraph item of the document.\n
 @author Robert Vogl
 */
class Para : public ListViewInterface
{
public:
    Para(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~Para();
    
    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:
    TQValueList<TQVariant> m_valueList;
};


/**
 Represents the DocBook \p Chapter element of the document.\n
 Structural item only (no references to a DOM node).
 @author Robert Vogl
 */
class Chapter : public ListViewInterface
{
public:
    Chapter(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~Chapter();
    
    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:    
    TQValueList<TQVariant> m_valueList;
};


/**
 Represents the DocBook \p Sec1 element of the document.\n
 Structural item only (no references to a DOM node).
 @author Robert Vogl
 */
class Sect1 : public ListViewInterface
{
public:
    Sect1(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~Sect1();

    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:    
    TQValueList<TQVariant> m_valueList;
};


/**
 Represents the DocBook \p Sec2 element of the document.\n
 Structural item only (no references to a DOM node).
 @author Robert Vogl
 */
class Sect2 : public ListViewInterface
{
public:
    Sect2(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~Sect2();

    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:    
    TQValueList<TQVariant> m_valueList;
};


/**
 Represents the DocBook \p Sec3 element of the document.\n
 Structural item only (no references to a DOM node).
 @author Robert Vogl
 */
class Sect3 : public ListViewInterface
{
public:
    Sect3(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~Sect3();
    
    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:    
    TQValueList<TQVariant> m_valueList;
};


/**
 Represents the DocBook \p Sec4 element of the document.\n
 Structural item only (no references to a DOM node).
 @author Robert Vogl
 */
class Sect4 : public ListViewInterface
{
public:
    Sect4(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~Sect4();
    
    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:    
    TQValueList<TQVariant> m_valueList;
};


/**
 Represents the DocBook \p Sec5 element of the document.\n
 Structural item only (no references to a DOM node).
 @author Robert Vogl
 */
class Sect5 : public ListViewInterface
{
public:
    Sect5(ListViewInterface *tqparent=0, ListViewInterface *after=0, TQString label=TQString());
    ~Sect5();
    
    bool setValue( unsigned int index, TQVariant data );
    TQVariant getValue( unsigned int index ) const;

private:    
    TQValueList<TQVariant> m_valueList;
};

#endif
