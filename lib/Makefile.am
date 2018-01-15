
lib_LTLIBRARIES = libcommonc++.la libcommonc++db.la

if USE_JAVA
lib_LTLIBRARIES += libcommonc++jvm.la
endif # USE_JAVA

if USE_EXPAT
lib_LTLIBRARIES += libcommonc++xml.la
endif # USE_EXPAT

# Versioning rules ( C:R:A )
#
# 1. Start with version 0:0:0.
# 2. If any of the sources have changed, increment R. This is a new revision
#    of the current interface.
# 3. If the interface has changed, increment C and set R to 0. This is the
#    first revision of a new interface.
# 4. If the new interface is a superset of the previous interface
#    (that is, if the previous interface has not been broken by the
#    changes in this new release), increment A. This release is backwards
#    compatible with the previous release.
# 5. If the new interface has removed elements with respect to the
#    previous interface, then backward compatibility is broken; set A to 0.
#    This release has a new, but backwards incompatible interface.
#
# For more info see section 6.3 of the GNU Libtool Manual.

VERINFO = -version-info 12:0:0

if WINDOWS
PLATFORM_SRC = Windows.c++ DLLMain.c++
PLATFORM_JVMSRC = DLLMainJVM.c++
PLATFORM_DBSRC = DLLMainDB.c++
PLATFORM_XMLSRC = DLLMainXML.c++
PLATFORM_HDR = commonc++/Windows.h++
cbitsinc = -I$(top_srcdir)/cbits
cbitslib = $(top_srcdir)/cbits/libcbits.la
else # ! WINDOWS
PLATFORM_SRC = POSIX.c++
PLATFORM_HDR = commonc++/POSIX.h++
endif # WINDOWS

libsrc = \
	AllocationMap.c++ \
	Application.c++ \
	AsyncIOPoller.c++ \
	AsyncIOTask.c++ \
	AtomicCounter.c++ \
	Base64.c++ \
	BitSet.c++ \
	Blob.c++ \
	Buffer.c++ \
	ByteArrayDataReader.c++ \
	ByteArrayDataWriter.c++ \
	ByteBufferDataReader.c++ \
	ByteBufferDataWriter.c++ \
	ByteOrder.c++ \
	Char.c++ \
	CharOps.c++ \
	CharRef.c++ \
	Checksum.c++ \
	CircularBuffer.c++ \
	CircularByteBufferDataReader.c++ \
	CircularByteBufferDataWriter.c++ \
	ConditionVar.c++ \
	Console.c++ \
	ConsoleLogger.c++ \
	CPUStats.c++ \
	CRC32Checksum.c++ \
	CriticalSection.c++ \
	CString.c++ \
	CStringBuilder.c++ \
	CStringLessThanFunctor.c++ \
	DataEncoder.c++ \
	DataFormatException.c++ \
	DataReader.c++ \
	DataWriter.c++ \
	DatagramSocket.c++ \
	Date.c++ \
	DateTime.c++ \
	DateTimeFormat.c++ \
	Digest.c++ \
	Dir.c++ \
	DirectoryWatcher.c++ \
	EncodingException.c++ \
	Exception.c++ \
	File.c++ \
	FileLogger.c++ \
	FileName.c++ \
	FilePtr.c++ \
	FileTraverser.c++ \
	Hash.c++ \
	Hex.c++ \
	InetAddress.c++ \
	InterruptedException.c++ \
	IntervalTimer.c++ \
	InvalidArgumentException.c++ \
	IOException.c++ \
	LoadableModule.c++ \
	LoadAverageStats.c++ \
	Locale.c++ \
	Log.c++ \
	LogFormat.c++ \
	Logger.c++ \
	MACAddress.c++ \
	MD5Digest.c++ \
	MD5Password.c++ \
	MemoryBlock.c++ \
	MemoryMappedFile.c++ \
	MemoryStats.c++ \
	MulticastSocket.c++ \
	Mutex.c++ \
	NetworkInterface.c++ \
	Network.c++ \
	NullPointerException.c++ \
	Numeric.c++ \
	OutOfBoundsException.c++ \
	ParseException.c++ \
	Permissions.c++ \
	Pipe.c++ \
	Plugin.c++ \
	PluginLoader.c++ \
	Process.c++ \
	PulseTimer.c++ \
	Random.c++ \
	ReadWriteLock.c++ \
	RegExp.c++ \
	SearchPath.c++ \
	Semaphore.c++ \
	SerialPort.c++ \
	ServerSocket.c++ \
	ServerStreamPipe.c++ \
	Service.c++ \
	SHA1Digest.c++ \
	SharedMemoryBlock.c++ \
	Socket.c++ \
	SocketAddress.c++ \
	SocketException.c++ \
	SocketSelector.c++ \
	SocketUtil.c++ \
	StopWatch.c++ \
	Stream.c++ \
	StreamDataReader.c++ \
	StreamDataWriter.c++ \
	StreamPipe.c++ \
	StreamSocket.c++ \
	String.c++ \
	System.c++ \
	SystemException.c++ \
	SystemLog.c++ \
	TempFile.c++ \
	Thread.c++ \
	ThreadLocalCounter.c++ \
	Time.c++ \
	TimeSpan.c++ \
	TimeSpec.c++ \
	UnsupportedOperationException.c++ \
	URL.c++ \
        UTFDecoder.c++ \
	UTF32Decoder.c++ \
	UTF8Decoder.c++ \
	UTF8Encoder.c++ \
	UUID.c++ \
	Variant.c++ \
	Version.c++ \
	XDRDecoder.c++ \
	XDREncoder.c++ \
	commonc++/Private.h++ \
	$(PLATFORM_SRC)

