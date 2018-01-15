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

#include "commonc++/JavaContext.h++"
#include "commonc++/Log.h++"

#include <cstring>

// Note: JNI does not use normal UTF-8. It uses "Modified UTF-8"
// see http://docs.oracle.com/javase/1.5.0/docs/guide/jni/spec/types.html
// Programs should use the NewString, GetStringLength, GetStringChars,
// ReleaseStringChars, GetStringRegion, GetStringCritical, and
// ReleaseStringCritical functions, which use UTF-16LE encoding on
// little-endian architectures and UTF-16BE on big-endian
// architectures, and then use a UTF-16 to standard UTF-8 conversion
// routine.

namespace ccxx {

/*
 */

JavaContext::JavaContext(JNIEnv *env)
  : _jvm(NULL),
    _env(env),
    _verbose(false)
{
}

/*
 */

JavaContext::~JavaContext()
{
}

/*
 */

String JavaContext::encodeType(const String& type)
{
  CString cstr_type = type.toUTF8();
  const char *p, *q, *s = cstr_type.data();
  int dim = 0, len = 0;
  uint_t i;
  bool end = false;
  String buf;

  for(p = s; *p; p++)
  {
    if(*p == '[')
    {
      end = true;
      dim++;
    }

    if(! end)
      len++;
  }

  for(i = dim; i--; buf += '[');

  if(! ::strncmp(s, "boolean", len))
    buf += 'Z';
  else if(! ::strncmp(s, "byte", len))
    buf += 'B';
  else if(! ::strncmp(s, "char", len))
    buf += 'C';
  else if(! ::strncmp(s, "short", len))
    buf += 'S';
  else if(! ::strncmp(s, "int", len))
    buf += 'I';
  else if(! ::strncmp(s, "long", len))
    buf += 'J';
  else if(! ::strncmp(s, "float", len))
    buf += 'F';
  else if(! ::strncmp(s, "double", len))
    buf += 'D';
  else if(! ::strncmp(s, "String", len))
    buf += "Ljava/lang/String;";
  else if(! ::strncmp(s, "Object", len))
    buf += "Ljava/lang/Object;";
  else if(! ::strncmp(s, "Class", len))
    buf += "Ljava/lang/Class;";
  else if(! ::strncmp(s, "void", len))
    buf += 'V';
  else
  {
    buf += 'L';
    for(i = len, q = s; i--; q++)
      buf += ((*q == '.') ? '/' : *q);
    buf += ';';
  }

  return(buf);
}

/*
 */

bool JavaContext::parseSignature(const String& signature, String& method,
                                 String& descriptor, bool& isStatic)
{
  static const char *init = "<init>", *void_type = "void";
  uint_t index = 0;
  String tok, ret;

  isStatic = false;

  if(signature[0] == '(')
  {
    method = init;
    ret = void_type;
    index++;
  }
  else
  {
    tok = signature.nextToken(index, " ");

    if(! tok)
      return(false);

    if(tok == "static")
    {
      isStatic = true;

      ret = signature.nextToken(index, " ");

      if(! ret)
        return(false);
    }
    else
      ret = tok;

    tok = signature.nextToken(index, " (");
    if(! tok)
      return(false);

    method = tok;
  }

  descriptor = "(";

  for(;;)
  {
    tok = signature.nextToken(index, "(, )");
    if(! tok)
      break;

    String type = encodeType(tok);
    descriptor += type;
  }

  descriptor += ')';

  String type = encodeType(ret);
  descriptor += type;

  return(true);
}

/*
 */

jclass JavaContext::findClass(const String& name)
{
  String tname = name;
  tname.replace('.', '/');

  CString cstr_tname = tname.toUTF8();
  jclass cl = _env->FindClass(cstr_tname.data());

  if(checkException(_verbose))
    throw JavaClassNotFoundException(name);

  return(cl);
}

jclass JavaContext::defineClass(const String& name, const byte_t* buf,
                                size_t length)
{
  jobject loader = NULL; // Use the system classloader.

  String tname = name;
  tname.replace('.', '/');

  CString cstr_tname = tname.toUTF8();

  jclass cl = _env->DefineClass(cstr_tname.data(), loader,
                                reinterpret_cast<const jbyte*>(buf),
                                static_cast<jsize>(length));

  if(checkException(_verbose))
    throw JavaClassLoaderException();

  return(cl);
}

/*
 */

jmethodID JavaContext::findMethod(jclass clazz, const String& signature)
{
  jmethodID mid;
  bool isStatic;
  String method, descriptor;

  if(! parseSignature(signature, method, descriptor, isStatic))
    throw JavaSignatureException();

  CString cstr_method = method.toUTF8();
  CString cstr_descriptor = descriptor.toUTF8();

  if(isStatic)
    mid = _env->GetStaticMethodID(clazz, cstr_method.data(),
                                  cstr_descriptor.data());
  else
    mid = _env->GetMethodID(clazz, cstr_method.data(), cstr_descriptor.data());

  if(checkException(_verbose))
  {
    String error;
    error << "no such method: " << method << " " << descriptor;
    throw JavaMethodNotFoundException(error);
  }

  return(mid);
}

/*
 */

jobject JavaContext::createObject(jclass clazz, jmethodID constructor, ...)
{
  va_list args;
  jobject obj;

  va_start(args, constructor);

  try
  {
    obj = createObjectVA(clazz, constructor, args);
  }
  catch(JavaException)
  {
    va_end(args);
    throw;
  }

  va_end(args);

  return(obj);
}

/*
 */

jobject JavaContext::createObjectVA(jclass clazz, jmethodID constructor,
                                    va_list args)
{
  Log_debug("JavaContext::createObjectVA()");

  jobject obj;

  obj = _env->NewObjectV(clazz, constructor, args);

  if(checkException(_verbose))
  {
    throw JavaInstantiationException();
  }

  return(obj);
}

/*
 */

void JavaContext::callVoidMethod(jobject object, jmethodID method, ...)
{
  Log_debug("JavaContext::callVoidMethod()");

  va_list arglist;

  va_start(arglist, method);
  _env->CallVoidMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();
}

/*
 */

jobject JavaContext::callObjectMethod(jobject object, jmethodID method, ...)
{
  Log_debug("JavaContext::callObjectMethod()");

  jobject r = NULL;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallObjectMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(r);
}

/*
 */

String JavaContext::callStringMethod(jobject object, jmethodID method, ...)
{
  Log_debug("JavaContext::callStringMethod()");

  jobject r = NULL;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallObjectMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  if (r == NULL)
    return(String::null);
  else
    return(decodeString(static_cast<jstring>(r)));
}

/*
 */

bool JavaContext::callBooleanMethod(jobject object, jmethodID method, ...)
{
  jboolean r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallBooleanMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<bool>(r));
}

