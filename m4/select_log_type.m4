dnl ===============================================================
dnl 
dnl Release under GPLv-3.0.
dnl 
dnl @file    select_log_type.m4
dnl @brief   
dnl @author  gnsyxiang <gnsyxiang@163.com>
dnl @date    11/03 2022 15:19
dnl @version v0.0.1
dnl 
dnl @since    note
dnl @note     note
dnl 
dnl     change log:
dnl     NO.     Author              Date            Modified
dnl     00      zhenquan.qiu        11/03 2022      create the file
dnl 
dnl     last modified: 11/03 2022 15:19
dnl ===============================================================

# SELECT_LOG_TYPE()
# --------------------------------------------------------------
# select log_type

AC_DEFUN([SELECT_LOG_TYPE],
    [
        log_type=""

        AC_ARG_WITH([log_type],
            [AS_HELP_STRING([--with-log_type=@<:@hy_log|log4cplus@:>@], [select log_type about @<:@hy_log|log4cplus@:>@ @<:@default=hy_log@:>@])],
            [],
            [with_log_type=hy_log])

        case "$with_log_type" in
            hy_log)
                AC_DEFINE(HAVE_SELECT_LOG_TYPE_HY_LOG,  1, [select hy_log log_type])
                log_type="hy_log"
            ;;
            log4cplus)
                AC_DEFINE(HAVE_SELECT_LOG_TYPE_LOG4CPLUS,  1, [select log4cplus log_type])
                log_type="log4cplus"
            ;;
            *)
                AC_MSG_ERROR([bad value ${with_log_type} for --with-log_type=@<:@hy_log|log4cplus@:>@])
            ;;
        esac

        AC_SUBST(log_type)

        AM_CONDITIONAL([COMPILE_SELECT_LOG_TYPE_HY_LOG],            [test "x$with_log_type" = "xhy_log"])
        AM_CONDITIONAL([COMPILE_SELECT_LOG_TYPE_LOG4CPLUS],         [test "x$with_log_type" = "xlog4cplus"])
    ])

