// Implements system dependent stuff for test framework.
#include "testsysdep.hpp"

#pragma comment( lib, "dbghelp.lib" )

#if defined (_WIN32)
//=============================================================
// Windows
//=============================================================
#include <Windows.h>
#include <math.h>
#include <float.h>
#include <winbase.h>
#include <eh.h>
#include <time.h>

#include <tchar.h>


// MS VC specific
//
#if defined (_MSC_VER)

#include <crtdbg.h>
#include <stdio.h>
#include <sys/timeb.h>

#include "testexcept.hpp"
#include "testmonitor.hpp"

#include "Dbghelp.h"
//=============================================================
// Helper function and classes to create crash information
//=============================================================

//=============================================================
// Windows version
//=============================================================

#define WUNKNOWNSTR	_T("unknown Windows version")

#define W95STR			_T("Windows 95")
#define W95SP1STR		_T("Windows 95 SP1")
#define W95OSR2STR		_T("Windows 95 OSR2")
#define W98STR			_T("Windows 98")
#define W98SP1STR		_T("Windows 98 SP1")
#define W98SESTR		_T("Windows 98 SE")
#define WMESTR			_T("Windows ME")

#define WNT351STR		_T("Windows NT 3.51")
#define WNT4STR			_T("Windows NT 4")
#define W2KSTR			_T("Windows 2000")
#define WXPSTR			_T("Windows XP")
#define W2003SERVERSTR	_T("Windows 2003 Server")
#define WVISTASTR			_T("Windows Vista")
#define W7STR					_T("Windows 7")
#define W2008STR			_T("Windows 2008")
#define W2008R2STR		_T("Windows 2008 R2")

#define WCESTR			_T("Windows CE")

// ----------------------------------------------------------


#define WUNKNOWN	0

#define W9XFIRST	1
#define W95			1
#define W95SP1		2
#define W95OSR2		3
#define W98			4
#define W98SP1		5
#define W98SE		6
#define WME			7
#define W9XLAST		99

#define WNTFIRST	101
#define WNT351		101
#define WNT4		102
#define W2K			103
#define WXP			104
#define W2003SERVER	105
#define WNTLAST		199
#define WVISTA	106
#define W7	107
#define W2008	108
#define W2008R2	106



#define WCEFIRST	201
#define WCE			201
#define WCELAST		299

#ifndef VER_PLATFORM_WIN32s
#define VER_PLATFORM_WIN32s             0
#endif
#ifndef VER_PLATFORM_WIN32_WINDOWS
#define VER_PLATFORM_WIN32_WINDOWS      1
#endif
#ifndef VER_PLATFORM_WIN32_NT
#define VER_PLATFORM_WIN32_NT           2
#endif
#ifndef VER_PLATFORM_WIN32_CE
#define VER_PLATFORM_WIN32_CE           3
#endif


/*
    This table has been assembled from Usenet postings, personal
    observations, and reading other people's code.  Please feel
    free to add to it or correct it.


         dwPlatFormID  dwMajorVersion  dwMinorVersion  dwBuildNumber
95             1              4               0             950
95 SP1         1              4               0        >950 && <=1080
95 OSR2        1              4             <10           >1080
98             1              4              10            1998
98 SP1         1              4              10       >1998 && <2183
98 SE          1              4              10          >=2183
ME             1              4              90            3000

NT 3.51        2              3              51
NT 4           2              4               0            1381
2000           2              5               0            2195
XP             2              5               1            2600
2003 Server    2              5               2            3790

CE             3

*/

//===============================================================
// Mutex at file scope for locking

CRITICAL_SECTION * getMutex (
	)
{
	static CRITICAL_SECTION mutex_s;

#if(_WIN32_WINNT >= 0x0403)
	static DWORD scount = 4000 | 0x80000000;
	static bool initialized_s = ( InitializeCriticalSectionAndSpinCount( &mutex_s, scount ), true );
#else
	static bool initialized_s = ( InitializeCriticalSection( &mutex_s ), true );
#endif

	return &mutex_s;
}


///////////////////////////////////////////////////////////////////////////////
// GetWinVer
BOOL GetWinVer(LPTSTR pszVersion, int *nVersion, LPTSTR pszMajorMinorBuild)
{
	if (!pszVersion || !nVersion || !pszMajorMinorBuild)
		return FALSE;
	lstrcpy(pszVersion, WUNKNOWNSTR);
	*nVersion = WUNKNOWN;

	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (!GetVersionEx(&osinfo))
		return FALSE;

	DWORD dwPlatformId   = osinfo.dwPlatformId;
	DWORD dwMinorVersion = osinfo.dwMinorVersion;
	DWORD dwMajorVersion = osinfo.dwMajorVersion;
	DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF;	// Win 95 needs this

	wsprintf(pszMajorMinorBuild, _T("%u.%u.%u"), dwMajorVersion, dwMinorVersion, dwBuildNumber);

	if ((dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (dwMajorVersion == 4))
	{
		if ((dwMinorVersion < 10) && (dwBuildNumber == 950))
		{
			lstrcpy(pszVersion, W95STR);
			*nVersion = W95;
		}
		else if ((dwMinorVersion < 10) && 
				((dwBuildNumber > 950) && (dwBuildNumber <= 1080)))
		{
			lstrcpy(pszVersion, W95SP1STR);
			*nVersion = W95SP1;
		}
		else if ((dwMinorVersion < 10) && (dwBuildNumber > 1080))
		{
			lstrcpy(pszVersion, W95OSR2STR);
			*nVersion = W95OSR2;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber == 1998))
		{
			lstrcpy(pszVersion, W98STR);
			*nVersion = W98;
		}
		else if ((dwMinorVersion == 10) && 
				((dwBuildNumber > 1998) && (dwBuildNumber < 2183)))
		{
			lstrcpy(pszVersion, W98SP1STR);
			*nVersion = W98SP1;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber >= 2183))
		{
			lstrcpy(pszVersion, W98SESTR);
			*nVersion = W98SE;
		}
		else if (dwMinorVersion == 90)
		{
			lstrcpy(pszVersion, WMESTR);
			*nVersion = WME;
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if ((dwMajorVersion == 3) && (dwMinorVersion == 51))
		{
			lstrcpy(pszVersion, WNT351STR);
			*nVersion = WNT351;
		}
		else if ((dwMajorVersion == 4) && (dwMinorVersion == 0))
		{
			lstrcpy(pszVersion, WNT4STR);
			*nVersion = WNT4;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 0))
		{
			lstrcpy(pszVersion, W2KSTR);
			*nVersion = W2K;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 1))
		{
			lstrcpy(pszVersion, WXPSTR);
			*nVersion = WXP;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 2))
		{
			lstrcpy(pszVersion, W2003SERVERSTR);
			*nVersion = W2003SERVER;
		}
		//
		else if ((dwMajorVersion == 6) && (dwMinorVersion == 0))
		{
			// extended info
			OSVERSIONINFOEX osinfo_ex;
			ZeroMemory(&osinfo_ex, sizeof(OSVERSIONINFOEX));
			osinfo_ex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

			if ( GetVersionEx ((OSVERSIONINFO *) &osinfo_ex)) {
				if (osinfo_ex.wProductType == VER_NT_WORKSTATION) {
					lstrcpy(pszVersion, WVISTASTR);
					*nVersion = WVISTA;
				}
				else {
					lstrcpy(pszVersion, W2008STR);
					*nVersion = W2008;
				}
			}
		}
		else if ((dwMajorVersion == 6) && (dwMinorVersion == 1))
		{
			// extended info
			OSVERSIONINFOEX osinfo_ex;
			ZeroMemory(&osinfo_ex, sizeof(OSVERSIONINFOEX));
			osinfo_ex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

			if ( GetVersionEx ((OSVERSIONINFO *) &osinfo_ex)) {
				if (osinfo_ex.wProductType == VER_NT_WORKSTATION) {
					lstrcpy(pszVersion, W7STR);
					*nVersion = W7;
				}
				else {
					lstrcpy(pszVersion, W2008R2STR);
					*nVersion = W2008R2;
				}
			}
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_CE)
	{
		lstrcpy(pszVersion, WCESTR);
		*nVersion = WCE;
	}
	return TRUE;
}

