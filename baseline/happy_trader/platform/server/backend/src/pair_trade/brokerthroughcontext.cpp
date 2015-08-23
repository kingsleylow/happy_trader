#include "brokerthroughcontext.hpp"

namespace AlgLib {
		// ---------------------------
	// context
	// ---------------------------

		MyBrokerContext::MyBrokerContext():
		issueAction_m(NULL)
		{
		}

		MyBrokerContext::MyBrokerContext(CppUtils::BaseAction<MachineContext> const& issueAction):
		issueAction_m(&issueAction)
		{
		}

		 MyBrokerContext::~MyBrokerContext()
		 {
		 }

		 BrkLib::BrokerContext* MyBrokerContext::clone() const
		{
			 MyBrokerContext* brkctx = new MyBrokerContext();
			 brkctx->issueAction_m = issueAction_m;
			
			 return brkctx;
		}

		 void MyBrokerContext::deallocate()
		 {
			 delete this;
		 }

		CppUtils::String MyBrokerContext::toString()	const
		{
			return getAction().toString();
		}

		CppUtils::BaseAction<MachineContext> const& MyBrokerContext::getAction() const
		{
			return *issueAction_m;
		}

};