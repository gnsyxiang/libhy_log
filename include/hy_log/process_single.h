/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    process_single.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/04 2022 14:33
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/04 2022      create the file
 * 
 *     last modified: 21/04 2022 14:33
 */
#ifndef __LIBHY_LOG_INCLUDE_PROCESS_SINGLE_H_
#define __LIBHY_LOG_INCLUDE_PROCESS_SINGLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct log_write_info_tag;

typedef struct process_single_s process_single_s;

process_single_s *process_single_create(uint32_t fifo_len);
void process_single_destroy(process_single_s **handle_pp);

void process_single_write(process_single_s *handle, struct log_write_info_tag *log_write_info);

#ifdef __cplusplus
}
#endif

#endif

