# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    02/02 2024 11:23
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        02/02 2024      create the file
# 
#     last modified: 02/02 2024 11:23
# ===============================================================

noinst_LTLIBRARIES = lib@vender@_@chip@.la

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

_define_com     +=

_include_com    += \
                   -I$(top_srcdir)/include/$(run_os)/$(vender)/$(chip) \
                   -I$(top_srcdir)/include/$(run_os) \
                   -I$(top_srcdir)/include

_sources_com    += \
                   hy_log.c

_cppflags_com   += \
                   $(_no_error_com) \
                   $(_include_com) \
                   $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldadd_com      += $(ldadd_com)

lib@vender@_@chip@_la_SOURCES   = $(_sources_com)
lib@vender@_@chip@_la_CPPFLAGS  = $(_cppflags_com)
lib@vender@_@chip@_la_CFLAGS    = $(_cflags_com)
lib@vender@_@chip@_la_CXXFLAGS  = $(_cxxflags_com)
lib@vender@_@chip@_la_LDFLAGS   = $(_ldflags_com)
lib@vender@_@chip@_la_LIBADD    = $(_ldadd_com)
