#ifndef _BRKLIBQUIK_WRAPTRANS2QUIK_INCLUDED
#define _BRKLIBQUIK_WRAPTRANS2QUIK_INCLUDED

// this is wrapper to QUIK API
#include "brklibquikdefs.hpp"

namespace BrkLib {

/** Helper macro to throw an exception. */
#define THROW_QUIK( MSG, CTX, ARG, QMSG, QCODE, QEXTCODE ) \
	{ \
		throw QuikApiException( MSG, CTX, ARG, QMSG, QCODE, QEXTCODE ); \
	}

	// ---------------------------------------


	class QuikApiException: public CppUtils::Exception
	{
		//======================================
	// Creators
	public:
	QuikApiException (
		CppUtils::String const &msg,
		CppUtils::String const &ctx,
		CppUtils::String const &arg,
		CppUtils::String const &qmsg,
		int const qcode,
		int const qextcode
	);

	QuikApiException (
		CppUtils::String const &msg,
		CppUtils::String const &ctx,
		long const arg,
		CppUtils::String const &qmsg,
		int const qcode,
		int const qextcode
	);

	QuikApiException (
		CppUtils::String const &msg,
		CppUtils::String const &ctx,
		float const arg,
		CppUtils::String const &qmsg,
		int const qcode,
		int const qextcode
	);


	//======================================
	// Methods

	CppUtils::String type (
	) const;

	virtual CppUtils::String addInfo() const;

	CppUtils::String const& getQuikError(
	) const;

	int getQuikCode(
	) const;

	int getQuikExtendedCode(
	) const;

protected:

	//======================================
	// Creators

	QuikApiException (
	);


	//======================================
	// Variables
	//

	/** Exception type description. */
	static const CppUtils::String exceptionType_scm;

	// quik error code 
	int quikErrorCode_m;

	CppUtils::String quikError_m;

	// extended code
	int extendedErrorCode_m;
};

// ----------------------------------------
// helper status struct
struct QuikApiStatus
{
	QuikApiStatus()
	{
		memset(this, '\0', sizeof(QuikApiStatus));
	}

	
		
	inline DWORD msgSize()
	{
		return sizeof(msg_m);
	}

	inline LPSTR msgPtr()
	{
		return msg_m;
	}

	inline long* extCodePtr()
	{
		return &code_m;
	}


	// --------------------------------

	void throw_exception(char const* funname)
	{
		THROW_QUIK("exc_QuikApiCall", "ctx_WrapTrans2Quik", funname, msg_m, code_m, extCode_m );
	}

	template<long R1>
	long check_non_equal(char const* FUN)
	{
		if (code_m != R1)	
			throw_exception( FUN );

		return code_m;
	}

	template<long R1, long R2>
	long check_non_equal(char const* FUN)
	{
		if (code_m != R1 && code_m != R2)	
				throw_exception( FUN );

		return code_m;
	}

	template<long R1, long R2, long R3>
	long check_non_equal(char const* FUN)
	{
		if (code_m != R1 && code_m != R2 && code_m != R3)	
				throw_exception( FUN );

		return code_m;
	}

	template<long R1>
	long check_equal(char const* FUN)
	{
		if (code_m == R1)	
				throw_exception( FUN );

		return code_m;
	}

	template<long R1, long R2>
	long check_equal(char const* FUN)
	{
		if (code_m == R1 || code_m == R2)	
				throw_exception( FUN );

		return code_m;
	}

	template<long R1, long R2, long R3>
	long check_equal(char const* FUN)
	{
		if (code_m == R1 || code_m == R2 || code_m == R3)	
				throw_exception( FUN );

		return code_m;
	}


	long no_check()
	{
		return code_m;
	}
	

	// -------------------------------
	// proptotypes
	QuikApiStatus& QuikApi_TRANS2QUIK_SEND_SYNC_TRANSACTION (
		LPSTR lpstTransactionString, 
		long* pnReplyCode, 
		PDWORD pdwTransId, 
		double* pdOrderNum, 
		LPSTR lpstrResultMessage, 
		DWORD dwResultMessageSize
	)
	{
		code_m = TRANS2QUIK_SEND_SYNC_TRANSACTION(
			lpstTransactionString,
			pnReplyCode, 
			pdwTransId,
			pdOrderNum, 
			lpstrResultMessage, 
			dwResultMessageSize, 
			extCodePtr(),
			msgPtr(), 
			msgSize()
		);

		return *this;
	}

