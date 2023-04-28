/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_zlog.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    28/04 2023 14:52
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        28/04 2023      create the file
 * 
 *     last modified: 28/04 2023 14:52
 */
#include <stdio.h>
#include <stdint.h>


#include "hy_log.h"

zlog_category_t *c;

void HyLogDeInit(void)
{
    zlog_fini();
}

int32_t HyLogInit(HyLogConfig_s *log_c)
{
    int32_t rc;

    if (!log_c->config_file) {
        printf("the config file name is NULL \n");
        return -1;
    }

    rc = zlog_init(log_c->config_file);
    if (rc) {
        printf("init failed\n");
        return -1;
    }

    c = zlog_get_category("my_cat");
    if (!c) {
        printf("get cat fail\n");
        zlog_fini();
        return -1;
    }

    return 0;
}