libhdr = \
	commonc++/AbstractBuffer.h++ \
	commonc++/AbstractBufferImpl.h++ \
	commonc++/AbstractCache.h++ \
	commonc++/AbstractCacheImpl.h++ \
	commonc++/AllocationMap.h++ \
	commonc++/Application.h++ \
	commonc++/Array.h++ \
	commonc++/AsyncIOPoller.h++ \
	commonc++/AsyncIOTask.h++ \
	commonc++/AtomicCounter.h++ \
	commonc++/Base64.h++ \
	commonc++/BitSet.h++ \
	commonc++/Blob.h++ \
	commonc++/BTree.h++ \
	commonc++/BTreeImpl.h++ \
	commonc++/BoundedQueue.h++ \
	commonc++/BoundedQueueImpl.h++ \
	commonc++/Buffer.h++ \
	commonc++/BufferImpl.h++ \
	commonc++/BufferedStream.h++ \
	commonc++/BasicBufferedStream.h++ \
	commonc++/BasicBufferedStreamImpl.h++ \
	commonc++/ByteArrayDataReader.h++ \
	commonc++/ByteArrayDataWriter.h++ \
	commonc++/ByteBufferDataReader.h++ \
	commonc++/ByteBufferDataWriter.h++ \
	commonc++/ByteOrder.h++ \
	commonc++/Cache.h++ \
	commonc++/CacheImpl.h++ \
	commonc++/Char.h++ \
	commonc++/CharRef.h++ \
	commonc++/CharOps.h++ \
	commonc++/Checksum.h++ \
	commonc++/CircularBuffer.h++ \
	commonc++/CircularBufferImpl.h++ \
	commonc++/CircularByteBufferDataReader.h++ \
	commonc++/CircularByteBufferDataWriter.h++ \
	commonc++/Common.h++ \
	commonc++/ConditionVar.h++ \
	commonc++/Console.h++ \
	commonc++/ConsoleLogger.h++ \
	commonc++/CPUStats.h++ \
	commonc++/CRC32Checksum.h++ \
	commonc++/CString.h++ \
	commonc++/CStringBuilder.h++ \
	commonc++/CStringLessThanFunctor.h++ \
	commonc++/CriticalSection.h++ \
	commonc++/DataEncoder.h++ \
	commonc++/DataFormatException.h++ \
	commonc++/DataReader.h++ \
	commonc++/DataWriter.h++ \
	commonc++/DatagramSocket.h++ \
	commonc++/Date.h++ \
	commonc++/DateTime.h++ \
	commonc++/DateTimeFormat.h++ \
	commonc++/DeleteFunctor.h++ \
	commonc++/Digest.h++ \
	commonc++/Dir.h++ \
	commonc++/DirectoryWatcher.h++ \
	commonc++/DynamicArray.h++ \
	commonc++/DynamicCache.h++ \
	commonc++/DynamicCacheImpl.h++ \
	commonc++/DynamicObjectPool.h++ \
	commonc++/DynamicObjectPoolImpl.h++ \
	commonc++/EnumTraits.h++ \
	commonc++/EventHandler.h++ \
	commonc++/EncodingException.h++ \
	commonc++/Exception.h++ \
	commonc++/File.h++ \
	commonc++/FileLogger.h++ \
	commonc++/FileName.h++ \
	commonc++/FilePtr.h++ \
	commonc++/FileTraverser.h++ \
	commonc++/Flags.h++ \
	commonc++/FlagsImpl.h++ \
	commonc++/Hash.h++ \
	commonc++/Hex.h++ \
	commonc++/InterruptedException.h++ \
	commonc++/IntervalTimer.h++ \
	commonc++/InvalidArgumentException.h++ \
	commonc++/IOException.h++ \
	commonc++/InetAddress.h++ \
	commonc++/Integers.h++ \
	commonc++/Iterator.h++ \
	commonc++/JavaException.h++ \
	commonc++/LoadableModule.h++ \
	commonc++/LoadAverageStats.h++ \
	commonc++/Locale.h++ \
	commonc++/Lock.h++ \
	commonc++/Log.h++ \
	commonc++/LogFormat.h++ \
	commonc++/Logger.h++ \
	commonc++/MACAddress.h++ \
	commonc++/MD5Digest.h++ \
	commonc++/MD5Password.h++ \
	commonc++/MemoryBlock.h++ \
	commonc++/MemoryMappedFile.h++ \
	commonc++/MemoryStats.h++ \
	commonc++/MulticastSocket.h++ \
	commonc++/Mutex.h++ \
	commonc++/NetworkInterface.h++ \
	commonc++/Network.h++ \
	commonc++/NullPointerException.h++ \
	commonc++/Numeric.h++ \
	commonc++/ObjectPool.h++ \
	commonc++/OutOfBoundsException.h++ \
	commonc++/ParseException.h++ \
	commonc++/Permissions.h++ \
	commonc++/Pipe.h++ \
	commonc++/Plugin.h++ \
	commonc++/PluginLoader.h++ \
	commonc++/POSIX.h++ \
	commonc++/Process.h++ \
	commonc++/PulseTimer.h++ \
	commonc++/ProgressTracker.h++ \
	commonc++/Random.h++ \
	commonc++/ReadWriteLock.h++ \
	commonc++/RefSet.h++ \
	commonc++/RefSetImpl.h++ \
	commonc++/RegExp.h++ \
	commonc++/Runnable.h++ \
	commonc++/ScopedLock.h++ \
	commonc++/ScopedPtr.h++ \
	commonc++/ScopedReadWriteLock.h++ \
	commonc++/SearchPath.h++ \
	commonc++/Semaphore.h++ \
	commonc++/SerialPort.h++ \
	commonc++/ServerSocket.h++ \
	commonc++/ServerStreamPipe.h++ \
	commonc++/Service.h++ \
	commonc++/SHA1Digest.h++ \
	commonc++/SharedMemoryBlock.h++ \
	commonc++/SharedPtr.h++ \
	commonc++/Socket.h++ \
	commonc++/SocketAddress.h++ \
	commonc++/SocketException.h++ \
	commonc++/SocketSelector.h++ \
	commonc++/SocketUtil.h++ \
	commonc++/StaticCache.h++ \
	commonc++/StaticCacheImpl.h++ \
	commonc++/StaticObjectPool.h++ \
	commonc++/StaticObjectPoolImpl.h++ \
	commonc++/StopWatch.h++ \
	commonc++/Stream.h++ \
	commonc++/StreamDataReader.h++ \
	commonc++/StreamDataWriter.h++ \
	commonc++/StreamPipe.h++ \
	commonc++/StreamSocket.h++ \
	commonc++/String.h++ \
	commonc++/System.h++ \
	commonc++/SystemException.h++ \
	commonc++/SystemLog.h++ \
	commonc++/SystemStats.h++ \
	commonc++/TempFile.h++ \
	commonc++/TerminalAttr.h++ \
	commonc++/Thread.h++ \
	commonc++/ThreadLocal.h++ \
	commonc++/ThreadLocalImpl.h++ \
	commonc++/ThreadLocalBuffer.h++ \
	commonc++/ThreadLocalCounter.h++ \
	commonc++/Time.h++ \
	commonc++/TimeSpan.h++ \
	commonc++/TimeSpec.h++ \
	commonc++/UnsupportedOperationException.h++ \
	commonc++/URL.h++ \
	commonc++/UTFDecoder.h++ \
	commonc++/UTF32Decoder.h++ \
	commonc++/UTF8Decoder.h++ \
	commonc++/UTF8Encoder.h++ \
	commonc++/UUID.h++ \
	commonc++/Variant.h++ \
	commonc++/Version.h++ \
	commonc++/XDRDecoder.h++ \
	commonc++/XDREncoder.h++ \
	$(PLATFORM_HDR)

