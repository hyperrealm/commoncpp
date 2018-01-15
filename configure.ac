# Process this file with autoconf to produce a configure script.
AC_INIT(libcommonc++, 0.7, hyperrealm@gmail.com, libcommonc++)
AC_CONFIG_AUX_DIR([aux-build])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([cpp_config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_DEFINE_UNQUOTED(TARGET, "${target}", [Configured target name.])

# Enable GNU extensions.
AC_GNU_SOURCE

LT_INIT()

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AM_PROG_AS
AM_PROG_CC_C_O

AX_CXX_GCC_ABI_DEMANGLE

# Platform checks.

case "$target" in
        *-*-cygwin* | *-*-mingw*)
                LIBS="-lws2_32 -liphlpapi -lrpcrt4 -luserenv $LIBS"
                WIN32=yes
                MINGW32=yes;;

	i686-*-linux* | x86_32-*-linux*)
		LINUX_X86=yes;;

	x86_64-*-linux*)
		LINUX_X86_64=yes;;

        i686-*-darwin*)
		DARWIN_X86=yes
                DARWIN=yes;;

	powerpc-*-darwin*)
		DARWIN_PPC=yes
		DARWIN=yes;;

	powerpc-*-linux*)
		LINUX_PPC=yes;;

        *-*-android*)
		LIBS="-llog $LIBS"
                ANDROID=yes;;

	*)
		GENERIC=yes;;
esac

AM_CONDITIONAL(WINDOWS, test x$WIN32 = xyes)
AM_CONDITIONAL(MACOSX, test x$DARWIN = xyes)
AM_CONDITIONAL(MACOSX_X86, test x$DARWIN_X86 = xyes)
AM_CONDITIONAL(MACOSX_PPC, test x$DARWIN_PPC = xyes)
AM_CONDITIONAL(LINUX_X86, test x$LINUX_X86 = xyes)
AM_CONDITIONAL(LINUX_X86_64, test x$LINUX_X86_64 = xyes)
AM_CONDITIONAL(LINUX_PPC, test x$LINUX_PPC = xyes)
AM_CONDITIONAL(MINGW, test x$MINGW32 = xyes)
AM_CONDITIONAL(ANDROID, test x$ANDROID = xyes)
AM_CONDITIONAL(GENERIC_OS, test x$GENERIC = xyes)

dnl Enable warnings, if we can determine an appropriate compiler switch.

AC_MSG_CHECKING([for compiler switch to enable full C/C++ warnings]);
case "${CC}" in

gcc)
	warn_c_sw="-Wall -Wdeclaration-after-statement -Wno-psabi"
	warn_cxx_sw="-Wall";;
*)
	warn_cxx_sw=""
	warn_c_sw="";;
esac

if test -n "${warn_c_sw}";
then
	CFLAGS="${CFLAGS} ${warn_c_sw}";
	CXXFLAGS="${CXXFLAGS} ${warn_cxx_sw}";
	AC_MSG_RESULT([C: ${warn_c_sw}, C++: ${warn_cxx_sw}])
else
	AC_MSG_RESULT([(cannot determine)])
