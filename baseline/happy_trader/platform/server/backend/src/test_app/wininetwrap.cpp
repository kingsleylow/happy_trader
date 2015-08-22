#include "wininetwrap.hpp"



LPSTR UnicodeToAnsi(LPCWSTR s)
{
	if (s==NULL) return NULL;
	int cw=lstrlenW(s);
	if (cw==0) {CHAR *psz=new CHAR[1];*psz='\0';return psz;}
	int cc=WideCharToMultiByte(CP_ACP,0,s,cw,NULL,0,NULL,NULL);
	if (cc==0) return NULL;
	CHAR *psz=new CHAR[cc+1];
	cc=WideCharToMultiByte(CP_ACP,0,s,cw,psz,cc,NULL,NULL);
	if (cc==0) {delete[] psz;return NULL;}
	psz[cc]='\0';
	return psz;
}


// ----------------------

