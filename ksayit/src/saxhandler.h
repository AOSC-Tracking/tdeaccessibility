//
// C++ Interface: saxhandler
//
// Description: TQt SAX2-Handler
//
//
// Author: Robert Vogl <voglrobe@web.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SAXHANDLER_H
#define SAXHANDLER_H

// STL includes
#include <map>
using namespace std;

// TQt includes
#include <tqxml.h>
#include <tqstring.h>

typedef map<TQString, TQString> TagMapT;


/**
@author Robert Vogl
*/
class SaxHandler : public TQXmlDefaultHandler
{
public:
    SaxHandler();
    ~SaxHandler();

    // Reimplementations from base class
    bool startElement(const TQString &namespaceURI,
                    const TQString &localName,
                    const TQString &qName,
                    const TQXmlAttributes &atts );

    bool endElement(const TQString &namespaceURI,
                    const TQString &localName,
                    const TQString &qName);

    bool characters(const TQString &ch);

    bool fatalError(const TQXmlParseException &exc);

    bool resolveEntity(const TQString &publicId,
                    const TQString &systemId,
                    TQXmlInputSource* &ret);

    bool externalEntityDecl(const TQString &name,
                    const TQString &publicId,
                    const TQString &systemId);

    bool internalEntityDecl(const TQString &name,
                    const TQString &value);

    bool skippedEntity(const TQString &name);

    /**
     * Returns the parser result.
     */
    void getData( TQString &data ) const;

    /**
     * Reset
     */
    void reset();

    /**
     * Set Mode:
     * \param rtf 'true' = Replace DocBook tags with RTF-tags.\n
     * 'false' = Ignore tags, return text content only. 
     */
    void setRTF(bool rtf);


private:
    TQString m_output;
    bool m_rtf;
    TagMapT m_tagmap;

};

#endif