fi;

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h netdb.h netinet/in.h stdlib.h string.h sys/file.h sys/ioctl.h sys/time.h termios.h unistd.h stdint.h crypt.h stropts.h sys/socket.h dlfcn.h execinfo.h ucontext.h getopt.h sys/vfs.h sys/param.h sys/mount.h sys/inotify.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_CHECK_MEMBERS([struct msghdr.msg_accrights, struct msghdr.msg_control],,,
[#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif])

AC_CHECK_SIZEOF(time_t)

dnl Check for functions

AH_TEMPLATE([HAVE_PTHREAD_MUTEX_TIMEDLOCK], [Define to 1 if you have the `pthread_mutex_timedlock' function.])
AC_CHECK_LIB(pthread, pthread_mutex_timedlock,
AC_DEFINE(HAVE_PTHREAD_MUTEX_TIMEDLOCK))

AH_TEMPLATE([HAVE_PTHREAD_RWLOCK_TIMEDRDLOCK], [Define to 1 if you have the `pthread_rwlock_timedrdlock' function.])
AC_CHECK_LIB(pthread, pthread_rwlock_timedrdlock,
AC_DEFINE(HAVE_PTHREAD_RWLOCK_TIMEDRDLOCK))

AH_TEMPLATE([HAVE_PTHREAD_RWLOCK_TIMEDWRLOCK], [Define to 1 if you have the `pthread_rwlock_timedwrlock' function.])
AC_CHECK_LIB(pthread, pthread_rwlock_timedwrlock,
AC_DEFINE(HAVE_PTHREAD_RWLOCK_TIMEDWRLOCK))

AH_TEMPLATE([HAVE_PTHREAD_YIELD], [Define to 1 if you have the `pthread_yield' function.])
AC_CHECK_LIB(pthread, pthread_yield,
AC_DEFINE(HAVE_PTHREAD_YIELD))

AH_TEMPLATE([HAVE_TIMER_CREATE], [Define to 1 if you have the `timer_create' function.])
AC_CHECK_LIB(rt, timer_create,
AC_DEFINE(HAVE_TIMER_CREATE))

AH_TEMPLATE([HAVE_TIMER_DELETE], [Define to 1 if you have the `timer_delete' function.])
AC_CHECK_LIB(rt, timer_delete,
AC_DEFINE(HAVE_TIMER_DELETE))

AH_TEMPLATE([HAVE_TIMER_SETTIME], [Define to 1 if you have the `timer_settime' function.])
AC_CHECK_LIB(rt, timer_settime,
AC_DEFINE(HAVE_TIMER_SETTIME))

dnl Checks for POSIX socket types

AH_TEMPLATE([HAVE_TYPE_SOCKLEN_T], [Define if sys/socket.h or unistd.h defines socklen_t])

AC_MSG_CHECKING(for socklen_t in sys/socket.h or unistd.h)
AC_TRY_LINK([#include <sys/socket.h>
#include <unistd.h>],
[socklen_t t;],
AC_DEFINE(HAVE_TYPE_SOCKLEN_T)
AC_MSG_RESULT(yes),
AC_MSG_RESULT(no))

AH_TEMPLATE([HAVE_TYPE_IN_ADDR_T], [Define if netinet/in.h or sys/types.h defines in_addr_t])

AC_MSG_CHECKING(for in_addr_t in netinet/in.h or sys/types.h)
AC_TRY_LINK([#include <sys/types.h>
#include <netinet/in.h>],
[in_addr_t t;],
AC_DEFINE(HAVE_TYPE_IN_ADDR_T)
AC_MSG_RESULT(yes),
AC_MSG_RESULT(no))

AH_TEMPLATE([HAVE_TYPE_IN_PORT_T], [Define if netinet/in.h or sys/types.h defines in_port_t])

AC_MSG_CHECKING(for in_port_t in netinet/in.h or sys/types.h)
AC_TRY_LINK([#include <sys/types.h>
#include <netinet/in.h>],
[in_port_t t;],
AC_DEFINE(HAVE_TYPE_IN_PORT_T)
AC_MSG_RESULT(yes),
AC_MSG_RESULT(no))

dnl check for sockaddr.sa_len

AH_TEMPLATE([HAVE_SOCKADDR_SA_LEN], [Define if struct sockaddr has a sa_len member.])

AC_CHECK_MEMBER([struct sockaddr.sa_len],
[ AC_DEFINE(HAVE_SOCKADDR_SA_LEN) ],
,
[
#include <sys/socket.h>
])


dnl Checks for gethostbyname_r & friends

AC_SEARCH_LIBS(gethostbyname_r, [socket nsl])
AC_SEARCH_LIBS(getservbyname_r, [socket nsl])
AC_SEARCH_LIBS(aio_read, [rt])

AH_TEMPLATE([HAVE_GETxxxxBYyyyy_R_POSIX], [Define if POSIX-style getXXXXbyYYYY_r functions are available.])

AC_MSG_CHECKING([for POSIX-style gethostbyXXXX_r and getservbyXXXX_r])
AC_TRY_LINK([
#define _POSIX_PTHREAD_SEMANTICS
#include <netdb.h>
#include <stddef.h>
],
[
  struct hostent *he;
  struct servent *se;

  he = gethostbyname_r((const char *)NULL, (struct hostent *)NULL,
	(char *)NULL, (int)0, (struct hostent **)NULL, (int *)NULL);
  he = gethostbyaddr_r((const char *)NULL, (int)0, (int)0,
	(struct hostent *)NULL, (char *)NULL, (int)0, (struct hostent **)NULL,
	(int *)NULL);

  se = getservbyname_r((const char *)NULL, (const char *)NULL,
	(struct servent *)NULL, (char *)NULL, (int)0, (struct servent **)NULL);
  se = getservbyport_r((int)0, (const char *)NULL,
	(struct servent *)NULL, (char *)NULL, (int)0, (struct servent **)NULL);
],
AC_DEFINE(HAVE_GETxxxxBYyyyy_R_POSIX)
AC_MSG_RESULT(yes),
AC_MSG_RESULT(no)
)

AH_TEMPLATE([HAVE_GETxxxxBYyyyy_R_SUN], [Define if Solaris-style getXXXXbyYYYY_r functions are available.])

AC_MSG_CHECKING([for Solaris-style gethostbyXXXX_r and getservbyXXXX_r])
AC_TRY_LINK([
#define _POSIX_PTHREAD_SEMANTICS
#include <netdb.h>
#include <stddef.h>
],
[ struct hostent *he;
  struct servent *se;

  he = gethostbyname_r((const char *)NULL, (struct hostent *)NULL,
	(char *)NULL, (int)0, (int *)NULL);
  he = gethostbyaddr_r((const char *)NULL, (int)0, (int)0,
	(struct hostent *)NULL, (char *)NULL, (int)0, (int *)NULL);

  se = getservbyname_r((const char *)NULL, (const char *)NULL,
	(struct servent *)NULL, (char *)NULL, (int)0);
  se = getservbyport_r((int)0, (const char *)NULL,
	(struct servent *)NULL, (char *)NULL, (int)0);
],
AC_DEFINE(HAVE_GETxxxxBYyyyy_R_SUN)
AC_MSG_RESULT(yes),
AC_MSG_RESULT(no)
)

dnl Checks for library functions.

AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_LSTAT
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MKTIME
AC_FUNC_MMAP
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([dup2 flockfile funlockfile ftruncate getcwd inet_ntoa inet_aton localtime_r memmove memset mkdir munmap pathconf select socket strchr strerror strpbrk uname getgrnam_r sranddev getcontext strtoll backtrace lseek64 setlocale freelocale newlocale __newlocale uselocale inotify_init rand_r])

dnl Checks for libraries.

AC_CHECK_LIB(rt, sem_open)
AC_CHECK_LIB(dl, dladdr)
AC_CHECK_FUNCS(dladdr)
AC_CHECK_LIB(pthread, pthread_create)

dnl UUID checks

AC_CHECK_LIB(uuid, uuid_generate)
AC_CHECK_HEADERS(uuid/uuid.h uuid.h sys/uuid.h aio.h locale.h langinfo.h xlocale.h)
AC_CHECK_FUNCS(uuid_create uuid_generate)

dnl Java checks

AC_ARG_WITH(java, AC_HELP_STRING([--with-java=JAVADIR],
[Compile Java VM support]),
[
JAVA_HOME="$withval"
AC_SUBST(java_home, $withval)
java=yes;

AC_JNI_INCLUDE_DIR
for JNI_INCLUDE_DIR in $JNI_INCLUDE_DIRS
do
  JNI_CPPFLAGS="$JNI_CPPFLAGS -I$JNI_INCLUDE_DIR"
done

AC_JNI_LIB_DIR
for JNI_LIB_DIR in $JNI_LIB_DIRS
do
  JNI_LDFLAGS="$JNI_LDFLAGS -L$JNI_LIB_DIR"
done

JNI_LDFLAGS="$JNI_LDFLAGS $JNI_LIB_LIBS"

for JNI_LIB_LIB in $JNI_LIB_LIBS
do
  JNI_LDADD="$JNI_LDADD $JNI_LIB_LIB"
done

],
)
AC_SUBST(JNI_CPPFLAGS)
AC_SUBST(JNI_LDFLAGS)
AC_SUBST(JNI_LDADD)

dnl XML (expat) checks

AC_ARG_WITH(expat, AC_HELP_STRING([--with-expat=EXPATDIR],
[Compile expat XML parser interface]),
[
EXPAT_HOME="$withval"
AC_SUBST(expat_home, $withval)
expat=yes;
],
)

dnl CppUnit checks

AC_ARG_WITH(cppunit, AC_HELP_STRING([--with-cppunit=CPPUNITDIR],
[Provide location of cppunit]),
[
CPPUNIT_HOME="$withval"
AC_SUBST(cppunit_home, $withval)
cppunit=yes;
],
)

dnl binutils checks

AC_ARG_WITH(binutils, AC_HELP_STRING([--with-binutils=BINUTILSDIR],
[Provide location of GNU binutils]),
[
BINUTILS_HOME="$withval"
AC_SUBST(binutils_home, $withval)
LIBS="-L${BINUTILS_HOME}/lib -lbfd  ${LIBS}"
binutils=yes;
AC_CHECK_LIB(bfd, bfd_init)
AC_CHECK_HEADERS(bfd.h)
],
)

dnl makefile conditionals

AM_CONDITIONAL(USE_JAVA, test x$java = xyes)
AM_CONDITIONAL(USE_EXPAT, test x$expat = xyes)
AM_CONDITIONAL(USE_CPPUNIT, test x$cppunit = xyes)
AM_CONDITIONAL(USE_BFD, test x$binutils = xyes)


cxx_pcre_version=8.41
cxx_sqlite_version=3.21.0
AC_SUBST(PCRE_VERSION, $cxx_pcre_version)
AC_SUBST(SQLITE_VERSION, $cxx_sqlite_version)

dnl Independent subprojects - third party libraries that commonc++ depends on.
AC_CONFIG_SUBDIRS([pcre-8.41 sqlite-3.21.0])

AC_SUBST(RELEASE_DATE, '31 Mar 2014')

AC_OUTPUT([Makefile
	cbits/Makefile
	libatomic/Makefile
	libstacktrace/Makefile
	lib/Makefile
	lib/commonc++.pc
	lib/commonc++jvm.pc
	lib/commonc++db.pc
	lib/commonc++xml.pc
	lib/doxygen.cfg
	doc/Makefile
	plugintool/Makefile
	plugintool/plugintool.1
	tests/Makefile
	tests/commonc++testmain.pc])

if test -z "${with_expat}"; then
	extopt_xml="NO"
else
	extopt_xml="YES (with ${with_expat})"
fi

if test -z "${with_java}"; then
	extopt_java="NO"
else
	extopt_java="YES (with ${with_java})"
fi

if test -z "${with_cppunit}"; then
	extopt_cppunit="NO"
else
	extopt_cppunit="YES (with ${with_cppunit})"
fi

if test x${binutils} = xyes; then
	extopt_binutils="YES (with ${binutils_home})";
else
	extopt_binutils="NO"
fi

cat <<EOF

$PACKAGE-$VERSION configuration summary:

  Install prefix ........... : ${prefix}
  C preprocessor ........... : ${CPP}
  C compiler ............... : ${CC}
  C++ preprocessor ......... : ${CXXCPP}
  C++ compiler ............. : ${CXX}
  Linker ................... : ${LD}
  C preprocessor flags ..... : ${CPPFLAGS}
  C compiler flags ......... : ${CFLAGS}
  C++ compiler flags ....... : ${CXXFLAGS}
  Linker flags ............. : ${LDFLAGS}
  Extra libraries .......... : ${LIBS}

  Optional components:

  Build Java VM support (commonc++jvm) ..... ${extopt_java}
  Build XML support (commonc++xml) ......... ${extopt_xml}
  Built libbfd-based stack trace support ... ${extopt_binutils}
  Build Unit Tests ......................... ${extopt_cppunit}

EOF

dnl end configure.ac
