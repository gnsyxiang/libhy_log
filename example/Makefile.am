# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    05/03 2021 10:38
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        05/03 2021      create the file
# 
#     last modified: 05/03 2021 10:38
# ===============================================================

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
bin_PROGRAMS    =

# ---------------------
# add necessary content
# ---------------------
_no_error_com   +=

_define_com     +=

_include_com    += -I$(top_srcdir)/include

if COMPILE_SELECT_LOG_TYPE_HY_LOG
_include_com    += -I$(top_srcdir)/include/hy_log
endif

if COMPILE_SELECT_LOG_TYPE_LOG4CPLUS
_include_com    += -I$(top_srcdir)/include/log4cplus
endif

if COMPILE_SELECT_LOG_TYPE_ZLOG
_include_com    += -I$(top_srcdir)/include/zlog
endif

_sources_com    +=

_cppflags_com   += \
                   $(_no_error_com) \
                   $(_include_com)  \
                   $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldflags_com    +=

_ldadd_com      += \
                   $(top_builddir)/src/lib@project_name@.la \
                   $(ldadd_com)

bin_PROGRAMS            += hy_log_demo
hy_log_demo_SOURCES    = $(_sources_com) hy_log_demo.c
hy_log_demo_CPPFLAGS   = $(_cppflags_com)
hy_log_demo_CFLAGS     = $(_cflags_com)
hy_log_demo_CXXFLAGS   = $(_cxxflags_com)
hy_log_demo_LDFLAGS    = $(_ldflags_com)
hy_log_demo_LDADD      = $(_ldadd_com)