libjvmsrc = \
	JavaBuffer.c++ \
	JavaContext.c++ \
	JavaException.c++ \
	JavaThreadLocalBuffer.c++ \
	JavaVirtualMachine.c++ \
	$(PLATFORM_JVMSRC)

libjvmhdr = \
	commonc++/JavaBuffer.h++ \
	commonc++/JavaContext.h++ \
	commonc++/JavaThreadLocalBuffer.h++ \
	commonc++/JavaVirtualMachine.h++

libdbsrc = \
	SQLDatabase.c++ \
	SQLException.c++ \
	SQLQuery.c++ \
	SQLValueBinder.c++ \
	SQLPrivate.h++ \
	$(PLATFORM_DBSRC)

libdbhdr = \
	commonc++/SQLDatabase.h++ \
	commonc++/SQLException.h++ \
	commonc++/SQLQuery.h++ \
	commonc++/SQLValueBinder.h++

libxmlsrc = \
	XMLDocument.c++ \
	XMLElement.c++ \
	XMLElementConstIterator.c++ \
	XMLElementIterator.c++ \
	$(PLATFORM_XMLSRC)

libxmlhdr = \
	commonc++/XMLDocument.h++ \
	commonc++/XMLElement.h++ \
	commonc++/XMLElementConstIterator.h++ \
	commonc++/XMLElementIterator.h++

