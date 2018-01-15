
ACLOCAL_AMFLAGS = -I m4

SUBDIRS=

if MINGW
SUBDIRS += cbits
endif

SUBDIRS += libatomic libstacktrace pcre-8.41 sqlite-3.21.0 lib doc plugintool

if USE_CPPUNIT
SUBDIRS += tests
endif

EXTRA_DIST = \
	$(top_srcdir)/*.sln \
	$(top_srcdir)/*.vcproj \
	$(top_srcdir)/*.vsprops \
	$(top_srcdir)/config*.opts \
	$(top_srcdir)/m4/*.m4 \
	$(top_srcdir)/commonc++.rc \
	$(top_srcdir)/deploy.bat \
	$(top_srcdir)/pcre-8.41/configure.gnu \
	$(top_srcdir)/pcre-8.41/*.win32 \
	$(top_srcdir)/pcre-8.41/*.vcproj \
	$(top_srcdir)/plugintool/*.vcproj \
	$(top_srcdir)/sqlite-3.21.0/configure.gnu \
	$(top_srcdir)/sqlite-3.21.0/*.vcproj \
	$(top_srcdir)/expat.vc8/examples/* \
	$(top_srcdir)/expat.vc8/lib/* \
	$(top_srcdir)/expat.vc8/xmlwf/*

dox:
	make -C $(top_srcdir)/lib dox

# TODO: inside each <File> tag for a '.c++' file, add this crap:
# <FileConfiguration Name="Debug|Win32">
#   <Tool Name="VCCLCompilerTool"/>
# </FileConfiguration>
# <FileConfiguration Name="Release|Win32">
#   <Tool Name="VCCLCompilerTool"/>
# </FileConfiguration>

