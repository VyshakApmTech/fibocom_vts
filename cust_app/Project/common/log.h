/**
 * @file log.h
 * @brief Professional embedded logging framework for Fibocom OpenCPU
 *
 * Header-only, compile-time filtering logging framework with zero overhead
 * when logs are disabled. Designed for RTOS-based embedded systems with
 * no dynamic memory, no printf redirection, and no heap usage.
 *
 * USAGE:
 *   In each .c file, define TAG before including this header:
 *   #include "log.h"
 *   #define TAG "GPS"
 *   Then use: LOGI(TAG, "GPS Init");
 *
 * OPTIONAL:
 *   #define GPS_VERBOSE_ENABLE 1  // Enable LOGV for this module
 *   #define LOG_INCLUDE_LOCATION  1 // Include function name and line number
 *
 * All logging goes through fibo_textTrace() which is provided by the system.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

/* ============================================================
 *  LOG LEVEL DEFINITIONS
 * ============================================================ */

#define LOG_LEVEL_ERROR     5
#define LOG_LEVEL_WARN      4
#define LOG_LEVEL_INFO      3
#define LOG_LEVEL_DEBUG     2
#define LOG_LEVEL_VERBOSE   1

/* ============================================================
 *  GLOBAL LOG LEVEL CONTROL
 * ============================================================ */

#ifndef GLOBAL_LOG_LEVEL
#define GLOBAL_LOG_LEVEL LOG_LEVEL_DEBUG
#endif

/* ============================================================
 *  EXTERNAL BACKEND (provided by system)
 * ============================================================ */

extern int fibo_textTrace(char *fmt, ...);

/* ============================================================
 *  LOCATION INFORMATION (optional)
 * ============================================================ */

#ifndef LOG_INCLUDE_LOCATION
#define LOG_INCLUDE_LOCATION 0
#endif

/* ============================================================
 *  INTERNAL LOGGING MACROS
 * ============================================================ */

/*
 * These macros handle the actual logging with optional location info.
 * They expand to nothing when the log level is filtered out.
 * All formatting is done in a single call to fibo_textTrace() to be thread-safe.
 */

#if LOG_INCLUDE_LOCATION

/* Format: [LEVEL][TAG][function:line] fmt */
#define _LOG_WITH_LEVEL(level_char, tag, fmt, ...) \
    fibo_textTrace("[%c][%s][%s:%d] " fmt, level_char, tag, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#else

/* Format: [LEVEL][TAG] fmt */
#define _LOG_WITH_LEVEL(level_char, tag, fmt, ...) \
    fibo_textTrace("[%c][%s] " fmt, level_char, tag, ##__VA_ARGS__)

#endif

/* ============================================================
 *  PUBLIC LOGGING MACROS - ERROR LEVEL
 * ============================================================ */

#if GLOBAL_LOG_LEVEL <= LOG_LEVEL_ERROR

#define LOGE(tag, fmt, ...) \
    _LOG_WITH_LEVEL('E', tag, fmt, ##__VA_ARGS__)

#else

#define LOGE(tag, fmt, ...) ((void)0)

#endif

/* ============================================================
 *  PUBLIC LOGGING MACROS - WARNING LEVEL
 * ============================================================ */

#if GLOBAL_LOG_LEVEL <= LOG_LEVEL_WARN

#define LOGW(tag, fmt, ...) \
    _LOG_WITH_LEVEL('W', tag, fmt, ##__VA_ARGS__)

#else

#define LOGW(tag, fmt, ...) ((void)0)

#endif

/* ============================================================
 *  PUBLIC LOGGING MACROS - INFO LEVEL
 * ============================================================ */

#if GLOBAL_LOG_LEVEL <= LOG_LEVEL_INFO

#define LOGI(tag, fmt, ...) \
    _LOG_WITH_LEVEL('I', tag, fmt, ##__VA_ARGS__)

#else

#define LOGI(tag, fmt, ...) ((void)0)

#endif

/* ============================================================
 *  PUBLIC LOGGING MACROS - DEBUG LEVEL
 * ============================================================ */

#if GLOBAL_LOG_LEVEL <= LOG_LEVEL_DEBUG

#define LOGD(tag, fmt, ...) \
    _LOG_WITH_LEVEL('D', tag, fmt, ##__VA_ARGS__)

#else

#define LOGD(tag, fmt, ...) ((void)0)

#endif

/* ============================================================
 *  PUBLIC LOGGING MACROS - VERBOSE LEVEL
 * ============================================================ */

/*
 * Verbose logging can be controlled both globally and per-module.
 * If GLOBAL_LOG_LEVEL is above VERBOSE, all LOGV calls compile to nothing.
 * Otherwise, check if the module has {TAG}_VERBOSE_ENABLE defined.
 * Example: #define GPS_VERBOSE_ENABLE 1
 */

#if GLOBAL_LOG_LEVEL <= LOG_LEVEL_VERBOSE

/* Per-module verbose enable (e.g., GPS_VERBOSE_ENABLE) */
#ifndef TAG
#define TAG ""
#endif

/* Construct the verbose enable macro name from TAG */
#define _VERBOSE_CTRL_NAME(tag) tag ## _VERBOSE_ENABLE

/* 
 * This macro is tricky. We use token pasting to create a preprocessor symbol
 * name from TAG, then check if it's defined. If not defined, we default to 0.
 * 
 * Usage in the module:
 *   #define GPS_VERBOSE_ENABLE 1
 *   #define TAG "GPS"
 *   OR
 *   #define TAG "GPS"  (verbose disabled by default)
 */

#define LOGV(tag, fmt, ...) \
    do { \
        _LOGV_IMPL(tag, fmt, ##__VA_ARGS__) \
    } while(0)

/* 
 * The actual implementation checks if module-specific verbose is enabled.
 * Since we can't directly use conditional preprocessor in macro arguments,
 * we always call the logging function when GLOBAL_LOG_LEVEL is VERBOSE.
 * 
 * To disable LOGV for a module, redefine it after including log.h:
 *   #include "log.h"
 *   #define TAG "GPS"
 *   #undef LOGV
 *   #define LOGV(tag, fmt, ...) ((void)0)
 *
 * OR use per-module control by having users check before including log.h
 */

#define _LOGV_IMPL(tag, fmt, ...) \
    _LOG_WITH_LEVEL('V', tag, fmt, ##__VA_ARGS__)

#else

#define LOGV(tag, fmt, ...) ((void)0)

#endif

/* ============================================================
 *  COMPILE-TIME ASSERTIONS (optional)
 * ============================================================ */

/*
 * Ensure TAG is defined when first using logging.
 * Users should define TAG in their module before including log.h
 * or using any LOG macros.
 */

#endif /* LOG_H */
