/**  
 \file LogLibrary.cpp
 \brief Implementation of Debug Utilities. 
 */
#include "LogLibrary.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define DBG_BUF_LEN  2048          /*!< Internal buffer. Determines max length of debug messages*/

static TDbgFuncW wc_dbg_func = NULL;   /*!< Debug function using wide characters  */
static TDbgFunc dbg_func = NULL;      /*!< Debug function using normal characters */
static TDbgFuncW wc_log_func = NULL;   /*!< Debug function using wide characters  */
static TDbgFunc log_func = NULL;      /*!< Debug function using normal characters */


static wchar_t  wc_buffer[DBG_BUF_LEN]; /*!< Internal buffer for messages with wide characters */
static char buffer[DBG_BUF_LEN];  /*!< Internal buffer for messages*/


void  wDbgPrintf(const wchar_t *s, ...)
{

	if (wc_dbg_func != NULL){
		va_list va; 
		va_start(va, s);
		vswprintf(wc_buffer, DBG_BUF_LEN, s,va);
		va_end(va); 
		wc_dbg_func(wc_buffer);
	}
}


void  DbgPrintf(const char *s, ...)
{

	if (dbg_func != NULL){
		va_list va; 
		va_start(va, s);
		vsprintf_s(buffer, DBG_BUF_LEN, s,va);
		va_end(va); 
		dbg_func(buffer);
	}
}



void SetDbgFunc(TDbgFunc p)
{
	dbg_func = p;
}



void SetDbgFuncW(TDbgFuncW p)
{
	wc_dbg_func = p;
}


void SetLogFunc(TDbgFunc p)
{
	log_func = p;
}



void SetLogFuncW(TDbgFuncW p)
{
	wc_log_func = p;
}


void  wLogPrintf(const wchar_t *s, ...)
{

	if (wc_log_func != NULL){
		va_list va; 
		va_start(va, s);
		vswprintf(wc_buffer, DBG_BUF_LEN, s,va);
		va_end(va); 
		wc_log_func(wc_buffer);
	}
}


void  LogPrintf(const char *s, ...)
{

	if (log_func != NULL){
		va_list va; 
		va_start(va, s);
		vsprintf_s(buffer, DBG_BUF_LEN, s,va);
		va_end(va); 
		log_func(buffer);
	}
}

