/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    thread_specific_data.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    28/04 2022 10:38
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        28/04 2022      create the file
 * 
 *     last modified: 28/04 2022 10:38
 */
#ifndef __LIBHY_LOG_INCLUDE_THREAD_SPECIFIC_DATA_H_
#define __LIBHY_LOG_INCLUDE_THREAD_SPECIFIC_DATA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#include "hy_log.h"

typedef struct {
    pthread_key_t   thread_key;

    void *(*create_cb)(void);
    void (*destroy_cb)(void **handle_pp);
    void (*reset_cb)(void *handle);
} thread_specific_s;

hy_s32_t thread_specific_create(thread_specific_s *private_data_c);
void thread_specific_destroy(void);

void *thread_specific_fetch(void);

#ifdef __cplusplus
}
#endif

#endif

