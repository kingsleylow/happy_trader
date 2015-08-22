#ifndef CORE_HEADER_INCLUDED_PIPECONSOLE
#define CORE_HEADER_INCLUDED_PIPECONSOLE

#include "shared/cpputils/src/cpputils.hpp"

#include "stdio.h"
#include "windows.h"


extern CONSOLE_SCREEN_BUFFER_INFO	g_ConsoleBufferInfo;
extern HANDLE	g_hPipe;
extern BOOL	g_bExtendedConsole;
extern HANDLE	g_hConsole;
extern DWORD	g_dwConsoleSize;
extern COORD	g_dwConsoleCoords;


long ConsoleLoop(void);
long ConsoleExLoop(void);
void cls(WORD color);
void cls(void);
void clear_eol(void);
void clear_eol(WORD color);
void gotoxy(int x,int y);
int getxy(int *x,int *y);
int getx(void);
int gety(void);


// 
template <class T>
T ReadPipe(T &v)
{
	DWORD cbRead;
	if ( !ReadFile(g_hPipe,&v,sizeof(T),&cbRead,NULL) || cbRead!=sizeof(T))
	{
		return 0;
	}
	return v;
}

WORD ReadWord(void);
DWORD ReadDword(void);
BYTE ReadByte(void);




#endif