
noinst_LTLIBRARIES = libatomic.la

libsrc = atomic.h

if WINDOWS
libsrc += atomic_win32_generic.c
endif

if MACOSX_X86
libsrc += atomic_generic_x86.c
endif

if MACOSX_PPC
libsrc += atomic_generic_ppc.s
endif

if LINUX_X86
libsrc += atomic_generic_x86.c
endif

if LINUX_X86_64
libsrc += atomic_generic_x86.c
endif

if ANDROID
libsrc += atomic_android_generic.c
endif

if LINUX_PPC
libsrc += atomic_generic_ppc.s

libatomic_la_CCASFLAGS = -mregnames -fno-leading-underscore
endif

if GENERIC_OS
libsrc += atomic_posix_generic.c
endif

libatomic_la_SOURCES = $(libsrc)

libatomic_la_CPPFLAGS = -Wall
libatomic_la_CFLAGS = -fPIC


EXTRA_DIST = libatomic.vcproj

