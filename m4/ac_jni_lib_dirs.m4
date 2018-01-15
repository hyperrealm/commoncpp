dnl Available from the GNU Autoconf Macro Archive at:
dnl Http://Www.Gnu.Org/Software/Ac-Archive/Htmldoc/Ac_Jni_Include_Dirs.Html
dnl
AC_DEFUN([AC_JNI_LIB_DIR],[

JNI_LIB_DIRS=""

#test "x$JAVAC" = x && AC_MSG_ERROR(['$JAVAC' undefined])
#AC_PATH_PROG(_ACJNI_JAVAC, $JAVAC, no)
#test "x$_ACJNI_JAVAC" = xno && AC_MSG_ERROR([$JAVAC could not be found in path])

AC_MSG_CHECKING(for JNI libraries)

#_ACJNI_FOLLOW_SYMLINKS("$_ACJNI_JAVAC")
_ACJNI_FOLLOW_SYMLINKS("$JAVA_HOME")

#_JTOPDIR=`echo "$_ACJNI_FOLLOWED" | sed -e 's://*:/:g' -e 's:/[[^/]]*$::' -e 's:/[[^/]]*$::'`"/jre/lib"

_JTOPDIR="$_ACJNI_FOLLOWED"

# echo "_JTOPDIR: $_JTOPDIR, host_os: ${host_os}";

case "${host_os}" in
        linux*)              _JLIB="$_JTOPDIR/jre/lib/i386 $_JTOPDIR/jre/lib/i386/native_threads $_JTOPDIR/jre/lib/i386/server";;
	solaris*)	     _JLIB="$_JTOPDIR/jre/lib/sparc $_JTOPDIR/jre/lib/sparc/native_threads";;
        *)                   _JLIB="$_JTOPDIR/Libraries";;
esac

for d in $_JLIB; do

	if test -f "$d/libjava.so"; then
        	JNI_LIB_DIRS="$JNI_LIB_DIRS $d"
		JNI_LIB_LIBS="$JNI_LIB_LIBS -ljava"
	fi

	if test -f "$d/libhpi.so"; then
        	JNI_LIB_DIRS="$JNI_LIB_DIRS $d"
		JNI_LIB_LIBS="$JNI_LIB_LIBS -lhpi"
	fi

	if test -f "$d/libjvm.so"; then
        	JNI_LIB_DIRS="$JNI_LIB_DIRS $d"
		JNI_LIB_LIBS="$JNI_LIB_LIBS -ljvm"
	fi

	if test -f "$d/libverify.so"; then
		JNI_LIB_LIBS="$JNI_LIB_LIBS -lverify"
	fi

	if test -f "$d/libjvm.dylib"; then
        	JNI_LIB_DIRS="$JNI_LIB_DIRS $d"
		JNI_LIB_LIBS="$JNI_LIB_LIBS -framework JavaVM"
	fi

done

# echo "JNI_LIB_DIRS: $JNI_LIB_DIRS"

AC_MSG_RESULT($JNI_LIB_DIRS $JNI_LIB_LIBS)
])

# _ACJNI_FOLLOW_SYMLINKS <path>
# Follows symbolic links on <path>,
# finally setting variable _ACJNI_FOLLOWED
# --------------------
AC_DEFUN([_ACJNI_FOLLOW_SYMLINKS],[
# find the include directory relative to the javac executable
_cur="$1"
while ls -ld "$_cur" 2>/dev/null | grep " -> " >/dev/null; do
        AC_MSG_CHECKING(symlink for $_cur)
        _slink=`ls -ld "$_cur" | sed 's/.* -> //'`
        case "$_slink" in
        /*) _cur="$_slink";;
        # 'X' avoids triggering unwanted echo options.
        *) _cur=`echo "X$_cur" | sed -e 's/^X//' -e 's:[[^/]]*$::'`"$_slink";;
        esac
        AC_MSG_RESULT($_cur)
done
_ACJNI_FOLLOWED="$_cur"
])# _ACJNI
