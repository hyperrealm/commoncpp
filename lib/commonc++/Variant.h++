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

#ifndef __ccxx_Variant_hxx
#define __ccxx_Variant_hxx

#include <commonc++/Common.h++>
#include <commonc++/Blob.h++>
#include <commonc++/String.h++>

#include <sys/types.h>
#include <iostream>

namespace ccxx {

/**
 * A general-purpose variant type. Variant can hold a value of one
 * of the following types: <i>null</i>, <i>bool</i>, <i>char</i>,
 * <i>char16_t</i>, <i>int32_t</i>, <i>uint32_t</i>, <i>float</i>,
 * <i>double</i>, <i>int64_t</i>, <i>uint64_t</i>, String, Blob,
 * opaque, list, or map. The latter two allow for the construction
 * of arbitrarily complex data structures composed of Variant
 * objects.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Variant
{
 public:

  /** Variant value types. */
  enum Type {
    /** null value */
    TypeNull = 0,
    /** bool value */
    TypeBool,
    /** char value */
    TypeChar,
    /** char16_t value */
    TypeChar16,
    /** int value */
    TypeInt,
    /** unsigned int value */
    TypeUInt,
    /** float value */
    TypeFloat,
    /** double value */
    TypeDouble,
    /** 64-bit int value */
    TypeInt64,
    /** 64-bit unsigned int value */
    TypeUInt64,
    /** String value */
    TypeString,
    /** Blob value */
    TypeBlob,
    /** opaque pointer value */
    TypeOpaque,
    /** list value */
    TypeList,
    /** map value */
    TypeMap
  };

  /** Construct a new Variant with a null type. */
  Variant();

  /** Copy constructor. */
  Variant(const Variant& other);

  /**
   * Construct a new <i>bool</i> variant.
   *
   * @param val The initial value.
   */
  explicit Variant(bool val);

  /**
   * Construct a new <i>char</i> variant.
   *
   * @param val The initial value.
   */
  explicit Variant(char val);

  /**
   * Construct a new <i>char16_t</i> variant.
   *
   * @param val The initial value.
   */
  explicit Variant(char16_t val);

  /**
   * Construct a new <i>int32_t</i> variant.
   *
   * @param val The initial value.
   */
  explicit Variant(int32_t val);

  /**
   * Construct a new <i>uint32_t</i> variant.
   *
   * @param val The initial value.
   */
  explicit Variant(uint32_t val);

  /**
   * Construct a new <i>float</i> variant.
   *
   * @param val The initial value.
   */
  explicit Variant(float val);

  /**
   * Construct a new <i>double</i> variant.
   *
   * @param val The initial value.
   */
  explicit Variant(const double& val);

  /**
   * Construct a new <i>int64_t</i> variant.
   *
   * @param val The initial value.
   */
  explicit Variant(const int64_t& val);

  /**
   * Construct a new <i>uint64_t</i> variant.
   *
   * @param val The initial value.
   */
  explicit Variant(const uint64_t& val);

  /**
   * Construct a new String variant.
   *
   * @param val The initial value.
   */
  explicit Variant(const String& val);

  /**
   * Construct a new String variant.
   *
   * @param val The initial value.
   */
  explicit Variant(const char* val);

  /**
   * Construct a new String variant.
   *
   * @param val The initial value.
   */
  explicit Variant(const char16_t* val);

  /**
   * Construct a new Blob variant.
   *
   * @param val The initial value.
   */
  explicit Variant(const Blob& val);

  /**
   * Construct a new opaque pointer value variant.
   *
   * @param val The initial value.
   */
  explicit Variant(void* val);

  /** Destructor. */
  virtual ~Variant();

  /** Get the type of this Variant. */
  inline Type getType() const
  { return(_type); }

  /**
   * Set the type of this variant. The value is initialized to the default
   * value for the given type. Any previous value is deleted.
   *
   * @param type The new type for the Variant.
   */
  void setType(Type type);

  /** Test if the Variant is <i>null</i>. */
  inline bool isNull() const
  { return(_type == TypeNull); }

