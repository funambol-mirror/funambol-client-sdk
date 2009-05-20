TESTS = @CPPUNIT_TESTS@
EXTRA_PROGRAMS = client-test
check_PROGRAMS = @CPPUNIT_TESTS@

CXXDIR  = $(srcdir)/../../../src/c++
INCDIR  = $(srcdir)/../../../src/include
TESTDIR = $(srcdir)/../../../test

LDADD = ../src/libfunambol.la 
AM_LDFLAGS = @CURL_LDFLAGS@ @CPPUNIT_LDFLAGS@
AM_CXXFLAGS = @CURL_CFLAGS@ @CPPUNIT_CXXFLAGS@
AM_CPPFLAGS = -I$(INCDIR)/posix -I$(INCDIR)/common -I$(TESTDIR)/integration -DFUNAMBOL_BUILD_API

vpath %.cpp	$(TESTDIR) \
		$(TESTDIR)/common/base/util \
		$(TESTDIR)/common/base \
	#	$(TESTDIR)/common/spds/ \
		$(TESTDIR)/posix/spdm/ \
		$(TESTDIR)/common/syncml \
		$(TESTDIR)/common/filter \
		$(TESTDIR)/common/client \
		$(TESTDIR)/integration \
		$(TESTDIR)/common/event \
                $(TESTDIR)/common/push

# Test runner sources
SOURCES = client-test-main.cpp ClientTest.cpp TestFileSource.cpp

# Testcase sources
TESTS_BASE = \
    ArrayListTest.cpp \
    BasicTimeTest.cpp \
    StringBufferTest.cpp \
    PropertyFileTest.cpp \
    XMLProcessorTest.cpp \
    base64Test.cpp

TESTS_FILTER = \
    ClauseTest.cpp \
    FilterTest.cpp

TESTS_SYNCML = \
    ParserTest.cpp \
    FormatterTest.cpp \
    ObjectDelTest.cpp

TESTS_SPDM = \
    ConfigTest.cpp

TESTS_CLIENT = \
    OptionParserTest.cpp \
    ConfigSyncSourceUnitTest.cpp

TESTS_EVENT = EventTest.cpp

TESTS_SPDS = \
    MappingsTest.cpp \
    ConfigSyncSourceTest.cpp \
    AccountFolderTest.cpp \
    FolderDataTest.cpp \
    FolderExtTest.cpp \
    MailAccountTest.cpp \
    MappingTestSyncSource.cpp

TESTS_PUSH = \
    FThreadTest.cpp \
    CTPServiceTest.cpp
    
TESTCASES = $(TESTS_BASE) $(TESTS_FILTER) $(TESTS_SYNCML) $(TESTS_SPDM) $(TESTS_EVENT) $(TESTS_CLIENT) $(TESTS_SPDS) $(TESTS_PUSH)

client_test_SOURCES = $(SOURCES) $(TESTCASES)

# ensure that files are in the current directory before the check runs
check-am check: client-test-files
.PHONY: client-test-files
client-test-files:
	@ [ -e synccompare.pl ] || ln -s $(TESTDIR)/synccompare.pl .
	@ [ -d testcases ] || ln -s $(TESTDIR)/testcases .
	@ [ -d Algorithm ] || ln -s $(TESTDIR)/Algorithm

# also install files in a test sub directory:
# in source files the API internal definitions need to
# be removed so that the files can be used externally
source_files = \
        test/ClientTest.h       \
	test/ClientTest.cpp     \
        test/TestFileSource.h   \
        test/TestFileSource.cpp \
        test/client-test-main.cpp
verbatim_files = \
	test/Algorithm/Artistic \
	test/Algorithm/copyright \
	test/Algorithm/Diff.pm \
	test/Algorithm/README \
	test/testcases/ical20.ics \
	test/testcases/itodo20.ics \
	test/testcases/vcal10.ics \
	test/testcases/vcard21.vcf \
	test/testcases/vcard30.vcf \
	test/testcases/imemo20.ics \
	test/testcases/devInfResults.xml \
	test/testcases/devInfResults2.xml \
	test/testcases/ex1.xml \
	test/testcases/ex2.xml \
	test/testcases/syncML1.xml \
	test/testcases/syncML2.xml \
	test/testcases/syncML3.xml \
	test/testcases/syncML4.xml \
	test/testcases/syncML5.xml \
	test/testcases/syncML6.xml \
	test/testcases/syncML7.xml \
	test/testcases/syncML8.xml \
	test/testcases/wrongex.xml \
        test/README.txt \
        test/synccompare.pl

nobase_pkginclude_HEADERS = \
	$(source_files) \
	$(verbatim_files)


CLEANFILES = $(nobase_pkginclude_HEADERS)

# fix files for public distribution
$(source_files): %: ../../../%
	mkdir -p ${@D}
	cp  $< $@

# just copy for distribution
$(verbatim_files): %: ../../../%
	mkdir -p ${@D}
	cp $< $@
