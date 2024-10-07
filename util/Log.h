#ifndef LOG_H_
#define LOG_H_
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#define AV_ERROR_MAX_STRING_SIZE 64
typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel;
const char* get_log_level_str(LogLevel level);
const char* get_log_level_color(LogLevel level);
void log_generic(LogLevel level, const char *file, const char *func, int line, const char *format, va_list args);
void log_debug(const char *file, const char *func, int line, const char *format, ...);
void log_info(const char *file, const char *func, int line, const char *format, ...);
void log_warn(const char *file, const char *func, int line, const char *format, ...);
void log_error(const char *file, const char *func, int line, const char *format, ...);
#define LOG_DEBUG(format, ...) log_debug(__FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) log_info(__FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) log_warn(__FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) log_error(__FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
void SetLogDegree(LogLevel level);
#endif //LOG_H_
