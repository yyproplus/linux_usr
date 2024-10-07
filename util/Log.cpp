#include "Log.h"
extern "C"{
    #include "libavutil/error.h"
    #include "libavcodec/avcodec.h"
}

LogLevel current_log_level = LOG_LEVEL_DEBUG;

const char* get_log_level_str(LogLevel level) {
    switch (level) {
        case LOG_LEVEL_DEBUG: return "DEBUG";
        case LOG_LEVEL_INFO: return "INFO";
        case LOG_LEVEL_WARN: return "WARN";
        case LOG_LEVEL_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

const char* get_log_level_color(LogLevel level) {
    switch (level) {
        case LOG_LEVEL_DEBUG: return "\x1b[36m"; // 青色
        case LOG_LEVEL_INFO: return "\x1b[32m";  // 绿色
        case LOG_LEVEL_WARN: return "\x1b[33m";  // 黄色
        case LOG_LEVEL_ERROR: return "\x1b[31m"; // 红色
        default: return "\x1b[0m";               // 重置颜色
    }
}

void log_generic(LogLevel level, const char *file, const char *func, int line, const char *format, va_list args) {
    if (level < current_log_level) {
        return;
    }

    time_t rawtime;
    struct tm *timeinfo;
    char time_buffer[20];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_buffer, 20, "%Y-%m-%d %H:%M:%S", timeinfo);

    const char* color = get_log_level_color(level);
    const char* reset_color = "\x1b[0m";

    printf("%s[%s] [%s] [%s:%s:%d] ", color, time_buffer, get_log_level_str(level), file, func, line);

    vprintf(format, args);

    printf("%s\n", reset_color);
}

void log_debug(const char *file, const char *func, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_generic(LOG_LEVEL_DEBUG, file, func, line, format, args);
    va_end(args);
}

void log_info(const char *file, const char *func, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_generic(LOG_LEVEL_INFO, file, func, line, format, args);
    va_end(args);
}

void log_warn(const char *file, const char *func, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_generic(LOG_LEVEL_WARN, file, func, line, format, args);
    va_end(args);
}

void log_error(const char *file, const char *func, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_generic(LOG_LEVEL_ERROR, file, func, line, format, args);
    va_end(args);
}


void SetLogDegree(LogLevel level){
    current_log_level=level;
    return;
}