/*-----------------------------------------------------------------------------------------------*/
/*
* Copyright (c) 2019 yanyongmeng. All rights reserved.
* 
*
*    filename:			funlog.h
*    author(s):		yanyongmeng@hotmail.com
*    version:			0.1
*    date:			2019/08/11
* History
*/
/*-----------------------------------------------------------------------------------------------*/

#ifndef _FUN_LOG_H_
#define _FUN_LOG_H_
#ifdef __cplusplus
extern "C" {
#endif

#ifdef	ENABLE_AVL_LOG		// if you want debug version please define ENABLE_AVL_LOG in Makefile; otherwise do nothing 
/*-----------------------------------------------------------------------------------------------*/
#define FUNLOG_BUFFER_SIZE			512

typedef void (*FUNLOG_CALLBACK_PRINTF)(char *str, int len);
typedef void (*FUNLOG_CALLBACK_SLEEP)(const char *module_abbr, const char *function_name, int line_number, enum_FUNLOG_BIT level);
typedef t_FUNLOG_MODULE* (*FUNLOG_PEEK_MODULE)(void);


typedef enum {
	FUNLOG_WARN			= (1 << 1),
	
	FUNLOG_API_FEW		= (1 << 2),
	FUNLOG_API_MANY		= (1 << 3),
	FUNLOG_API_MYRIAD	= (1 << 4),
	
	FUNLOG_IN_FEW		= (1 << 5),
	FUNLOG_IN_MANY 		= (1 << 6),
	FUNLOG_IN_MYRIAD	= (1 << 7),

	FUNLOG_CALLEE_FEW	= (1 << 8),
	FUNLOG_CALLEE_MANY 	= (1 << 9),
	FUNLOG_CALLEE_MYRIAD= (1 << 10),
}enum_FUNLOG_BIT;


typedef struct _funlog_module_profile_
{
	const char 				*abbreviation;
	enum_FUNLOG_BIT			level;
}t_FUNLOG_MODULE;

/** !!!!!!!!NOTE:
 * t_FUNLOG_FUNCTION is a private struct special for log system. Please do NOT directly use it in your code.
 */
typedef struct _funlog_function_profile
{
	const char 				*name;
	int						line_number;
	FUNLOG_PEEK_MODULE		*PeekModule;
	enum_FUNLOG_BIT			level;
}t_FUNLOG_FUNCTION;


/* Please use the 4 below macros. */
#define FUNLOG_FUNC_START(MODULE, LEVEL, str, x)  \
			//static const t_FUNLOG_FUNCTION _s_funlog_function_ = {__FUNCTION__, __LINE__, MODULE, LEVEL }; \
			const t_FUNLOG_FUNCTION _s_funlog_function_ = {__FUNCTION__, __LINE__, MODULE, LEVEL }; \
			int _funlog_is_valid_ = FunLog_Func_Start(&_s_funlog_function_, str, x);													\

#define FUNLOG_FUNC(str, x)	\
			if(_funlog_is_valid_) 				\
				FunLog_Func_Middle(&_s_funlog_function_, __LINE__, str, x)
			
#define FUNLOG_FUNC_END(str, x)	\
			int _funlog_end_only_once = 1;	/* this variable is just used to avoid 2 or more FUNLOG_FUNC_END, and will be removed when optimization */\
			if(_funlog_is_valid_ && _funlog_end_only_once)	\
				FunLog_Func_End(&_s_funlog_function_, __LINE__, str, x)		
#if 0
#define FUNLOG_PRINTF(MODULE, LEVEL, FmtStr...) \
			if(g_funlog_mode)	{						\
				const static t_FUNLOG_FUNCTION _s_s_funlog_function_ = {__FUNCTION__, __LINE__, MODULE, LEVEL}; \
				FunLog_Printf(&_s_s_funlog_function_, __LINE__, FmtStr);	\
			}
#endif			

void FunLog_Initialize(FUNLOG_CALLBACK_PRINTF Printf, FUNLOG_CALLBACK_SLEEP Sleep);
void FunLog_Destroy(void);
void FunLog_EnableModule(FUNLOG_PEEK_MODULE PeekModule, const char *new_name, enum_FUNLOG_BIT level);
void FunLog_DisableModule(FUNLOG_PEEK_MODULE PeekModule);
void FunLog_SetMode(int mode, enum_FUNLOG_BIT level);
void FunLog_SetSleepDuration(int milliseconds);


/* Please do NOT call the 4 below APIs in your code, calling their corresponding macro is best */
void FunLog_Printf(static const t_FUNLOG_FUNCTION *function, int line_number, const char* fmt, ...);
int  FunLog_Func_Start(static const t_FUNLOG_FUNCTION *function, const char *str, int x);
void FunLog_Func_End(static const t_FUNLOG_FUNCTION *function, int line_number, const char *str, int x);
void FunLog_Func_Middle(static const t_FUNLOG_FUNCTION *function, int line_number, const char *str, int x);
/*-----------------------------------------------------------------------------------------------*/

#else
/*-----------------------------------------------------------------------------------------------*/
#define FunLog_Initialize(func)						{}
#define FunLog_EnableModule(module)					{}
#define FunLog_EnableLevel(level)						{}
#define FunLog_SetRemoteOutput(ip, port)				{}
#define FunLog_SetLocalFileOutput(logfile)				{}
#define FunLog_SetDelay(milliseconds)					{}

#define FUNLOG_PRINTF(MODULE, LEVEL, FmtStr...)		{}	
#define FUNLOG_FUNC_START(MODULE, LEVEL, str, x)		{}
#define FUNLOG_FUNC_END(str, x)						{}
#define FUNLOG_FUNC(str, x)							{}
/*-----------------------------------------------------------------------------------------------*/
#endif


#ifdef __cplusplus
}
#endif
#endif /* _FUN_LOG_H_ */

