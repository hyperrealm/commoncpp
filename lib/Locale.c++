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

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include <commonc++/Locale.h++>

#ifndef CCXX_OS_ANDROID

#ifdef CCXX_OS_POSIX

#ifdef HAVE_LANGINFO_H
#include <langinfo.h>
#endif

#ifdef HAVE_XLOCALE_H
#include <xlocale.h>
#endif

#if defined(HAVE___NEWLOCALE) && !defined(HAVE_NEWLOCALE)
// Must be a pre-2008 POSIX system
#define locale_t      __locale_t
#define newlocale     __newlocale
#define freelocale    __freelocale
#define nl_langinfo_l __nl_langinfo_l
#define LC_ALL        __LC_ALL
#define LC_ALL_MASK   __LC_ALL
#endif // defined(HAVE___NEWLOCALE) && !defined(HAVE_NEWLOCALE)

#endif // CCXX_OS_POSIX

namespace ccxx {

/*
 */

#ifdef CCXX_OS_WINDOWS

struct CodePage
{
  int id;
  const char *encoding;
};

static const struct CodePage __codepages[] = {
  {       0, "MS-ANSI" },
  {       1, "WINDOWS-1252" },
  {     437, "MS-ANSI" },
  {     737, "MS-GREEK" },
  {     775, "WINBALTRIM" },
  {     850, "MS-ANSI" },
  {     852, "MS-EE" },
  {     857, "MS-TURK" },
  {     862, "CP862" },
  {     864, "CP864" },
  {     866, "MS-CYRL" },
  {     874, "WINDOWS-874" },
  {     932, "CP932" },
  {     936, "CP936" },  // equivalent to GB2312
  {     949, "CP949" },
  {     950, "CP950" },
  {    1200, "UTF-16LE" },
  {    1201, "UTF-16BE" },
  {    1250, "WINDOWS-1250" },
  {    1251, "WINDOWS-1251" },
  {    1252, "WINDOWS-1252" },
  {    1253, "WINDOWS-1253" },
  {    1254, "WINDOWS-1254" },
  {    1255, "WINDOWS-1255" },
  {    1256, "WINDOWS-1256" },
  {    1257, "WINDOWS-1257" },
  {    1258, "WINDOWS-1258" },
  {   12000, "UTF-32LE" },
  {   12001, "UTF-32BE" },
  { CP_UTF7, "UTF-7" },
  { CP_UTF8, "UTF-8" },
  { 0, NULL }
};

/*
 */

struct Language
{
  const char *language;
  const char *country;
  WORD id;
};

static const struct Language __languages[] = {

  { "af", NULL, MAKELANGID(LANG_AFRIKAANS,  SUBLANG_DEFAULT) },
  { "sq", NULL, MAKELANGID(LANG_ALBANIAN,   SUBLANG_DEFAULT) },
  { "ar", NULL, MAKELANGID(LANG_ARABIC,     SUBLANG_DEFAULT) },
  { "ar", "IQ", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_IRAQ) },
  { "ar", "EG", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_EGYPT) },
  { "ar", "LY", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_LIBYA) },
  { "ar", "DZ", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_ALGERIA) },
  { "ar", "MA", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_MOROCCO) },
  { "ar", "TN", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_TUNISIA) },
  { "ar", "OM", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_OMAN) },
  { "ar", "YE", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_YEMEN) },
  { "ar", "SY", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_SYRIA) },
  { "ar", "JO", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_JORDAN) },
  { "ar", "LB", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_LEBANON) },
  { "ar", "KW", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_KUWAIT) },
  { "ar", "AE", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_UAE) },
  { "ar", "BH", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_BAHRAIN) },
  { "ar", "QA", MAKELANGID(LANG_ARABIC,     SUBLANG_ARABIC_QATAR) },
  { "hy", NULL, MAKELANGID(LANG_ARMENIAN,   SUBLANG_DEFAULT) },
  { "as", NULL, MAKELANGID(LANG_ASSAMESE,   SUBLANG_DEFAULT) },
  { "az", "AZ", MAKELANGID(LANG_AZERI,      SUBLANG_AZERI_LATIN) },
  { "eu", NULL, MAKELANGID(LANG_BASQUE,     SUBLANG_DEFAULT) },
  { "be", NULL, MAKELANGID(LANG_BELARUSIAN, SUBLANG_DEFAULT) },
  { "bn", NULL, MAKELANGID(LANG_BENGALI,    SUBLANG_DEFAULT) },
  { "bg", NULL, MAKELANGID(LANG_BULGARIAN,  SUBLANG_DEFAULT) },
  { "ca", NULL, MAKELANGID(LANG_CATALAN,    SUBLANG_DEFAULT) },
