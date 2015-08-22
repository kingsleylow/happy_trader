#include "wraptrans2quik.hpp"

#define WRAPTRANS2QUIK "WRAPTRANS2QUIK"
namespace BrkLib {

// Helper defs 
// ------------------------------

#define CHECK_E1(CALLFUN, R1) QuikApiStatus().CALLFUN.check_equal<R1>(__FUNCTION__)
#define CHECK_E2(CALLFUN, R1, R2)	QuikApiStatus().CALLFUN.check_equal<R1, R2>(__FUNCTION__)
#define CHECK_E3(CALLFUN, R1, R2, R3)	QuikApiStatus().CALLFUN.check_equal<R1, R2, R3>(__FUNCTION__)

#define CHECK_NE1(CALLFUN, R1)	QuikApiStatus().CALLFUN.check_non_equal< R1>(__FUNCTION__)
#define CHECK_NE2(CALLFUN, R1, R2)	QuikApiStatus().CALLFUN.check_non_equal<R1, R2>(__FUNCTION__)
#define CHECK_NE3(CALLFUN, R1, R2, R3)	QuikApiStatus().CALLFUN.check_non_equal<R1, R2, R3>(__FUNCTION__)

#define CHECK_NO(CALLFUN) QuikApiStatus().CALLFUN.no_check()

// ------------------------------

const CppUtils::String QuikApiException::exceptionType_scm = "Quik API exception";

// ------------------------------

QuikApiException::QuikApiException (
		CppUtils::String const &msg,
		CppUtils::String const &ctx,
		CppUtils::String const &arg,
		CppUtils::String const &qmsg,
		int const qcode,
		int const qextcode
):
CppUtils::Exception( msg, ctx, arg ),
quikError_m(qmsg ),
quikErrorCode_m( qcode ),
extendedErrorCode_m( qextcode)
{
}

QuikApiException::QuikApiException (
		CppUtils::String const &msg,
		CppUtils::String const &ctx,
		long const arg,
		CppUtils::String const &qmsg,
		int const qcode,
		int const qextcode
) : 
	CppUtils::Exception( msg, ctx, arg ),
	quikError_m(qmsg ),
	quikErrorCode_m( qcode ),
	extendedErrorCode_m( qextcode)
{
}

QuikApiException::QuikApiException (
		CppUtils::String const &msg,
		CppUtils::String const &ctx,
		float const arg,
		CppUtils::String const &qmsg,
		int const qcode,
		int const qextcode
):
	CppUtils::Exception( msg, ctx, arg ),
	quikError_m(qmsg ),
	quikErrorCode_m( qcode ),
	extendedErrorCode_m( qextcode)
{
	
}

CppUtils::String const& QuikApiException::getQuikError(
) const
{

	return quikError_m;
}

int QuikApiException::getQuikCode(
) const
{
	return quikErrorCode_m;
}

int QuikApiException::getQuikExtendedCode(
	) const
{
	return extendedErrorCode_m;
}

CppUtils::String QuikApiException::type (
	) const
{
	return exceptionType_scm;
}

CppUtils::String QuikApiException::addInfo() const
{
	CppUtils::String result = CppUtils::String("Quik API code: ");
	result += CppUtils::long2String(quikErrorCode_m);
	result += " extended code: ";
	result += CppUtils::long2String(extendedErrorCode_m);
	result += " error message: ";
	result +=  quikError_m;
  
	return result;
}

// -----------------------------------------

QuikApiWrap::QuikApiWrap()
{
	// init this library
	// set connect callback


	// transaction callback
	CHECK_NE1 (
		QuikApi_TRANS2QUIK_SET_TRANSACTIONS_REPLY_CALLBACK(
			call_TRANS2QUIK_TransactionsReplyCallback_m.bind<QuikApiWrap, void, long,long,long,DWORD,double,LPCSTR >(this, &QuikApiWrap::TRANS2QUIK_TransactionsReplyCallback)
		),
		TRANS2QUIK_SUCCESS
	);

	// connection callback
	CHECK_NE1 (
		QuikApi_TRANS2QUIK_SET_CONNECTION_STATUS_CALLBACK(
			call_TRANS2QUIK_ConnectionStatusCallback_m.bind<QuikApiWrap, void, long , long, LPCSTR >(this, &QuikApiWrap::TRANS2QUIK_ConnectionStatusCallback)
		),
		TRANS2QUIK_SUCCESS
	);

		
	
}

QuikApiWrap::~QuikApiWrap()
{
}

void QuikApiWrap::qConnect(CppUtils::String const& connectString) const
{
	
	CHECK_NE2 (
		QuikApi_TRANS2QUIK_CONNECT( (LPSTR)connectString.c_str() ),
		TRANS2QUIK_SUCCESS, 
		TRANS2QUIK_ALREADY_CONNECTED_TO_QUIK
	);
	
}

bool QuikApiWrap::isDllConnected() const
{
	long result = CHECK_NO(
		QuikApi_TRANS2QUIK_IS_DLL_CONNECTED()
	);

	if (result == TRANS2QUIK_DLL_CONNECTED)
		return true;

	return false;
}

bool QuikApiWrap::isQuikConnected() const
{
	long result = CHECK_NO(
		QuikApi_TRANS2QUIK_IS_QUIK_CONNECTED()
	);

	if (result == TRANS2QUIK_QUIK_CONNECTED )
		return true;

	return false;
}

void QuikApiWrap::qDisconnect() const
{	
	
	CHECK_E1(
		QuikApi_TRANS2QUIK_DISCONNECT(),
		TRANS2QUIK_FAILED
	);
	
	
};

void QuikApiWrap::qAsynchTransact(CppUtils::String const& transactStr) const
{
		CHECK_NE1 (
			QuikApi_TRANS2QUIK_SEND_ASYNC_TRANSACTION((LPSTR)transactStr.c_str()),
			TRANS2QUIK_SUCCESS
		);
}





// ----------------------------------

void QuikApiWrap::TRANS2QUIK_ConnectionStatusCallback (long nConnectionEvent, long nExtendedErrorCode, LPCSTR lpcstrInfoMessage)
{
	/*
	if (nConnectionEvent == TRANS2QUIK_DLL_CONNECTED);

	if (nConnectionEvent == TRANS2QUIK_DLL_DISCONNECTED);

	if (nConnectionEvent == TRANS2QUIK_QUIK_CONNECTED);
		
	if (nConnectionEvent == TRANS2QUIK_QUIK_DISCONNECTED);
	*/
		
}


void QuikApiWrap::TRANS2QUIK_TransactionsReplyCallback (
	long nTransactionResult, 
	long nTransactionExtendedErrorCode, 
	long nTransactionReplyCode, 
	DWORD dwTransId, 
	double dOrderNum, 
	LPCSTR lpcstrTransactionReplyMessage
)
{
	try {
		transactionCallback( 
			nTransactionResult, 
			nTransactionExtendedErrorCode, 
			nTransactionReplyCode, 
			dwTransId, 
			dOrderNum, 
			lpcstrTransactionReplyMessage 
		);
	
	}
	catch(CppUtils::Exception& e)
	{
		LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, WRAPTRANS2QUIK, "Exception in transaction callback",  e.message() << e.arg() << e.context() << e.addInfo() );

	}
	catch(...)
	{
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, WRAPTRANS2QUIK, "Unknown exception in transaction callback");

	}
	
}




}; // end of namespace