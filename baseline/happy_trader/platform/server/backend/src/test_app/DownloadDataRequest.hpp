#ifndef _WININET_DOWNLOADDATAREQUEST_HPP
#define _WININET_DOWNLOADDATAREQUEST_HPP

#include "wininetwrap.hpp"
#include "ReceiveStruct.hpp"

// -----------------
// implementaton

class DownloadDataRequest : public WinHttpRequest<DownloadDataRequest, WLSendStructure::WL_STRUCT_LENGTH >
{
public:
    HRESULT Initialize(PCWSTR path,
                       PCWSTR verb,
											 const WinHttpConnection& connection);
  
    HRESULT OnReadComplete(const void* buffer, DWORD bytesRead);
   
    void OnResponseComplete(HRESULT result);
   	
		HRESULT OnWriteData();
		

private:
   
};

#endif