//  { "zh", NULL, MAKELANGID(LANG_CHINESE,    SUBLANG_DEFAULT) },
  { "zh", "CN", MAKELANGID(LANG_CHINESE,    SUBLANG_CHINESE_SIMPLIFIED) },
  { "zh", "TW", MAKELANGID(LANG_CHINESE,    SUBLANG_CHINESE_TRADITIONAL) },
  { "zh", "HK", MAKELANGID(LANG_CHINESE,    SUBLANG_CHINESE_HONGKONG) },
  { "zh", "SG", MAKELANGID(LANG_CHINESE,    SUBLANG_CHINESE_SINGAPORE) },
  { "zh", "MO", MAKELANGID(LANG_CHINESE,    SUBLANG_CHINESE_MACAU) },
  { "hr", NULL, MAKELANGID(LANG_CROATIAN,   SUBLANG_DEFAULT) },
  { "cs", NULL, MAKELANGID(LANG_CZECH,      SUBLANG_DEFAULT) },
  { "da", NULL, MAKELANGID(LANG_DANISH,     SUBLANG_DEFAULT) },
  { "nl", NULL, MAKELANGID(LANG_DUTCH,      SUBLANG_DUTCH) },
  { "nl", "NL", MAKELANGID(LANG_DUTCH,      SUBLANG_DUTCH) },
  { "nl", "BE", MAKELANGID(LANG_DUTCH,      SUBLANG_DUTCH_BELGIAN) },
//  { "en", NULL, MAKELANGID(LANG_ENGLISH,    SUBLANG_DEFAULT) },
  { "en", "US", MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_US) },
  { "en", "GB", MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_UK) },
  { "en", "AU", MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_AUS) },
  { "en", "CA", MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_CAN) },
  { "en", "NZ", MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_NZ) },
  { "en", "IE", MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_EIRE) },
  { "en", "ZA", MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_SOUTH_AFRICA) },
  { "en", "JM", MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_JAMAICA) },
  { "en", "BZ", MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_BELIZE) },
  { "en", "TT", MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_TRINIDAD) },
  { "en", "PH", MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_PHILIPPINES) },
  { "en", "ZW", MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_ZIMBABWE) },
  { "et", NULL, MAKELANGID(LANG_ESTONIAN,   SUBLANG_DEFAULT) },
  { "fo", NULL, MAKELANGID(LANG_FAEROESE,   SUBLANG_DEFAULT) },
  { "fa", NULL, MAKELANGID(LANG_FARSI,      SUBLANG_DEFAULT) },
  { "fi", NULL, MAKELANGID(LANG_FINNISH,    SUBLANG_DEFAULT) },
//  { "fr", NULL, MAKELANGID(LANG_FRENCH,     SUBLANG_DEFAULT) },
  { "fr", "FR", MAKELANGID(LANG_FRENCH,     SUBLANG_FRENCH) },
  { "fr", "BE", MAKELANGID(LANG_FRENCH,     SUBLANG_FRENCH_BELGIAN) },
  { "fr", "CA", MAKELANGID(LANG_FRENCH,     SUBLANG_FRENCH_CANADIAN) },
  { "fr", "CH", MAKELANGID(LANG_FRENCH,     SUBLANG_FRENCH_SWISS) },
  { "fr", "LU", MAKELANGID(LANG_FRENCH,     SUBLANG_FRENCH_LUXEMBOURG) },
  { "fr", "MC", MAKELANGID(LANG_FRENCH,     SUBLANG_FRENCH_MONACO) },
  { "ka", NULL, MAKELANGID(LANG_GEORGIAN,   SUBLANG_DEFAULT) },
