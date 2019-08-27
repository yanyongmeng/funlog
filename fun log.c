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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define _ENABLE_FUNLOG_		// general switch
#include "funlog.h"

static int g_funlog_mode = 0;
static int s_funlog_level = 0;
static int s_sleep_duration = 0;
static FUNLOG_CALLBACK_PRINTF	s_Callback_Printf = NULL;
static FUNLOG_CALLBACK_SLEEP	s_Callback_Sleep = NULL;

/*-----------------------------------------------------------------------------------------------*/
#ifdef _LINUX_
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

static pthread_mutex_t s_funlog_mutex;
static void s_FunLog_InitLock(void)
{
	pthread_mutex_init(&s_funlog_mutex, NULL);
}
static void s_FunLog_DestroyLock(void)
{
	pthread_mutex_destroy(&s_funlog_mutex);
}
static void s_FunLog_Lock(void)
{
	pthread_mutex_lock(&s_funlog_mutex);
}
static void s_FunLog_Unlock(void)
{
	pthread_mutex_unlock(&s_funlog_mutex);
}
static void s_FunLog_Sleep(int ms)
{
	usleep(ms * 1000);
}
static int s_FunLog_AddHeadFlag(static const char *module_abbr, int line_number, char *buf)
{
	int ms, s, m, h;
	struct timeval time_x = {0,0};

	gettimeofday( &time_x, NULL );
	ms = time_x.tv_usec/1000;
	s  = time_x.tv_sec%60;
	m  = (time_x.tv_sec%3600)/60;
	h  = (time_x.tv_sec/3600)%24;
	return sprintf(buf, "[%-8s %02d:%02d:%02d:%03d  %08x  %4d]--", module_abbr, h, m, s, ms, (int)pthread_self(), line_number);
}
#endif

#ifdef _UCOS_
#endif

#ifdef _ECOS_
#endif

void FunLog_Printf(static const t_FUNLOG_FUNCTION *function,int line_number, const char* fmt, ...)
{
	va_list ap;
	int len, ret, duration = s_sleep_duration;
	static char buf[FUNLOG_BUFFER_SIZE] = "";
	t_FUNLOG_MODULE *module = function->PeekModule();

	s_FunLog_Lock();
	len = s_FunLog_AddHeadFlag(module->abbreviation, line_number, buf);
	va_start (ap, fmt);
	ret = vsnprintf (buf + len, FUNLOG_BUFFER_SIZE - 1 - len, fmt, ap);
	va_end (ap);
	if (ret > 0) 


	{
		len += ret;

		if(s_Callback_Printf)
			s_Callback_Printf(buf, len);
		else
			printf("%s", buf);
	}
	s_FunLog_Unlock();	
	
	if(s_Callback_Sleep)
	{
		s_Callback_Sleep(module->abbreviation, function->name, line_number, function->level);
	}
	else if(duration)
	{
		if( duration > 0 && duration < 1001 )
		{
			s_FunLog_Sleep(duration);
		}
		else if(duration < 0 && duration > -1001)
		{
			s_FunLog_Sleep(random()%(-duration));
		}
	}
		
	return (ret > 0);
}

int FunLog_Func_Start(static const t_FUNLOG_FUNCTION *function, const char *str, int x)
{
	int module_level;
	
	if(!g_funlog_mode)
		return 0;

	module_level = (function->PeekModule())->level;
	if(g_funlog_mode < 0)	// print all but exception
	{
		if(!(!module_level && (function->level & s_funlog_level)))
			return 0;
	}
	else	// print asigned
	{
		if(!(module_level & function->level))
			return 0;
	}

	char *s = (char *)str;
	FunLog_Printf(function, function->line_number, "%s start: %s %d.\n\r", function->name, s? s:"null", x );
	return 1;
}

void FunLog_Func_End(static const t_FUNLOG_FUNCTION *function, int line_number, const char *str, int x)
{
	char *s = (char *)str;
	FunLog_Printf(function, line_number, "%s   end: %s %d.\n\r", function->name, s? s:"null", x );
}

void FunLog_Func_Middle(static const t_FUNLOG_FUNCTION *function, int line_number, const char *str, int x)
{
	char *s = (char *)str;
	FunLog_Printf(function, line_number, "%s      : %s %d.\n\r", function->name, s? s:"null", x );
}

/*-----------------------------------------------------------------------------------------------*/
void FunLog_Initialize(FUNLOG_CALLBACK_PRINTF Printf, FUNLOG_CALLBACK_SLEEP Sleep)
{
	s_Callback_Printf = Printf;
	s_Callback_Sleep = Sleep;
	s_FunLog_InitLock();
}

void FunLog_Destroy(void)
{
	g_funlog_mode = 0;
	s_FunLog_DestroyLock();
}

void FunLog_EnableModule(FUNLOG_PEEK_MODULE PeekModule, const char *new_abbr, enum_FUNLOG_BIT level)
{
	t_FUNLOG_MODULE *module = PeekModule();

	if(new_abbr)
		module->abbreviation = new_abbr;
	if(level)
		module->level = level;
}

void FunLog_DisableModule(FUNLOG_PEEK_MODULE PeekModule)
{
	t_FUNLOG_MODULE *module = PeekModule();

	module->level = 0;
}

void FunLog_SetMode(int mode, enum_FUNLOG_BIT level)
{
	g_funlog_mode	= mode;
	s_funlog_level	= level;
}

void FunLog_SetSleepDuration(int milliseconds)
{
	s_sleep_duration = milliseconds;
}
