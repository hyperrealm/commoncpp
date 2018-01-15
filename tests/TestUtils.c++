#include "TestUtils.h++"

#include <commonc++/Buffer.h++>
#include <commonc++/File.h++>
#include <commonc++/IOException.h++>
#include <commonc++/Log.h++>

#include <cppunit/extensions/HelperMacros.h>

using namespace ccxx;

// static
bool TestUtils::compareFiles(const String &path1, const String &path2)
{
  byte_t buf1[4096];
  byte_t buf2[4096];
  bool matched = true;

  try
  {
    File file1(path1);
    File file2(path2);

    int64_t len = file1.getSize();

    if(len != file2.getSize())
    {
      Log_info("File sizes differ: " INT64_FMT " != " INT64_FMT,
               len, file2.getSize());
      return(false);
    }

    file1.open(IORead, FileOpen);
    file2.open(IORead, FileOpen);

    while(len > 0)
    {
      size_t wanted = CCXX_LENGTHOF(buf1);

      if(len < static_cast<int64_t>(wanted))
        wanted = static_cast<size_t>(len);
      file1.readFully(buf1, wanted);
      file2.readFully(buf2, wanted);

      Log_debug("comparing %lu bytes", static_cast<unsigned long>(wanted));

      if(std::memcmp(buf1, buf2, wanted) != 0)
      {
        matched = false;
        break;
      }

      len -= wanted;
    }

    file1.close();
    file2.close();
  }
  catch(IOException &ioex)
  {
    CString cstr_message = ioex.toString().toUTF8();
    Log_info("exception while reading files: %s", cstr_message.data());
    matched = false;
  }

  return(matched);
}

/*
 */

// static
void TestUtils::failWithMessage(const ccxx::String &message)
{
  CString cstr_message = message.toUTF8();
  CPPUNIT_FAIL(cstr_message.data());
}
