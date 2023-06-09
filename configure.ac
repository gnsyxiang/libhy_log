#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])

m4_include([m4/check_project_info.m4])

AC_INIT([PROJECT_NAME], [VERSION_NUM], [EMAIL_INFO], [PROJECT_NAME], [PROJECT_URL])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AM_SILENT_RULES([yes])
AM_PROG_AR
LT_INIT

PKG_PROG_PKG_CONFIG([0.24])
PKG_INSTALLDIR

# check m4 macro
CHECK_PROJECT_INFO
CHECK_VERSION_INFO
CHECK_DOXYGEN
# CHECK_JAVA
CHECK_PTHREAD

SELECT_VENDER
SELECT_CHIP
# SELECT_PRODUCT
# SELECT_LANGUAGE
SELECT_LOG_TYPE

AC_ARG_ENABLE([debug_info],
              [AS_HELP_STRING([--enable-debug_info], [enable support for output debug info])],
              [], [enable_debug_info=no])
AM_CONDITIONAL(COMPILE_DEBUG_INFO, [test "$enable_debug_info" = "yes"])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
# AC_PROG_RANLIB

CHECK_LIBCUNIT
CHECK_LIBLOG4CPLUS
CHECK_ZLOG

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [pthread_create])

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h stdint.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit gettimeofday localtime_r memset strerror])

AC_CONFIG_FILES([Makefile
                 $project_name.pc
                 build-script/Makefile
                 build-script/arterytek/Makefile
                 build-script/arterytek/at32f4xx/Makefile
                 build-script/eeasytech/Makefile
                 build-script/eeasytech/SV823/Makefile
                 build-script/fullhan/MC6810E/Makefile
                 build-script/fullhan/Makefile
                 build-script/nxp/Makefile
                 build-script/nxp/mx6ull/Makefile
                 build-script/pc/Makefile
                 build-script/pc/pc-chip/Makefile
                 build-script/rock-chips/Makefile
                 build-script/rock-chips/rk3568/Makefile
                 doc/Makefile
                 example/Makefile
                 include/Makefile
                 include/hy_log/Makefile
                 include/log4cplus/Makefile
                 include/zlog/Makefile
                 res/Makefile
                 res/voice/Makefile
                 res/voice/cn/Makefile
                 res/voice/en/Makefile
                 src/Makefile
                 src/hy_log/Makefile
                 src/log4cplus/Makefile
                 src/zlog/Makefile
                 test/Makefile])
AC_OUTPUT

AC_MSG_RESULT([
               $PACKAGE $VERSION

               CC:                  ${CC}
               CPPFLAGS:            ${CPPFLAGS}
               CFLAGS:              ${CFLAGS}
               CXX:                 ${CXX}
               CXXFLAGS:            ${CXXFLAGS}
               LDFLAGS:             ${LDFLAGS}
               LIBS:                ${LIBS}

               prefix:              ${prefix}
               sysconfdir:          ${sysconfdir}
               libdir:              ${libdir}
               includedir:          ${includedir}
               pkgconfigdir:        ${pkgconfigdir}
               ])