  /** Test if the Variant's value is a <i>bool</i>. */
  inline bool isBool() const
  { return(_type == TypeBool); }

  /** Test if the Variant's value is a <i>char</i>. */
  inline bool isChar() const
  { return(_type == TypeChar); }

  /** Test if the Variant's value is a <i>char16_t</i>. */
  inline bool isChar16() const
  { return(_type == TypeChar16); }

  /** Test if the Variant's value is an <i>int32_t</i>. */
  inline bool isInt() const
  { return(_type == TypeInt); }

  /** Test if the Variant's value is a <i>uint32_t</i>. */
  inline bool isUInt() const
  { return(_type == TypeUInt); }

  /** Test if the Variant's value is a <i>float</i>. */
  inline bool isFloat() const
  { return(_type == TypeFloat); }

  /** Test if the Variant's value is a <i>double</i>. */
  inline bool isDouble() const
  { return(_type == TypeDouble); }

  /** Test if the Variant's value is an <i>int64_t</i>. */
  inline bool isInt64() const
  { return(_type == TypeInt64); }

  /** Test if the Variant's value is a <i>uint64_t</i>. */
  inline bool isUInt64() const
  { return(_type == TypeUInt64); }

  /** Test if the Variant's value is a String. */
  inline bool isString() const
  { return(_type == TypeString); }

  /** Test if the Variant's value is a Blob. */
  inline bool isBlob() const
  { return(_type == TypeBlob); }

  /** Test if the Variant's value is opaque. */
  inline bool isOpaque() const
  { return(_type == TypeOpaque); }

  /** Test if the Variant's value is a list. */
  inline bool isList() const
  { return(_type == TypeList); }

  /** Test if the Variant's value is a map. */
  inline bool isMap() const
  { return(_type == TypeMap); }

  /**
   * Return the Variant's value as a <i>bool</i>, converting it if
   * necessary, or return <code>false</code> if a conversion is
   * not possible or not meaningful.
   */
  bool toBool() const;

  /**
   * Return the Variant's value as a <i>char</i>, converting it if
   * necessary, or return <code>'\\0'</code> if a conversion is
   * not possible or not meaningful.
   */
  char toChar() const;

  /**
   * Return the Variant's value as a <i>char16_t</i>, converting it if
   * necessary, or return <code>0</code> if a conversion is
   * not possible or not meaningful.
   */
  char16_t toChar16() const;

  /**
   * Return the Variant's value as an <i>int32_t</i>, converting it if
   * necessary, or return <code>0</code> if a conversion is
   * not possible or not meaningful.
   */
  int32_t toInt() const;

  /**
   * Return the Variant's value as a <i>uint32_t</i>, converting it if
   * necessary, or return <code>0</code> if a conversion is
   * not possible or not meaningful.
   */
  uint32_t toUInt() const;

  /**
   * Return the Variant's value as a <i>float</i>, converting it if
   * necessary, or return <code>0.0f</code> if a conversion is
   * not possible or not meaningful.
   */
  float toFloat() const;

  /**
   * Return the Variant's value as a <i>double</i>, converting it if
   * necessary, or return <code>0.0</code> if a conversion is
   * not possible or not meaningful.
   */
  double toDouble() const;

  /**
   * Return the Variant's value as an <i>int64_t</i>, converting it
   * if necessary, or return <code>0</code> if a conversion is
   * not possible or not meaningful.
   */
  int64_t toInt64() const;

  /**
   * Return the Variant's value as a <i>uint64_t</i>, converting it
   * if necessary, or return <code>0</code> if a conversion is
   * not possible or not meaningful.
   */
  uint64_t toUInt64() const;

  /**
   * Return the Variant's value as an opaque pointer, or
   * <code>NULL</code> if the value is not an opaque.
   */
  void* toOpaque() const;

  /**
   * Return the Variant's value as a String, converting it
   * if necessary, or return <code>String::null</code> if a
   * conversion is not possible or not meaningful.
   */
  String toString() const;

  /**
   * Return the Variant's value as a Blob, converting it
   * if necessary, or return <code>Blob::null</code> if a
   * conversion is not possible or not meaningful.
   */
  Blob toBlob() const;

