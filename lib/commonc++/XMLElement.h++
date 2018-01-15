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

#ifndef __ccxx_XMLElement_hxx
#define __ccxx_XMLElement_hxx

#include <commonc++/Common.h++>
#include <commonc++/InvalidArgumentException.h++>
#include <commonc++/String.h++>

#include <vector>

namespace ccxx {

class XMLElement; // fwd decl

/**
 * An object representing an XML element. XMLElement objects cannot
 * be constructed directly by the caller, to reinforce the
 * convention that they are owned by their enclosing XMLDocument.
 *
 * All methods in this class return XMLElement::null if a matching or
 * requested element was not found; this is a special instance of
 * XMLElement for which isNull() returns <b>true</b> and whose mutator
 * methods are all no-ops.
 *
 * To iterate over the child elements of a given element <code>elem</code>,
 * use an XMLElementIterator or XMLElementConstIterator.
 *
 * @author Mark Lindner
 */
class COMMONCPPXML_API XMLElement
{
  friend class XMLDocument;
  friend class XMLElementConstIterator;
  friend class XMLElementIterator;

 public:

  /** Get the name of the element. */
  inline String getName() const
  { return(_name); }

  /**
   * Set a String attribute in the element.
   *
   * @param name The attribute name.
   * @param value The attribute value. If the element already contains an
   * attribute with the given name, the old value is overwritten.
   * @throw InvalidArgumentException If the name is invalid.
   */
  void setAttribute(const String& name, const String& value);

  /**
   * Set an integer attribute in the element.
   *
   * @param name The attribute name.
   * @param value The attribute value. If the element already contains an
   * attribute with the given name, the old value is overwritten.
   * @throw InvalidArgumentException If the name is invalid.
   */
  void setAttribute(const String& name, int value);

  /**
   * Set a 64-bit integer attribute in the element.
   *
   * @param name The attribute name.
   * @param value The attribute value. If the element already contains an
   * attribute with the given name, the old value is overwritten.
   * @throw InvalidArgumentException If the name is invalid.
   */
  void setAttribute(const String& name, const int64_t& value);

  /**
   * Set a floating point attribute in the element.
   *
   * @param name The attribute name.
   * @param value The attribute value. If the element already contains an
   * attribute with the given name, the old value is overwritten.
   * @throw InvalidArgumentException If the name is invalid.
   */
  void setAttribute(const String& name, const double& value);

  /**
   * Set a boolean attribute in the element.
   *
   * @param name The attribute name.
   * @param value The attribute value. If the element already contains an
   * attribute with the given name, the old value is overwritten.
   * @throw InvalidArgumentException If the name is invalid.
   */
  void setAttribute(const String& name, bool value);

  /**
   * Get the parent element of this element.
   *
   * @return The parent element, or XMLElement::null if the element
   * has no parent (i.e., if it is the root element).
   */
  XMLElement& getParent();

  /**
   * Get the parent element of this element.
   *
   * @return The parent element, or XMLElement::null if the element
   * has no parent (i.e., if it is the root element).
   */
  const XMLElement& getParent() const;

  /**
   * Get the value of a String attribute in the element.
   *
   * @param name The attribute name.
   * @param value The value to fill in.
   * @param defaultValue The value to fill in if the element has no
   * attribute with the given name.
   * @return <b>true</b> if the value was found, <b>false</b> if the
   * default value was filled in.
   * @throw InvalidArgumentException If the name is invalid.
   */
  bool getAttribute(const String& name, String& value,
                    const String& defaultValue = "") const;

  /**
   * Get the value of an integer attribute in the element.
   *
   * @param name The attribute name.
   * @param value The value to fill in.
   * @param defaultValue The value to fill in if the element has no
   * attribute with the given name.
   * @return <b>true</b> if the value was found, <b>false</b> if the
   * default value was filled in.
   * @throw InvalidArgumentException If the name is invalid.
   */
  bool getAttribute(const String& name, int& value, int defaultValue = 0)
    const;

  /**
   * Get the value of a 64-bit integer attribute in the element.
   *
   * @param name The attribute name.
   * @param value The value to fill in.
   * @param defaultValue The value to fill in if the element has no
   * attribute with the given name.
   * @return <b>true</b> if the value was found, <b>false</b> if the
   * default value was filled in.
   * @throw InvalidArgumentException If the name is invalid.
   */
  bool getAttribute(const String& name, int64_t& value,
                    const int64_t& defaultValue = 0) const;