//  { "de", NULL, MAKELANGID(LANG_GERMAN,     SUBLANG_DEFAULT) },
  { "de", "DE", MAKELANGID(LANG_GERMAN,     SUBLANG_GERMAN) },
  { "de", "CH", MAKELANGID(LANG_GERMAN,     SUBLANG_GERMAN_SWISS) },
  { "de", "AT", MAKELANGID(LANG_GERMAN,     SUBLANG_GERMAN_AUSTRIAN) },
  { "de", "LU", MAKELANGID(LANG_GERMAN,     SUBLANG_GERMAN_LUXEMBOURG) },
  { "de", "LI", MAKELANGID(LANG_GERMAN,     SUBLANG_GERMAN_LIECHTENSTEIN) },
  { "el", NULL, MAKELANGID(LANG_GREEK,      SUBLANG_DEFAULT) },
  { "gu", NULL, MAKELANGID(LANG_GUJARATI,   SUBLANG_DEFAULT) },
  { "he", NULL, MAKELANGID(LANG_HEBREW,     SUBLANG_DEFAULT) },
  { "hi", NULL, MAKELANGID(LANG_HINDI,      SUBLANG_DEFAULT) },
  { "hu", NULL, MAKELANGID(LANG_HUNGARIAN,  SUBLANG_DEFAULT) },
  { "is", NULL, MAKELANGID(LANG_ICELANDIC,  SUBLANG_DEFAULT) },
  { "id", NULL, MAKELANGID(LANG_INDONESIAN, SUBLANG_DEFAULT) },
