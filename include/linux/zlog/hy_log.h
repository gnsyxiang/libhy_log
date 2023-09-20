/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_log.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    28/04 2023 15:13
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        28/04 2023      create the file
 * 
 *     last modified: 28/04 2023 15:13
 */
#ifndef __LIBHY_LOG_INCLUDE_HY_LOG_H_
#define __LIBHY_LOG_INCLUDE_HY_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <zlog.h>

#include "hy_type.h"

extern zlog_category_t *HyGMyCategory;

/**
 * @brief 打印等级定义
 *
 * @note 数字越小越紧急
 */
typedef enum {
    HY_LOG_LEVEL_FATAL,                     ///< 致命错误，立刻停止程序
    HY_LOG_LEVEL_ERROR,                     ///< 错误，停止程序
    HY_LOG_LEVEL_WARN,                      ///< 警告
    HY_LOG_LEVEL_INFO,                      ///< 追踪，记录程序运行到哪里
    HY_LOG_LEVEL_DEBUG,                     ///< 调试程序相关打印
    HY_LOG_LEVEL_TRACE,                     ///< 程序打点调试

    HY_LOG_LEVEL_MAX = 0xffffffff,
} HyLogLevel_e;

/**
 * @brief 配置log输出的格式
 */
typedef enum {
    HY_LOG_OUTPUT_FORMAT_COLOR          = (0x1U << 0),  ///< 颜色输出
    HY_LOG_OUTPUT_FORMAT_LEVEL_INFO     = (0x1U << 1),  ///< 等级提示字母
    HY_LOG_OUTPUT_FORMAT_TIME           = (0x1U << 2),  ///< 时间输出
    HY_LOG_OUTPUT_FORMAT_PID_ID         = (0x1U << 3),  ///< 进程线程id输出
    HY_LOG_OUTPUT_FORMAT_FUNC_LINE      = (0x1U << 4),  ///< 函数行号输出
    HY_LOG_OUTPUT_FORMAT_USR_MSG        = (0x1U << 5),  ///< 用户数据输出
    HY_LOG_OUTPUT_FORMAT_COLOR_RESET    = (0x1U << 6),  ///< 颜色输出恢复
} HyLogOutputFormat_e;

/**
 * @brief 默认配置，输出所有格式
 */
#define HY_LOG_OUTFORMAT_ALL                    \
(HY_LOG_OUTPUT_FORMAT_COLOR                     \
    | HY_LOG_OUTPUT_FORMAT_LEVEL_INFO           \
    | HY_LOG_OUTPUT_FORMAT_TIME                 \
    | HY_LOG_OUTPUT_FORMAT_PID_ID               \
    | HY_LOG_OUTPUT_FORMAT_FUNC_LINE            \
    | HY_LOG_OUTPUT_FORMAT_USR_MSG              \
    | HY_LOG_OUTPUT_FORMAT_COLOR_RESET)

/**
 * @brief 默认配置去除线程进程id格式
 */
#define HY_LOG_OUTFORMAT_ALL_NO_PID_ID          \
(HY_LOG_OUTPUT_FORMAT_COLOR                     \
    | HY_LOG_OUTPUT_FORMAT_LEVEL_INFO           \
    | HY_LOG_OUTPUT_FORMAT_TIME                 \
    | HY_LOG_OUTPUT_FORMAT_FUNC_LINE            \
    | HY_LOG_OUTPUT_FORMAT_USR_MSG              \
    | HY_LOG_OUTPUT_FORMAT_COLOR_RESET)

/**
 * @brief 默认配置中去除颜色格式
 */
#define HY_LOG_OUTFORMAT_ALL_NO_COLOR           \
(HY_LOG_OUTPUT_FORMAT_LEVEL_INFO                \
    | HY_LOG_OUTPUT_FORMAT_TIME                 \
    | HY_LOG_OUTPUT_FORMAT_PID_ID               \
    | HY_LOG_OUTPUT_FORMAT_FUNC_LINE            \
    | HY_LOG_OUTPUT_FORMAT_USR_MSG)

/**
 * @brief 默认配置中去除颜色和线程进程id格式
 */