//=============================================================
// Mini version
//=============================================================

class CMiniVersion
{
// constructors
public:
	CMiniVersion(LPCTSTR lpszPath = NULL)
	{
		ZeroMemory(m_szPath, sizeof(m_szPath));

		if (lpszPath && lpszPath[0] != 0)
		{
			lstrcpyn(m_szPath, lpszPath, sizeof(m_szPath)-1);
		}
		else
		{
		}

		m_pData = NULL;
		m_dwHandle = 0;

		for (int i = 0; i < 4; i++)
		{
			m_wFileVersion[i] = 0;
			m_wProductVersion[i] = 0;
		}

		m_dwFileFlags = 0;
		m_dwFileOS = 0;
		m_dwFileType = 0;
		m_dwFileSubtype = 0;

		ZeroMemory(m_szCompanyName, sizeof(m_szCompanyName));
		ZeroMemory(m_szProductName, sizeof(m_szProductName));
		ZeroMemory(m_szFileDescription, sizeof(m_szFileDescription));

		Init();
	}
	
	BOOL Init()
	{
		DWORD dwHandle;
		DWORD dwSize;
		BOOL rc;

		dwSize = ::GetFileVersionInfoSize((LPSTR)(LPCTSTR)m_szPath, &dwHandle);
		if (dwSize == 0)
			return FALSE;
			
		m_pData = new BYTE [dwSize + 1];	
		ZeroMemory(m_pData, dwSize+1);

		rc = ::GetFileVersionInfo((LPSTR)(LPCTSTR)m_szPath, dwHandle, dwSize, m_pData);
		if (!rc)
			return FALSE;

		// get fixed info

		VS_FIXEDFILEINFO FixedInfo;
		
		if (GetFixedInfo(FixedInfo))
		{
			m_wFileVersion[0] = HIWORD(FixedInfo.dwFileVersionMS);
			m_wFileVersion[1] = LOWORD(FixedInfo.dwFileVersionMS);
			m_wFileVersion[2] = HIWORD(FixedInfo.dwFileVersionLS);
			m_wFileVersion[3] = LOWORD(FixedInfo.dwFileVersionLS);

			m_wProductVersion[0] = HIWORD(FixedInfo.dwProductVersionMS);
			m_wProductVersion[1] = LOWORD(FixedInfo.dwProductVersionMS);
			m_wProductVersion[2] = HIWORD(FixedInfo.dwProductVersionLS);
			m_wProductVersion[3] = LOWORD(FixedInfo.dwProductVersionLS);

			m_dwFileFlags   = FixedInfo.dwFileFlags;
			m_dwFileOS      = FixedInfo.dwFileOS;
			m_dwFileType    = FixedInfo.dwFileType;
			m_dwFileSubtype = FixedInfo.dwFileSubtype;
		}
		else
			return FALSE;

		// get string info

		GetStringInfo(_T("CompanyName"),     m_szCompanyName);
		GetStringInfo(_T("FileDescription"), m_szFileDescription);
		GetStringInfo(_T("ProductName"),     m_szProductName);

		return TRUE;		
	}
	void Release()
	{
		// do this manually, because we can't use objects requiring
		// a dtor within an exception handler
		if (m_pData)
			delete [] m_pData;
		m_pData = NULL;
	}

// operations
public:

// attributes
public:
	// fixed info
	BOOL GetFileVersion(WORD *pwVersion)
	{
		for (int i = 0; i < 4; i++)
			*pwVersion++ = m_wFileVersion[i];
		return TRUE;
	}
	BOOL GetProductVersion(WORD* pwVersion)
	{
		for (int i = 0; i < 4; i++)
			*pwVersion++ = m_wProductVersion[i];
		return TRUE;
	}
	BOOL GetFileFlags(DWORD& rdwFlags)
	{
		rdwFlags = m_dwFileFlags;
		return TRUE;
	}

	BOOL GetFileOS(DWORD& rdwOS)
	{
		rdwOS = m_dwFileOS;
		return TRUE;
	}
	BOOL GetFileType(DWORD& rdwType)
	{
		rdwType = m_dwFileType;
		return TRUE;
	}

	BOOL GetFileSubtype(DWORD& rdwType)
	{
		rdwType = m_dwFileSubtype;
		return TRUE;
	}

	// string info
	BOOL GetCompanyName(LPTSTR lpszCompanyName, int nSize)
	{
		if (!lpszCompanyName)
			return FALSE;
		ZeroMemory(lpszCompanyName, nSize);
		lstrcpyn(lpszCompanyName, m_szCompanyName, nSize-1);
		return TRUE;
	}

