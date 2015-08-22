#include "mtqueuemanager.hpp"
#include "mtqueuecore.hpp"



#define MTQUEUEMANAGER "MTQUEUEMANAGER"

namespace MtQueue {


	// ----------------------------------------------

		EnviromentHandler::EnviromentHandler():
			responseHandlers_m(NULL),
			transport_m(NULL)
		{
		
		}

		EnviromentHandler::EnviromentHandler(EnviromentHandler const& src):
			transport_m(src.transport_m),
			responseHandlers_m(src.responseHandlers_m)
		{
		}

		EnviromentHandler::~EnviromentHandler()
		{
			
		}

		bool EnviromentHandler::isValid() const
		{
			// it is valid from
			return (transport_m != NULL);
		}

		// clear resources and invalidate
		void EnviromentHandler::clear()
		{
			if (transport_m) {

				LOG(MSG_DEBUG, MTQUEUEMANAGER, "Removing transport" );
				delete transport_m;
				transport_m = NULL;

			
				
				if (responseHandlers_m) {
					delete responseHandlers_m;
					responseHandlers_m = NULL;

					LOG(MSG_DEBUG, MTQUEUEMANAGER, "Cleared all response handlers" );
				}


				LOG(MSG_INFO, MTQUEUEMANAGER, "Enviroment handler cleared: " << this->getId() );
			}

			
		}

		void EnviromentHandler::init()
		{
			if (!transport_m) {
				transport_m = new TransportLayer();
				responseHandlers_m = new HandlerStorage<ResponseHandler>("Resp storage");
				LOG(MSG_INFO, MTQUEUEMANAGER, "Enviroment handler initialized" );
			}
			else
				THROW(CppUtils::OperationFailed, "exc_EnvHandlerBusy", "ctx_init_EnviromentHandler", "");
		}

		TransportLayer* EnviromentHandler::getTransport()
		{
			return transport_m;
		}

		HandlerStorage<ResponseHandler>* EnviromentHandler::getResponseHandlers()
		{
			return responseHandlers_m;
		}
		// ---------------------------

		ResponseHandler::ResponseHandler():
			result_m(NULL)
		{
		}

		ResponseHandler::ResponseHandler(ResponseHandler const& src):
			result_m(src.result_m)
		{
		}

		ResponseHandler::~ResponseHandler()
		{
		
		}

		bool ResponseHandler::isValid() const
		{
			return (result_m != NULL);

		}

		// clear resources and invalidate
		void ResponseHandler::clear()
		{
			if (result_m) {
				delete result_m;
				result_m = NULL;

				LOG(MSG_INFO, MTQUEUEMANAGER, "Response handler cleared: " << this->getId() );
			}
		}

		void ResponseHandler::init(Result const& result)
		{
			if (!result_m) {
				result_m = new Result(result);
				LOG(MSG_INFO, MTQUEUEMANAGER, "Response handler initialized"  );
			}
			else
					THROW(CppUtils::OperationFailed, "exc_RespHandlerBusy", "ctx_init_ResponseHandler", "");
		};

		Result* ResponseHandler::getResultPtr()
		{
			return result_m;
		}

		// --------------------------------


	MtQueueManager::MtQueueManager():
		envHandlers_m("Env storage")
	{  
		LOG(MSG_INFO, MTQUEUEMANAGER, "Ctor");
	}




	MtQueueManager::~MtQueueManager()
	{
		
		LOG(MSG_INFO, MTQUEUEMANAGER, "Dtor");
		
	}

	int MtQueueManager::initEnv()
	{
		// creates enviroment handler
		EnviromentHandler handlerPtr;
		handlerPtr.init();
		int newId = envHandlers_m.addItem(handlerPtr);

		return newId;
	}


	PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{ 
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 

    // Retrieve the bitmap color format, width, and height.  
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
       return NULL;

    // Convert the color format to a count of bits.  
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure  
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
    // data structures.)  

     if (cClrBits < 24) 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER) + 
                    sizeof(RGBQUAD) * (1<< cClrBits)); 

     // There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

     else 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER)); 

    // Initialize the fields in the BITMAPINFO structure.  

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag.  
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color  
    // indices and store the result in biSizeImage.  
    // The width must be DWORD aligned unless the bitmap is RLE 
    // compressed. 
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
                                  * pbmi->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the  
    // device colors are important.  
     pbmi->bmiHeader.biClrImportant = 0; 
     return pbmi; 
 } 

	void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, 
                  HBITMAP hBMP, HDC hDC) 
 { 
     HANDLE hf;                 // file handle  
    BITMAPFILEHEADER hdr;       // bitmap file-header  
    PBITMAPINFOHEADER pbih;     // bitmap info-header  
    LPBYTE lpBits;              // memory pointer  
    DWORD dwTotal;              // total count of bytes  
    DWORD cb;                   // incremental count of bytes  
    BYTE *hp;                   // byte pointer  
    DWORD dwTmp; 

    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits) 
         //errhandler("GlobalAlloc", hwnd); 
					 THROW(CppUtils::OperationFailed, "exc_GlobalAlloc", "ctx_CreateBMPFile", "" );

    // Retrieve the color table (RGBQUAD array) and the bits  
    // (array of palette indices) from the DIB.  
    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, 
        DIB_RGB_COLORS)) 
    {
        //errhandler("GetDIBits", hwnd); 
				THROW(CppUtils::OperationFailed, "exc_GetDIBits", "ctx_CreateBMPFile", "" );
    }

    // Create the .BMP file.  
    hf = CreateFile(pszFile, 
                   GENERIC_READ | GENERIC_WRITE, 
                   (DWORD) 0, 
                    NULL, 
                   CREATE_ALWAYS, 
                   FILE_ATTRIBUTE_NORMAL, 
                   (HANDLE) NULL); 
    if (hf == INVALID_HANDLE_VALUE) 
        //errhandler("CreateFile", hwnd); 
				THROW(CppUtils::OperationFailed, "exc_CreateFile", "ctx_CreateBMPFile", "" );
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
    // Compute the size of the entire file.  
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
                 pbih->biSize + pbih->biClrUsed 
                 * sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 

    // Compute the offset to the array of color indices.  
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
                    pbih->biSize + pbih->biClrUsed 
                    * sizeof (RGBQUAD); 

    // Copy the BITMAPFILEHEADER into the .BMP file.  
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
        (LPDWORD) &dwTmp,  NULL)) 
    {
       //errhandler("WriteFile", hwnd); 
			THROW(CppUtils::OperationFailed, "exc_CreateFile", "ctx_WriteFile", "" );
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
                  + pbih->biClrUsed * sizeof (RGBQUAD), 
                  (LPDWORD) &dwTmp, ( NULL)))
        //errhandler("WriteFile", hwnd);
				THROW(CppUtils::OperationFailed, "exc_WriteFile", "ctx_WriteFile", "" );

    // Copy the array of color indices into the .BMP file.  
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) 
           //errhandler("WriteFile", hwnd); 
						THROW(CppUtils::OperationFailed, "exc_WriteFile2", "ctx_WriteFile", "" );

    // Close the .BMP file.  
     if (!CloseHandle(hf)) 
           //errhandler("CloseHandle", hwnd); 
					THROW(CppUtils::OperationFailed, "exc_CloseHandle", "ctx_WriteFile", "" );

    // Free memory.  
    GlobalFree((HGLOBAL)lpBits);
}

	void MtQueueManager::makeScreenShot()
	{
		int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    HWND hDesktopWnd = GetDesktopWindow();
    HDC hDesktopDC = GetDC(hDesktopWnd);
    HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
    HBITMAP hCaptureBitmap =CreateCompatibleBitmap(hDesktopDC, 
                            nScreenWidth, nScreenHeight);
    SelectObject(hCaptureDC,hCaptureBitmap); 
    BitBlt(hCaptureDC,0,0,nScreenWidth,nScreenHeight,
           hDesktopDC,0,0,SRCCOPY|CAPTUREBLT); 
    
		// save biit map
		PBITMAPINFO bmi = CreateBitmapInfoStruct(hDesktopWnd,hCaptureBitmap);
		if (!bmi)
			THROW(CppUtils::OperationFailed, "exc_InvalidBitmap", "ctx_makeScreenShot", "" );

		CppUtils::String tmpFile = CppUtils::getTempPath() + "\\screenshot.bmp";
		LOG(MSG_DEBUG, MTQUEUEMANAGER, "Writing to bitmap file: " << tmpFile );
		CreateBMPFile(hDesktopWnd, (char*)tmpFile.c_str(), bmi,  hCaptureBitmap, hCaptureDC) ;


    ReleaseDC(hDesktopWnd,hDesktopDC);
    DeleteDC(hCaptureDC);
    DeleteObject(hCaptureBitmap);
		LOG(MSG_DEBUG, MTQUEUEMANAGER, "Finished bitmap creation" );

		int ret_open = CppUtils::shellExecute(NULL, tmpFile.c_str(), "", CppUtils::getTempPath().c_str(),SW_SHOW );
		LOG(MSG_DEBUG, MTQUEUEMANAGER, "Showing bitmap: " << tmpFile );
		
	}
	
	void MtQueueManager::setEnvParams(int const envHndl, char const* key, char const* secret, char const* apiUrl)
	{
		EnviromentHandler hndl = envHandlers_m.getItem(envHndl);
		hndl.getTransport()->init(key? key: "", secret ? secret : "", apiUrl? apiUrl : "");
	
	}
		
	void MtQueueManager::btc_getInfo(int const envHndl, int& newId)
	{
	
		btc_generalCall(envHndl, NULL, "getInfo", NULL, newId);
	}

	void MtQueueManager::btc_generalCall(int const envHndl, char const* nonce, char const* method, char const* parameters, int& newId)
	{
		EnviromentHandler hndl = envHandlers_m.getItem(envHndl);

		CppUtils::String parameters_s = parameters ? parameters : "";
		Result res = hndl.getTransport()->makeBtcRequest(method, nonce, parameters_s);
	
		ResponseHandler respHndlPtr;
		respHndlPtr.init(res);

		newId = hndl.getResponseHandlers()->addItem(respHndlPtr);

	}
	
	void MtQueueManager::respGetError(int const envHndlr, int const respHndlr, int& statusCode, CppUtils::String& statusMessage)
	{
	
		// get response
		ResponseHandler respHndl = getResponseHandlerHelper(envHndlr, respHndlr);

		statusMessage = respHndl.getResultPtr()->getMessage();
		statusCode = respHndl.getResultPtr()->getCode();

		LOG(MSG_DEBUG, MTQUEUEMANAGER, "respGetError(...) returned: " << statusCode << " - " << statusMessage);
	}

	void MtQueueManager::respStartMessageRead(int const envHndlr, int const respHndlr, int& messageLength)
	{
		ResponseHandler respHndl = getResponseHandlerHelper(envHndlr, respHndlr);

		respHndl.getResultPtr()->getReadStringPtr() = 0;
		messageLength = respHndl.getResultPtr()->strData().size();

		LOG(MSG_DEBUG, MTQUEUEMANAGER, "Mesage of length " << messageLength << " is ready to be read" );
	}

	void MtQueueManager::respReadMessage(int const envHndlr, int const respHndlr, CppUtils::Byte* buffer, int const bufferlength, int& readBytes)
	{
		ResponseHandler respHndl = getResponseHandlerHelper(envHndlr, respHndlr);

		if (bufferlength<=0)
			THROW(CppUtils::OperationFailed, "exc_InvalidBufferLength", "ctx_respReadMessage", "");

		if (buffer == NULL)
			THROW(CppUtils::OperationFailed, "exc_InvalidBuffer", "ctx_respReadMessage", "");


		CppUtils::Byte* srcPtr =  (CppUtils::Byte*)respHndl.getResultPtr()->strData().c_str();
		CppUtils::Byte* fromRead = srcPtr + respHndl.getResultPtr()->getReadStringPtr();
		int totalMessageLength = respHndl.getResultPtr()->strData().size();
		readBytes = totalMessageLength - respHndl.getResultPtr()->getReadStringPtr();

		if (readBytes <=0) {

			// nothing to read
			readBytes = 0;
			LOG(MSG_DEBUG, MTQUEUEMANAGER, "respReadMessage(...) - read finished, zero length returning");
			return;
		}

		if (readBytes > bufferlength)
			readBytes = bufferlength;

		memcpy(buffer, fromRead, readBytes );
		respHndl.getResultPtr()->getReadStringPtr() += readBytes;

		LOG(MSG_DEBUG, MTQUEUEMANAGER, "respReadMessage(...) - Read: " << readBytes << " bytes");

	}
		
	void MtQueueManager::freeEnv(int const envHndlr)
	{
		if (envHndlr == HANDLER_INVALID)
			THROW(CppUtils::OperationFailed, "exc_InvalidHandler", "ctx_freeEnv", "");

		// check what handler to remove
		envHandlers_m.freeItem(envHndlr);
	}
	

	void MtQueueManager::freeResponse(int const envHndlr, int const respHndlr)
	{
		if (envHndlr == HANDLER_INVALID)
			THROW(CppUtils::OperationFailed, "exc_InvalidHandler", "ctx_freeResponse", "");

		EnviromentHandler hndl = envHandlers_m.getItem(envHndlr);

		hndl.getResponseHandlers()->freeItem(respHndlr);
	}

	// --------------------------------

	/**
	* Helpers
	*/

	ResponseHandler MtQueueManager::getResponseHandlerHelper(int const envHndlr, int const respHndlr)
	{
		if (envHndlr == HANDLER_INVALID)
			THROW(CppUtils::OperationFailed, "exc_InvalidEnvHandler", "ctx_getResponseHandlerHelper", "");

		if (respHndlr == HANDLER_INVALID)
			THROW(CppUtils::OperationFailed, "exc_InvalidRespHandler", "ctx_getResponseHandlerHelper", "");

		// get enviroment
		EnviromentHandler hndl = envHandlers_m.getItem(envHndlr);

		// get response
		return hndl.getResponseHandlers()->getItem(respHndlr);
		
	}

	

}; // end of namespace