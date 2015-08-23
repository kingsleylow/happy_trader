// dllmain.h : Declaration of module class.

#ifndef _SMARTCOM_PROXY2DLLMAIN_INCLUDED
#define _SMARTCOM_PROXY2DLLMAIN_INCLUDED

class Csmartcom_proxy2Module : public CAtlDllModuleT< Csmartcom_proxy2Module >
{
public :
	DECLARE_LIBID(LIBID_smartcom_proxy2Lib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SMARTCOM_PROXY2, "{4501F940-3C01-4D3F-80A6-7DC38B7A5E04}")
		
};

extern class Csmartcom_proxy2Module _AtlModule;

#endif