#include <commonc++/String.h++>

#include <cstdio>
#include <iostream>

using namespace ccxx;

int main(void)
{
  String foo = "CCXX_TEST";
  CString cstr_foo = foo.toUTF8();

  std::cout << "foo length: " << foo.length() << std::endl;
  printf("foo: %s\n", cstr_foo.data());
}