//  { "it", NULL, MAKELANGID(LANG_ITALIAN,    SUBLANG_DEFAULT) },
  { "it", "IT", MAKELANGID(LANG_ITALIAN,    SUBLANG_ITALIAN) },
  { "it", "CH", MAKELANGID(LANG_ITALIAN,    SUBLANG_ITALIAN_SWISS) },
  { "ja", NULL, MAKELANGID(LANG_JAPANESE,   SUBLANG_DEFAULT) },
  { "kn", NULL, MAKELANGID(LANG_KANNADA,    SUBLANG_DEFAULT) },
  { "ks", NULL, MAKELANGID(LANG_KASHMIRI,   SUBLANG_DEFAULT) },
  { "ks", "IN", MAKELANGID(LANG_KASHMIRI,   SUBLANG_KASHMIRI_INDIA) },
  { "kk", NULL, MAKELANGID(LANG_KAZAK,      SUBLANG_DEFAULT) },
  { "ko", NULL, MAKELANGID(LANG_KOREAN,     SUBLANG_KOREAN) },
  { "lv", NULL, MAKELANGID(LANG_LATVIAN,    SUBLANG_DEFAULT) },
  { "lt", NULL, MAKELANGID(LANG_LITHUANIAN, SUBLANG_LITHUANIAN) },
  { "mk", NULL, MAKELANGID(LANG_MACEDONIAN, SUBLANG_DEFAULT) },
  { "ms", NULL, MAKELANGID(LANG_MALAY,      SUBLANG_DEFAULT) },
  { "ms", "BN", MAKELANGID(LANG_MALAY,      SUBLANG_MALAY_BRUNEI_DARUSSALAM) },
  { "ms", "MY", MAKELANGID(LANG_MALAY,      SUBLANG_MALAY_MALAYSIA) },
  { "ml", NULL, MAKELANGID(LANG_MALAYALAM,  SUBLANG_DEFAULT) },
  { "mr", NULL, MAKELANGID(LANG_MARATHI,    SUBLANG_DEFAULT) },
  { "ne", NULL, MAKELANGID(LANG_NEPALI,     SUBLANG_DEFAULT) },
  { "ne", "IN", MAKELANGID(LANG_NEPALI,     SUBLANG_NEPALI_INDIA) },
  { "no", NULL, MAKELANGID(LANG_NORWEGIAN,  SUBLANG_DEFAULT) },
  // NORWEGIAN_BOKMAL
  // NORWEGIAN_NYNORSK
  { "or", NULL, MAKELANGID(LANG_ORIYA,      SUBLANG_DEFAULT) },
  { "pl", NULL, MAKELANGID(LANG_POLISH,     SUBLANG_DEFAULT) },
  { "pt", NULL, MAKELANGID(LANG_PORTUGUESE, SUBLANG_DEFAULT) },
  { "pt", "PT", MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE) },
  { "pt", "BR", MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN) },
  { "pa", NULL, MAKELANGID(LANG_PUNJABI,    SUBLANG_DEFAULT) },
  { "ro", NULL, MAKELANGID(LANG_ROMANIAN,   SUBLANG_DEFAULT) },
  { "ru", NULL, MAKELANGID(LANG_RUSSIAN,    SUBLANG_DEFAULT) },
  { "sa", NULL, MAKELANGID(LANG_SANSKRIT,   SUBLANG_DEFAULT) },
  { "sr", NULL, MAKELANGID(LANG_SERBIAN,    SUBLANG_DEFAULT) },
  // SUBLANG_SERBIAN_LATIN
  // SUBLANG_SERBIAN_CYRILLIC
  { "sd", NULL, MAKELANGID(LANG_SINDHI,     SUBLANG_DEFAULT) },
  { "sk", NULL, MAKELANGID(LANG_SLOVAK,     SUBLANG_DEFAULT) },
  { "sl", NULL, MAKELANGID(LANG_SLOVENIAN,  SUBLANG_DEFAULT) },
  { "es", NULL, MAKELANGID(LANG_SPANISH,    SUBLANG_DEFAULT) },
  { "es", "MX", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_MEXICAN) },
  { "es", "GT", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_GUATEMALA) },
  { "es", "CR", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_COSTA_RICA) },
  { "es", "PA", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_PANAMA) },
  { "es", "DO", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_DOMINICAN_REPUBLIC) },
  { "es", "VE", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_VENEZUELA) },
  { "es", "CO", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_COLOMBIA) },
  { "es", "PE", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_PERU) },
  { "es", "AR", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_ARGENTINA) },
  { "es", "EC", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_ECUADOR) },
  { "es", "CL", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_CHILE) },
  { "es", "UY", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_URUGUAY) },
  { "es", "PY", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_PARAGUAY) },
  { "es", "BO", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_BOLIVIA) },
  { "es", "SV", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_EL_SALVADOR) },
  { "es", "HN", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_HONDURAS) },
  { "es", "NI", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_NICARAGUA) },
  { "es", "PR", MAKELANGID(LANG_SPANISH,    SUBLANG_SPANISH_PUERTO_RICO) },
  // SUBLANG_SPANISH_MODERN
  { "sw", NULL, MAKELANGID(LANG_SWAHILI,    SUBLANG_DEFAULT) },
  { "sv", NULL, MAKELANGID(LANG_SWEDISH,    SUBLANG_DEFAULT) },
  { "sv", "SE", MAKELANGID(LANG_SWEDISH,    SUBLANG_SWEDISH) },
  { "sv", "FI", MAKELANGID(LANG_SWEDISH,    SUBLANG_SWEDISH_FINLAND) },
  { "ta", NULL, MAKELANGID(LANG_TAMIL,      SUBLANG_DEFAULT) },
  { "tt", NULL, MAKELANGID(LANG_TATAR,      SUBLANG_DEFAULT) },
  { "te", NULL, MAKELANGID(LANG_TELUGU,     SUBLANG_DEFAULT) },
  { "th", NULL, MAKELANGID(LANG_THAI,       SUBLANG_DEFAULT) },
  { "tr", NULL, MAKELANGID(LANG_TURKISH,    SUBLANG_DEFAULT) },
  { "uk", NULL, MAKELANGID(LANG_UKRAINIAN,  SUBLANG_DEFAULT) },
  { "ur", NULL, MAKELANGID(LANG_URDU,       SUBLANG_DEFAULT) },
  { "ur", "IN", MAKELANGID(LANG_URDU,       SUBLANG_URDU_INDIA) },
  { "ur", "PK", MAKELANGID(LANG_URDU,       SUBLANG_URDU_PAKISTAN) },
  { "uz", NULL, MAKELANGID(LANG_UZBEK,      SUBLANG_DEFAULT) },
  { "vi", NULL, MAKELANGID(LANG_VIETNAMESE, SUBLANG_DEFAULT) },
  { NULL, NULL, 0 }
};

