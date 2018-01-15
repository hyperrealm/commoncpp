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

#ifndef __ccxx_Locale_hxx
#define __ccxx_Locale_hxx

#include <commonc++/String.h++>

#ifndef CCXX_OS_ANDROID

namespace ccxx {

/**
 * A class that encapsulates locale-specific information.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Locale
{
 public:

  /**
   * Construct a new Locale for the given language, and, optionally,
   * country code and variant.
   */
  Locale(String language, String country = String::null,
         String variant = String::null);

  /** Destructor. */
  ~Locale();

  /** Determine if this object represents a valid locale. */
  bool isValid() const;

  /** Get the default character encoding for this locale. */
  String getEncoding() const;

  /** Get the <i>ante meridiem</i> suffix for this locale, if any. */
  String getAMAffix() const;

  /** Get the <i>post meridiem</i> suffix for this locale, if any. */
  String getPMAffix() const;

  /**
   * Get the full weekday name for the given weekday in this locale.
   *
   * @param day The weekday number (1 - 7; where 1 indicates Monday).
   */
  String getWeekDayName(int day) const;

  /**
   * Get the abbreviated weekday name for the given weekday in this locale.
   *
   * @param day The weekday number (1 - 7; where 1 indicates Monday).
   */
  String getAbbreviatedWeekDayName(int day) const;

  /**
   * Get the full month name for the given month in this locale.
   *
   * @param month The month number (1 - 12).
   */
  String getMonthName(int month) const;

  /**
   * Get the abbreviated month name for the given month in this locale.
   *
   * @param month The month number (1 - 12).
   */
  String getAbbreviatedMonthName(int month) const;

  /** Get the radix character for this locale. */
  String getRadixCharacter() const;

  /** Get the thousands separator for this locale. */
  String getThousandsSeparator() const;

  /** Get the currency symbol for this locale. */
  String getCurrencySymbol() const;

  /** Get the language component of this locale. */
  inline String getLanguage() const
  { return(_language); }

  /** Get the country component of this locale. */
  inline String getCountry() const
  { return(_country); }

  /** Get the variant component of this locale. */
  inline String getVariant() const
  { return(_variant); }

  /** Get a String representation of this locale. */
  inline String toString() const
  { return(_id); }

  /** Get the current character encoding. */
  static String getCurrentEncoding();

  /** Get the current locale. */
  static const Locale getCurrent();

  /** Set the current locale. */
  static bool setCurrent(const Locale& locale);

  /** The "en_CA" locale. */
  static const Locale CANADA;
  /** The "fr_CA" locale. */
  static const Locale CANADA_FRENCH;
  /** The "zh_CN" locale. */
  static const Locale CHINA;
  /** The "zh" locale. */
  static const Locale CHINESE;
  /** The "en" locale. */
  static const Locale ENGLISH;
  /** The "fr_FR" locale. */
  static const Locale FRANCE;
  /** The "fr" locale. */
  static const Locale FRENCH;
  /** The "de_DE" locale. */
  static const Locale GERMANY;
  /** The "de" locale. */
  static const Locale GERMAN;
  /** The "it_IT" locale. */
  static const Locale ITALY;
  /** The "it" locale. */
  static const Locale ITALIAN;
  /** The "ja" locale. */
  static const Locale JAPAN;
  /** The "ja" locale. */
  static const Locale JAPANESE;
  /** The "ko" locale. */
  static const Locale KOREA;
  /** The "ko" locale. */
  static const Locale KOREAN;
  /** The "es" locale. */
  static const Locale SPANISH;
  /** The "zh_CN" locale. */
  static const Locale SIMPLIFIED_CHINESE;
  /** The "zh_TW" locale. */
  static const Locale TRADITIONAL_CHINESE;
  /** The "zh_TW" locale. */
  static const Locale TAIWAN;
  /** The "en_GB" locale. */
  static const Locale UNITED_KINGDOM;
  /** The "en_US" locale. */
  static const Locale UNITED_STATES;

 private:

#ifdef CCXX_OS_WINDOWS
  Locale(LCID locale);
#endif

  String _language;
  String _country;
  String _variant;
  String _id;
#ifdef CCXX_OS_WINDOWS
  LCID _locale;
#else
  void *_locale;
#endif
};

} // namespace ccxx

#endif // CCXX_OS_ANDROID

#endif // __ccxx_Locale_hxx