	BOOL GetFileDescription(LPTSTR lpszFileDescription, int nSize)
	{
		if (!lpszFileDescription)
			return FALSE;
		ZeroMemory(lpszFileDescription, nSize);
		lstrcpyn(lpszFileDescription, m_szFileDescription, nSize-1);
		return TRUE;
	}

	BOOL GetProductName(LPTSTR lpszProductName, int nSize)
	{
		if (!lpszProductName)
			return FALSE;
		ZeroMemory(lpszProductName, nSize);
		lstrcpyn(lpszProductName, m_szProductName, nSize-1);
		return TRUE;
	}

// implementation
protected:
	BOOL GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo)
	{
		BOOL rc;
		UINT nLength;
		VS_FIXEDFILEINFO *pFixedInfo = NULL;

		if (!m_pData)
			return FALSE;

		if (m_pData)
			rc = ::VerQueryValue(m_pData, _T("\\"), (void **) &pFixedInfo, &nLength);
		else
			rc = FALSE;
			
		if (rc)
			memcpy (&rFixedInfo, pFixedInfo, sizeof (VS_FIXEDFILEINFO));	

		return rc;
	}
	BOOL GetStringInfo(LPCSTR lpszKey, LPTSTR lpszReturnValue)
	{
		BOOL rc;
		DWORD *pdwTranslation;
		UINT nLength;
		LPTSTR lpszValue;
		
		if (m_pData == NULL)
			return FALSE;

		if (!lpszReturnValue)
			return FALSE;

		if (!lpszKey)
			return FALSE;

		*lpszReturnValue = 0;

		rc = ::VerQueryValue(m_pData, _T("\\VarFileInfo\\Translation"), 
									(void**) &pdwTranslation, &nLength);
		if (!rc)
			return FALSE;

		char szKey[2000];
		wsprintf(szKey, _T("\\StringFileInfo\\%04x%04x\\%s"),
					LOWORD (*pdwTranslation), HIWORD (*pdwTranslation),
					lpszKey);

		rc = ::VerQueryValue(m_pData, szKey, (void**) &lpszValue, &nLength);

		if (!rc)
			return FALSE;
			
		lstrcpy(lpszReturnValue, lpszValue);

		return TRUE;
	}

	BYTE*		m_pData;
	DWORD		m_dwHandle;
	WORD		m_wFileVersion[4];
	WORD		m_wProductVersion[4];
	DWORD		m_dwFileFlags;
	DWORD		m_dwFileOS;
	DWORD		m_dwFileType;
	DWORD		m_dwFileSubtype;

	TCHAR		m_szPath[MAX_PATH*2];
	TCHAR		m_szCompanyName[MAX_PATH*2];
	TCHAR		m_szProductName[MAX_PATH*2];
	TCHAR		m_szFileDescription[MAX_PATH*2];
};


//=============================================================
// Exception info generating routines
//=============================================================

#pragma warning(disable : 4514)
#pragma warning(disable : 4201)

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

const int NumCodeBytes = 16;	// Number of code bytes to record.
const int MaxStackDump = 3072;	// Maximum number of DWORDS in stack dumps.
const int StackColumns = 4;		// Number of columns in stack dump.

#define	ONEK			1024
#define	SIXTYFOURK		(64*ONEK)
#define	ONEM			(ONEK*ONEK)
#define	ONEG			(ONEK*ONEK*ONEK)


///////////////////////////////////////////////////////////////////////////////
// lstrrchr (avoid the C Runtime )
static TCHAR * lstrrchr(LPCTSTR string, int ch)
{
	TCHAR *start = (TCHAR *)string;

	while (*string++)                       /* find end of string */
		;
											/* search towards front */
	while (--string != start && *string != (TCHAR) ch)
		;

	if (*string == (TCHAR) ch)                /* char found ? */
		return (TCHAR *)string;

	return NULL;
}


// hprintf behaves similarly to printf, with a few vital differences.
// It uses wvsprintf to do the formatting, which is a system routine,
// thus avoiding C run time interactions. For similar reasons it
// uses WriteFile rather than fwrite.
// The one limitation that this imposes is that wvsprintf, and
// therefore hprintf, cannot handle floating point numbers.

// Too many calls to WriteFile can take a long time, causing
// confusing delays when programs crash. Therefore I implemented
// a simple buffering scheme for hprintf

#define HPRINTF_BUFFER_SIZE (8*1024)				// must be at least 2048
static TCHAR hprintf_buffer[HPRINTF_BUFFER_SIZE];	// wvsprintf never prints more than one K.
static int  hprintf_index = 0;