/*
 */

WORD __lookupLanguage(const char *language, const char *country)
{
  for(const struct Language *lang = __languages;
      lang->language != NULL;
      ++lang)
  {
    if(! ::strcmp(language, lang->language))
    {
      if(country)
      {
        if(! ::strcmp(country, lang->country))
          return(lang->id);
      }
      else if(! lang->country)
        return(lang->id);
    }
  }

  return(0);
}

/*
 */

void __lookupLanguageID(WORD langid, const char **language,
                        const char **country)
{
  for(const struct Language *lang = __languages;
      lang->language != NULL;
      ++lang)
  {
    if(lang->id == langid)
    {
      *language = lang->language;
      *country = lang->country;
      return;
    }
  }
}

#endif // CCXX_OS_WINDOWS

/*
 */

Locale::Locale(String language, String country /* = String::null */,
               String variant /* = String::null */)
  : _language(language),
    _country(country),
    _variant(variant)
{
  if((language == "C") || (language == "POSIX"))
    _id = language;
  else
  {
    _id = language.toLowerCase();
    if(country.length() == 2)
    {
      _id << '_' << country.toUpperCase();

      if(! variant.isEmpty())
        _id << '.' << variant;
    }
  }

#ifdef CCXX_OS_WINDOWS

  WORD langid = 0;

  if(! language.isEmpty())
  {
    CString cstr_language = language.toUTF8();
    CString cstr_country = country.toUTF8();

    const char *lang_id = cstr_language.data();
    const char *country_id = (country.isEmpty() ? NULL : cstr_country.data());

    langid = __lookupLanguage(lang_id, country_id);
    if(! langid)
      langid = __lookupLanguage(lang_id, NULL);
  }

  _locale = MAKELCID(langid, SORT_DEFAULT);

#else

  CString cstr_id = _id.toUTF8();
  _locale = static_cast<void *>(::newlocale(LC_ALL_MASK, cstr_id.data(),
                                            NULL));

#endif
}

/*
 */

#ifdef CCXX_OS_WINDOWS

Locale::Locale(LCID locale)
  : _locale(locale)
{
  WORD langid = static_cast<WORD>(locale & 0xFFFF);
  const char *lang = NULL, *country = NULL;

  __lookupLanguageID(langid, &lang, &country);

  _language = lang;
  _country = country;

  // TODO: _variant, _id ?
}

#endif

/*
 */

Locale::~Locale()
{
#ifdef CCXX_OS_WINDOWS

  // nothing to do

#else

  locale_t locale = static_cast<locale_t>(_locale);

  if(locale)
    ::freelocale(locale);

#endif
}

/*
 */

bool Locale::isValid() const
{
#ifdef CCXX_OS_WINDOWS

  return(_locale != 0);

#else

  return(_locale != NULL);

#endif
}

/*
 */

String Locale::getEncoding() const
{
  const char *enc = NULL;

#ifdef CCXX_OS_WINDOWS

  char buf[32];
  ::GetLocaleInfoA(_locale, LOCALE_IDEFAULTCODEPAGE, buf, sizeof(buf));
  int encoding_id = ::atoi(buf);

  for(const struct CodePage *cp = __codepages; cp->encoding != NULL; ++cp)
  {
    if(cp->id == encoding_id)
    {
      enc = cp->encoding;
      break;
    }
  }

#else

  enc = ::nl_langinfo_l(CODESET, static_cast<locale_t>(_locale));

#endif

  return(enc);
}

/*
 */

String Locale::getAMAffix() const
{
  const char *affix = NULL;

#ifdef CCXX_OS_WINDOWS

  char buf[128];
  ::GetLocaleInfoA(_locale, LOCALE_S1159, buf, sizeof(buf));
  affix = buf;

#else

  affix = ::nl_langinfo_l(AM_STR, static_cast<locale_t>(_locale));

#endif

  return(affix);
}

/*
 */