  /** Cast operator. Equivalent to toBool(). */
  inline operator bool() const
  { return(toBool()); }

  /** Cast operator. Equivalent to toChar(). */
  inline operator char() const
  { return(toChar()); }

  /** Cast operator. Equivalent to toChar16(). */
  inline operator char16_t() const
  { return(toChar16()); }

  /** Cast operator. Equivalent to toInt(). */
  inline operator int32_t() const
  { return(toInt()); }

  /** Cast operator. Equivalent to toUInt(). */
  inline operator uint32_t() const
  { return(toUInt()); }

  /** Cast operator. Equivalent to toFloat(). */
  inline operator float() const
  { return(toFloat()); }

  /** Cast operator. Equivalent to toDouble(). */
  inline operator double() const
  { return(toDouble()); }

  /** Cast operator. Equivalent to toString(). */
  inline operator String() const
  { return(toString()); }

  /** Cast operator. Equivalent to toBlob(). */
  inline operator Blob() const
  { return(toBlob()); }

  /** Cast operator. Equivalent to toInt64(). */
  inline operator int64_t() const
  { return(toInt64()); }

  /** Cast operator. Equivalent to toUInt64(). */
  inline operator uint64_t() const
  { return(toUInt64()); }

  /**
   * Boolean evaluation cast operator. This is <b>not</b>
   * equivalent to toOpaque().
   */
  inline operator const void*() const
  { return(isNull() ? NULL : this); }

  /** Equivalent to isNull(). */
  inline bool operator!() const
  { return(isNull()); }

  /**
   * Get the length of the value. For scalar types, this method
   * returns 0.  For maps and lists, it returns the number of items
   * in the map or list.
   */
  uint_t length() const;

  /**
   * Clear the Variant. Sets the value to the default value for the
   * current type.
   */
  void clear();

  /** Set the value to <b>null</b>. Any previous value is deleted. */
  void setNull();

  /** Assign a <code>bool</code> value to the Variant. */
  Variant& operator=(bool val);

  /** Assign a <code>char</code> value to the Variant. */
  Variant& operator=(char val);

  /** Assign a <code>char16_t</code> value to the Variant. */
  Variant& operator=(char16_t val);

  /** Assign an <code>int32_t</code> value to the Variant. */
  Variant& operator=(int32_t val);

  /** Assign a <code>uint32_t</code> value to the Variant. */
  Variant& operator=(uint32_t val);

  /** Assign a <code>float</code> value to the Variant. */
  Variant& operator=(float val);

  /** Assign a <code>double</code> value to the Variant. */
  Variant& operator=(const double& val);

  /** Assign an <code>int64_t</code> value to the Variant. */
  Variant& operator=(const int64_t& val);

  /** Assign a <code>uint64_t</code> value to the Variant. */
  Variant& operator=(const uint64_t& val);

  /** Assign a String value to the Variant. */
  Variant& operator=(const String& val);

  /** Assign a string value to the Variant. */
  inline Variant& operator=(const char* val)
  { return(operator=(String(val))); }

  /** Assign a Blob value to the Variant. */
  Variant& operator=(const Blob& val);

  /** Assign an opaque pointer value to the Variant. */
  Variant& operator=(void* val);

  /** Assignment operator. */
  Variant& operator=(const Variant& other);

  /**
   * Return the value for the given name from this Variant, whose
   * value is assumed to be a map. If the variant is not a map, it
   * is automatically re-initialized as a map. If no value with the given
   * name exists, a new one is automatically inserted into the map.
   *
   * @param name The name of the value.
   * @return The value for the given name, or if the requested value was
   * not found, the new, <b>null</b> value that was added to the map.
   */
  Variant& operator[](const String& name);

  /**
   * Return the value for the given name from this Variant, whose
   * value is assumed to be a map. If the variant is not a map, it
   * is automatically re-initialized as a map. If no value with the given
   * name exists, a new one is automatically inserted into the map.
   *
   * @param name The name of the value.
   * @return The value for the given name, or if the requested value was
   * not found, the new, <b>null</b> value that was added to the map.
   */
  inline Variant& operator[](const char* name)
  { return(operator[](String(name))); }