///////////////////////////////////////////////////////////////////////////////
// hflush
static void hflush(HANDLE LogFile)
{
	if (hprintf_index > 0)
	{
		DWORD NumBytes;
		WriteFile(LogFile, hprintf_buffer, lstrlen(hprintf_buffer), &NumBytes, 0);
		hprintf_index = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
// hprintf
static void hprintf(HANDLE LogFile, LPCTSTR Format, ...)
{
	if (hprintf_index > (HPRINTF_BUFFER_SIZE-1024))
	{
		DWORD NumBytes;
		WriteFile(LogFile, hprintf_buffer, lstrlen(hprintf_buffer), &NumBytes, 0);
		hprintf_index = 0;
	}

	va_list arglist;
	va_start( arglist, Format);
	hprintf_index += wvsprintf(&hprintf_buffer[hprintf_index], Format, arglist);
	va_end( arglist);
}

///////////////////////////////////////////////////////////////////////////////
// FormatTime
// Format the specified FILETIME to output in a human readable format,
// without using the C run time.
static void FormatTime(LPTSTR output, FILETIME TimeToPrint)
{
	output[0] = _T('\0');
	WORD Date, Time;
	if (FileTimeToLocalFileTime(&TimeToPrint, &TimeToPrint) &&
				FileTimeToDosDateTime(&TimeToPrint, &Date, &Time))
	{
		wsprintf(output, _T("%d/%d/%d %02d:%02d:%02d"),
					(Date / 32) & 15, Date & 31, (Date / 512) + 1980,
					(Time >> 11), (Time >> 5) & 0x3F, (Time & 0x1F) * 2);
	}
}

///////////////////////////////////////////////////////////////////////////////
// DumpModuleInfo
// Print information about a code module (DLL or EXE) such as its size,
// location, time stamp, etc.
static bool DumpModuleInfo(HANDLE LogFile, HINSTANCE ModuleHandle, int nModuleNo)
{
	bool rc = false;
	TCHAR szModName[MAX_PATH*2];
	ZeroMemory(szModName, sizeof(szModName));

	__try
	{
		if (GetModuleFileName(ModuleHandle, szModName, sizeof(szModName)-2) > 0)
		{
			// If GetModuleFileName returns greater than zero then this must
			// be a valid code module address. Therefore we can try to walk
			// our way through its structures to find the link time stamp.
			IMAGE_DOS_HEADER *DosHeader = (IMAGE_DOS_HEADER*)ModuleHandle;
		    if (IMAGE_DOS_SIGNATURE != DosHeader->e_magic)
	    	    return false;

			IMAGE_NT_HEADERS *NTHeader = (IMAGE_NT_HEADERS*)((TCHAR *)DosHeader
						+ DosHeader->e_lfanew);
		    if (IMAGE_NT_SIGNATURE != NTHeader->Signature)
	    	    return false;

			// open the code module file so that we can get its file date and size
			HANDLE ModuleFile = CreateFile(szModName, GENERIC_READ,
						FILE_SHARE_READ, 0, OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL, 0);

			TCHAR TimeBuffer[100];
			TimeBuffer[0] = _T('\0');
			
			DWORD FileSize = 0;
			if (ModuleFile != INVALID_HANDLE_VALUE)
			{
				FileSize = GetFileSize(ModuleFile, 0);
				FILETIME LastWriteTime;
				if (GetFileTime(ModuleFile, 0, 0, &LastWriteTime))
				{
					FormatTime(TimeBuffer, LastWriteTime);
				}
				CloseHandle(ModuleFile);
			}
			hprintf(LogFile, _T("Module %d\r\n"), nModuleNo);
			hprintf(LogFile, _T("%s\r\n"), szModName);
			hprintf(LogFile, _T("Image Base: 0x%08x  Image Size: 0x%08x\r\n"), 
				NTHeader->OptionalHeader.ImageBase, 
				NTHeader->OptionalHeader.SizeOfImage), 

			hprintf(LogFile, _T("Checksum:   0x%08x  Time Stamp: 0x%08x\r\n"), 
				NTHeader->OptionalHeader.CheckSum,
				NTHeader->FileHeader.TimeDateStamp);

			hprintf(LogFile, _T("File Size:  %-10d  File Time:  %s\r\n"),
						FileSize, TimeBuffer);

			hprintf(LogFile, _T("Version Information:\r\n"));

			CMiniVersion ver(szModName);
			TCHAR szBuf[200];
			WORD dwBuf[4];

			ver.GetCompanyName(szBuf, sizeof(szBuf)-1);
			hprintf(LogFile, _T("   Company:    %s\r\n"), szBuf);

			ver.GetProductName(szBuf, sizeof(szBuf)-1);
			hprintf(LogFile, _T("   Product:    %s\r\n"), szBuf);

			ver.GetFileDescription(szBuf, sizeof(szBuf)-1);
			hprintf(LogFile, _T("   FileDesc:   %s\r\n"), szBuf);

			ver.GetFileVersion(dwBuf);
			hprintf(LogFile, _T("   FileVer:    %d.%d.%d.%d\r\n"), 
				dwBuf[0], dwBuf[1], dwBuf[2], dwBuf[3]);

			ver.GetProductVersion(dwBuf);
			hprintf(LogFile, _T("   ProdVer:    %d.%d.%d.%d\r\n"), 
				dwBuf[0], dwBuf[1], dwBuf[2], dwBuf[3]);

			ver.Release();

			hprintf(LogFile, _T("\r\n"));

			rc = true;
		} 
	}
	// Handle any exceptions by continuing from this point.
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
	return rc;
}

///////////////////////////////////////////////////////////////////////////////
// DumpModuleList
// Scan memory looking for code modules (DLLs or EXEs). VirtualQuery is used
// to find all the blocks of address space that were reserved or committed,
// and ShowModuleInfo will display module information if they are code
// modules.
static void DumpModuleList(HANDLE LogFile)
{
	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);

	const size_t PageSize = SystemInfo.dwPageSize;

	// Set NumPages to the number of pages in the 4GByte address space,
	// while being careful to avoid overflowing ints
	const size_t NumPages = 4 * size_t(ONEG / PageSize);
	size_t pageNum = 0;
	void *LastAllocationBase = 0;

	int nModuleNo = 1;

	while (pageNum < NumPages)
	{
		MEMORY_BASIC_INFORMATION MemInfo;
		if (VirtualQuery((void *)(pageNum * PageSize), &MemInfo,
					sizeof(MemInfo)))
		{
			if (MemInfo.RegionSize > 0)
			{
				// Adjust the page number to skip over this block of memory
				pageNum += MemInfo.RegionSize / PageSize;
				if (MemInfo.State == MEM_COMMIT && MemInfo.AllocationBase >
							LastAllocationBase)
				{
					// Look for new blocks of committed memory, and try
					// recording their module names - this will fail
					// gracefully if they aren't code modules
					LastAllocationBase = MemInfo.AllocationBase;
					if (DumpModuleInfo(LogFile, 
									   (HINSTANCE)LastAllocationBase, 
									   nModuleNo))
					{
						nModuleNo++;
					}
				}
			}
			else
				pageNum += SIXTYFOURK / PageSize;
		}
		else
			pageNum += SIXTYFOURK / PageSize;

		// If VirtualQuery fails we advance by 64K because that is the
		// granularity of address space doled out by VirtualAlloc()
	}
}

///////////////////////////////////////////////////////////////////////////////
// DumpSystemInformation
// Record information about the user's system, such as processor type, amount
// of memory, etc.
static void DumpSystemInformation(HANDLE LogFile)
{
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	TCHAR szTimeBuffer[100];
	FormatTime(szTimeBuffer, CurrentTime);

	hprintf(LogFile, _T("Error occurred at %s.\r\n"), szTimeBuffer);

	TCHAR szModuleName[MAX_PATH*2];
	ZeroMemory(szModuleName, sizeof(szModuleName));
	if (GetModuleFileName(0, szModuleName, _countof(szModuleName)-2) <= 0)
		lstrcpy(szModuleName, _T("Unknown"));

	TCHAR szUserName[200];
	ZeroMemory(szUserName, sizeof(szUserName));
	DWORD UserNameSize = _countof(szUserName)-2;
	if (!GetUserName(szUserName, &UserNameSize))
		lstrcpy(szUserName, _T("Unknown"));

	hprintf(LogFile, _T("%s, run by %s.\r\n"), szModuleName, szUserName);

	// print out operating system
	TCHAR szWinVer[50], szMajorMinorBuild[50];
	int nWinVer;
	GetWinVer(szWinVer, &nWinVer, szMajorMinorBuild);
	hprintf(LogFile, _T("Operating system:  %s (%s).\r\n"), 
		szWinVer, szMajorMinorBuild);

	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);
	hprintf(LogFile, _T("%d processor(s), type %d.\r\n"),
				SystemInfo.dwNumberOfProcessors, SystemInfo.dwProcessorType);

	MEMORYSTATUS MemInfo;
	MemInfo.dwLength = sizeof(MemInfo);
	GlobalMemoryStatus(&MemInfo);

	// Print out info on memory, rounded up.
	hprintf(LogFile, _T("%d%% memory in use.\r\n"), MemInfo.dwMemoryLoad);
	hprintf(LogFile, _T("%d MBytes physical memory.\r\n"), (MemInfo.dwTotalPhys +
				ONEM - 1) / ONEM);
	hprintf(LogFile, _T("%d MBytes physical memory free.\r\n"), 
		(MemInfo.dwAvailPhys + ONEM - 1) / ONEM);
	hprintf(LogFile, _T("%d MBytes paging file.\r\n"), (MemInfo.dwTotalPageFile +
				ONEM - 1) / ONEM);
	hprintf(LogFile, _T("%d MBytes paging file free.\r\n"), 
		(MemInfo.dwAvailPageFile + ONEM - 1) / ONEM);
	hprintf(LogFile, _T("%d MBytes user address space.\r\n"), 
		(MemInfo.dwTotalVirtual + ONEM - 1) / ONEM);
	hprintf(LogFile, _T("%d MBytes user address space free.\r\n"), 
		(MemInfo.dwAvailVirtual + ONEM - 1) / ONEM);
}

