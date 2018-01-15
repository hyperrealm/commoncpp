
noinst_LTLIBRARIES = libstacktrace.la

libsrc = stacktrace.h

if WINDOWS

if MINGW

libsrc += stacktrace_generic.cpp

else

libsrc += stacktrace_win32.cpp

endif

else

libsrc += stacktrace_generic.cpp

endif

libstacktrace_la_SOURCES = $(libsrc)

libstacktrace_la_CPPFLAGS = -Wall
libstacktrace_la_CXXFLAGS = -fPIC

if USE_BFD

BINUTILS_HOME = @binutils_home@

libstacktrace_la_CPPFLAGS += -I$(BINUTILS_HOME)/include -DUSE_BFD

endif

EXTRA_DIST = libstacktrace.vcproj
