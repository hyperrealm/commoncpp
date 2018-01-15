
noinst_PROGRAMS = commonc++_tests commonc++db_tests \
	service_test application_test stub

lib_LIBRARIES = libcommonc++testmain.a

if USE_EXPAT

EXPAT_HOME = @expat_home@

noinst_PROGRAMS += commonc++xml_tests

endif

if USE_JAVA

JAVA_HOME = @java_home@

noinst_PROGRAMS += commonc++jvm_tests

endif

CPPUNIT_HOME = @cppunit_home@

#----------------------------------------------------------------------------

lib_LTLIBRARIES = libdummy.la libtestplugin.la

libdummy_la_SOURCES = dummylib.c

libdummy_la_LDFLAGS = -version-info 0:0:0 -no-undefined

libtestplugin_la_SOURCES = TestPlugin.c++

libtestplugin_la_CXXFLAGS = -I$(top_srcdir)/lib -std=c++11

libtestplugin_la_LDFLAGS = -avoid-version -module -no-undefined

libtestplugin_la_LIBADD = -L$(top_builddir)/lib -lcommonc++

#----------------------------------------------------------------------------

libcommonc__testmain_a_SOURCES = \
	TestHarness.c++ TestHarness.h++ Pinwheel.h++ \
	TestUtils.c++ TestUtils.h++

libcommonc__testmain_a_CPPFLAGS = -DDEBUG -I$(top_builddir)/lib \
	-I$(CPPUNIT_HOME)/include -std=c++11

#----------------------------------------------------------------------------

commonc___tests_DEPENDENCIES = libcommonc++testmain.a