///////////////////////////////////////////////////////////////////////////////
// GetExceptionDescription
// Translate the exception code into something human readable
static const TCHAR *GetExceptionDescription(DWORD ExceptionCode)
{
	struct ExceptionNames
	{
		DWORD	ExceptionCode;
		TCHAR *	ExceptionName;
	};

#if 0  // from winnt.h
#define STATUS_WAIT_0                    ((DWORD   )0x00000000L)    
#define STATUS_ABANDONED_WAIT_0          ((DWORD   )0x00000080L)    
#define STATUS_USER_APC                  ((DWORD   )0x000000C0L)    
#define STATUS_TIMEOUT                   ((DWORD   )0x00000102L)    
#define STATUS_PENDING                   ((DWORD   )0x00000103L)    
#define STATUS_SEGMENT_NOTIFICATION      ((DWORD   )0x40000005L)    
#define STATUS_GUARD_PAGE_VIOLATION      ((DWORD   )0x80000001L)    
#define STATUS_DATATYPE_MISALIGNMENT     ((DWORD   )0x80000002L)    
#define STATUS_BREAKPOINT                ((DWORD   )0x80000003L)    
#define STATUS_SINGLE_STEP               ((DWORD   )0x80000004L)    
#define STATUS_ACCESS_VIOLATION          ((DWORD   )0xC0000005L)    
#define STATUS_IN_PAGE_ERROR             ((DWORD   )0xC0000006L)    
#define STATUS_INVALID_HANDLE            ((DWORD   )0xC0000008L)    
#define STATUS_NO_MEMORY                 ((DWORD   )0xC0000017L)    
#define STATUS_ILLEGAL_INSTRUCTION       ((DWORD   )0xC000001DL)    
#define STATUS_NONCONTINUABLE_EXCEPTION  ((DWORD   )0xC0000025L)    
#define STATUS_INVALID_DISPOSITION       ((DWORD   )0xC0000026L)    
#define STATUS_ARRAY_BOUNDS_EXCEEDED     ((DWORD   )0xC000008CL)    
#define STATUS_FLOAT_DENORMAL_OPERAND    ((DWORD   )0xC000008DL)    
#define STATUS_FLOAT_DIVIDE_BY_ZERO      ((DWORD   )0xC000008EL)    
#define STATUS_FLOAT_INEXACT_RESULT      ((DWORD   )0xC000008FL)    
#define STATUS_FLOAT_INVALID_OPERATION   ((DWORD   )0xC0000090L)    
#define STATUS_FLOAT_OVERFLOW            ((DWORD   )0xC0000091L)    
#define STATUS_FLOAT_STACK_CHECK         ((DWORD   )0xC0000092L)    
#define STATUS_FLOAT_UNDERFLOW           ((DWORD   )0xC0000093L)    
#define STATUS_INTEGER_DIVIDE_BY_ZERO    ((DWORD   )0xC0000094L)    
#define STATUS_INTEGER_OVERFLOW          ((DWORD   )0xC0000095L)    
#define STATUS_PRIVILEGED_INSTRUCTION    ((DWORD   )0xC0000096L)    
#define STATUS_STACK_OVERFLOW            ((DWORD   )0xC00000FDL)    
#define STATUS_CONTROL_C_EXIT            ((DWORD   )0xC000013AL)    
#define STATUS_FLOAT_MULTIPLE_FAULTS     ((DWORD   )0xC00002B4L)    
#define STATUS_FLOAT_MULTIPLE_TRAPS      ((DWORD   )0xC00002B5L)    
#define STATUS_ILLEGAL_VLM_REFERENCE     ((DWORD   )0xC00002C0L)     
#endif

	ExceptionNames ExceptionMap[] =
	{
		{0x40010005, _T("a Control-C")},
		{0x40010008, _T("a Control-Break")},
		{0x80000002, _T("a Datatype Misalignment")},
		{0x80000003, _T("a Breakpoint")},
		{0xc0000005, _T("an Access Violation")},
		{0xc0000006, _T("an In Page Error")},
		{0xc0000017, _T("a No Memory")},
		{0xc000001d, _T("an Illegal Instruction")},
		{0xc0000025, _T("a Noncontinuable Exception")},
		{0xc0000026, _T("an Invalid Disposition")},
		{0xc000008c, _T("a Array Bounds Exceeded")},
		{0xc000008d, _T("a Float Denormal Operand")},
		{0xc000008e, _T("a Float Divide by Zero")},
		{0xc000008f, _T("a Float Inexact Result")},
		{0xc0000090, _T("a Float Invalid Operation")},
		{0xc0000091, _T("a Float Overflow")},
		{0xc0000092, _T("a Float Stack Check")},
		{0xc0000093, _T("a Float Underflow")},
		{0xc0000094, _T("an Integer Divide by Zero")},
		{0xc0000095, _T("an Integer Overflow")},
		{0xc0000096, _T("a Privileged Instruction")},
		{0xc00000fD, _T("a Stack Overflow")},
		{0xc0000142, _T("a DLL Initialization Failed")},
		{0xe06d7363, _T("a Microsoft C++ Exception")},
	};

	for (int i = 0; i < sizeof(ExceptionMap) / sizeof(ExceptionMap[0]); i++)
		if (ExceptionCode == ExceptionMap[i].ExceptionCode)
			return ExceptionMap[i].ExceptionName;

	return _T("an Unknown exception type");
}

