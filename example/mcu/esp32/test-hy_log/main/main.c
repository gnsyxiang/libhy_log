/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    main.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    06/01 2024 08:25
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        06/01 2024      create the file
 * 
 *     last modified: 06/01 2024 08:25
 */
#include <stdio.h>
#include <string.h>

#include <hy_log/hy_log.h>

void app_main(void)
{
    HyLogConfig_s log_c;
    memset(&log_c, 0, sizeof(log_c));
    log_c.fifo_len                  = 10 * 1024;
    log_c.save_c.level              = HY_LOG_LEVEL_TRACE;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL_NO_PID_ID;
    log_c.config_file               = "../res/hy_log/zlog.conf";
    log_c.port                      = 5000;

    if (0 != HyLogInit(&log_c)) {
        printf("HyLogInit error \n");
        return ;
    }

    LOGT("-1-hello log \n");
    LOGD("-2-hello log \n");
    LOGI("-3-hello log \n");
    LOGW("-4-hello log \n");
    LOGE("-5-hello log \n");
    LOGES("-6-hello log \n");
}