  /**
   * Get the value of a boolean attribute in the element.
   *
   * @param name The attribute name.
   * @param value The value to fill in.
   * @param defaultValue The value to fill in if the element has no
   * attribute with the given name.
   * @return <b>true</b> if the value was found, <b>false</b> if the
   * default value was filled in.
   * @throw InvalidArgumentException If the name is invalid.
   */
  bool getAttribute(const String& name, bool& value,
                    bool defaultValue = false) const;

  /**
   * Get the value of a floating point attribute in the element.
   *
   * @param name The attribute name.
   * @param value The value to fill in.
   * @param defaultValue The value to fill in if the element has no
   * attribute with the given name.
   * @return <b>true</b> if the value was found, <b>false</b> if the
   * default value was filled in.
   * @throw InvalidArgumentException If the name is invalid.
   */
  bool getAttribute(const String& name, double& value,
                    const double& defaultValue = 0.0) const;

  /**
   * Get the names of all of the element's attributes.
   *
   * @param names The vector in which to place the names.
   */
  void getAttributeNames(StringVec& names) const;

  /** Remove all attributes from the element. */
  void removeAttributes();

  /**
   * Test if the element has any attributes.
   *
   * @return <b>true</b> if the element has at least one attribute,
   * <b>false</b> otherwise.
   */
  bool hasAttributes() const;

  /**
   * Test if the element has content.
   *
   * @return <b>true</b> if the element has content (including
   * content that consists solely of whitespace characters),
   * <b>false</b> otherwise.
   */
  inline  bool hasContent() const
  { return(_content.getLength() > 0); }

  /**
   * Set the element's content.
   *
   * @param content The content.
   */
  inline void setContent(const String& content)
  { _content = content; }

  /**
   * Get the element's content.
   *
   * @return The content.
   */
  inline const String getContent() const
  { return(_content); }

  /**
   * Test if the element has children.
   *
   * @return <b>true</b> if the element has at least one child, <b>false</b>
   * otherwise.
   */
  bool hasChildren() const;

  /**
   * Get the element's child count.
   *
   * @return The number of child elements.
   */
  uint_t getChildCount() const;

  /**
   * Add a new child element to the element.
   *
   * @param name The name for the new element.
   * @return A reference to the new XMLElement.
   * @throw InvalidArgumentException If the name is invalid.
   */
  XMLElement& addChild(const String& name);

  /**
   * Get the first child of the element.
   *
   * @param name If not <b>String::null</b>, gets the firt child element
   * with the given name.
   * @return The first child element, or XMLElement::null if no
   * matching element was found.
   */
  XMLElement& getChild(const String& name = String::null);

  /**
   * Get the first child of the element.
   *
   * @param name If not <b>String::null</b>, gets the firt child element
   * with the given name.
   * @return The first child element, or XMLElement::null if no
   * matching element was found.
   */
  const XMLElement& getChild(const String& name = String::null) const;

  /**
   * Get the child at the given index.
   *
   * @param index The index.
   * @return The child element, or XMLElement::null if the index is out
   * of range.
   */
  XMLElement& getChild(uint_t index);

  /**
   * Get the child at the given index.
   *
   * @param index The index.
   * @return The child element, or XMLElement::null if the index is out
   * of range.
   */
  const XMLElement& getChild(uint_t index) const;

  /**
   * Test if the element has at least one child element with the
   * given name.
   *
   * @param name The name.
   * @return <b>true</b> if a child with the given name exists, <b>false</b>
   * otherwise.
   * @throw InvalidArgumentException If the name is invalid.
   */
  bool hasChild(const String& name) const;

  /**
   * Get the first child with the given name. If no such child exists, a
   * new child with the given name is automatically added.
   *
   * @param name The name.
   * @return A reference to the child element.
   * @throw InvalidArgumentException If the name is invalid.
   */
  XMLElement& operator[](const String& name);

  /**
   * Get the first child with the given name. If no such child exists, a
   * new child with the given name is automatically added.
   *
   * @param name The name.
   * @return A reference to the child element.
   * @throw InvalidArgumentException If the name is invalid.
   */
  XMLElement& operator[](const char* name);

