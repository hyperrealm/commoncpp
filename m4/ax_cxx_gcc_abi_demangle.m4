# ===========================================================================
#     http://www.nongnu.org/autoconf-archive/ax_cxx_gcc_abi_demangle.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_GCC_ABI_DEMANGLE
#
# DESCRIPTION
#
#   If the compiler supports GCC C++ ABI name demangling (has header
#   cxxabi.h and abi::__cxa_demangle() function), define
#   HAVE_GCC_ABI_DEMANGLE
#
#   Adapted from AC_CXX_RTTI by Luc Maisonobe
#
# LICENSE
#
#   Copyright (c) 2008 Neil Ferguson <nferguso@eso.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_CXX_GCC_ABI_DEMANGLE],
[AC_CACHE_CHECK(whether the compiler supports GCC C++ ABI name demangling,
ac_cv_cxx_gcc_abi_demangle,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <typeinfo>
#include <cxxabi.h>
#include <stdlib.h>
#include <string>
using namespace __cxxabiv1;

template<typename TYPE>
class A {};
],[A<int> instance;
int status = 0;
char* c_name = 0;

c_name = abi::__cxa_demangle(typeid(instance).name(), 0, 0, &status);

std::string name(c_name);
free(c_name);

return name == "A<int>";
],
 ac_cv_cxx_gcc_abi_demangle=yes, ac_cv_cxx_gcc_abi_demangle=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_gcc_abi_demangle" = yes; then
  AC_DEFINE(HAVE_GCC_ABI_DEMANGLE,1,
            [define if the compiler supports GCC C++ ABI name demangling])
fi
])