///////////////////////////////////////////////////////////////////////////////
// GetFilePart
static TCHAR * GetFilePart(LPCTSTR source)
{
	TCHAR *result = lstrrchr(source, _T('\\'));
	if (result)
		result++;
	else
		result = (TCHAR *)source;
	return result;
}



static void DumpHumanStack(HANDLE LogFile, PCONTEXT context)
{
	__try {
		
			
			::SymSetOptions(SYMOPT_LOAD_LINES);
			if (!::SymInitialize(::GetCurrentProcess(), NULL , true))
			{
					hprintf(LogFile, _T("\r\nCannot create human readable stack dump.\r\n"));
					return;
			}

			hprintf(LogFile, _T("\r\n\r\nHuman readable stack:\r\n"));
			// init stack frame structure
			STACKFRAME64 stackFrame;
			ZeroMemory(&stackFrame, sizeof(STACKFRAME64));
			stackFrame.AddrPC.Offset = context->Eip;
			stackFrame.AddrPC.Mode = AddrModeFlat;
			stackFrame.AddrFrame.Offset = context->Ebp;
			stackFrame.AddrFrame.Mode = AddrModeFlat;

			// unwind stack
			for (unsigned int i = 0; ::StackWalk64(IMAGE_FILE_MACHINE_I386, ::GetCurrentProcess(), ::GetCurrentThread(), &stackFrame, context, NULL, ::SymFunctionTableAccess64, ::SymGetModuleBase64, 0); ++i)
			{
						char symbolBuf[sizeof(SYMBOL_INFO) + 512];

						//SYMBOL_INFO* symbol  = (SYMBOL_INFO*)std::calloc(1, sizeof(SYMBOL_INFO) + 512);
						SYMBOL_INFO* symbol = (SYMBOL_INFO*)symbolBuf;
						symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
						symbol->MaxNameLen   = 512;

						DWORD64 offset;
						if (::SymFromAddr(::GetCurrentProcess(), stackFrame.AddrPC.Offset, &offset, symbol))
						{
								hprintf(LogFile, _T("\r\n< %s >"), symbol->Name);
						}

						IMAGEHLP_LINE64 Line = {0};
						Line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

						DWORD LineDisplacement = 0;
            if (SymGetLineFromAddr64(
                ::GetCurrentProcess(),
                stackFrame.AddrPC.Offset,
                &LineDisplacement,
								&Line)) 
						{
							hprintf(LogFile, _T(" - \"%s\" { %d }"), Line.FileName, Line.LineNumber);
						}

		        
			}

			hprintf(LogFile, _T("\r\n\r\n"));
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		hprintf(LogFile, _T("Exception encountered during human readable stack dump.\r\n"));
	}
}

///////////////////////////////////////////////////////////////////////////////
// DumpStack
static void DumpStack(HANDLE LogFile, DWORD *pStack)
{
	hprintf(LogFile, _T("\r\n\r\nStack:\r\n"));

	__try
	{
		// Esp contains the bottom of the stack, or at least the bottom of
		// the currently used area.
		DWORD* pStackTop;

		__asm
		{
			// Load the top (highest address) of the stack from the
			// thread information block. It will be found there in
			// Win9x and Windows NT.
			mov	eax, fs:[4]
			mov pStackTop, eax
		}

		if (pStackTop > pStack + MaxStackDump)
			pStackTop = pStack + MaxStackDump;

		int Count = 0;

		DWORD* pStackStart = pStack;

		int nDwordsPrinted = 0;

		while (pStack + 1 <= pStackTop)
		{
			if ((Count % StackColumns) == 0)
			{
				pStackStart = pStack;
				nDwordsPrinted = 0;
				hprintf(LogFile, _T("0x%08x: "), pStack);
			}

			if ((++Count % StackColumns) == 0 || pStack + 2 > pStackTop)
			{
				hprintf(LogFile, _T("%08x "), *pStack);
				nDwordsPrinted++;

				int n = nDwordsPrinted;
				while (n < 4)
				{
					hprintf(LogFile, _T("         "));
					n++;
				}

				for (int i = 0; i < nDwordsPrinted; i++)
				{
					DWORD dwStack = *pStackStart;
					for (int j = 0; j < 4; j++)
					{
						char c = (char)(dwStack & 0xFF);
						if (c < 0x20 || c > 0x7E)
							c = '.';
#ifdef _UNICODE
						WCHAR w = (WCHAR)c;
						hprintf(LogFile, _T("%c"), w);
#else
						hprintf(LogFile, _T("%c"), c);
#endif
						dwStack = dwStack >> 8;
					}
					pStackStart++;
				}

				hprintf(LogFile, _T("\r\n"));
			}
			else
			{
				hprintf(LogFile, _T("%08x "), *pStack);
				nDwordsPrinted++;
			}
			pStack++;
		}
		hprintf(LogFile, _T("\r\n"));
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		hprintf(LogFile, _T("Exception encountered during stack dump.\r\n"));
	}
}

