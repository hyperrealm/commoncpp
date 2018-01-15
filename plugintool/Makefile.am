
bin_PROGRAMS = plugintool

plugintool_SOURCES = \
	PluginTool.c++ PluginTool.h++ Main.c++

plugintool_CPPFLAGS = -DDEBUG -I$(top_srcdir)/lib -std=c++11

plugintool_LDADD = -L. \
	-L$(top_builddir)/lib/.libs -lcommonc++

man_MANS = plugintool.1
