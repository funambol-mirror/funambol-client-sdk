dnl Process this file with autoconf to produce a configure script.
AC_INIT([funambol], [8.5])
AC_CANONICAL_SYSTEM
AC_CONFIG_SRCDIR([../../src/cpp/common/base/base64.cpp])
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

AC_CONFIG_MACRO_DIR([m4])

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL

dnl AM_GNU_GETTEXT - not currently used

dnl Use a specific transport agent
AC_ARG_WITH(transport-agent,
            AS_HELP_STRING([--with-transport-agent=curl/mac/mozilla],
                           [force to use the specified transport agent. NOTE: the Mozilla transport agent requires --with-mozilla-sdk-path=DIR for setting the Mozilla SDK path]),
            transport_agent="$withval", transport_agent="no")

dnl use_curl_ta="no"
dnl use_mac_ta="no"
dnl use_mozilla_ta="no"

dnl If the transport agent is not specified, set it to default: "mac" for Mac OS, "curl" for all the other posix systems
if test "x$transport_agent" = "xno"; then
    case $target in
        *apple-darwin*)  use_curl_ta="yes" ;;
        *) use_curl_ta="yes" ;;
    esac
else
    case $transport_agent in
        curl)  use_curl_ta="yes" ;;
        mac)  use_mac_ta="yes" ;;
        mozilla)  use_mozilla_ta="yes" ;;
        *) AC_MSG_ERROR([you must choose one of the following transport agents: curl/mac/mozilla]) ;;
    esac
fi

dnl check for libcurl now instead of during make
if test "x$use_curl_ta" = "xyes"; then
    AC_MSG_CHECKING([for libcurl])
    if CURL_LDFLAGS=`sh -c 'curl-config --libs' 2>&AS_MESSAGE_LOG_FD` && \
       CURL_CFLAGS=`sh -c 'curl-config --cflags' 2>&AS_MESSAGE_LOG_FD`; then
            AC_MSG_RESULT([found])
            AC_SUBST(CURL_LDFLAGS)
            AC_SUBST(CURL_CFLAGS)
            CPPFLAGS="-DFUN_TRANSPORT_AGENT=FUN_CURL_TRANSPORT_AGENT $CPPFLAGS"
    else
            AC_MSG_RESULT([missing])
            AC_MSG_ERROR([libcurl is required, check that its development package is installed and curl-config is in your PATH])
    fi
fi

if test "x$use_mac_ta" = "xyes"; then
    CURL_LDFLAGS="-framework CoreServices"
    AC_SUBST(CURL_LDFLAGS)
    CPPFLAGS="-DFUN_TRANSPORT_AGENT=FUN_MAC_TRANSPORT_AGENT $CPPFLAGS"
fi

dnl check for mozilla sdk
if test "x$use_mozilla_ta" = "xyes"; then

        AC_ARG_WITH(mozilla-sdk-path,
                    AS_HELP_STRING([--with-mozilla-sdk-path=DIR],
                                   [use the Mozilla SDK from directory DIR]),
                    mozilla_sdk_path="$withval", 
                    [AC_MSG_ERROR([Mozilla SDK is required, you must specify its path using --with-mozilla-sdk-path=DIR])])

        CPPFLAGS="-I$mozilla_sdk_path/include -I$mozilla_sdk_path/include/xpcom -I$mozilla_sdk_path/include/nspr -I$mozilla_sdk_path/include/string -I$mozilla_sdk_path/include/content $CPPFLAGS"
        LDFLAGS="-L$mozilla_sdk_path/lib -lxpcomglue_s -lxpcom -lnspr4 -lplds4 -lplc4 $LDFLAGS"

        AC_MSG_CHECKING([for mozilla headers])
        AC_CHECK_HEADERS(mozilla-config.h,
                         [],
                         [AC_MSG_ERROR([Mozilla headers not found, you must specify the Mozilla SDK path using --with-mozilla-sdk-path=DIR])])

        CPPFLAGS="-DFUN_TRANSPORT_AGENT=FUN_MOZ_TRANSPORT_AGENT $CPPFLAGS"
fi

dnl Check for certain headers.
AC_CHECK_HEADERS(time.h)

AC_ARG_ENABLE(unit-tests,
              AS_HELP_STRING([--enable-unit-tests],
                             [enables tests embedded in the source code of the library (changes content of the library)]),
              enable_unit_tests="$enableval", enable_unit_tests="no")
AC_ARG_ENABLE(integration-tests,
              AS_HELP_STRING([--enable-integration-tests],
                             [enables tests outside of the library (can be used together with normal builds of the library)]),
              enable_integration_tests="$enableval", enable_integration_tests="no")
AC_ARG_ENABLE(assert,
              AS_HELP_STRING([--enable-assert],
                             [turn on assertions]),
              enable_assert="$enableval", enable_assert="no")

if test $enable_assert == "no"; then
        AC_DEFINE(NDEBUG, 1, [Define to 1 if assertions should be disabled.])
fi

# Optionally use wchar_t for unicode strings
AC_ARG_ENABLE(wchar,
              AS_HELP_STRING([--enable-wchar],
                             [enable Unicode wchar usage]),
              enable_wchar="$enableval", enable_wchar="no")

if test $enable_wchar == "yes"; then
        AC_DEFINE(USE_WCHAR, 1, [Define to 1 if unicode wchar_t should be used.])
        AC_CHECK_HEADERS(wchar.h)
fi

# cppunit needed?
if test $enable_unit_tests == "yes" || test $enable_integration_tests == yes; then
        CPPUNIT_CXXFLAGS=`cppunit-config --cflags` || AC_MSG_ERROR("cppunit-config --cflags failed - is it installed?")
        CPPUNIT_LDFLAGS=`cppunit-config --libs` || AC_MSG_ERROR("cppunit-config --libs failed - is it installed?")
        CPPUNIT_TESTS=client-test
        AC_CHECK_HEADERS(signal.h)
fi
AC_SUBST(CPPUNIT_CXXFLAGS)
AC_SUBST(CPPUNIT_LDFLAGS)
AC_SUBST(CPPUNIT_TESTS)

# mangle VERSION to set library version info
SO_VERSION_INFO=$(echo $VERSION | sed -e 's/\./:/g')
AC_SUBST(SO_VERSION_INFO)

if test $enable_unit_tests == "yes"; then
        AC_DEFINE(ENABLE_UNIT_TESTS, 1, [enable unit tests inside the library's source code])
fi
if test $enable_integration_tests == "yes"; then
        AC_DEFINE(ENABLE_INTEGRATION_TESTS, 1, [enable unit tests outside of the library's source code])
fi

AC_OUTPUT(Makefile libfunambol.pc src/Makefile include/Makefile test/Makefile)