/*
 */

byte_t JavaContext::callByteMethod(jobject object, jmethodID method, ...)
{
  jbyte r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallByteMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<byte_t>(r));
}

/*
 */

char16_t JavaContext::callCharMethod(jobject object, jmethodID method, ...)
{
  jchar r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallCharMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<char16_t>(r));
}

/*
 */

int16_t JavaContext::callShortMethod(jobject object, jmethodID method, ...)
{
  jshort r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallShortMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int16_t>(r));
}

/*
 */

int32_t JavaContext::callIntMethod(jobject object, jmethodID method, ...)
{
  jint r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallIntMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int32_t>(r));
}

/*
 */

int64_t JavaContext::callLongMethod(jobject object, jmethodID method, ...)
{
  jlong r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallLongMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int64_t>(r));
}

/*
 */

float JavaContext::callFloatMethod(jobject object, jmethodID method, ...)
{
  jfloat r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallFloatMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<float>(r));
}

/*
 */

double JavaContext::callDoubleMethod(jobject object, jmethodID method, ...)
{
  jdouble r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallDoubleMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<double>(r));
}

/*
 */

void JavaContext::callNonvirtualVoidMethod(jobject object, jclass clazz,
                                           jmethodID method, ...)
{
  va_list arglist;

  va_start(arglist, method);
  _env->CallNonvirtualVoidMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();
}

