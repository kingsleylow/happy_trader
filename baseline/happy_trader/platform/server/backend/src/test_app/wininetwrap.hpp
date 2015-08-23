#ifndef _WININET_WRAP_HPP
#define _WININET_WRAP_HPP


// We compile for NT4.0 SP3
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN


//#include <specstrings.h> // declares "headre annotations"
#include <Winhttp.h>

#include "shared/testframe/src/testframe.hpp"
#include "shared/cpputils/src/cpputils.hpp"

LPSTR UnicodeToAnsi(LPCWSTR s);



// this class is intended to parse the received data



// ------------------------------------

class SimpleBuffer {
public:

	SimpleBuffer():
		buf_m(0),
		sz_m(0)
	{

	}

	~SimpleBuffer()
	{
		if (buf_m) {
			delete [] buf_m;
			sz_m = 0;
			buf_m = 0;
		}
	}

	void Initialize(DWORD sz)
	{
		if (!buf_m) {
			buf_m = new BYTE[sz];
			sz_m = sz;
		}
	}

	LPVOID GetData()
	{
		return buf_m;
	}

	DWORD GetCount()
	{
		return sz_m;
	}


private:

	BYTE* buf_m;

	DWORD sz_m;

};


// -----------------

class WinHttpHandle
{
public:
    WinHttpHandle() :
        m_handle(0)
    {}

    ~WinHttpHandle()
    {
        Close();
    }

    bool Attach(HINTERNET handle)
    {
        ASSERT(0 == m_handle );
        m_handle = handle;
        return 0 != m_handle;
    }

    HINTERNET Detach()
    {
        HANDLE handle = m_handle;
        m_handle = 0;
        return handle;
    }

    virtual void Close()
    {
        if (0 != m_handle)
        {   
            ::WinHttpCloseHandle(m_handle);
            m_handle = 0;
        }
    }

    HRESULT SetOption(DWORD option,
                      const void* value,
                      DWORD length)
    {
        if (!::WinHttpSetOption(m_handle,
                                option,
                                const_cast<void*>(value),
                                length))
        {
            return HRESULT_FROM_WIN32(::GetLastError());
        }

        return S_OK;
    }

    HRESULT QueryOption(DWORD option,
                        void* value,
                        DWORD& length) const
    {
        if (!::WinHttpQueryOption(m_handle,
                                  option,
                                  value,
                                  &length))
        {
            return HRESULT_FROM_WIN32(::GetLastError());
        }

        return S_OK;
    }

    HINTERNET m_handle;
};


// -----------------

class WinHttpSession : public WinHttpHandle
{
public:
    HRESULT Initialize()
    {
        if (!Attach(::WinHttpOpen(0, // no agent string
                                  WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                  WINHTTP_NO_PROXY_NAME,
                                  WINHTTP_NO_PROXY_BYPASS,
                                  WINHTTP_FLAG_ASYNC)))
        {
            return HRESULT_FROM_WIN32(::GetLastError());
        }

        return S_OK;
    }
};


// -----------------

class WinHttpConnection : public WinHttpHandle
{
public:
    HRESULT Initialize(PCWSTR serverName,
                       INTERNET_PORT portNumber,
                       const WinHttpSession& session)
    {
        if (!Attach(::WinHttpConnect(session.m_handle,
                                     serverName,
                                     portNumber,
                                     0))) // reserved
        {
            return HRESULT_FROM_WIN32(::GetLastError());
        }

        return S_OK;
    }
};


// -----------------

template <typename T, int bufferlength>
class WinHttpRequest : public WinHttpHandle
{
public:

		

    HRESULT Initialize(PCWSTR path,
                       PCWSTR verb,
                       const WinHttpConnection& connection)
    {
        m_buffer.Initialize(bufferlength);

        // Call WinHttpOpenRequest and WinHttpSetStatusCallback.
				m_handle = ::WinHttpOpenRequest(connection.m_handle,
																				 verb? verb: 0, // use GET as the default verb
                                         path,
                                         0, // use HTTP version 1.1
                                         WINHTTP_NO_REFERER,
                                         WINHTTP_DEFAULT_ACCEPT_TYPES,
                                         0); // flags
				
				if (!m_handle) 
				{
					return HRESULT_FROM_WIN32(::GetLastError());
				}

				if (WINHTTP_INVALID_STATUS_CALLBACK==::WinHttpSetStatusCallback(m_handle,  Callback, WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS,  0)) // reserved
				{
					// Call GetLastError for error information
					return HRESULT_FROM_WIN32(::GetLastError());

				}

			
				return S_OK;

    }