	QuikApiStatus&  QuikApi_TRANS2QUIK_SEND_ASYNC_TRANSACTION (
		LPSTR lpstTransactionString

	)
	{
		code_m = TRANS2QUIK_SEND_ASYNC_TRANSACTION(lpstTransactionString, extCodePtr(), msgPtr(), msgSize() );
		return *this;
	}

	QuikApiStatus&  QuikApi_TRANS2QUIK_CONNECT (
		LPSTR lpstConnectionParamsString 
	)
	{
		code_m = TRANS2QUIK_CONNECT(lpstConnectionParamsString, extCodePtr(), msgPtr(), msgSize() );
		return *this;
	}

	QuikApiStatus&  QuikApi_TRANS2QUIK_DISCONNECT (
	)
	{
		code_m = TRANS2QUIK_DISCONNECT(extCodePtr(), msgPtr(), msgSize());
		return *this;
	}

	QuikApiStatus&  QuikApi_TRANS2QUIK_SET_CONNECTION_STATUS_CALLBACK (
		TRANS2QUIK_CONNECTION_STATUS_CALLBACK pfConnectionStatusCallback
	)
	{
		 code_m = TRANS2QUIK_SET_CONNECTION_STATUS_CALLBACK(pfConnectionStatusCallback, extCodePtr(), msgPtr(), msgSize()) ;
		 return *this;
	}

	QuikApiStatus&  QuikApi_TRANS2QUIK_SET_TRANSACTIONS_REPLY_CALLBACK (
		TRANS2QUIK_TRANSACTION_REPLY_CALLBACK pfTransactionReplyCallback
	)
	{
		 code_m = TRANS2QUIK_SET_TRANSACTIONS_REPLY_CALLBACK(pfTransactionReplyCallback, extCodePtr(), msgPtr(), msgSize()) ;
		 return *this;
	}

	QuikApiStatus&  QuikApi_TRANS2QUIK_IS_QUIK_CONNECTED (
		)
	{
		 code_m = TRANS2QUIK_IS_QUIK_CONNECTED(extCodePtr(), msgPtr(), msgSize()) ;
		 return *this;
	}

	QuikApiStatus&  QuikApi_TRANS2QUIK_IS_DLL_CONNECTED (
	
	)
	{
		code_m = TRANS2QUIK_IS_DLL_CONNECTED(extCodePtr(), msgPtr(), msgSize());
		return *this;
	}




	// -------------------------------

	// result code
	long code_m;
	
	// extended code
	long extCode_m;

	// extended message
	char msg_m[1024];

	
};

// ---------------------------------------



class QuikApiWrap
{
	public:
		QuikApiWrap();

		~QuikApiWrap();

		void qConnect(CppUtils::String const& connectString) const;

		// throws no exception
		bool isDllConnected() const;

		// throws no exception
		bool isQuikConnected() const;

		void qDisconnect() const;

		void qAsynchTransact(CppUtils::String const& transactStr) const;

		// virtual callbacks
		virtual void transactionCallback(
			long nTransactionResult, 
			long nTransactionExtendedErrorCode, 
			long nTransactionReplyCode, 
			DWORD dwTransId, 
			double dOrderNum, 
			LPCSTR lpcstrTransactionReplyMessage
		) = 0;

	
private:

	// QUIK callback
	void CALLBACK TRANS2QUIK_ConnectionStatusCallback (long nConnectionEvent, long nExtendedErrorCode, LPCSTR lpcstrInfoMessage);
	
	void CALLBACK TRANS2QUIK_TransactionsReplyCallback (
		long nTransactionResult, 
		long nTransactionExtendedErrorCode, 
		long nTransactionReplyCode, 
		DWORD dwTransId, 
		double dOrderNum, 
		LPCSTR lpcstrTransactionReplyMessage
	);


	// proxy to that
	CppUtils::stdcall_proxy call_TRANS2QUIK_ConnectionStatusCallback_m;

	CppUtils::stdcall_proxy call_TRANS2QUIK_TransactionsReplyCallback_m;
};


}; // end of namespace


#endif;