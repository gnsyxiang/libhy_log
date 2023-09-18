/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    uart.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/09 2023 14:59
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/09 2023      create the file
 * 
 *     last modified: 18/09 2023 14:59
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#include "uart.h"

typedef struct {
    uart_save_config_s  save_c;
} uart_s;

hy_s32_t uart_write(void *handle, char *buf, hy_u32_t len)
{
    assert(handle);
    assert(buf);

    printf("%s", buf);
    // dprintf(1, "%s", buf);

    return len;
}

void uart_destroy(void **handle_pp)
{
    uart_s *handle = *handle_pp;

    if (!handle_pp || !*handle_pp) {
        log_e("the param is NULL \n");
        return;
    }

    log_i("uart destroy, handle: %p \n", handle);
    free(handle);
    *handle_pp = NULL;
}

void *uart_create(uart_config_s *uart_c)
{
    uart_s *handle = NULL;

    if (!uart_c) {
        log_e("uart_c is NULL \n");
        return NULL;
    }

    do {
        handle = calloc(1, sizeof(uart_s));
        if (!handle) {
            log_e("calloc failed \n");
            break;
        }

        memcpy(&handle->save_c, &uart_c->save_c, sizeof(uart_save_config_s));

        log_i("uart create, handle: %p \n", handle);
        return handle;
    } while(0);

    log_e("uart create failed \n");
    uart_destroy((void **)&handle);
    return NULL;
}
