# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    14/12 2023 16:45
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        14/12 2023      create the file
# 
#     last modified: 14/12 2023 16:45
# ===============================================================

SUBDIRS =

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
_no_install_headers_com =
_install_headers_com    =

install_includedir      = $(includedir)/@project_name@

# ---------------------
# add necessary content
# ---------------------
_no_install_headers_com +=

_install_headers_com    +=

# ------------------
# make install files
# ------------------
noinst_HEADERS          = $(_no_install_headers_com)
install_include_HEADERS = $(_install_headers_com)
