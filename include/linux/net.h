/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    net.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    19/09 2023 11:55
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        19/09 2023      create the file
 * 
 *     last modified: 19/09 2023 11:55
 */
#ifndef __LIBHY_LOG_INCLUDE_NET_H_
#define __LIBHY_LOG_INCLUDE_NET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_log.h"

typedef struct {
    hy_s32_t            reserve;
} net_save_config_s;

typedef struct {
    net_save_config_s   save_c;

    hy_u16_t            port;
} net_config_s;

void *net_create(net_config_s *net_c);
void net_destroy(void **handle_pp);

hy_s32_t net_write(void *handle, char *buf, hy_u32_t len);

#ifdef __cplusplus
}
#endif

#endif

