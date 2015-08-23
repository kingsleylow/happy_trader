#ifndef _MTPROXY_MTPROXYCORE_INCLUDED
#define _MTPROXY_MTPROXYCORE_INCLUDED

#include "mtproxydefs.hpp"
#include "mtproxymanager.hpp"



// for polling queue
#define MTST_POLLING_NODATA -2
#define MTST_POLLING_OK -3
#define MTST_POLLING_OK_LAST -4
//

#define MTST_GENERAL_ERROR -1
#define MTST_NO_ERROR 0
#define MST_BOOL_TRUE 100
#define MST_BOOL_FALSE 200

#ifdef _DEBUG

#define MTPROXY_VERSION 2014052400

#else
#define MTPROXY_VERSION 20140524

#endif






extern "C" {


// write entries and apply exclusive lock to the file

MTPROXY_EXP int PROCCALL	getVersion();
MTPROXY_EXP int PROCCALL	initialize(int dieSec  = -1);
MTPROXY_EXP int PROCCALL	getTerminalUid(wchar_t* terminal_uid);
MTPROXY_EXP int PROCCALL saveFileContent(wchar_t const* file, wchar_t const* content);
MTPROXY_EXP void PROCCALL internallog(int const level, wchar_t const* content);
MTPROXY_EXP int PROCCALL getStringDataArray(int const hndl, char* buffer, int const bufferLength);
MTPROXY_EXP void PROCCALL setloglevel(int const level);


}

#endif