#------------------------------------------------------------------------------

libcommonc___la_LDFLAGS = $(VERINFO) -no-undefined

libcommonc___la_SOURCES = $(libsrc)

libcommonc___la_CPPFLAGS = -D_REENTRANT @JNI_CPPFLAGS@ \
	-I$(top_srcdir)/libatomic \
	-I$(top_srcdir)/libstacktrace \
	-I$(top_srcdir)/pcre-$(PCRE_VERSION) $(cbitsinc) -DDLL_EXPORT \
	-std=c++11

if MINGW
libcommonc___la_CPPFLAGS += -DCOMMONCPP_EXPORTS -D_USE_32BIT_TIME_T
endif # MINGW

libcommonc___la_CFLAGS = -g

nobase_include_HEADERS = $(libhdr)

libcommonc___la_LIBADD = \
	$(top_builddir)/libatomic/libatomic.la \
	$(top_builddir)/libstacktrace/libstacktrace.la \
	$(cbitslib) \
	$(top_builddir)/pcre-$(PCRE_VERSION)/libpcre16.la

#-------- commonc++jvm

if USE_JAVA

libcommonc__jvm_la_LDFLAGS = $(VERINFO) -no-undefined

JAVA_HOME = @java_home@

libcommonc__jvm_la_SOURCES = $(libjvmsrc)

libcommonc__jvm_la_CPPFLAGS = -D_REENTRANT -std=c++11 @JNI_CPPFLAGS@

if MINGW
libcommonc__jvm_la_CPPFLAGS += -DCOMMONCPPJVM_EXPORTS
endif # MINGW

nobase_include_HEADERS += $(libjvmhdr)

libcommonc__jvm_la_LIBADD = @JNI_LDFLAGS@ -lcommonc++

endif # USE_JAVA

#-------- commonc++db

libcommonc__db_la_LDFLAGS = $(VERINFO) -no-undefined

libcommonc__db_la_SOURCES = $(libdbsrc)

libcommonc__db_la_CPPFLAGS = -D_REENTRANT -std=c++11 \
	-I$(top_srcdir)/sqlite-$(SQLITE_VERSION)

if MINGW
libcommonc__db_la_CPPFLAGS += -DCOMMONCPPDB_EXPORTS
endif # MINGW

nobase_include_HEADERS += $(libdbhdr)

libcommonc__db_la_LIBADD = \
	$(top_builddir)/sqlite-$(SQLITE_VERSION)/libsqlite3.la -lcommonc++

#-------- commonc++xml

if USE_EXPAT

libcommonc__xml_la_LDFLAGS = $(VERINFO) -no-undefined

EXPAT_HOME = @expat_home@

libcommonc__xml_la_SOURCES = $(libxmlsrc)

libcommonc__xml_la_CPPFLAGS = -D_REENTRANT -std=c++11 -I$(EXPAT_HOME)/include

if MINGW
libcommonc__xml_la_CPPFLAGS += -DCOMMONCPPXML_EXPORTS
endif # MINGW

nobase_include_HEADERS += $(libxmlhdr)

libcommonc__xml_la_LIBADD = -L$(EXPAT_HOME)/lib -lexpat -L.libs -lcommonc++

endif # USE_EXPAT

dox:
	doxygen doxygen.cfg

EXTRA_DIST = doxygen.cfg

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = commonc++.pc commonc++jvm.pc commonc++db.pc commonc++xml.pc