#define HY_LOG_OUTFORMAT_ALL_NO_COLOR_PID_ID    \
(HY_LOG_OUTPUT_FORMAT_COLOR                     \
    | HY_LOG_OUTPUT_FORMAT_LEVEL_INFO           \
    | HY_LOG_OUTPUT_FORMAT_TIME                 \
    | HY_LOG_OUTPUT_FORMAT_FUNC_LINE            \
    | HY_LOG_OUTPUT_FORMAT_USR_MSG              \
    | HY_LOG_OUTPUT_FORMAT_COLOR_RESET)

/**
 * @brief 配置参数
 */
typedef struct {
    HyLogLevel_e        level;              ///< 打印等级

    hy_u32_t            output_format;      ///< log输出格式
} HyLogSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyLogSaveConfig_s   save_c;             ///< 配置参数，详见HyLogSaveConfig_s

    hy_u32_t            fifo_len;           ///< fifo大小，异步方式用于保存log
    const char          *config_file;       ///< 配置文件路径
    hy_u16_t            port;               ///< 配置服务器端口号，等于0不开启网络服务
} HyLogConfig_s;

/**
 * @brief 初始化log模块
 *
 * @param log_c 配置参数，详见HyLogConfig_s
 *
 * @return 成功返回0，失败返回-1
 *
 * @note 使用该模块时，需要在上层忽略SIGPIPE信号，否则程序会退出
 */
hy_s32_t HyLogInit(HyLogConfig_s *log_c);

/**
 * @brief 初始化log模块宏
 *
 * @param _fifo_len fifo大小
 * @param _level 等级
 * @param _output_format log输出格式
 * @param _config_file 配置文件
 * @param _port 配置服务端端口号
 *
 * @return 成功返回0，失败返回-1
 */
#define HyLogInit_m(_fifo_len, _level, _output_format, _config_file, _port)     \
({                                                                              \
    HyLogConfig_s _log_c;                                                       \
    memset(&_log_c, '\0', sizeof(_log_c));                                      \
    _log_c.fifo_len             = _fifo_len;                                    \
    _log_c.config_file          = _config_file;                                 \
    _log_c.save_c.level         = _level;                                       \
    _log_c.save_c.output_format = _output_format;                               \
    _log_c.port                 = _port;                                        \
    HyLogInit(&_log_c);                                                         \
})

/**
 * @brief 销毁log模块
 */
void HyLogDeInit(void);

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   define LOGF(fmt, ...)  zlog_fatal(HyGMyCategory, fmt, ##__VA_ARGS__)
#   define LOGES(fmt, ...) zlog_error(HyGMyCategory, fmt, ##__VA_ARGS__)
#   define LOGE(fmt, ...)  zlog_error(HyGMyCategory, fmt, ##__VA_ARGS__)
#   define LOGW(fmt, ...)  zlog_warn(HyGMyCategory, fmt, ##__VA_ARGS__)
#   define LOGI(fmt, ...)  zlog_notice(HyGMyCategory, fmt, ##__VA_ARGS__)
#   define LOGD(fmt, ...)  zlog_info(HyGMyCategory, fmt, ##__VA_ARGS__)
#   define LOGT(fmt, ...)  zlog_debug(HyGMyCategory, fmt, ##__VA_ARGS__)
#else
#   define LOGF(fmt, ...)  zlog_fatal(HyGMyCategory, fmt, ##args)
#   define LOGES(fmt, ...) zlog_error(HyGMyCategory, fmt, ##args)
#   define LOGE(fmt, ...)  zlog_error(HyGMyCategory, fmt, ##args)
#   define LOGW(fmt, ...)  zlog_warn(HyGMyCategory, fmt, ##args)
#   define LOGI(fmt, ...)  zlog_notice(HyGMyCategory, fmt, ##args)
#   define LOGD(fmt, ...)  zlog_info(HyGMyCategory, fmt, ##args)
#   define LOGT(fmt, ...)  zlog_debug(HyGMyCategory, fmt, ##args)
#endif

#ifdef __cplusplus
}
#endif

#endif

