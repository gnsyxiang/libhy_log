# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    05/03 2021 10:35
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        05/03 2021      create the file
# 
#     last modified: 05/03 2021 10:35
# ===============================================================

SUBDIRS =

noinst_LTLIBRARIES = lib@project_name@_@run_os@.la

if COMPILE_SELECT_LOG_TYPE_HY_LOG
SUBDIRS += hy_log
endif

if COMPILE_SELECT_LOG_TYPE_LOG4CPLUS
SUBDIRS += log4cplus
endif

if COMPILE_SELECT_LOG_TYPE_ZLOG
SUBDIRS += zlog
endif

# ------------------
# defining variables
# ------------------
_no_error_com   =
_define_com     =
_include_com    = $(include_com)
_sources_com    =
_cppflags_com   =
_cflags_com     =
_cxxflags_com   =
_ldflags_com    =
_ldadd_com      =

# ---------------------
# add necessary content
# ---------------------
_no_error_com   +=

_define_com     += -D_GNU_SOURCE

_include_com    += -I$(top_srcdir)/include

_sources_com    +=

_cppflags_com   += \
                   $(_no_error_com) \
                   $(_include_com)  \
                   $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldadd_com      += $(ldadd_com)

if COMPILE_SELECT_LOG_TYPE_HY_LOG
_ldadd_com      += $(top_builddir)/src/$(run_os)/hy_log/libhy_loger.la
endif

if COMPILE_SELECT_LOG_TYPE_LOG4CPLUS
_ldadd_com      += $(top_builddir)/src/$(run_os)/log4cplus/libhy_log4cplus.la
endif

if COMPILE_SELECT_LOG_TYPE_ZLOG
_ldadd_com      += $(top_builddir)/src/$(run_os)/zlog/libhy_zlog.la
endif

lib@project_name@_@run_os@_la_SOURCES    = $(_sources_com)
lib@project_name@_@run_os@_la_CPPFLAGS   = $(_cppflags_com)
lib@project_name@_@run_os@_la_CFLAGS     = $(_cflags_com)
lib@project_name@_@run_os@_la_CXXFLAGS   = $(_cxxflags_com)
lib@project_name@_@run_os@_la_LDFLAGS    = $(_ldflags_com)
lib@project_name@_@run_os@_la_LIBADD     = $(_ldadd_com)
