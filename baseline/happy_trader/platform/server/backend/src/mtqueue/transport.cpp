#include "transport.hpp"
#include "mtqueuemanager.hpp"

#define _CRT_SECURE_NO_DEPRECATE
#define CRYPTOPP_DEFAULT_NO_DLL
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include "shared/crypto/config.h"
#include "shared/crypto/misc.h"
#include "shared/crypto/hex.h"
#include "shared/crypto/cryptlib.h"
#include "shared/crypto/default.h"
#include "shared/crypto/md5.h"


#define MTTRANSPORTLAYER "MTTRANSPORTLAYER"
#define WININET_USER_AGENT "Mozilla/5.0 (iPad; U; CPU OS 3_2_1 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Mobile/7B405"
#define BTC_GATE_DEF "https://btc-e.com/tapi"

namespace MtQueue {






	// ----------------

	TransportLayer::TransportLayer( )
	{
	}

	TransportLayer::~TransportLayer()
	{
	}

	void TransportLayer::init(CppUtils::String const& key, CppUtils::String const& secret, CppUtils::String const btcGate)
	{
		key_m = key;
		secret_m  =secret;
		btcGate_m =btcGate;

#ifdef DEMO_VERSION
		requestsCnt_m = 0;
#endif

		if (key_m.size() <=0)
			THROW(CppUtils::OperationFailed, "exc_InvalidKeyData", "ctx_init", "" );

		if (secret_m.size() <=0)
			THROW(CppUtils::OperationFailed, "exc_InvalidSecretData", "ctx_init", "" );

		if (btcGate_m.size() <=0) {
			btcGate_m = BTC_GATE_DEF;
		}

		LOG(MSG_INFO, MTTRANSPORTLAYER, "BTC gate was resolved to: " << btcGate_m );
	}

	Result TransportLayer::makeBtcRequest(CppUtils::String const& methodName, char const* nonce, CppUtils::String const& additionalParameters)
	{
		

		LOG(MSG_DEBUG, MTTRANSPORTLAYER, "Calling "<< methodName );
		SimpleDataEntry se;
		se.url_m= btcGate_m;

		if (methodName.size() <=0)
			THROW(CppUtils::OperationFailed, "exc_InvalidMethod", "ctx_makeBtcRequest", "");

		CppUtils::String nonce_s = nonce ? nonce : "";

		CppUtils::String queryString="nonce=";
		queryString.append(nonce_s.size() > 0 ? nonce_s : CppUtils::long2String(CppUtils::getTime()));
		queryString.append("&method=").append(methodName);
		if (additionalParameters.size() > 0)
			queryString.append("&").append(additionalParameters);
		
		LOG(MSG_DEBUG, MTTRANSPORTLAYER, "Query string generated: " << queryString);

		CppUtils::String signature = CppUtils::toLowerCase(signRequest(queryString, secret_m));

		// need to sign queryString with secret_s

		se.props_m["Key"] =  key_m;
		se.props_m["Sign"]= signature;

		// this is post data
		se.data_m = queryString;
		
		Result res = sendSimpleData(se);
		return res;
	}

	Result TransportLayer::sendSimpleData(SimpleDataEntry const& data)
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



						readDataWithoutContentLength(hRequest, result.data());
						result.convertToStringResponse();
						
						LOG(MSG_DEBUG, MTTRANSPORTLAYER,"Request sent OK, headers:\n" << readHeaders(hRequest) << "\n,data:\n" << result.strData() << "\n");

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


	CppUtils::String TransportLayer::signRequest(CppUtils::String const& requestString, CppUtils::String const& secret)
	{
		CppUtils::String mac_print;
		try
		{
			
			CryptoPP::HMAC< CryptoPP::SHA512 > hma;
			hma.SetKey((byte*)secret.c_str(), secret.size());

			CppUtils::String mac;
			CryptoPP::StringSource(requestString, true,  new CryptoPP::HashFilter(hma, new CryptoPP::StringSink(mac) ) ); // StringSource
			CryptoPP::StringSource(mac, true, new CryptoPP::HexEncoder( new CryptoPP::StringSink(mac_print)  ) ); 

		}
		catch(const CryptoPP::Exception& e)
		{
			THROW(CppUtils::OperationFailed, "exc_SignRequestError", "ctx_signRequest", e.what());
		}
		
		return mac_print;
	}

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

	void TransportLayer::readData(HINTERNET hRequest, CppUtils::MemoryChunk &retVal) const
	{
	
		DWORD availDataLen;
		char Buff[4096];
		DWORD readCount = ERROR_INTERNET_CONNECTION_RESET;

		BOOL qar = InternetQueryDataAvailable(hRequest, &availDataLen, 0, 0);
		if (!qar) 
			THROW(CppUtils::OperationFailed, "exc_InternetQueryDataAvailableFail", "ctx_readData", CppUtils::getOSError());

		LOG(MSG_DEBUG, MTTRANSPORTLAYER, "InternetQueryDataAvailable(...) - available length: " << availDataLen );


		if (availDataLen>0)
			retVal.length(availDataLen);
		else {
			retVal.length(0);
			return;
		}

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

	
	}

	void TransportLayer::readDataWithoutContentLength(HINTERNET hRequest, CppUtils::MemoryChunk &retVal) const
	{
#ifdef DEMO_VERSION
		requestsCnt_m++;
		if (requestsCnt_m > MAX_REQUESTS) {
			LOG(MSG_WARN, MTTRANSPORTLAYER,"readDataWithoutContentLength(...) failed  - DVR error");
			return;
		}

#endif

		char Buff[4096];
		DWORD readCount = 0;

		retVal.length(0);

		int readTotalCount = 0;
		while(true)
		{
			BOOL rrf = InternetReadFile(hRequest, Buff, 4096, &readCount);
			if (!rrf) 
				THROW(CppUtils::OperationFailed, "exc_InternetReadFileFail", "ctx_readData", CppUtils::getOSError());

			if (readCount <=0)
				break;

			// resize
			retVal.length(readTotalCount + readCount);


			memcpy(retVal.data() + readTotalCount, Buff, readCount );
			readTotalCount += readCount;

		}
	}
};
