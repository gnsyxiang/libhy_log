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

SELECT_VENDER
SELECT_CHIP
# SELECT_PRODUCT

# check m4 macro
CHECK_PROJECT_INFO
CHECK_VERSION_INFO
CHECK_DOXYGEN
CHECK_DEBUG_RELEASE
# CHECK_JAVA
CHECK_PTHREAD

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
# AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create])

CHECK_LIBCUNIT

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h inttypes.h stdint.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit gettimeofday localtime_r memset select socket strerror strrchr])

AC_CONFIG_FILES([Makefile
                 $project_name.pc
                 build-script/Makefile
                 build-script/pc/Makefile
                 build-script/pc/ubuntu/Makefile
                 build-script/pc/windows/Makefile
                 build-script/rock-chips/Makefile
                 build-script/rock-chips/rk3568/Makefile
                 doc/Makefile
                 example/Makefile
                 example/linux/Makefile
                 example/mcu/Makefile
                 example/windows/Makefile
                 include/Makefile
                 include/linux/Makefile
                 include/mcu/Makefile
                 include/windows/Makefile
                 res/Makefile
                 src/Makefile
                 src/linux/Makefile
                 src/mcu/Makefile
                 src/windows/Makefile
                 test/Makefile
                 test/linux/Makefile
                 test/windows/Makefile])
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
