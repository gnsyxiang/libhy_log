/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    process_handle_data.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    28/04 2022 08:41
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        28/04 2022      create the file
 * 
 *     last modified: 28/04 2022 08:41
 */
#ifndef __LIBHY_LOG_INCLUDE_PROCESS_HANDLE_DATA_H_
#define __LIBHY_LOG_INCLUDE_PROCESS_HANDLE_DATA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <stdint.h>

#include "log_fifo.h"

typedef void (*process_handle_data_cb_t)(char *buf, uint32_t len, void *args);

typedef struct process_handle_data_s process_handle_data_s;

process_handle_data_s *process_handle_data_create(const char *name,
                                                  uint32_t fifo_len,
                                                  process_handle_data_cb_t cb,
                                                  void *args);
void process_handle_data_destroy(process_handle_data_s **context_pp);

int32_t process_handle_data_write(process_handle_data_s *context,
                                  const void *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif

