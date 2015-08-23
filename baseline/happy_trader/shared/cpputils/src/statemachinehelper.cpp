#include "statemachinehelper.hpp"


namespace CppUtils {
	const String StateMachineException::exceptionType_scm = "State Machine Fatal Exception";

	StateMachineException::StateMachineException (
			String const &msg,
			String const &ctx,
			String const &arg
			): Exception(msg, ctx, arg) 
		{
		}

	String StateMachineException::addInfo() const
	{
		return "N/A";
	}
		
	String StateMachineException::type() const
	{
		return exceptionType_scm;
	}

}; // end of namespace