/*
 */

jobject JavaContext::callNonvirtualObjectMethod(jobject object, jclass clazz,
                                                jmethodID method, ...)
{
  jobject r = NULL;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualObjectMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(r);
}

/*
 */

bool JavaContext::callNonvirtualBooleanMethod(jobject object, jclass clazz,
                                              jmethodID method, ...)
{
  jboolean r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualBooleanMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<bool>(r));
}

/*
 */

byte_t JavaContext::callNonvirtualByteMethod(jobject object, jclass clazz,
                                             jmethodID method, ...)
{
  jbyte r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualByteMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<byte_t>(r));
}

/*
 */

char16_t JavaContext::callNonvirtualCharMethod(jobject object, jclass clazz,
                                               jmethodID method, ...)
{
  jchar r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualCharMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<char16_t>(r));
}

/*
 */

int16_t JavaContext::callNonvirtualShortMethod(jobject object, jclass clazz,
                                               jmethodID method, ...)
{
  jshort r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualShortMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int16_t>(r));
}

/*
 */

int32_t JavaContext::callNonvirtualIntMethod(jobject object, jclass clazz,
                                             jmethodID method, ...)
{
  jint r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualIntMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int32_t>(r));
}

/*
 */

int64_t JavaContext::callNonvirtualLongMethod(jobject object, jclass clazz,
                                              jmethodID method, ...)
{
  jlong r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualLongMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int64_t>(r));
}

/*
 */

float JavaContext::callNonvirtualFloatMethod(jobject object, jclass clazz,
                                             jmethodID method, ...)
{
  jfloat r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualFloatMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<float>(r));
}

/*
 */

double JavaContext::callNonvirtualDoubleMethod(jobject object, jclass clazz,
                                               jmethodID method, ...)
{
  jdouble r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualDoubleMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<double>(r));
}

/*
 */

void JavaContext::callStaticVoidMethod(jclass clazz, jmethodID method, ...)
{
  va_list arglist;

  va_start(arglist, method);
  _env->CallStaticVoidMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();
}

/*
 */

jobject JavaContext::callStaticObjectMethod(jclass clazz, jmethodID method,
                                            ...)
{
  jobject r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticObjectMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(r);
}

/*
 */

String JavaContext::callStaticStringMethod(jclass clazz, jmethodID method,
                                            ...)
{
  jobject r = NULL;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticObjectMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  if (r == NULL)
    return(String::null);
  else
    return(decodeString(static_cast<jstring>(r)));
}

/*
 */

bool JavaContext::callStaticBooleanMethod(jclass clazz, jmethodID method,
                                          ...)
{
  jboolean r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticBooleanMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<bool>(r));
}

/*
 */

byte_t JavaContext::callStaticByteMethod(jclass clazz, jmethodID method, ...)
{
  jbyte r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticByteMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<byte_t>(r));
}

/*
 */

char16_t JavaContext::callStaticCharMethod(jclass clazz, jmethodID method, ...)
{
  jchar r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticCharMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<char16_t>(r));
}

/*
 */

int16_t JavaContext::callStaticShortMethod(jclass clazz, jmethodID method, ...)
{
  jshort r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticShortMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int16_t>(r));
}

/*
 */

int32_t JavaContext::callStaticIntMethod(jclass clazz, jmethodID method, ...)
{
  jint r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticIntMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int32_t>(r));
}

/*
 */

int64_t JavaContext::callStaticLongMethod(jclass clazz, jmethodID method, ...)
{
  jlong r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticLongMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int64_t>(r));
}

/*
 */

float JavaContext::callStaticFloatMethod(jclass clazz, jmethodID method, ...)
{
  jfloat r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticFloatMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<float>(r));
}

/*
 */

