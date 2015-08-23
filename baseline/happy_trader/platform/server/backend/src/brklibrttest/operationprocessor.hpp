#ifndef _BRKLIBRTTEST_OPERATIONPROCESSOR_INCLUDED
#define _BRKLIBRTTEST_OPERATIONPROCESSOR_INCLUDED

#include "brklibrttestdefs.hpp"
#include "positionholder.hpp"
#include "commonstruct.hpp"
#include "rtsession.hpp"

namespace BrkLib {
	class BrokerEngine;
	class OperationProcessor: public CppUtils::Mutex
	 {
	 public:

		 
		 OperationProcessor(BrokerEngine& brokerEngine, OperationProcessorCallBack& callback);
		 
		 ~OperationProcessor();

		 // if it is market operation
		 // just executes market order

		 // for pending orders - check if they can be executed and store if not
		 void processOperation(OrderOperation& oper);

		 void processOperation(Price const& p, OrderOperation& oper);

		 // checks pending operations and process them if necessary
		 void processPendingOperations(Price const& p);

		 BrokerEngine& getBrokerEngine()
		 {
			 return brokerEngine_m;
		 }

	 private:

		 // cancels order
		 void cancelOrder(OrderOperation const& oper, Price const& p);

		 void moveOrder(OrderOperation const& oper, Price const& p);

		 void checkCommonOperationFields(OrderOperation const& oper);

		 void checkLimitOperation(OrderOperation const& oper,Price const& p);

		 void checkStopOperation(OrderOperation const& oper,Price const& p);

		 void checkStopLimitOperation(OrderOperation const& oper,Price const& p);

	 private:


		 BrokerEngine& brokerEngine_m;

		 OperationProcessorCallBack& callback_m;

		 map<CppUtils::Uid, OrderOperation> pendingOps_m;

	 };
};

#endif