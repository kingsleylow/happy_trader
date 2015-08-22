#ifndef _SMARTCOM_ALG_MISHA2_BROKERTHROUGHCONTEXT_INCLUDED
#define _SMARTCOM_ALG_MISHA2_BROKERTHROUGHCONTEXT_INCLUDED

#include "smartcom_alg_misha2defs.hpp"
#include "statemachinecontext.hpp"

namespace AlgLib {
	/**
	* This class goes serves as a context which goes as input on broker ops and returned inside all broker callbacks
	*/

	class MyBrokerContext: public BrkLib::BrokerContext
	{
	public :
		MyBrokerContext();

		MyBrokerContext(CppUtils::BaseAction<MachineContext> const& issueAction);
		
		~MyBrokerContext();
		 

		virtual BrkLib::BrokerContext* clone() const;

		virtual void deallocate();

		virtual CppUtils::String toString()	const;

		CppUtils::BaseAction<MachineContext> const& getAction() const;


	private:
		
		CppUtils::BaseAction<MachineContext> const* issueAction_m;
		
	};
	
};

#endif