  /**
   * Return the value for the given name from this Variant, whose value
   * is assumed to be a map.
   *
   * @param name The name of the value.
   * @return The value for the given name, or Variant::null if no value
   * exists for the given name, or if this Variant is not a map.
   */
  const Variant &get(const String& name) const;

  /**
   * Return the value for the given name from this Variant, whose value
   * is assumed to be a map.
   *
   * @param name The name of the value.
   * @return The value for the given name, or Variant::null if no value
   * exists for the given name, or if this Variant is not a map.
   */
  inline const Variant &operator[](const String& name) const
  { return(get(name)); }

  /**
   * Return the value for the given name from this Variant, whose value
   * is assumed to be a map. Equivalent to get().
   *
   * @param name The name of the value.
   * @return The value for the given name, or Variant::null if no value
   * exists for the given name, or if this Variant is not a map.
   */
  inline const Variant &operator[](const char* name) const
  { return(get(String(name))); }

  /**
   * Return the value at the given index from this Variant, whose
   * value is assumed to be a list. If the variant is not a list, it
   * is automatically re-initialized as a list.  If the index is out
   * of range, the list is automatically extended with new values up
   * to that index.
   *
   * @return The at the given index, or Variant::null if this Variant is
   * not a list.
   * @param index The index.
   * @throw OutOfBoundsException If the index is negative.
   */
  Variant& operator[](int index);

  /**
   * Return the value at the given index from this Variant, whose value
   * is assumed to be a list.
   *
   * @param index The index.
   * @return The value at the given index, or Variant::null if the index
   * is out of range, or if this Variant is not a list.
   * @throw OutOfBoundsException If the index is negative.
   */
  const Variant &get(int index) const;

  /**
   * Return the value at the given index from this Variant, whose value
   * is assumed to be a list. Equivalent to get().
   *
   * @param index The index.
   * @return The value at the given index, or Variant::null if the index
   * is out of range, or if this Variant is not a list.
   * @throw OutOfBoundsException If the index is negative.
   */
  inline const Variant &operator[](int index) const
  { return(get(index)); }

  /**
   * If this Variant is a map, stores a list of the map keys into
   * the supplied vector and returns <b>true</b>. Otherwise, returns
   * <b>false</b>.
   *
   * @param vec The vector in which to store the keys.
   */
  bool getKeys(StringVec& vec) const;

  /**
   * If this Variant is a map, determines if the map contains the specified
   * key.
   *
   * @param key The key to search for.
   * @return <b>true</b> if the key is found, <b>false</b> otherwise.
   */
  bool hasKey(const String& key) const;

  /** Equality operator. */
  bool operator==(const Variant& other) const;

  /** Inequality operator. */
  inline bool operator!=(const Variant& other) const
  { return(! operator==(other)); }

  /**
   * Write the value of this Variant to a stream.
   *
   * @param stream The stream to write to.
   */
  void write(std::ostream &stream) const;

  /** A <b>null</b> Variant. */
  static const Variant null;

 private:

  class VariantList; // fwd decl
  class VariantMap; // fwd decl

  void _copy(const Variant& other);
  void _deleteValue();
  void _initValue();

  Type _type;

  union {
    bool boolVal;
    char charVal;
    char16_t char16Val;
    int32_t intVal;
    uint32_t uintVal;
    float floatVal;
    double doubleVal;
    int64_t int64Val;
    uint64_t uint64Val;
    String* stringVal;
    Blob* blobVal;
    VariantList* listVal;
    VariantMap* mapVal;
    void* opaqueVal;
  } _value;

};

/** Write a Variant to an output stream.
 *
 * @param stream The stream to write to.
 * @param v The variant to write.
 * @return The stream.
 */
inline std::ostream& operator<<(std::ostream& stream, const Variant& v) {
  v.write(stream);
  return(stream);
}

} // namespace ccxx

#endif // __ccxx_Variant_hxx