commonc___tests_SOURCES = \
	AllocationMapTest.c++ AllocationMapTest.h++ \
	ArrayTest.c++ ArrayTest.h++ \
	AsyncIOTest.c++ AsyncIOTest.h++ \
	AsyncIOPollerTest.c++ AsyncIOPollerTest.h++ \
	AtomicCounterTest.c++ AtomicCounterTest.h++ \
	BTreeTest.c++ BTreeTest.h++ \
	Base64Test.c++ Base64Test.h++ \
	BitSetTest.c++ BitSetTest.h++ \
	BlobTest.c++ BlobTest.h++ \
	BoundedQueueTest.c++ BoundedQueueTest.h++ \
	BufferTest.c++ BufferTest.h++ \
	BufferedStreamTest.c++ BufferedStreamTest.h++ \
	ByteArrayDataReaderTest.c++ ByteArrayDataReaderTest.h++ \
	ByteArrayDataWriterTest.c++ ByteArrayDataWriterTest.h++ \
	ByteBufferDataWriterTest.c++ ByteBufferDataWriterTest.h++ \
	ByteOrderTest.c++ ByteOrderTest.h++ \
	CacheTest.c++ CacheTest.h++ \
	CharTest.c++ CharTest.h++ \
	CircularBufferTest.c++ CircularBufferTest.h++ \
	CircularByteBufferDataReaderTest.c++ CircularByteBufferDataReaderTest.h++ \
	CircularByteBufferDataWriterTest.c++ CircularByteBufferDataWriterTest.h++ \
	ConditionVarTest.c++ ConditionVarTest.h++ \
	CPUStatsTest.c++ CPUStatsTest.h++ \
	CriticalSectionTest.c++ CriticalSectionTest.h++ \
	CStringBuilderTest.c++ CStringBuilderTest.h++ \
	DatagramSocketTest.c++ DatagramSocketTest.h++ \
	DateTest.c++ DateTest.h++ \
	DateTimeTest.c++ DateTimeTest.h++ \
	DateTimeFormatTest.c++ DateTimeFormatTest.h++ \
	DirTest.c++ DirTest.h++ \
	DirectoryWatcherTest.c++ DirectoryWatcherTest.h++ \
	DynamicArrayTest.c++ DynamicArrayTest.h++ \
	DynamicObjectPoolTest.c++ DynamicObjectPoolTest.h++ \
	EventHandlerTest.c++ EventHandlerTest.h++ \
	FileNameTest.c++ FileNameTest.h++ \
	FileStreamTest.c++ FileStreamTest.h++ \
	FileTest.c++ FileTest.h++ \
	FileTraverserTest.c++ FileTraverserTest.h++ \
	HexTest.c++ HexTest.h++ \
	InetAddressTest.c++ InetAddressTest.h++ \
	IntervalTimerTest.c++ IntervalTimerTest.h++ \
	LoadableModuleTest.c++ LoadableModuleTest.h++ \
	LocaleTest.c++ LocaleTest.h++ \
	LogFormatTest.c++ LogFormatTest.h++ \
	LogTest.c++ LogTest.h++ \
	MD5DigestTest.c++ MD5DigestTest.h++ \
	MD5PasswordTest.c++ MD5PasswordTest.h++ \
	MemoryBlockTest.c++ MemoryBlockTest.h++ \
	MemoryMappedFileTest.c++ MemoryMappedFileTest.h++ \
	MemoryStatsTest.c++ MemoryStatsTest.h++ \
	MulticastSocketTest.c++ MulticastSocketTest.h++ \
	MutexTest.c++ MutexTest.h++ \
	NetworkTest.c++ NetworkTest.h++ \
	NumericTest.c++ NumericTest.h++ \
	PermissionsTest.c++ PermissionsTest.h++ \
	ProcessTest.c++ ProcessTest.h++ \
	PulseTimerTest.c++ PulseTimerTest.h++ \
	RandomTest.c++ RandomTest.h++ \
	ReadWriteLockTest.c++ ReadWriteLockTest.h++ \
	RefSetTest.c++ RefSetTest.h++ \
	RegExpTest.c++ RegExpTest.h++ \
	ScopedPtrTest.c++ ScopedPtrTest.h++ \
	SearchPathTest.c++ SearchPathTest.h++ \
	SemaphoreTest.c++ SemaphoreTest.h++ \
	SerialPortTest.c++ SerialPortTest.h++ \
	ServerSocketTest.c++ ServerSocketTest.h++ \
	SharedMemoryBlockTest.c++ SharedMemoryBlockTest.h++ \
	SharedPtrTest.c++ SharedPtrTest.h++ \
	SHA1DigestTest.c++ SHA1DigestTest.h++ \
	SocketAddressTest.c++ SocketAddressTest.h++ \
	SocketSelectorTest.c++ SocketSelectorTest.h++ \
	StaticObjectPoolTest.c++ StaticObjectPoolTest.h++ \
	StopWatchTest.c++ StopWatchTest.h++ \
	StreamDataWriterTest.c++ StreamDataWriterTest.h++ \
	StreamPipeTest.c++ StreamPipeTest.h++ \
	StreamSocketTest.c++ StreamSocketTest.h++ \
	StringTest.c++ StringTest.h++ \
	SystemLogTest.c++ SystemLogTest.h++ \
	SystemTest.c++ SystemTest.h++ \
	TempFileTest.c++ TempFileTest.h++ \
	ThreadLocalTest.c++ ThreadLocalTest.h++ \
	ThreadTest.c++ ThreadTest.h++ \
	TimeSpanTest.c++ TimeSpanTest.h++ \
	TimeSpecTest.c++ TimeSpecTest.h++ \
	TimeTest.c++ TimeTest.h++ \
	URLTest.c++ URLTest.h++ \
	UTF8DecoderTest.c++ UTF8DecoderTest.h++ \
	UUIDTest.c++ UUIDTest.h++ \
	VariantTest.c++ VariantTest.h++ \
	VersionTest.c++ VersionTest.h++ \
	XDREncoderTest.c++ XDREncoderTest.h++