  /**
   * Get the first child with the given name.
   *
   * @param name The name.
   * @return The first matching child element, or XMLElement::null if no
   * matching element was found.
   * @throw InvalidArgumentException If the name is invalid.
   */
  inline const XMLElement& operator[](const String& name) const
  { return(_getChild(name)); }

  /**
   * Get the first child with the given name.
   *
   * @param name The name.
   * @return The first matching child element, or XMLElement::null if no
   * matching element was found.
   * @throw InvalidArgumentException If the name is invalid.
   */
  inline const XMLElement& operator[](const char* name) const
  { return(_getChild(name)); }

  /**
   * Get the child at the given index.
   *
   * @param index The index.
   * @return The child element, or XMLElement::null if if the index is
   * out of range.
   * throw OutOfBoundsException If the index is negative.
   */
  XMLElement& operator[](int index);

  /**
   * Get the child at the given index.
   *
   * @param index The index.
   * @return A reference to the child element, or XMLElement::null if the
   * index is out of range.
   * throw OutOfBoundsException If the index is negative.
   */
  const XMLElement& operator[](int index) const;

  /** Remove (and delete) all of the child elements. */
  void removeChildren();

  /**
   * Remove (and delete) all of the child elements with the given name.
   *
   * @param name The name.
   * @throw InvalidArgumentException If the name is invalid.
   */
  void removeChildren(const String& name);

  /**
   * Set the element's content to the string representation of a
   * <i>bool</i>.
   */
  XMLElement& operator=(bool val);

  /**
   * Set the element's content to the string representation of a
   * <i>long</i>.
   */
  XMLElement& operator=(long val);

  /**
   * Set the element's content to the string representation of an
   * <i>unsigned long</i>.
   */
  XMLElement& operator=(unsigned long val);

  /**
   * Set the element's content to the string representation of an
   * <i>int64_t</i>.
   */
  XMLElement& operator=(const int64_t& val);

  /**
   * Set the element's content to the string representation of a
   * <i>uint64_t</i>.
   */
  XMLElement& operator=(const uint64_t& val);

  /**
   * Set the element's content to the string representation of a
   * <i>float</i>.
   */
  XMLElement& operator=(float val);

  /**
   * Set the element's content to the string representation of a
   * <i>double</i>.
   */
  XMLElement& operator=(const double& val);

  /** Set the element's content. Equivalent to setContent(). */
  XMLElement& operator=(const String& val);

  /** Return the element's content as a <i>bool</i>. */
  bool toBool() const;

  /** Return the element's content as a <i>long</i>. */
  int toInt() const;

  /** Return the element's content as an <i>unsigned long</i>. */
  uint_t toUInt() const;

  /** Return the element's content as an <i>int64_t</i>. */
  int64_t toInt64() const;

  /** Return the element's content as a <i>uint64_t</i>. */
  uint64_t toUInt64() const;

  /** Return the element's content as a <i>float</i>. */
  float toFloat() const;

  /** Return the element's content as a <i>double</i>. */
  double toDouble() const;

  /** Return the element's content as a String. Equivalent to getContent(). */
  inline String toString() const
  { return(getContent()); }

  /** Test if the element is a "null" element. */
  inline bool isNull() const
  { return(_name.isNull()); }

  /** Boolean evaluation cast operator. */
  inline operator const void*() const
  { return(isNull() ? NULL : this); }

  /** Test if the element is a "null" element. */
  inline bool operator!() const
  { return(isNull()); }

  /** Write the element (and its children, recursively) to a stream. */
  void write(std::ostream& stream, int depth, uint_t tabWidth) const;

  /** Determine if an element or attribute name is valid. */
  static void validateName(const String& name);

  /** The "null" element. */
  static XMLElement null;

 private:

  const XMLElement& _getChild(const String& name = String::null) const;
  const XMLElement& _getChild(uint_t index) const;

  String _name;
  String _content;

  class XMLAttributeMap; // fwd decl

  XMLAttributeMap* _attrs;
  XMLElement* _firstChild;
  XMLElement* _lastChild;
  XMLElement* _next;
  XMLElement* _parent;

  XMLElement(const String& name, XMLElement* parent = NULL);
  ~XMLElement();

  CCXX_COPY_DECLS(XMLElement);
};

} // namespace ccxx

#endif // __ccxx_XMLElement_hxx