String Locale::getPMAffix() const
{
  const char *affix = NULL;

#ifdef CCXX_OS_WINDOWS

  char buf[128];
  ::GetLocaleInfoA(_locale, LOCALE_S2359, buf, sizeof(buf));
  affix = buf;

#else

  affix = ::nl_langinfo_l(PM_STR, static_cast<locale_t>(_locale));

#endif

  return(affix);
}

/*
 */

String Locale::getWeekDayName(int day) const
{
  if(day == 0)
    day = 7;
  else if((day < 1) || (day > 7))
    return(String::null);

  const char *name = NULL;

#ifdef CCXX_OS_WINDOWS

  LCTYPE type;

  switch(day)
  {
    case 1:
      type = LOCALE_SDAYNAME1; // Monday
      break;
    case 2:
      type = LOCALE_SDAYNAME2; // Tuesday
      break;
    case 3:
      type = LOCALE_SDAYNAME3; // Wednesday
      break;
    case 4:
      type = LOCALE_SDAYNAME4; // Thursday
      break;
    case 5:
      type = LOCALE_SDAYNAME5; // Friday
      break;
    case 6:
      type = LOCALE_SDAYNAME6; // Saturday
      break;
    case 7:
      type = LOCALE_SDAYNAME7; // Sunday
      break;
    default:
      break;
  }

  char buf[128];
  ::GetLocaleInfoA(_locale, type, buf, sizeof(buf));
  name = buf;

#else

  nl_item item = 0;

  switch(day)
  {
    case 1:
      item = DAY_2; // Monday
      break;
    case 2:
      item = DAY_3; // Tuesday
      break;
    case 3:
      item = DAY_4; // Wednesday
      break;
    case 4:
      item = DAY_5; // Thursday
      break;
    case 5:
      item = DAY_6; // Friday
      break;
    case 6:
      item = DAY_7; // Saturday
      break;
    case 7:
      item = DAY_1; // Sunday
      break;
    default:
      break;
  }

  name = ::nl_langinfo_l(item, static_cast<locale_t>(_locale));

#endif

  return(name);
}

/*
 */

String Locale::getAbbreviatedWeekDayName(int day) const
{
  if(day == 0)
    day = 7;
  else if((day < 1) || (day > 7))
    return(String::null);

  const char *name = NULL;

#ifdef CCXX_OS_WINDOWS

  LCTYPE type;

  switch(day)
  {
    case 1:
      type = LOCALE_SABBREVDAYNAME1; // Monday
      break;
    case 2:
      type = LOCALE_SABBREVDAYNAME2; // Tuesday
      break;
    case 3:
      type = LOCALE_SABBREVDAYNAME3; // Wednesday
      break;
    case 4:
      type = LOCALE_SABBREVDAYNAME4; // Thursday
      break;
    case 5:
      type = LOCALE_SABBREVDAYNAME5; // Friday
      break;
    case 6:
      type = LOCALE_SABBREVDAYNAME6; // Saturday
      break;
    case 7:
      type = LOCALE_SABBREVDAYNAME7; // Sunday
      break;
    default:
      break;
  }

  char buf[128];
  ::GetLocaleInfoA(_locale, type, buf, sizeof(buf));
  name = buf;

#else

  nl_item item = 0;

  switch(day)
  {
    case 1:
      item = ABDAY_2; // Monday
      break;
    case 2:
      item = ABDAY_3; // Tuesday
      break;
    case 3:
      item = ABDAY_4; // Wednesday
      break;
    case 4:
      item = ABDAY_5; // Thursday
      break;
    case 5:
      item = ABDAY_6; // Friday
      break;
    case 6:
      item = ABDAY_7; // Saturday
      break;
    case 7:
      item = ABDAY_1; // Sunday
      break;
    default:
      break;
  }

  name = ::nl_langinfo_l(item, static_cast<locale_t>(_locale));

#endif

  return(name);
}

/*
 */

