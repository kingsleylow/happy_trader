
#include "DownloadDatarequest.hpp"

HRESULT DownloadDataRequest::Initialize(PCWSTR path,
                    PCWSTR verb,
										const WinHttpConnection& connection)
{
  
    return __super::Initialize(path,  verb, connection);
}

HRESULT DownloadDataRequest::OnReadComplete(
				const void* buffer,
        DWORD bytesRead)
{

	// read and parse data
	WLSendStructure wl_struct;

	if (bytesRead != WLSendStructure::WL_STRUCT_LENGTH)
		return E_FAIL;

	memcpy(wl_struct.getData(), buffer, bytesRead);

	BYTE flag = wl_struct.getPacketType();
	double ltime  = wl_struct.getUnixTime();
	CppUtils::String dt = CppUtils::getGmtTime(ltime);
	double vol = wl_struct.getVolume();

	double open = wl_struct.getOpen();
	double high = wl_struct.getHigh();
	double low = wl_struct.getLow();
	double close = wl_struct.getClose();

	double open2 = wl_struct.getOpen();
	double high2 = wl_struct.getHigh();
	double low2 = wl_struct.getLow();
	double close2 = wl_struct.getClose();
	

  return S_OK;
}

void DownloadDataRequest::OnResponseComplete(HRESULT result)
{
    if (S_OK == result)
    {
        // Download succeeded
    }
}

HRESULT DownloadDataRequest::OnWriteData()
{
	return S_FALSE;
}