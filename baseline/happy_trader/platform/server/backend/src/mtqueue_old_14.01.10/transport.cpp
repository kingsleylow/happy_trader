#include "transport.hpp"
#include "mtqueuemanager.hpp"


#define MTTRANSPORTLAYER "MTTRANSPORTLAYER"

namespace MtQueue {
	
	

	


	// ----------------

	TransportLayer::TransportLayer(MtQueueManager& base):
		base_m(base)
	{
	}
		
	TransportLayer::~TransportLayer()
	{
	}

	TransportLayer::Result TransportLayer::sendSimpleData(SimpleDataEntry const& data)
	{
		Result result;
		LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Starting sending simple data");

		if (data.data_m.size()  <=0)
			THROW(CppUtils::OperationFailed, "exc_InvalidData", "ctx_sendSimpleData", data.url_m );
		
		parsed_url* pu = NULL;

		try {
			pu = parse_url(data.url_m.c_str());
			if(!pu)
				THROW(CppUtils::OperationFailed, "exc_CannotParseURL", "ctx_sendSimpleData", data.url_m );
			
			LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Opening connection handle");
			
			HINTERNET  hInternet = InternetOpen(WININET_USER_AGENT, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
			if(hInternet)	{
				
				bool is_https = false;
				int port_i = 80;
				if (_stricmp(pu->scheme, "http")==0) {
					port_i = atoi(pu->port ? pu->port : "80");

				}
				else if (_stricmp(pu->scheme, "https")==0) {
					port_i = atoi(pu->port ? pu->port : "443");
					is_https = true;
				}
			
				LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Resolved port: " << port_i);

				CppUtils::String host_s = (pu->host ? pu->host : "");

				CppUtils::String path_s = (pu->path ? pu->path : "");
				CppUtils::String query_s = (pu->query ? pu->query : "");
				CppUtils::String path_query_s=  path_s + "?" + query_s;

				

				if (host_s.size() <=0)
					THROW(CppUtils::OperationFailed, "exc_InvalidHost", "ctx_sendSimpleData", "");

				LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Trying to connect to : " << host_s << ":" << port_i);
				HINTERNET hConnection = InternetConnect(hInternet, pu->host, port_i, "", "", INTERNET_SERVICE_HTTP, 0, NULL);
        if(hConnection) {
					
					// request

					LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Trying to open POST request: " << path_query_s);
					PCTSTR lplpszAcceptTypes[] = {"text/*", NULL};

					HINTERNET hRequest = HttpOpenRequest(hConnection, "POST", path_query_s.c_str(), NULL, NULL, lplpszAcceptTypes,
                INTERNET_FLAG_DONT_CACHE | 
								INTERNET_FLAG_IGNORE_CERT_CN_INVALID | 
								INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |
                INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP | 
								INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS | 
								INTERNET_FLAG_KEEP_CONNECTION |
                INTERNET_FLAG_NO_AUTH | 
								INTERNET_FLAG_NO_UI | 
								INTERNET_FLAG_PRAGMA_NOCACHE |
								INTERNET_FLAG_NO_CACHE_WRITE |
								( is_https? INTERNET_FLAG_SECURE: 0),
							0);
            
					if(hRequest){

							// set HTTP headers

							for(auto i2 = data.props_m.begin(); i2 != data.props_m.end(); i2++ ) {

								CppUtils::String header_i = i2->first + ":" + i2->second;
								if (!HttpAddRequestHeaders(hRequest, header_i.c_str(), header_i.length(), HTTP_ADDREQ_FLAG_ADD))
									THROW(CppUtils::OperationFailed, "exc_CannotAddHeaders", "ctx_sendSimpleData", header_i << " - " << CppUtils::getOSError());


								LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Set request header: " << header_i);
							}
						
							
							static char hdrs[] = "Content-Type: application/x-www-form-urlencoded";
							
							LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Going to send request, POST DATA: \n" << data.data_m );
							BOOL rc = HttpSendRequest(hRequest,hdrs, strlen(hdrs), (void*)data.data_m.c_str(), data.data_m.size());
							if (!rc)
								THROW(CppUtils::OperationFailed, "exc_CannotSendRequest", "ctx_sendSimpleData", CppUtils::getOSError());

							
						
							result.data() = readData(hRequest);
							CppUtils::String resultData;
							resultData.resize(result.data().length()+1);

							strncpy ((char*) resultData.c_str(), result.data().data(), result.data().length());
							resultData[resultData.length()] = '\0';
							
							LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Request sent ok:\nheaders:\n" << readHeaders(hRequest) << " data:\n" << resultData << "\n");

							// may go on
							// need to read response

							
							InternetCloseHandle(hRequest);
						}
						else
							THROW(CppUtils::OperationFailed, "exc_CannotOpenRequest", "ctx_sendSimpleData", CppUtils::getOSError());
				

					InternetCloseHandle(hConnection);
				}
				else
					THROW(CppUtils::OperationFailed, "exc_CannotEstablisInetConnection", "ctx_sendSimpleData", CppUtils::getOSError());
				
				
				InternetCloseHandle(hInternet);
			}
			else
				THROW(CppUtils::OperationFailed, "exc_CannotOpenInetHandle", "ctx_sendSimpleData", CppUtils::getOSError());


			
		}
		catch(CppUtils::Exception& e)
		{
			result.setUp(-1, e.message());
			LOG(MSG_WARN, MTTRANSPORTLAYER,"Data sending error: " << e.message());
		}
		catch(...)
		{
			result.setUp(-1, "Unknown failure");
			LOG(MSG_WARN, MTTRANSPORTLAYER,"Data sending unknown error");
		}

		if (pu)
			parsed_url_free( pu );

		LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Finish simple internet session with result: " << result.getMessage());

		return result;
	}

	TransportLayer::Result TransportLayer::sendData(DataToSend const& data)
	{
		Result result;
		LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Start sending event: [" << data.getEventId() << "]");

		FileDataEntry const& data_to_send = data.data();
		parsed_url* pu = NULL;

		try {
			pu = parse_url(data_to_send.url_m.c_str());
			if(!pu)
				THROW(CppUtils::OperationFailed, "exc_CannotParseURL", "ctx_sendData", data_to_send.url_m );
			
			LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Opening connection handle");
			HINTERNET  hInternet = InternetOpen(WININET_USER_AGENT, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
			if(hInternet)	{
				


				int port_i = atoi(pu->port);
				CppUtils::String host_s = (pu->host ? pu->host : "");
				CppUtils::String path_s = (pu->path ? pu->path : "");
				CppUtils::String query_s = (pu->query ? pu->query : "");
				CppUtils::String path_query_s=  path_s + "?" + query_s;

				if (host_s.size() <=0)
					THROW(CppUtils::OperationFailed, "exc_InvalidHost", "ctx_sendData", "");

				LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Trying to connect to : " << host_s << ":" << port_i);
				HINTERNET hConnection = InternetConnect(hInternet, pu->host, port_i, "", "", INTERNET_SERVICE_HTTP, 0, NULL);
        if(hConnection) {
					
					// request

					LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Trying to open POST request: " << path_query_s);
					PCTSTR lplpszAcceptTypes[] = {"text/*", NULL};

					HINTERNET hRequest = HttpOpenRequest(hConnection, "POST", path_query_s.c_str(), NULL, NULL, lplpszAcceptTypes,
                INTERNET_FLAG_DONT_CACHE | 
								INTERNET_FLAG_IGNORE_CERT_CN_INVALID | 
								INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |
                INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP | 
								INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS | 
								INTERNET_FLAG_KEEP_CONNECTION |
                INTERNET_FLAG_NO_AUTH | 
								INTERNET_FLAG_NO_UI | 
								INTERNET_FLAG_PRAGMA_NOCACHE |
								INTERNET_FLAG_NO_CACHE_WRITE,
							0);
            if(hRequest){

							LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Trying to send POST request");
							
							CppUtils::String strBoundary = "---------------------------autoupdater";
							CppUtils::String strContentHeader =  "Host: "+ host_s +"\r\n"
                              "Content-Type: multipart/form-data; boundary=";
							strContentHeader+=strBoundary;

							if (!HttpAddRequestHeaders(hRequest, strContentHeader.c_str(), strContentHeader.length(), HTTP_ADDREQ_FLAG_ADD))
								THROW(CppUtils::OperationFailed, "exc_CannotAddHeaders", "ctx_sendData", CppUtils::getOSError());

							CppUtils::String strHeaders = "--" + strBoundary +
											"\r\n" +
											"Content-Disposition: form-data; name=\"userfile\"; " +
											"filename=\""+data_to_send.fileName_m+"\"\r\n" +
											"Content-Type: application/octet-stream\r\n\r\n";
											
							 
							CppUtils::String s = 
								strHeaders+
								//append with file data:
								data_to_send.fileData_m +
								//trailing end of data:
								"\r\n--"+
								strBoundary+
								"--\r\n";
							
							
							BOOL rc = HttpSendRequest(hRequest,NULL,0, (void*)s.c_str(), s.length());
							if (!rc)
								THROW(CppUtils::OperationFailed, "exc_CannotSendREquest", "ctx_sendData", CppUtils::getOSError());

							
							
							LOG(MSG_DEBUG_DETAIL, MTTRANSPORTLAYER,"Request sent ok:\nheaders:\n" << readHeaders(hRequest) << " data:\n" << readData(hRequest).toString() << "\n");

							// may go on
							// need to read response

							
							InternetCloseHandle(hRequest);
						}
						else
							THROW(CppUtils::OperationFailed, "exc_CannotOpenRequest", "ctx_sendData", CppUtils::getOSError());
				

					InternetCloseHandle(hConnection);
				}
				else
					THROW(CppUtils::OperationFailed, "exc_CannotEstablisInetConnection", "ctx_sendData", CppUtils::getOSError());
				
				
				InternetCloseHandle(hInternet);
			}
			else
				THROW(CppUtils::OperationFailed, "exc_CannotOpenInetHandle", "ctx_sendData", CppUtils::getOSError());


			
		}
		catch(CppUtils::Exception& e)
		{
			result.setUp(-1, e.message());
			LOG(MSG_WARN, MTTRANSPORTLAYER,"Data sending error: " << e.message());
		}
		catch(...)
		{
			result.setUp(-1, "Unknown failure");
			LOG(MSG_WARN, MTTRANSPORTLAYER,"Data sending unknown error");
		}

		if (pu)
			parsed_url_free( pu );

		LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Finish internet session with event: [" << data.getEventId() << "] with result: " << result.getMessage());

		return result;
	}

	/**
	* Hepers
	*/

	CppUtils::String TransportLayer::readHeaders(HINTERNET hRequest) const
	{
		CppUtils::String result;
		DWORD dwInfoLevel=HTTP_QUERY_RAW_HEADERS_CRLF;

    DWORD dwInfoBufferLength=10;
    char* pInfoBuffer=(char*)malloc(dwInfoBufferLength+1);
    while(!HttpQueryInfo(hRequest,dwInfoLevel,pInfoBuffer,&dwInfoBufferLength,NULL)){
        if (GetLastError()==ERROR_INSUFFICIENT_BUFFER){
            free(pInfoBuffer);
						dwInfoBufferLength *= 2;
            pInfoBuffer=(char*)malloc(dwInfoBufferLength + 1);
        }
				else{
						THROW(CppUtils::OperationFailed, "exc_CannotreadHeaders", "ctx_readHeaders", CppUtils::getOSError());
           
        }
    }
    
		pInfoBuffer[dwInfoBufferLength] = '\0';
    result = pInfoBuffer;

		return result;
	}

	CppUtils::MemoryChunk TransportLayer::readData(HINTERNET hRequest) const
	{
							CppUtils::MemoryChunk retVal;
							DWORD availDataLen;
              char Buff[4096];
              DWORD readCount = ERROR_INTERNET_CONNECTION_RESET;

              BOOL qar = InternetQueryDataAvailable(hRequest, &availDataLen, 0, 0);
							if (!qar) 
								THROW(CppUtils::OperationFailed, "exc_InternetQueryDataAvailableFail", "ctx_readData", CppUtils::getOSError());
							
							LOG(MSG_DEBUG, MTTRANSPORTLAYER, "InternetQueryDataAvailable - available: " << availDataLen );


							if (availDataLen>0)
								retVal.length(availDataLen);
							else
								return retVal;

							int readTotalCount = 0;
							while(readTotalCount < availDataLen)
              {
                   BOOL rrf = InternetReadFile(hRequest, Buff, min(sizeof(Buff), availDataLen), &readCount);
									 if (!rrf) 
											THROW(CppUtils::OperationFailed, "exc_InternetReadFileFail", "ctx_readData", CppUtils::getOSError());

									if (readCount <=0)
										break;

                 
									memcpy(retVal.data() + readTotalCount, Buff, readCount );
                  readTotalCount += readCount;
									 
              }

							return retVal;
	}
};