///////////////////////////////////////////////////////////////////////////////
// DumpRegisters
static void DumpRegisters(HANDLE LogFile, PCONTEXT Context)
{
	// Print out the register values in an XP error window compatible format.
	hprintf(LogFile, _T("\r\n"));
	hprintf(LogFile, _T("Context:\r\n"));
	hprintf(LogFile, _T("EDI:    0x%08x  ESI: 0x%08x  EAX:   0x%08x\r\n"),
				Context->Edi, Context->Esi, Context->Eax);
	hprintf(LogFile, _T("EBX:    0x%08x  ECX: 0x%08x  EDX:   0x%08x\r\n"),
				Context->Ebx, Context->Ecx, Context->Edx);
	hprintf(LogFile, _T("EIP:    0x%08x  EBP: 0x%08x  SegCs: 0x%08x\r\n"),
				Context->Eip, Context->Ebp, Context->SegCs);
	hprintf(LogFile, _T("EFlags: 0x%08x  ESP: 0x%08x  SegSs: 0x%08x\r\n"),
				Context->EFlags, Context->Esp, Context->SegSs);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// RecordExceptionInfo
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int __cdecl RecordExceptionInfo(PEXCEPTION_POINTERS pExceptPtrs, LPCTSTR lpszMessage, LPCTSTR lpszCrashFileName, LPCTSTR mark)
{
	
	static bool bFirstTime = true;
	if (!bFirstTime)	// Going recursive! That must mean this routine crashed!
		return EXCEPTION_CONTINUE_SEARCH;
	bFirstTime = false;

	// Create a filename to record the error information to.
	// Storing it in the executable directory works well.

	TCHAR szModuleName[MAX_PATH*2];
	ZeroMemory(szModuleName, sizeof(szModuleName));
	if (GetModuleFileName(0, szModuleName, _countof(szModuleName)-2) <= 0)
		lstrcpy(szModuleName, _T("Unknown"));

	TCHAR *pszFilePart = GetFilePart(szModuleName);

	// Extract the file name portion and remove it's file extension
	TCHAR szFileName[MAX_PATH*2];
	lstrcpy(szFileName, pszFilePart);
	TCHAR *lastperiod = lstrrchr(szFileName, _T('.'));
	if (lastperiod)
		lastperiod[0] = 0;

	// Replace the executable filename with our error log file name
	
	lstrcpy(pszFilePart, lpszCrashFileName);

	// create out full file name
	TCHAR lpszFullCrashFileName[1024];
	ZeroMemory((void*)lpszFullCrashFileName, sizeof(lpszFullCrashFileName));
	lstrcpy((LPSTR)lpszFullCrashFileName, szModuleName );

	//HANDLE hLogFile = CreateFile(szModuleName, GENERIC_WRITE, 0, 0,
	//			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 0);

	HANDLE hLogFile = CreateFile(szModuleName, GENERIC_WRITE, 0, 0,
				OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 0);

	if (hLogFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugString(_T("Error creating exception report\r\n"));
		return EXCEPTION_CONTINUE_SEARCH;
	}

	// Append to the error log
	SetFilePointer(hLogFile, 0, 0, FILE_END);

	// Print out a blank line to separate this error log from any previous ones
	hprintf(hLogFile, mark );
	hprintf(hLogFile, _T("\r\n"));

	PEXCEPTION_RECORD Exception = pExceptPtrs->ExceptionRecord;
	PCONTEXT          Context   = pExceptPtrs->ContextRecord;

	

	TCHAR szCrashModulePathName[MAX_PATH*2];
	ZeroMemory(szCrashModulePathName, sizeof(szCrashModulePathName));

	TCHAR *pszCrashModuleFileName = _T("Unknown");

	MEMORY_BASIC_INFORMATION MemInfo;

	// VirtualQuery can be used to get the allocation base associated with a
	// code address, which is the same as the ModuleHandle. This can be used
	// to get the filename of the module that the crash happened in.
	if (VirtualQuery((void*)Context->Eip, &MemInfo, sizeof(MemInfo)) &&
						(GetModuleFileName((HINSTANCE)MemInfo.AllocationBase,
										  szCrashModulePathName,
										  sizeof(szCrashModulePathName)-2) > 0))
	{
		pszCrashModuleFileName = GetFilePart(szCrashModulePathName);
	}

	// Print out the beginning of the error log in a Win95 error window
	// compatible format.
	
	// ZVV
	hprintf(hLogFile, _T("This file is: %s\n"),  szModuleName);

	hprintf(hLogFile, _T("%s caused %s (0x%08x) \r\nin module %s at %04x:%08x.\r\n\r\n"),
				szFileName, GetExceptionDescription(Exception->ExceptionCode),
				Exception->ExceptionCode,
				pszCrashModuleFileName, Context->SegCs, Context->Eip);

	hprintf(hLogFile, _T("Exception handler called in %s.\r\n"), lpszMessage);

	DumpSystemInformation(hLogFile);

	
	
	// If the exception was an access violation, print out some additional
	// information, to the error log and the debugger.
	if (Exception->ExceptionCode == STATUS_ACCESS_VIOLATION &&
				Exception->NumberParameters >= 2)
	{
		TCHAR szDebugMessage[1000];
		const TCHAR* readwrite = _T("Read from");
		if (Exception->ExceptionInformation[0])
			readwrite = _T("Write to");
		wsprintf(szDebugMessage, _T("%s location %08x caused an access violation.\r\n"),
					readwrite, Exception->ExceptionInformation[1]);

#ifdef	_DEBUG
		// The Visual C++ debugger doesn't actually tell you whether a read
		// or a write caused the access violation, nor does it tell what
		// address was being read or written. So I fixed that.
		OutputDebugString(_T("Exception handler: "));
		OutputDebugString(szDebugMessage);
#endif

		hprintf(hLogFile, _T("%s"), szDebugMessage);
	}

	

	DumpRegisters(hLogFile, Context);
	

	
	
	 
	// Print out the bytes of code at the instruction pointer. Since the
	// crash may have been caused by an instruction pointer that was bad,
	// this code needs to be wrapped in an exception handler, in case there
	// is no memory to read. If the dereferencing of code[] fails, the
	// exception handler will print '??'.
	hprintf(hLogFile, _T("\r\nBytes at CS:EIP:\r\n"));
	BYTE * code = (BYTE *)Context->Eip;
	for (int codebyte = 0; codebyte < NumCodeBytes; codebyte++)
	{
		__try
		{
			hprintf(hLogFile, _T("%02x "), code[codebyte]);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			hprintf(hLogFile, _T("?? "));
		}
	}
	 
	
	

	// Time to print part or all of the stack to the error log. This allows
	// us to figure out the call stack, parameters, local variables, etc.

	// Esp contains the bottom of the stack, or at least the bottom of
	// the currently used area
	DWORD* pStack = (DWORD *)Context->Esp;

	DumpHumanStack(hLogFile,Context);
	DumpStack(hLogFile, pStack);

	DumpModuleList(hLogFile);
	

	hprintf(hLogFile, _T("\r\n===================== [end] =====================\r\n"));
	hflush(hLogFile);
	CloseHandle(hLogFile);

	// return the magic value which tells Win32 that this handler didn't
	// actually handle the exception - so that things will proceed as per
	// normal.
	
	return EXCEPTION_CONTINUE_SEARCH;
}



//=============================================================
// 
//=============================================================


//=============================================================
// 
//=============================================================


//=============================================================
// MSVC Exceptions
//=============================================================
class ExceptionMSVC : public Testframe::Exception {

public:
	/** Ctor taking message string as argument. */
	ExceptionMSVC(char const *msg) : msg_m( strdup(msg) ) {};
	/** Dtor. */
	~ExceptionMSVC() { free(msg_m); }
	/** Access the type of exception. */
	std::string type (void) const { return "MSVC run-time exception"; };
	/** Access the exception message. */
	std::string message (void) const { return msg_m; };

	/** Access the additional info. */
	std::string addInfo() const { return "MSVC exception"; };

private:
	/** Message string. */
	char *msg_m;
};

static int fpu_cw = 0;

int defaultFpuCw(void) {
	// Get the default control word
	int cw = _controlfp( 0, 0 );

	// Set the exception masks off, turn exceptions on
	// EM_INEXACT is not turned on, as you get it even for sqrt(2.0)!
	cw &=~(EM_OVERFLOW | EM_UNDERFLOW | EM_ZERODIVIDE | EM_INVALID | EM_DENORMAL);

	// Set 64 bit precision
	cw &= ~(_PC_24 | _PC_53);

	return cw;
}


//=============================================================
// FPU
//=============================================================
/** Initialize the FPU to report all problems as exception and
		reset it. */
void initFPU (void) {
	// Reset FPU
	_fpreset();

	// Set the control word
	_controlfp( fpu_cw, MCW_EM | _MCW_PC );
}

 typedef BOOL (WINAPI *LPSymSetOptions)(
        __in  DWORD SymOptions
);


//=============================================================
// Map MSVC's structured exception handling
//=============================================================
/** Translation function to map MSVC's structured exception
		handling into C++ exceptions. */
void trans_func( unsigned int u, _EXCEPTION_POINTERS* pExp )
{
	EnterCriticalSection( getMutex() );

	// Set exception string
	const char *error;

	switch( pExp->ExceptionRecord->ExceptionCode ) {
		case EXCEPTION_ACCESS_VIOLATION:
			error = "Access violation";
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT     :
			error = "Misaligned data";
			break;
		case EXCEPTION_BREAKPOINT                :
			error = "Breakpoint encoutered";
			break;
		case EXCEPTION_SINGLE_STEP               :
			error = "Single step complete";
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED     :
			error = "Array element out of bounds";
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND      :
			error = "Denormal floating-point operand";
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO        :
			error = "Floating-point division by zero";
			break;
		case EXCEPTION_FLT_INEXACT_RESULT        :
			error = "Inexact FPU result";
			break;
		case EXCEPTION_FLT_INVALID_OPERATION     :
			error = "Invalid operation (FPU)";
			break;
		case EXCEPTION_FLT_OVERFLOW              :
			error = "Floating-point value overflow";
			break;
		case EXCEPTION_FLT_STACK_CHECK           :
			error = "FPU stack over/underflow";
			break;
		case EXCEPTION_FLT_UNDERFLOW             :
			error = "Floating-point value underflow";
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO        :
			error = "Integer division by zero";
			break;
		case EXCEPTION_INT_OVERFLOW              :
			error = "Integer value overflow";
			break;
		case EXCEPTION_PRIV_INSTRUCTION          :
			error = "Instruction not allowed in current machine mode";
			break;
		case EXCEPTION_IN_PAGE_ERROR             :
			error = "In page error";
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION       :
			error = "Invalid instruction";
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION  :
			error = "Noncontinuable exception";
			break;
		case EXCEPTION_STACK_OVERFLOW            :
			error = "Stack overflow";
			break;
		case EXCEPTION_INVALID_DISPOSITION       :
			error = "Invalid disposition";
			break;
		default:
			error = "<unknown>";
	}

	// file name
	TCHAR file_name_buf[256];
	TCHAR mark[64];

	struct _timeb now; _ftime( &now );
	sprintf( mark, "Error ID #%u", now.time );
	//sprintf(file_name_buf, "ht_crash_dumps_%u_%u.txt", now.time, now.millitm );
	
	sprintf(file_name_buf, "ht_crash_dumps.txt" );

	initFPU();
	RecordExceptionInfo(pExp, "MSVC exception", file_name_buf, mark);
	

	char buf[2048];
	sprintf(buf, "%s @ %p - file: %s [ %s ]", error, pExp->ExceptionRecord->ExceptionAddress, file_name_buf, mark );

	// Reset FPU and set control word
	//initFPU();

	// Throw exception
	throw ExceptionMSVC( buf );
	
	LeaveCriticalSection( getMutex() );
}

//=============================================================
// Boot code
//=============================================================
/** System dependent initializations for running tests. */
void Testframe::init (int newCw) {
	EnterCriticalSection( getMutex() );

	// Map MSVC structured exceptions to C++ expceptions
	_set_se_translator( trans_func );

	// use a special FPU codeword if requested
	if ( newCw != 0 ) {
		fpu_cw = newCw;
	}
	else {
		// set default fpu Codeword
		fpu_cw = defaultFpuCw();
	}

	// Init FPU
	initFPU();

	// Init timezone; this allocs memory; otherwise the 1st call to time() does this
	_tzset();


	LeaveCriticalSection( getMutex() );
}

double getDoubleTime(void) {
	struct _timeb now;
	_ftime( &now );

	return now.time + now.millitm / 1000.0;
}

#endif // MSVC

#else	// WIN32

#include <sys/time.h> /* pick up gettimeofday(), timeval, timezone */

double getDoubleTime(void) {
	struct timeval now;
	struct timezone tz;

	gettimeofday( &now, &tz );

	return now.tv_sec + now.tv_usec/1000000.0;
}

#endif // WIN32
