#ifndef _BRKLIB_BRKEXCEPTIONS_INCLUDED
#define _BRKLIB_BRKEXCEPTIONS_INCLUDED

#include "brklibdefs.hpp"
#include "brokerstruct.hpp"
#include "brklogger.hpp"


#define THROW_BRK( MODULE, MSG, CTX, ARG ) \
	{ \
		stringstream sr_arg;	\
		sr_arg << ARG;	\
		BRK_LOG( MODULE, BRK_MSG_WARN, "EXCEPTION", "BrokerException" << " : <" << MSG << "> in context <" << CTX << "> on argument:" <<	\
		"\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n" << \
		sr_arg.str() << \
		"\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n" ); \
		LOG( MSG_WARN, "EXCEPTION", "BrokerException" << " : <" << MSG << "> in context <" << CTX << ">" ); \
		throw BrkLib::BrokerException( MSG, CTX, sr_arg.str() ); \
	}

namespace BrkLib {


	// broker exception
	class BRKLIB_EXP BrokerException: public CppUtils::Exception
	{
	public:
		BrokerException (
			CppUtils::String const &msg,
			CppUtils::String const &ctx,
			CppUtils::String const &arg
		);

		
		CppUtils::String type () const;


		virtual CppUtils::String addInfo() const;
	};

}; // end of ns

#endif