double JavaContext::callStaticDoubleMethod(jclass clazz, jmethodID method,
                                           ...)
{
  jdouble r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticDoubleMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<double>(r));
}

/*
 */

bool JavaContext::checkException(bool verbose /* = false */)
{
  if(_env->ExceptionCheck() == JNI_TRUE)
  {
    if(verbose)
      _env->ExceptionDescribe();

    return(true);
  }

  return(false);
}

/*
 */

void JavaContext::clearException()
{
  _env->ExceptionClear();
}

/*
 */

jthrowable JavaContext::getException()
{
  return(_env->ExceptionOccurred());
}

/*
 */

bool JavaContext::throwException(jclass exception, const String& message)
{
  CString cstr_message = message.toUTF8();
  jint i = _env->ThrowNew(exception, cstr_message.data());

  return(i == 0);
}

/*
 */

jfieldID JavaContext::findField(jclass clazz, const String& signature)
{
  jfieldID fid;
  bool isStatic = false;
  uint_t index = 0;
  String sig;

  // get the field type
  String tok = signature.nextToken(index, " ");
  if(! tok)
    throw JavaSignatureException();

  if(tok == "static")
  {
    isStatic = true;
    tok = signature.nextToken(index, " ");
  }

  sig = encodeType(tok);

  // get the field name
  String name = signature.nextToken(index, " ");

  CString cstr_sig = sig.toUTF8();
  CString cstr_name = name.toUTF8();

  if(isStatic)
    fid = _env->GetStaticFieldID(clazz, cstr_name.data(), cstr_sig.data());
  else
    fid = _env->GetFieldID(clazz, cstr_name.data(), cstr_sig.data());

  if(checkException(_verbose))
    throw JavaFieldNotFoundException(signature);

  return(fid);
}

/*
 */

jobject JavaContext::getObjectField(jobject object, jfieldID field)
{
  jobject r = _env->GetObjectField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(r);
}

/*
 */

bool JavaContext::getBooleanField(jobject object, jfieldID field)
{
  jboolean r = _env->GetBooleanField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<bool>(r));
}

/*
 */

byte_t JavaContext::getByteField(jobject object, jfieldID field)
{
  jbyte r = _env->GetByteField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<byte_t>(r));
}

/*
 */

char16_t JavaContext::getCharField(jobject object, jfieldID field)
{
  jchar r = _env->GetCharField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<char16_t>(r));
}

/*
 */

int16_t JavaContext::getShortField(jobject object, jfieldID field)
{
  jshort r = _env->GetShortField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<int16_t>(r));
}

/*
 */

int32_t JavaContext::getIntField(jobject object, jfieldID field)
{
  jint r = _env->GetIntField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<int32_t>(r));
}

/*
 */

int64_t JavaContext::getLongField(jobject object, jfieldID field)
{
  jlong r = _env->GetLongField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<int64_t>(r));
}

/*
 */

float JavaContext::getFloatField(jobject object, jfieldID field)
{
  jfloat r = _env->GetFloatField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<float>(r));
}

/*
 */

double JavaContext::getDoubleField(jobject object, jfieldID field)
{
  jdouble r = _env->GetDoubleField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<double>(r));
}

/*
 */

String JavaContext::getStringField(jobject object, jfieldID field)
{
  jstring str = static_cast<jstring>(_env->GetObjectField(object, field));

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(decodeString(str));
}

/*
 */

jobject JavaContext::getStaticObjectField(jclass clazz, jfieldID field)
{
  jobject r = _env->GetStaticObjectField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(r);
}

/*
 */

bool JavaContext::getStaticBooleanField(jclass clazz, jfieldID field)
{
  jboolean r = _env->GetStaticBooleanField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<bool>(r));
}

/*
 */

byte_t JavaContext::getStaticByteField(jclass clazz, jfieldID field)
{
  jbyte r = _env->GetStaticByteField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<byte_t>(r));
}

/*
 */

char16_t JavaContext::getStaticCharField(jclass clazz, jfieldID field)
{
  jchar r = _env->GetStaticCharField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<char16_t>(r));
}

/*
 */

