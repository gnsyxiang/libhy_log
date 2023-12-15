/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_log.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    29/10 2021 20:29
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        29/10 2021      create the file
 * 
 *     last modified: 29/10 2021 20:29
 */
#ifndef __LIBHY_LOG_INCLUDE_HY_LOG_H_
#define __LIBHY_LOG_INCLUDE_HY_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "hy_type.h"

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
} HyLogLevel_e;

/**
 * @brief log相关信息
 */
typedef struct {
    HyLogLevel_e        level;              ///< 打印等级
    char                *err_str;           ///< 错误信息，由strerror(errno)提供
    const char          *file;              ///< 文件名，去掉了路径
    const char          *func;              ///< 函数名
    hy_u32_t            line;               ///< 行号
    pthread_t           tid;                ///< 线程id

    const char          *fmt;               ///< 用户格式
    va_list             *str_args;          ///< 用户信息
} HyLogAddiInfo_s;

/**
 * @brief 配置log输出的格式
 */
typedef enum {
    HY_LOG_OUTPUT_FORMAT_COLOR          = (0x1U << 0),  ///< 颜色输出
    HY_LOG_OUTPUT_FORMAT_LEVEL_INFO     = (0x1U << 1),  ///< 等级提示字母
    HY_LOG_OUTPUT_FORMAT_TIME           = (0x1U << 2),  ///< 时间输出
    HY_LOG_OUTPUT_FORMAT_FUNC_LINE      = (0x1U << 3),  ///< 函数行号输出
    HY_LOG_OUTPUT_FORMAT_USR_MSG        = (0x1U << 4),  ///< 用户数据输出
    HY_LOG_OUTPUT_FORMAT_COLOR_RESET    = (0x1U << 5),  ///< 颜色输出恢复
} HyLogOutputFormat_e;

/**
 * @brief 默认配置，输出所有格式
 */
#define HY_LOG_OUTFORMAT_ALL                    \
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

/**
 * @brief 获取打印等级
 *
 * @return 当前的打印等级
 */
HyLogLevel_e HyLogLevelGet(void);

/**
 * @brief 设置打印等级
 *
 * @param level 新的打印等级
 */
void HyLogLevelSet(HyLogLevel_e level);

/**
 * @brief log函数
 *
 * @param addi_info log相关信息，详见HyLogAddiInfo_s
 * @param fmt 用户输入的格式
 * @param ... 格式对应的数据
 */
void HyLogWrite(HyLogAddiInfo_s *addi_info, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

/*
 * 1，去掉文件路径，只获取文件名
 *  1.1，使用strrchr函数，包含头文件#include <string.h>
 *      #define HY_STRRCHR_FILE (strrchr(__FILE__, '/'))
 *      #define HY_FILENAME     (HY_STRRCHR_FILE ? (HY_STRRCHR_FILE + 1) : __FILE__)
 *  1.2，使用basename函数，包含头文件#include <libgen.h>
 *      basename(__FILE__)
 */
#define HY_STRRCHR_FILE (strrchr(__FILE__, '/'))
#define HY_FILENAME     (HY_STRRCHR_FILE ? (HY_STRRCHR_FILE + 1) : __FILE__)

/**
 * @brief log宏
 *
 * @param _level 该log对应的等级
 * @param _err_str 错误对应的字符串
 * @param fmt 用户输入的格式
 * @param ... 格式对应的数据
 */
#define LOG(_level, _err_str, _fmt, ...)                    \
do {                                                        \
    if (HyLogLevelGet() >= _level) {                        \
        HyLogAddiInfo_s addi_info;                          \
        addi_info.level     = _level;                       \
        addi_info.err_str   = _err_str;                     \
        addi_info.file      = HY_FILENAME;                  \
        addi_info.func      = __func__;                     \
        addi_info.line      = __LINE__;                     \
        addi_info.tid       = pthread_self();               \
        HyLogWrite(&addi_info, _fmt, ##__VA_ARGS__);        \
    }                                                       \
} while (0)

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   define LOGF(fmt, ...)  LOG(HY_LOG_LEVEL_FATAL, strerror(errno), fmt, ##__VA_ARGS__)
#   define LOGES(fmt, ...) LOG(HY_LOG_LEVEL_ERROR, strerror(errno), fmt, ##__VA_ARGS__)
#   define LOGE(fmt, ...)  LOG(HY_LOG_LEVEL_ERROR, NULL,            fmt, ##__VA_ARGS__)
#   define LOGW(fmt, ...)  LOG(HY_LOG_LEVEL_WARN,  NULL,            fmt, ##__VA_ARGS__)
#   define LOGI(fmt, ...)  LOG(HY_LOG_LEVEL_INFO,  NULL,            fmt, ##__VA_ARGS__)
#   define LOGD(fmt, ...)  LOG(HY_LOG_LEVEL_DEBUG, NULL,            fmt, ##__VA_ARGS__)
#   define LOGT(fmt, ...)  LOG(HY_LOG_LEVEL_TRACE, NULL,            fmt, ##__VA_ARGS__)
#else
#   define LOGF(fmt, args...)  LOG(HY_LOG_LEVEL_FATAL, strerror(errno), fmt, ##args)
#   define LOGES(fmt, args...) LOG(HY_LOG_LEVEL_ERROR, strerror(errno), fmt, ##args)
#   define LOGE(fmt, args...)  LOG(HY_LOG_LEVEL_ERROR, NULL,            fmt, ##args)
#   define LOGW(fmt, args...)  LOG(HY_LOG_LEVEL_WARN,  NULL,            fmt, ##args)
#   define LOGI(fmt, args...)  LOG(HY_LOG_LEVEL_INFO,  NULL,            fmt, ##args)
#   define LOGD(fmt, args...)  LOG(HY_LOG_LEVEL_DEBUG, NULL,            fmt, ##args)
#   define LOGT(fmt, args...)  LOG(HY_LOG_LEVEL_TRACE, NULL,            fmt, ##args)
#endif

#ifdef __cplusplus
}
#endif

#endif