String Locale::getMonthName(int month) const
{
  if((month < 1) || (month > 12))
    return(String::null);

  const char *name = NULL;

#ifdef CCXX_OS_WINDOWS

  LCTYPE type;

  switch(month)
  {
    case 1:
      type = LOCALE_SMONTHNAME1; // January
      break;
    case 2:
      type = LOCALE_SMONTHNAME2; // February
      break;
    case 3:
      type = LOCALE_SMONTHNAME3; // March
      break;
    case 4:
      type = LOCALE_SMONTHNAME4; // April
      break;
    case 5:
      type = LOCALE_SMONTHNAME5; // May
      break;
    case 6:
      type = LOCALE_SMONTHNAME6; // June
      break;
    case 7:
      type = LOCALE_SMONTHNAME7; // July
      break;
    case 8:
      type = LOCALE_SMONTHNAME8; // August
      break;
    case 9:
      type = LOCALE_SMONTHNAME9; // September
      break;
    case 10:
      type = LOCALE_SMONTHNAME10; // October
      break;
    case 11:
      type = LOCALE_SMONTHNAME11; // November
      break;
    case 12:
      type = LOCALE_SMONTHNAME12; // December
      break;
    default:
      break;
  }

  char buf[128];
  ::GetLocaleInfoA(_locale, type, buf, sizeof(buf));
  name = buf;

#else

  nl_item item = 0;

  switch(month)
  {
    case 1:
      item = MON_1; // January
      break;
    case 2:
      item = MON_2; // February
      break;
    case 3:
      item = MON_3; // March
      break;
    case 4:
      item = MON_4; // April
      break;
    case 5:
      item = MON_5; // May
      break;
    case 6:
      item = MON_6; // June
      break;
    case 7:
      item = MON_7; // July
      break;
    case 8:
      item = MON_8; // August
      break;
    case 9:
      item = MON_9; // September
      break;
    case 10:
      item = MON_10; // October
      break;
    case 11:
      item = MON_11; // November
      break;
    case 12:
      item = MON_12; // December
      break;
    default:
      break;
  }

  name = ::nl_langinfo_l(item, static_cast<locale_t>(_locale));

#endif

  return(name);
}

/*
 */

String Locale::getAbbreviatedMonthName(int month) const
{
  if((month < 1) || (month > 12))
    return(String::null);

  const char *name = NULL;

#ifdef CCXX_OS_WINDOWS

  LCTYPE type;

  switch(month)
  {
    case 1:
      type = LOCALE_SABBREVMONTHNAME1; // January
      break;
    case 2:
      type = LOCALE_SABBREVMONTHNAME2; // February
      break;
    case 3:
      type = LOCALE_SABBREVMONTHNAME3; // March
      break;
    case 4:
      type = LOCALE_SABBREVMONTHNAME4; // April
      break;
    case 5:
      type = LOCALE_SABBREVMONTHNAME5; // May
      break;
    case 6:
      type = LOCALE_SABBREVMONTHNAME6; // June
      break;
    case 7:
      type = LOCALE_SABBREVMONTHNAME7; // July
      break;
    case 8:
      type = LOCALE_SABBREVMONTHNAME8; // August
      break;
    case 9:
      type = LOCALE_SABBREVMONTHNAME9; // September
      break;
    case 10:
      type = LOCALE_SABBREVMONTHNAME10; // October
      break;
    case 11:
      type = LOCALE_SABBREVMONTHNAME11; // November
      break;
    case 12:
      type = LOCALE_SABBREVMONTHNAME12; // December
      break;
    default:
      break;
  }

  char buf[128];
  ::GetLocaleInfoA(_locale, type, buf, sizeof(buf));
  name = buf;

#else

  nl_item item = 0;

  switch(month)
  {
    case 1:
      item = ABMON_1; // January
      break;
    case 2:
      item = ABMON_2; // February
      break;
    case 3:
      item = ABMON_3; // March
      break;
    case 4:
      item = ABMON_4; // April
      break;
    case 5:
      item = ABMON_5; // May
      break;
    case 6:
      item = ABMON_6; // June
      break;
    case 7:
      item = ABMON_7; // July
      break;
    case 8:
      item = ABMON_8; // August
      break;
    case 9:
      item = ABMON_9; // September
      break;
    case 10:
      item = ABMON_10; // October
      break;
    case 11:
      item = ABMON_11; // November
      break;
    case 12:
      item = ABMON_12; // December
      break;
    default:
      break;
  }

  name = ::nl_langinfo_l(item, static_cast<locale_t>(_locale));

#endif

  return(name);
}

