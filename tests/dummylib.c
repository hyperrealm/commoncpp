
#ifdef WIN32
#include <windows.h>

BOOL DllMain(HINSTANCE dll, DWORD reason, LPVOID reserved)
{
  return(TRUE);
}

#define DUMMY_API __declspec(dllexport)

#else

#define DUMMY_API

#endif

DUMMY_API const int dummy_const = 12345;

DUMMY_API int dummy_test1(int a, int b)
{
  return(a + b);
}

DUMMY_API const char *dummy_test_two(void)
{
  return("hello");
}

// eof
