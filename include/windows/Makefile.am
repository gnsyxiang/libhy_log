# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    14/12 2023 16:46
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        14/12 2023      create the file
# 
#     last modified: 14/12 2023 16:46
# ===============================================================

# ------------------
# defining variables
# ------------------
_no_install_headers_com =
_install_headers_com    =

install_includedir      = $(includedir)/$(project_name)

# ---------------------
# add necessary content
# ---------------------
_no_install_headers_com += \
                           dynamic_array.h \
                           format_cb.h \
                           log.h \
                           log_fifo.h \
                           net.h \
                           thread_specific.h \
                           uart.h

_install_headers_com    += \
                           hy_log.h

# ------------------
# make install files
# ------------------
noinst_HEADERS          = $(_no_install_headers_com)
install_include_HEADERS = $(_install_headers_com)