commonc___tests_CPPFLAGS = -DDEBUG -I$(top_srcdir)/lib \
	-I$(CPPUNIT_HOME)/include -std=c++11

commonc___tests_LDADD = -L. -lcommonc++testmain \
	-L$(top_builddir)/lib/.libs -lcommonc++ \
	-L$(CPPUNIT_HOME)/lib -lcppunit

#----------------------------------------------------------------------------

if USE_EXPAT

commonc__xml_tests_DEPENDENCIES = libcommonc++testmain.a

commonc__xml_tests_SOURCES = XMLDocumentTest.c++ XMLDocumentTest.h++

commonc__xml_tests_CPPFLAGS = -DDEBUG -I$(top_srcdir)/lib \
	-I$(CPPUNIT_HOME)/include -I$(EXPAT_HOME)/include -std=c++11

commonc__xml_tests_LDADD = -L. -lcommonc++testmain \
	-L$(top_builddir)/lib/.libs -lcommonc++xml -lcommonc++ \
	-L$(CPPUNIT_HOME)/lib -lcppunit \
	-L$(EXPAT_HOME)/lib -lexpat

endif

#----------------------------------------------------------------------------

commonc__db_tests_DEPENDENCIES = libcommonc++testmain.a

commonc__db_tests_SOURCES = SQLDatabaseTest.c++ SQLDatabaseTest.h++

commonc__db_tests_CPPFLAGS = -DDEBUG -I$(top_srcdir)/lib \
	-I$(CPPUNIT_HOME)/include -std=c++11

commonc__db_tests_LDADD = -L. -lcommonc++testmain \
	-L$(top_builddir)/lib/.libs -lcommonc++db -lcommonc++ \
	-L$(CPPUNIT_HOME)/lib -lcppunit

#----------------------------------------------------------------------------

if USE_JAVA

commonc__jvm_tests_DEPENDENCIES = libcommonc++testmain.a

commonc__jvm_tests_SOURCES = JavaVirtualMachineTest.c++ \
	JavaVirtualMachineTest.h++

commonc__jvm_tests_CPPFLAGS = -DDEBUG -I$(top_srcdir)/lib \
	-I$(CPPUNIT_HOME)/include -std=c++11 \
	@JNI_CPPFLAGS@

commonc__jvm_tests_LDADD = -L. -lcommonc++testmain \
	-L$(top_builddir)/lib/.libs -lcommonc++jvm -lcommonc++ \
	-L$(CPPUNIT_HOME)/lib -lcppunit @JNI_LDFLAGS@

endif

#----------------------------------------------------------------------------

service_test_SOURCES = StubService.c++ StubService.h++

service_test_CPPFLAGS = -DDEBUG -I$(top_srcdir)/lib -I$(CPPUNIT_HOME)/include \
	-std=c++11

service_test_LDADD = -L$(top_builddir)/lib/.libs -lcommonc++ \
	-L$(CPPUNIT_HOME)/lib -lcppunit

#----------------------------------------------------------------------------

application_test_SOURCES = StubApp.c++ StubApp.h++

application_test_CPPFLAGS = -DDEBUG -I$(top_srcdir)/lib \
	-I$(CPPUNIT_HOME)/include -std=c++11

application_test_LDADD =  -L$(top_builddir)/lib/.libs -lcommonc++ \
	-L$(CPPUNIT_HOME)/lib -lcppunit

#----------------------------------------------------------------------------

stub_SOURCES = Stub.c++

stub_CPPFLAGS = -DDEBUG -I$(top_srcdir)/lib \
	-I$(CPPUNIT_HOME)/include -std=c++11

stub_LDADD = -L$(top_builddir)/lib/.libs -lcommonc++ \
	-L$(CPPUNIT_HOME)/lib -lcppunit

#----------------------------------------------------------------------------

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = commonc++testmain.pc

#----------------------------------------------------------------------------

# supporting files
datafiles = 

EXTRA_DIST = $(datafiles)


