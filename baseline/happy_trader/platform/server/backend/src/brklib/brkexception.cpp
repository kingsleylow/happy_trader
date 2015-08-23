#include "brkexception.hpp"

namespace BrkLib {
	BrokerException::BrokerException (
		CppUtils::String const &msg,
		CppUtils::String const &ctx,
		CppUtils::String const &arg
	) :
		Exception( msg, ctx, arg )
	{
	}

		CppUtils::String BrokerException::type (
	) const
	{
		return "Broker Exception";
	}


	CppUtils::String BrokerException::addInfo() const
	{
		return "N/A";
	}


};