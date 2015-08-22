#ifndef _UDF_MYSQL_QUIKDEFS_INCLUDED
#define _UDF_MYSQL_QUIKDEFS_INCLUDED

#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (UDF_MYSQL_QUIK_BCK_EXPORTS)
#				define UDF_MYSQL_QUIK_EXP __declspec(dllexport)
#			else
#				define UDF_MYSQL_QUIK_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define UDF_MYSQL_QUIK_EXP
#		endif		
#	endif




/* STANDARD is defined, don't use any mysql functions */
#ifdef STANDARD
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong;	/* Microsofts 64 bit types */
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else
#include <my_global.h>
#include <my_sys.h>

#endif
#include <mysql.h>
#include <ctype.h>

static pthread_mutex_t LOCK_hostname;




namespace AlgLib {




};


#endif // _UDF_MYSQL_QUIKDEFS_INCLUDED
