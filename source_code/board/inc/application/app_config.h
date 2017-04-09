#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/*
 * Log related configurations
*/

#define DEBUG_ENABLED

#ifdef DEBUG_ENABLED
#define LOG_DBG(x) printf x
#define LOG_INFO0(x) printf x
#else
#define LOG_DBG(x) do {} while (0)
#endif

#ifdef LOG_ENABLED
#ifndef LOG_INFO0
#define LOG_INFO0(x) printf x
#endif
#else
#ifndef LOG_INFO0
#define LOG_INFO0(x) do {} while (0)
#endif
#endif

#define LOG_INFO(x) printf x
#define LOG_ERR(x) printf x

/****************************************************/


#endif