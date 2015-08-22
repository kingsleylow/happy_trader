#include "except.hpp"

#include "log.hpp"
#include "misc_ht.hpp"


namespace CppUtils {


#define EXCEPTION "EXCEPTION"


//=============================================================
// Class Exception
//=============================================================
Exception::Exception (
		String const &msg,
		String const &ctx,
		String const &arg
	) :
		message_m( msg ),
		context_m( ctx ),
		arg_m( arg )
{
}



//-------------------------------------------------------------
Exception::Exception (
		Exception const &src
	) :
		message_m( src.message_m ),
		context_m( src.context_m ),
		arg_m( src.arg_m )
{
}
//-------------------------------------------------------------
Exception::~Exception (
	)
{
}
//======================================
// Methods
//======================================
String Exception::message (
		void
	) const
{
	return message_m;
}
//-------------------------------------------------------------
String Exception::context (
		void
	) const
{
	return context_m;
}
//-------------------------------------------------------------
String Exception::arg (
		void
	) const
{
	return arg_m;
}





//=============================================================
// Class BadArgument
//=============================================================
const String BadArgument::exceptionType_scm = "Bad Argument";
//======================================
// Creators
//======================================
BadArgument::BadArgument (
		String const &msg,
		String const &ctx,
		String const &arg
	) :
		Exception( msg, ctx, arg )
{
}


//======================================
// Accessor
//======================================
String BadArgument::type (
	) const
{
	return exceptionType_scm;
}


CppUtils::String BadArgument::addInfo() const
{
	return "N/A";
}


//=============================================================
// Class AccessRejected
//=============================================================
const String AccessRejected::exceptionType_scm = "Access Rejected";
//======================================
// Creators
//======================================
AccessRejected::AccessRejected (
		String const &msg,
		String const &ctx,
		String const &arg
	) :
		Exception( msg, ctx, arg )
{
}


//======================================
// Accessor
//======================================
String AccessRejected::type (
	) const
{
	return exceptionType_scm;
}

CppUtils::String AccessRejected::addInfo() const
{
	return "N/A";
}








//=============================================================
// Class NoSuchObject
//=============================================================
const String NoSuchObject::exceptionType_scm = "No Such Object";
//======================================
// Creators
//======================================
NoSuchObject::NoSuchObject (
		String const &msg,
		String const &ctx,
		String const &arg
	) :
		Exception( msg, ctx, arg )
{
}


//======================================
// Accessor
//======================================
String NoSuchObject::type (
	) const
{
	return exceptionType_scm;
}


CppUtils::String NoSuchObject::addInfo() const
{
	return "N/A";
}






//=============================================================
// Class TransactionFailure
//=============================================================
const String TransactionFailure::exceptionType_scm = "Transaction Failure";
//======================================
// Creators
//======================================
TransactionFailure::TransactionFailure (
		String const &msg,
		String const &ctx,
		String const &arg
	) :
		Exception( msg, ctx, arg )
{
}


//======================================
// Accessor
//======================================
String TransactionFailure::type (
	) const
{
	return exceptionType_scm;
}


CppUtils::String TransactionFailure::addInfo() const
{
	return "N/A";
}





//=============================================================
// Class OperationFailed
//=============================================================
const String OperationFailed::exceptionType_scm = "Operation Failed";
//======================================
// Creators
//======================================
OperationFailed::OperationFailed (
		String const &msg,
		String const &ctx,
		String const &arg
	) :
		Exception( msg, ctx, arg )
{
}


//======================================
// Accessor
//======================================
String OperationFailed::type (
	) const
{
	return exceptionType_scm;
}

CppUtils::String OperationFailed::addInfo() const
{
	return "N/A";
}






//=============================================================
// Class ExceptionInternal
//=============================================================
const String ExceptionInternal::exceptionType_scm = "Internal Exception";
//======================================
// Creators
//======================================
ExceptionInternal::ExceptionInternal (
		String const &msg,
		String const &ctx,
		String const &arg
	) :
		Exception( msg, ctx, arg )
{
}


//======================================
// Methods
//======================================
String ExceptionInternal::type (
	) const
{
	return exceptionType_scm;
}

CppUtils::String ExceptionInternal::addInfo() const
{
	return "N/A";
}





//=============================================================
// Class ResourceFailure
//=============================================================
const String ResourceFailure::exceptionType_scm = "Resource Failure";
//======================================
// Creators
//======================================
ResourceFailure::ResourceFailure (
		String const &msg,
		String const &ctx,
		String const &arg
	) :
		Exception( msg, ctx, arg )
{
}


//======================================
// Methods
//======================================
String ResourceFailure::type (
	) const
{
	return exceptionType_scm;
}

CppUtils::String ResourceFailure::addInfo() const
{
	return "N/A";
}







// =======================================================
// Class IntegrityViolation
// =======================================================
const String IntegrityViolation::exceptionType_scm = "Integrity Violation";
//======================================
// Creators
//======================================
IntegrityViolation::IntegrityViolation (
		String const &msg,
		String const &ctx,
		String const &arg
	) :
		Exception( msg, ctx, arg )
{
}


//======================================
// Methods
//======================================
String IntegrityViolation::type (
  ) const
{
  return exceptionType_scm;
};

CppUtils::String IntegrityViolation::addInfo() const
{
	return "N/A";
}



// End of namespace
}