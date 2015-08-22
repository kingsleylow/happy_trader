#ifndef _CPPUTILS_EXCEPT_INCLUDED
#define _CPPUTILS_EXCEPT_INCLUDED


#include "cpputilsdefs.hpp"

#include "log.hpp"
#include "reference.hpp"



namespace CppUtils {



/** Helper macro to throw an exception. */
#define THROW( EXC, MSG, CTX, ARG ) \
	{ \
		LOG( MSG_WARN, "EXCEPTION", #EXC << " : <" << MSG << "> in context <" << CTX << "> on argument <" << ARG << "> " ); \
		stringstream sr;	\
		sr << ARG << ends;	\
		throw EXC( MSG, CTX, sr.str() ); \
	}

#define EXC2STR( E ) \
	CppUtils::String("(") + E.message() + ") in context (" + E.context() + ") on argument (" + E.arg() + ")"




//=============================================================
// Class Exception
//=============================================================
/** Base class for all exceptions thrown.
		instance holds a string to carry an arbitrary message. In
		addition, each derived class overloads a virtual method to
		obtain the type of exception.

	
*/

#ifdef _DEBUG

// Derive from the exception base class from the test framework
class CPPUTILS_EXP Exception : public Testframe::Exception {

#else // _DEBUG

class CPPUTILS_EXP Exception {

#endif // _DEBUG


public:

	//======================================
	// Creators

	/** Constructor taking message, context and argument string. */
	Exception (
		String const &msg,
		String const &ctx,
		String const &arg
	);

	

	/** Copy constructor. */
	Exception (
		Exception const &src
	);

	/** Dtor. */
	~Exception(
	);


	//======================================
	// Methods

	/** Pure virtual function to retrieve a string
			describing the type of exception.

			\attention Each derived class must overload this
			function to return the type of exception in human-
			readable form.
	*/
	virtual String type (
	) const = 0;

	/** Function to access the message string
			associated with the exception object. */
	virtual String message (
	) const;

	/** Function to access the context string
			associated with the exception object. */
	virtual String context (
	) const;

	/** Function to access the argument string
			associated with the exception object. */
	virtual String arg (
	) const;


	/** Returns additional info depending on the exception type*/
	virtual CppUtils::String addInfo() const = 0;

protected:

	//======================================
	// Creators

	/** Standard ctor. */
	Exception (
	);


	//======================================
	// Variables

	/** Message string holding information on the exception. */
	String message_m;

	/** Context string. */
	String context_m;
	
	/** Argument string holding argument info. */
	String arg_m;
};








//=============================================================
// Class BadArgument
//=============================================================
/** Class for exceptions raised when a caller supplies an
		invalid argument, e.g. of wrong type.

		\author Olaf Lenzmann
*/
class CPPUTILS_EXP BadArgument : public Exception {

public:

	//======================================
	// Creators

	BadArgument (
		String const &msg,
		String const &ctx,
		String const &arg
	);

	


	//======================================
	// Methods

	String type (
	) const;


	virtual CppUtils::String addInfo() const;

protected:

	//======================================
	// Creators

	BadArgument (
	);


	//======================================
	// Variables
	//

	/** Exception type description. */
	static const String exceptionType_scm;
};






//=============================================================
// Class AccessRejected
//=============================================================
/** Class for exceptions raised when a caller does not have the
		required privileges to perform an operation. 

		\author Olaf Lenzmann
*/
class CPPUTILS_EXP AccessRejected : public Exception {

public:

	//======================================
	// Creators

	AccessRejected (
		String const &msg,
		String const &ctx,
		String const &arg
	);



	//======================================
	// Methods

	String type (
	) const;


	virtual CppUtils::String addInfo() const;

protected:

	//======================================
	// Creators

	AccessRejected (
	);


	//======================================
	// Variables

	/** Exception type description. */
	static const String exceptionType_scm;
};







//=============================================================
// Class NoSuchObject
//=============================================================
/** Class for exceptions raised when a caller references an object
		that does not exist. 

		\author Olaf Lenzmann
*/
class CPPUTILS_EXP NoSuchObject : public Exception {

public:

	//======================================
	// Creators
	//

	NoSuchObject (
		String const &msg,
		String const &ctx,
		String const &arg
	);

	

	//======================================
	// Methods

	String type (
	) const;

	virtual CppUtils::String addInfo() const;


protected:

	//======================================
	// Creators

	NoSuchObject (
	);


	//======================================
	// Variables

	/** Exception type description. */
	static const String exceptionType_scm;
};








//=============================================================
// Class OperationFailed
//=============================================================
/** Class for generic exceptions raised when an operation failed.

	
*/
class CPPUTILS_EXP OperationFailed : public Exception {

public:

	//======================================
	// Creators

	OperationFailed (
		String const &str,
		String const &ctx,
		String const &arg
	);



	//======================================
	// Methods

	String type (
	) const;


	virtual CppUtils::String addInfo() const;

protected:

	//======================================
	// Creators

	OperationFailed (
	);


	//======================================
	// Variables

	/** Exception type description. */
	static const String exceptionType_scm;
};







//=============================================================
// Class TransactionFailure
//=============================================================
/** Class for exceptions raised when a transaction fails.

		\author Olaf Lenzmann
*/
class CPPUTILS_EXP TransactionFailure : public Exception {

public:

	//======================================
	// Creators

	TransactionFailure (
		String const &str,
		String const &ctx,
		String const &arg
	);


	//======================================
	// Methods

	String type (
	) const;


	virtual CppUtils::String addInfo() const;

protected:

	//======================================
	// Creators

	TransactionFailure (
	);


	//======================================
	// Variables

	/** Exception type description. */
	static const String exceptionType_scm;
};







//=============================================================
// Class ExceptionInternal
//=============================================================
/** Class for all internal exceptions thrown in VPI software.
		Internal exceptions are to be raised when a problem occurrs
		that is not caused by improper use of the software or external
		situations, but by internal errors. Insofar, this exception
		relates to coding errors in VPI software.

		\author Olaf Lenzmann
*/
class CPPUTILS_EXP ExceptionInternal : public Exception {

public:

	//======================================
	// Creators

	ExceptionInternal (
		String const &msg,
		String const &ctx,
		String const &arg
	);




	//======================================
	// Methods

	String type (
	) const;


	virtual CppUtils::String addInfo() const;

protected:

	//======================================
	// Creators

	ExceptionInternal (
	);


	//======================================
	// Variables

	/** Exception type description. */
	static const String exceptionType_scm;
};






//=============================================================
// Class ResourceFailure
//=============================================================
/** Class for all exceptions due to the failure of a resource
		that the program depends on.

		\author Olaf Lenzmann
*/
class CPPUTILS_EXP ResourceFailure : public Exception {

public:

	//======================================
	// Creators

	ResourceFailure (
		String const &msg,
		String const &ctx,
		String const &arg
	);

	

	//======================================
	// Methods

	String type (
	) const;


	virtual CppUtils::String addInfo() const;

protected:

	//======================================
	// Creators

	ResourceFailure (
	);


	//======================================
	// Variables

	/** Exception type description. */
	static const String exceptionType_scm;
};







// =======================================================
// Class IntegrityViolation
//
/** Exception thrown whenever an attempt is made to change the program
		state which would result in an inconsistent state. 

    \author Olaf Lenzmann
*/
class CPPUTILS_EXP IntegrityViolation : public Exception {

public:

  //======================================
  // Creators
  //

  IntegrityViolation (
    String const &str,
		String const &ctx,
    String const &arg
  );

	

  //======================================
  // Methods

  String type (
  ) const;

	virtual CppUtils::String addInfo() const;

private:

	//======================================
	// Creators

	IntegrityViolation (
	);


  //======================================
  // Variables

  /** Exception type description. */
  static const String exceptionType_scm;
};






// End of namespace
}


#endif // _CPPUTILS_EXCEPT_INCLUDED
