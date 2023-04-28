dnl ===============================================================
dnl 
dnl Release under GPLv-3.0.
dnl 
dnl @file    check_libzlog.m4
dnl @brief   
dnl @author  gnsyxiang <gnsyxiang@163.com>
dnl @date    30/10 2021 14:41
dnl @version v0.0.1
dnl 
dnl @since    note
dnl @note     note
dnl 
dnl     change log:
dnl     NO.     Author              Date            Modified
dnl     00      zhenquan.qiu        30/10 2021      create the file
dnl 
dnl     last modified: 30/10 2021 14:41
dnl ===============================================================

# CHECK_ZLOG()
# --------------------------------------------------------------
# check libzlog

AC_DEFUN([CHECK_ZLOG],
    [
        AC_ARG_ENABLE([zlog],
            [AS_HELP_STRING([--enable-zlog], [enable support for zlog])],
            [],
            [enable_zlog=no])

        case "$enable_zlog" in
            yes)
                have_zlog=no

                case "$PKG_CONFIG" in
                    '') ;;
                    *)
                        ZLOG_LIBS=`$PKG_CONFIG --libs zlog 2>/dev/null`

                        case "$ZLOG_LIBS" in
                            '') ;;
                            *)
                                ZLOG_LIBS="$ZLOG_LIBS"
                                ZLOG_INCS=`$PKG_CONFIG --cflags zlog 2>/dev/null`
                                have_zlog=yes
                            ;;
                        esac

                    ;;
                esac

                case "$have_zlog" in
                    yes) ;;
                    *)
                        save_LIBS="$LIBS"
                        LIBS=""
                        ZLOG_LIBS=""

                        # clear cache
                        unset ac_cv_search_zlog_init
                        AC_SEARCH_LIBS([zlog_init],
                                [zlog],
                                [have_zlog=yes ZLOG_LIBS="$LIBS"],
                                [have_zlog=no],
                                [])
                        LIBS="$save_LIBS"
                    ;;
                esac

                CPPFLAGS_SAVE=$CPPFLAGS
                CPPFLAGS="$CPPFLAGS $ZLOG_INCS"
                AC_CHECK_HEADERS([zlog.h], [], [have_zlog=no])

                CPPFLAGS=$CPPFLAGS_SAVE
                AC_SUBST(ZLOG_INCS)
                AC_SUBST(ZLOG_LIBS)

                case "$have_zlog" in
                    yes)
                        AC_DEFINE(HAVE_ZLOG, 1, [Define if the system has zlog])
                    ;;
                    *)
                        AC_MSG_ERROR([zlog is a must but can not be found. You should add the \
directory containing `zlog.pc' to the `PKG_CONFIG_PATH' environment variable, \
or set `CPPFLAGS' and `LDFLAGS' directly for zlog, or use `--disable-zlog' \
to disable support for zlog encryption])
                    ;;
                esac
            ;;
        esac

        # check if we have and should use zlog
        AM_CONDITIONAL(COMPILE_ZLOG, [test "$enable_zlog" != "no" && test "$have_zlog" = "yes"])
    ])