    HRESULT SendRequest(__in_opt PCWSTR headers,
                        DWORD headersLength,
                        __in_opt LPVOID optional,
                        DWORD optionalLength,
                        DWORD totalLength)
    {
        T* pT = static_cast<T*>(this);

        // Call WinHttpSendRequest with pT as the context value.
				if (!::WinHttpSendRequest(m_handle,
													headers? headers : WINHTTP_NO_ADDITIONAL_HEADERS, // if to add headers
													headers? -1L: 0, // of there are headers  - assume null terminated
													optional? optional : WINHTTP_NO_REQUEST_DATA, // send if optional headers
													optional? optionalLength: 0, // request data length
                          totalLength, // total length
                          (DWORD_PTR)pT)) // context
				{
					// Call GetLastError for error information
					return HRESULT_FROM_WIN32(::GetLastError());
				}

				return S_OK;

    }

		
		HRESULT cancelRequest()
		{
			// reset callback
			if (WINHTTP_INVALID_STATUS_CALLBACK==::WinHttpSetStatusCallback(m_handle,  0, WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS,  0)) // reserved
			{
				// Call GetLastError for error information
				return HRESULT_FROM_WIN32(::GetLastError());

			}

			// close handle
			__super::Close();

			return S_OK;

		}

protected:
    static void CALLBACK Callback(HINTERNET handle,
                                  DWORD_PTR context,
                                  DWORD code,
                                  void* info,
                                  DWORD length)
    {
        if (0 != context)
        {
            T* pT = reinterpret_cast<T*>(context);

            HRESULT result = pT->OnCallback(code,
                                            info,
                                            length);

            if (FAILED(result))
            {
                pT->OnResponseComplete(result);
            }
        }
    }

    HRESULT OnCallback(DWORD code,
                       const void* info,
                       DWORD length)
    {
        T* pT = static_cast<T*>(this);
			
				switch (code)
				{

					case WINHTTP_CALLBACK_STATUS_RESOLVING_NAME:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_NAME_RESOLVED:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_CONNECTING_TO_SERVER:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_CONNECTED_TO_SERVER:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_SENDING_REQUEST:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_REQUEST_SENT:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_CONNECTION_CLOSED:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_HANDLE_CREATED:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_DETECTING_PROXY:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_REDIRECT:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_INTERMEDIATE_RESPONSE:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_SECURE_FAILURE:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
					{
						break;
					}

					case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
					{
						break;
					}
					// sent request
					// or handle POST request
					case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
					case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
					{
							HRESULT result = pT->OnWriteData();

							if (FAILED(result))
							{
									return result;
							}

							if (S_FALSE == result)
							{
									if (!::WinHttpReceiveResponse(m_handle,0)) 
									{
											return HRESULT_FROM_WIN32(::GetLastError());
									}
							}

							break;
					}


					// headers arrived
					case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
					{
							DWORD statusCode = 0;
							DWORD statusCodeSize = sizeof(DWORD);

							if (!::WinHttpQueryHeaders(m_handle,
														WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
														WINHTTP_HEADER_NAME_BY_INDEX,
														&statusCode,
														&statusCodeSize,
														WINHTTP_NO_HEADER_INDEX))
							{
									return HRESULT_FROM_WIN32(::GetLastError());
							}

							if (HTTP_STATUS_OK != statusCode)
							{
								// read what happened
	
								/*
								char buf[1024];
								DWORD buflen = 1024-2;
								if (!::WinHttpQueryHeaders(m_handle,
														WINHTTP_QUERY_RAW_HEADERS,
														0,
														buf,
														&buflen,
														WINHTTP_NO_HEADER_INDEX))
								{
									return HRESULT_FROM_WIN32(::GetLastError());
								}


								char * txt = UnicodeToAnsi((LPCWSTR)buf);
								*/
								return E_FAIL;
							}

							if (!::WinHttpReadData(m_handle,
														m_buffer.GetData(),
														m_buffer.GetCount(),
														0)) // async result
							{
									return HRESULT_FROM_WIN32(::GetLastError());
							}

							break;
					}

					
					case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
					{
							if (0 < length)
							{
									HRESULT r = pT->OnReadComplete(m_buffer.GetData(), length);

									if (FAILED(r))
										return r;

									if (!::WinHttpReadData(m_handle,
																				m_buffer.GetData(),
																				m_buffer.GetCount(),
																				0)) // async result
									{
											return HRESULT_FROM_WIN32(::GetLastError());
									}
							}
							else
							{
									pT->OnResponseComplete(S_OK);
							}

							break;
					}


					default:
					{
							return S_FALSE;
					}
				}

				return S_OK;

    }

    SimpleBuffer m_buffer;

		
};





// -----------------

// -----------------
#endif