int16_t JavaContext::getStaticShortField(jclass clazz, jfieldID field)
{
  jshort r = _env->GetStaticShortField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<int16_t>(r));
}

/*
 */

int32_t JavaContext::getStaticIntField(jclass clazz, jfieldID field)
{
  jint r = _env->GetStaticIntField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<int32_t>(r));
}

/*
 */

int64_t JavaContext::getStaticLongField(jclass clazz, jfieldID field)
{
  jlong r = _env->GetStaticLongField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<int64_t>(r));
}

/*
 */

float JavaContext::getStaticFloatField(jclass clazz, jfieldID field)
{
  jfloat r = _env->GetStaticFloatField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<float>(r));
}

/*
 */

double JavaContext::getStaticDoubleField(jclass clazz, jfieldID field)
{
  jdouble r = _env->GetStaticDoubleField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<double>(r));
}

/*
 */

String JavaContext::getStaticStringField(jclass clazz, jfieldID field)
{
  jstring str = static_cast<jstring>(_env->GetStaticObjectField(clazz, field));

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(decodeString(str));
}

/*
 */

void JavaContext::setObjectField(jobject object, jfieldID field, jobject value)
{
  _env->SetObjectField(object, field, value);

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setBooleanField(jobject object, jfieldID field, bool value)
{
  _env->SetBooleanField(object, field, static_cast<jboolean>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setByteField(jobject object, jfieldID field, byte_t value)
{

  _env->SetByteField(object, field, static_cast<jbyte>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setCharField(jobject object, jfieldID field, char16_t value)
{
  _env->SetCharField(object, field, static_cast<jchar>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setShortField(jobject object, jfieldID field, int16_t value)
{
  _env->SetShortField(object, field, static_cast<jshort>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setIntField(jobject object, jfieldID field, int32_t value)
{
  _env->SetIntField(object, field, static_cast<jint>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setLongField(jobject object, jfieldID field, int64_t value)
{
  _env->SetLongField(object, field, static_cast<jlong>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setFloatField(jobject object, jfieldID field, float value)
{
  _env->SetFloatField(object, field, static_cast<jfloat>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setDoubleField(jobject object, jfieldID field, double value)
{
  _env->SetDoubleField(object, field, static_cast<jdouble>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStringField(jobject object, jfieldID field,
                                 const String& value)
{
  jstring jstr = encodeString(value);
  _env->SetObjectField(object, field, jstr);

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticObjectField(jclass clazz, jfieldID field,
                                       jobject value)
{
  _env->SetStaticObjectField(clazz, field, value);

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticBooleanField(jclass clazz, jfieldID field,
                                        bool value)
{
  _env->SetStaticBooleanField(clazz, field, static_cast<jboolean>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticByteField(jclass clazz, jfieldID field,
                                     byte_t value)
{
  _env->SetStaticByteField(clazz, field, static_cast<jbyte>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticCharField(jclass clazz, jfieldID field,
                                     char16_t value)
{
  _env->SetStaticCharField(clazz, field, static_cast<jchar>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticShortField(jclass clazz, jfieldID field,
                                      int16_t value)
{
  _env->SetStaticShortField(clazz, field, static_cast<jshort>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticIntField(jclass clazz, jfieldID field,
                                    int32_t value)
{
  _env->SetStaticIntField(clazz, field, static_cast<jint>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticLongField(jclass clazz, jfieldID field,
                                     int64_t value)
{
  _env->SetStaticLongField(clazz, field, static_cast<jlong>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticFloatField(jclass clazz, jfieldID field,
                                      float value)
{
  _env->SetStaticFloatField(clazz, field, static_cast<float>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticDoubleField(jclass clazz, jfieldID field,
                                       double value)
{
  _env->SetStaticDoubleField(clazz, field, static_cast<jdouble>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setObjectArrayElement(jobjectArray array, uint_t index,
                                        jobject value)
{
  _env->SetObjectArrayElement(array, static_cast<jsize>(index), value);

  if(checkException(_verbose))
    throw JavaArrayAccessException();
}

/*
 */

jobject JavaContext::getObjectArrayElement(jobjectArray array, uint_t index)
{
  jobject r = _env->GetObjectArrayElement(array, static_cast<jsize>(index));

  if(checkException(_verbose))
    throw JavaArrayAccessException();

  return(r);
}

/*
 */

uint_t JavaContext::getArrayLength(jarray array)
{
  jsize r = _env->GetArrayLength(array);

  if(checkException(_verbose))
    throw JavaArrayAccessException();

  return((uint_t)r);
}

/*
 */

jbyteArray JavaContext::createByteArray(uint_t length)

{
  jbyteArray r = _env->NewByteArray((jsize)length);

  if(checkException(_verbose))
    throw JavaInstantiationException();

  return(r);
}

/*
 */

jarray JavaContext::createObjectArray(jclass type, uint_t length,
                                      jobject initialValue /* = NULL */)
{
  jarray r = _env->NewObjectArray(static_cast<jsize>(length), type,
                                 initialValue);

  if(checkException(_verbose))
    throw JavaInstantiationException();

  return(r);
}

/*
 */

jobject JavaContext::createDirectByteBuffer(byte_t *buf, size_t size)
{
  jobject r = _env->NewDirectByteBuffer((void *)buf, static_cast<jlong>(size));

  if(checkException(_verbose))
    throw JavaOutOfMemoryException();

  return(r);
}

/*
 */

void *JavaContext::getDirectBufferRegion(jobject buf, uint64_t& size)
{
  void *base = _env->GetDirectBufferAddress(buf);
  if(base)
  {
    jlong sz = _env->GetDirectBufferCapacity(buf);
    size = static_cast<uint64_t>(sz);
  }

  return(base);
}

/*
 */

jobject JavaContext::createLocalRef(jobject object)
{
  jobject r = _env->NewLocalRef(object);

  if(r == NULL)
    throw JavaOutOfMemoryException();

  return(r);
}

/*
 */

void JavaContext::deleteLocalRef(jobject ref)
{
  _env->DeleteLocalRef(ref);
}

/*
 */

void JavaContext::ensureLocalCapacity(uint_t capacity)
{
  jint r = _env->EnsureLocalCapacity(static_cast<jint>(capacity));

  if(checkException(_verbose) || (r != 0))
    throw JavaOutOfMemoryException();
}

/*
 */

jobject JavaContext::createGlobalRef(jobject object)

{
  jobject r = _env->NewGlobalRef(object);

  if(r == NULL)
    throw JavaOutOfMemoryException();

  return(r);
}

/*
 */

void JavaContext::deleteGlobalRef(jobject ref)
{
  _env->DeleteGlobalRef(ref); // NULL ref is OK
}

/*
 */

String JavaContext::decodeString(jstring str)
{
  jboolean iscopy;
  const jchar *r = _env->GetStringChars(str, &iscopy);
  jsize len = _env->GetStringLength(str);

  String s(static_cast<const char16_t*>(r), 0, static_cast<size_t>(len));
  _env->ReleaseStringChars(str, r);

  return(s);
}

/*
 */

jstring JavaContext::encodeString(const String& str)
{
  if(!str)
    return(NULL);

  jstring r = _env->NewString(static_cast<const jchar *>(str.data()),
                              static_cast<jsize>(str.length()));

  return(r);
}

/*
 */

uint_t JavaContext::getStringLength(jstring str)
{
  jsize sz = _env->GetStringLength(str);
  return(static_cast<uint_t>(sz));
}

/*
 */

void JavaContext::releaseArrayData(jarray array, void *data)
{
  _env->ReleasePrimitiveArrayCritical(array, data, 0);
}

/*
 */

void *JavaContext::getArrayData(jarray array, uint_t& length,
                                bool* isCopy /* = NULL */)
{
  length = _env->GetArrayLength(array);

  jboolean copy = false;
  void *r = _env->GetPrimitiveArrayCritical(array, &copy);

  if(checkException(_verbose))
    throw JavaOutOfMemoryException();

  if(isCopy)
    *isCopy = static_cast<bool>(copy);

  return(r);
}

/*
 */

jbyteArray JavaContext::createByteArray(const byte_t *data, size_t size)
{
  jbyteArray array = createByteArray(size);

  _env->SetByteArrayRegion(array, (jsize)0, (jsize)size, (jbyte *)data);

  if(checkException(_verbose))
    throw JavaArrayAccessException();

  return(array);
}

/*
 */

bool JavaContext::isSameObject(jobject object1, jobject object2)
{
  jboolean b = _env->IsSameObject(object1, object2);

  return(b == JNI_TRUE);
}

/*
 */

bool JavaContext::isInstanceOf(jobject object, jclass clazz)
{
  jboolean b = _env->IsInstanceOf(object, clazz);

  return(b == JNI_TRUE);
}

/*
 */

bool JavaContext::isAssignableFrom(jclass clazz1, jclass clazz2)
{
  jboolean b = _env->IsAssignableFrom(clazz1, clazz2);

  return(b == JNI_TRUE);
}

/*
 */

jclass JavaContext::getClassForObject(jobject object)
{
  return(_env->GetObjectClass(object));
}

/*
 */

jclass JavaContext::getSuperclass(jclass clazz)
{
  return(_env->GetSuperclass(clazz));
}

/*
 */

void JavaContext::enterMonitor(jobject object)
{
  jint r = _env->MonitorEnter(object);

  if(checkException(_verbose) || (r != 0))
    throw JavaMonitorException(); // may also mean memory exhausted
}

/*
 */

void JavaContext::exitMonitor(jobject object)
{
  jint r = _env->MonitorExit(object);

  if(checkException(_verbose) || (r != 0))
    throw JavaMonitorException(); // may also mean memory exhausted
}

/*
 */

void JavaContext::registerNativeMethods(
  jclass clazz, const JavaNativeMethod methods[])
{
  int i, len, r = 0;
  String method, descriptor, signature;
  bool is_static, ok = true;

  // figure out how long the vector is
  for(len = 0; methods[len].signature != NULL; len++);

  JNINativeMethod *jnim = new JNINativeMethod[len];

  for(i = 0; i < len; ++i)
  {
    signature = methods[i].signature;

    if(! signature)
      break;

    if(parseSignature(signature, method, descriptor, is_static))
    {
      CString cstr_method = method.toUTF8();
      CString cstr_descriptor = descriptor.toUTF8();

      jnim[i].name = ::strdup(cstr_method.data());
      jnim[i].signature = ::strdup(cstr_descriptor.data());
      jnim[i].fnPtr = methods[i].function;
    }
    else
    {
      jnim[i].name = NULL;
      jnim[i].signature = NULL;
    }
  }

  if(ok)
    r = (int)_env->RegisterNatives(clazz, jnim, len);

  for(i = 0; i < len; ++i)
  {
    free(jnim[i].name);
    free(jnim[i].signature);
  }

  delete[] jnim;

  if(! ok || (r < 0))
  {
    checkException(_verbose);
    throw JavaMethodNotFoundException();
  }
}

/*
 */

void JavaContext::unregisterNativeMethods(jclass clazz)
{
  _env->UnregisterNatives(clazz);
}

/*
 */

void JavaContext::pushLocalFrame(uint_t capacity)
{
  jint r = _env->PushLocalFrame((jint)capacity);

  if(checkException(_verbose) || (r != 0))
    throw JavaOutOfMemoryException();
}

/*
 */

void JavaContext::popLocalFrame()
{
  _env->PopLocalFrame(NULL);
}

/*
 */

void JavaContext::attach(JavaVirtualMachine *jvm, JNIEnv* env, bool verbose)
{
  _jvm = jvm;
  _env = env;
  _verbose = verbose;
}

/*
 */

void JavaContext::detach()
{
  _jvm = NULL;
  _env = NULL;
  _verbose = false;
}


} // namespace ccxx
