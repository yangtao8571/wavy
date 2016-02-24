#ifndef __MY_DEBUG_H__
#define __MY_DEBUG_H__

/**
 * 类似于楼层,每个类(.c文件)站在其楼层向上看,能看到所有上层
 */

#define MY_LOG_LEVEL_INFO	4
#define MY_LOG_LEVEL_DEBUG	2

#define DEBUGFMT  "%s(%s():%d): "
#define DEBUGARGS __FILE__,__FUNCTION__,__LINE__

#if MY_LOG_LEVEL <= MY_LOG_LEVEL_DEBUG
	#include <glib.h>
	#define my_debug(fmt,...) g_debug(DEBUGFMT fmt, DEBUGARGS, ##__VA_ARGS__)
#else
	#define my_debug(fmt,...)  
#endif

// WARNING 暂时代替 info，因为2.24没有g_info函数
#if MY_LOG_LEVEL <= MY_LOG_LEVEL_INFO
	#include <glib.h>
	#define my_info(fmt,...) g_warning(DEBUGFMT fmt, DEBUGARGS, ##__VA_ARGS__)
#else
	#define my_info(fmt,...)  
#endif

#endif /* __MY_DEBUG_H__ */