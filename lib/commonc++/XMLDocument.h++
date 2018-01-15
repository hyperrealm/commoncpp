/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2014  Mark A Lindner

   This file is part of commonc++.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   ---------------------------------------------------------------------------
*/

#ifndef __ccxx_XMLDocument_hxx
#define __ccxx_XMLDocument_hxx

#include <commonc++/Common.h++>
#include <commonc++/IOException.h++>
#include <commonc++/InvalidArgumentException.h++>
#include <commonc++/ParseException.h++>
#include <commonc++/String.h++>

#include <map>
#include <vector>

#include <expat.h>

namespace ccxx {

class XMLElement; // fwd ref

/**
 * An object representing an XML document.
 *
 * @author Mark Lindner
 */
class COMMONCPPXML_API XMLDocument
{
 public:

  /** Construct a new XMLDocument. */
  XMLDocument();

  /** Destructor. */
  ~XMLDocument();

  /**
   * Get the root element of the document.
   *
   * @return A reference to the root element, or XMLElement::null if the
   * document is empty.
   */
  XMLElement& getRoot();

  /**
   * Get the root element of the document.
   *
   * @return A reference to the root element, or XMLElement::null if
   * the document is empty.
   */
  const XMLElement& getRoot() const;

  /**
   * Set the root element of the document. The previous content of the
   * document is deleted.
   *
   * @param name The name for the new element.
   * @return A reference to the new element.
   * @throw InvalidArgumentException If the name is invalid.
   */
  XMLElement& setRoot(const String& name);

  /**
   * Find an XML element via a path.
   *
   * @param path The path, a dot- or slash-separated list of element names
   * beginning with the root element.
   * @return The first matching element, if found, otherwise
   * XMLElement::null.
   * @throw InvalidArgumentException If the path contains invalid element
   * names.
   */
  XMLElement& find(const String& path);

  /**
   * Find an XML element via a path.
   *
   * @param path The path, a dot- or slash-separated list of element names
   * beginning with the root element.
   * @return The first matching element, if found, otherwise
   * XMLElement::null.
   * @throw InvalidArgumentException If the path contains invalid element
   * names.
   */
  const XMLElement& find(const String& path) const;

  /**
   * Read and parse the XML document from a stream.
   *
   * @param stream The stream to read from.
   * @throw ParseException If a parse error occurs.
   * @throw IOException If an I/O error occurs.
   */
  void read(std::istream& stream);

  /**
   * Read and parse the XML document from a String.
   *
   * @param str The String to read from.
   * @throw ParseException If a parse error occurs.
   */
  void read(const String& str);

  /**
   * Format and write the XML document to a stream.
   *
   * @param stream The stream to write to.
   * @param tabWidth The tab width for indentation; a value of 0 disables
   * pretty-printing.
   * @throw IOException If an I/O error occurs.
   */
  void write(std::ostream& stream, uint_t tabWidth = 2) const;

  /**
   * Format and write the XML document to a String.
   *
   * @param str The String to write to.
   * @param tabWidth The tab width for indentation; a value of 0 disables
   * pretty-printing.
   */
  void write(String& str, uint_t tabWidth = 2) const;

  /**
   * Test if the document is empty.
   *
   * @return <b>true</b> if the document is empty, <b>false</b> otherwise.
   */
  inline bool isEmpty() const
  { return(_root == NULL); }

  /**
   * Test if the document is empty.
   *
   * @return <b>true</b> if the document is empty, <b>false</b> otherwise.
   */
  inline bool operator!() const
  { return(isEmpty()); }

 private:

  const XMLElement& _find(const String& path) const;

  // RAII class to encapsulate parser object
  class XMLParser
  {
    friend class XMLDocument;

   public:

    XMLParser(XMLDocument *owner);
    ~XMLParser();

    void parse(const char* data, size_t length, bool done = true);

    inline void appendCharData(const XML_Char* data, int len)
    { _charData.append(data, len); }

   private:

    XMLDocument* _document;
    XMLElement* _current;
    String _charData;
    XML_Parser _parser;
    bool _status;
  };

  XMLElement* _root;

  static void _elementStartHandler(void* userData, const XML_Char* name,
                                   const XML_Char** attr);
  static void _elementEndHandler(void* userData, const XML_Char* name);
  static void _charDataHandler(void* userData, const XML_Char* data, int len);
  static void _commentHandler(void* userData, const XML_Char* data);

  CCXX_COPY_DECLS(XMLDocument);
};

} // namespace ccxx

#endif // __ccxx_XMLDocument_hxx
