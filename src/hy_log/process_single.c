/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    process_single.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/04 2022 14:36
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/04 2022      create the file
 * 
 *     last modified: 21/04 2022 14:36
 */
#include <stdio.h>

#include "format_cb.h"
#include "log_private.h"
#include "process_handle_data.h"

#include "process_single.h"

struct process_single_s {
    process_handle_data_s   *terminal_handle_data;
};

void process_single_write(process_single_s *handle, log_write_info_s *log_write_info)
{
    HyLogAddiInfo_s *addi_info = log_write_info->addi_info;
    dynamic_array_s *dynamic_array = log_write_info->dynamic_array;

    for (uint32_t i = 0; i < log_write_info->format_cb_cnt; ++i) {
        if (log_write_info->format_cb[i]) {
            log_write_info->format_cb[i](dynamic_array, addi_info);
        }
    }

    process_handle_data_write(handle->terminal_handle_data,
                              dynamic_array->buf, dynamic_array->cur_len);
}

static void _process_handle_data_terminal_cb(void *buf, uint32_t len, void *args)
{
    printf("%s", (char *)buf);
}

void process_single_destroy(process_single_s **handle_pp)
{
    process_single_s *handle = *handle_pp;

    process_handle_data_destroy(&handle->terminal_handle_data);

    log_i("process single handle: %p destroy \n", handle);
    free(handle);
    *handle_pp = NULL;
}

process_single_s *process_single_create(uint32_t fifo_len)
{
    if (fifo_len <= 0) {
        log_e("the param is error \n");
        return NULL;
    }

    process_single_s *handle = NULL;

    do {
        handle = calloc(1, sizeof(*handle));
        if (!handle) {
            log_e("calloc failed \n");
            break;
        }

        handle->terminal_handle_data = process_handle_data_create("HY_log_loop",
                fifo_len, _process_handle_data_terminal_cb, handle);
        if (!handle->terminal_handle_data) {
            log_e("process_handle_data_create failed \n");
            break;
        }

        log_i("process single handle: %p create \n", handle);
        return handle;
    } while (0);

    log_e("process single create failed \n");
    process_single_destroy(&handle);
    return handle;
}
