/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    uart.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/09 2023 14:50
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/09 2023      create the file
 * 
 *     last modified: 18/09 2023 14:50
 */
#ifndef __LIBHY_LOG_INCLUDE_UART_H_
#define __LIBHY_LOG_INCLUDE_UART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_type.h"

typedef struct {
    hy_s32_t            reserve;
} uart_save_config_s;

typedef struct {
    uart_save_config_s  save_c;
} uart_config_s;

void *uart_create(uart_config_s *uart_c);
void uart_destroy(void **handle_pp);

hy_s32_t uart_write(void *handle, char *buf, hy_u32_t len);

#ifdef __cplusplus
}
#endif

#endif