/*
 */

String Locale::getRadixCharacter() const
{
  char *radix = NULL;

#ifdef CCXX_OS_WINDOWS

  char buf[4];
  ::GetLocaleInfoA(_locale, LOCALE_SDECIMAL, buf, sizeof(buf));
  radix = buf;

#else

  radix = ::nl_langinfo_l(RADIXCHAR, static_cast<locale_t>(_locale));

#endif

  return(radix);
}

/*
 */

String Locale::getThousandsSeparator() const
{
  char *separator = NULL;

#ifdef CCXX_OS_WINDOWS

  char buf[4];
  ::GetLocaleInfoA(_locale, LOCALE_SMONTHOUSANDSEP, buf, sizeof(buf));
  separator = buf;

#else

  separator = ::nl_langinfo_l(THOUSEP, static_cast<locale_t>(_locale));

#endif

  return(separator);
}

/*
 */

String Locale::getCurrencySymbol() const
{
  char *symbol = NULL;

#ifdef CCXX_OS_WINDOWS

  char buf[6];
  ::GetLocaleInfoA(_locale, LOCALE_SCURRENCY, buf, sizeof(buf));
  symbol = buf;

#else

  symbol = ::nl_langinfo_l(CRNCYSTR, static_cast<locale_t>(_locale));

#endif

  return(symbol);
}

/*
 */

String Locale::getCurrentEncoding()
{
  const char *enc = NULL;

#ifdef CCXX_OS_WINDOWS

  // Things are never easy with Windows...sigh...

  char buf[32];
  ::GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_IDEFAULTCODEPAGE, buf,
                   sizeof(buf));
  int encoding_id = ::atoi(buf);

  for(const struct CodePage *cp = __codepages; cp->encoding != NULL; ++cp)
  {
    if(cp->id == encoding_id)
    {
      enc = cp->encoding;
      break;
    }
  }

#else

  enc = nl_langinfo(CODESET);

#endif

  return(enc);
}

/*
 */

bool Locale::setCurrent(const Locale &locale)
{
#ifdef CCXX_OS_WINDOWS

  return(::SetThreadLocale(locale._locale) != 0);

#else

#if defined(HAVE_USELOCALE)

  ::uselocale(static_cast<locale_t>(locale._locale));
  return(::uselocale(NULL) == static_cast<locale_t>(locale._locale));

#elif defined(HAVE_SETLOCALE)

  CString cstr_locale = locale.toString().toUTF8();
  return(::setlocale(LC_ALL, cstr_locale.data()) != NULL);

#else

#warning "No means to set current locale."

#endif

#endif
}

/*
 */

const Locale Locale::getCurrent()
{
#ifdef CCXX_OS_WINDOWS

  LCID locale = ::GetThreadLocale();
  return Locale(locale);

#else

  // TODO: cache this value and return by reference.
  const char *name = ::setlocale(LC_CTYPE, 0);
  return Locale(name);

#endif
}

/*
 */

const Locale Locale::CANADA("en", "CA");
const Locale Locale::CANADA_FRENCH("fr", "CA");

const Locale Locale::CHINA("zh", "CN");
const Locale Locale::CHINESE("zh");

const Locale Locale::ENGLISH("en");

const Locale Locale::FRANCE("fr", "FR");
const Locale Locale::FRENCH("fr");

const Locale Locale::GERMANY("de", "DE");
const Locale Locale::GERMAN("de");

const Locale Locale::ITALY("it", "IT");
const Locale Locale::ITALIAN("it");

const Locale Locale::JAPAN("ja");
const Locale Locale::JAPANESE("ja");

const Locale Locale::KOREA("ko");
const Locale Locale::KOREAN("ko");

const Locale Locale::SIMPLIFIED_CHINESE("zh", "CN");
const Locale Locale::TRADITIONAL_CHINESE("zh", "TW");

const Locale Locale::SPANISH("es");

const Locale Locale::TAIWAN("zh", "TW");

const Locale Locale::UNITED_KINGDOM("en", "GB");
const Locale Locale::UNITED_STATES("en", "US");


} // namespace ccxx

#endif // CCXX_OS_ANDROID
