#include "brkoper.hpp"
#include "broker.hpp"
#include "../inqueue/inqueue.hpp"

#define BRKOPER "BRKOPER"
namespace BrkLib {
		BrokerOperation::BrokerOperation(BrokerConnect& connect):
		brkConnect_m(connect)
		{
		}

		BrokerOperation::~BrokerOperation()
		{
		}

		double BrokerOperation::getBrokerSystemTime()
		{
			if (!isOperationAllowed())
				THROW(CppUtils::OperationFailed, "exc_BrokerSessionInvalid", "ctx_BrokerOperation", "" );

			return getBroker().getBrokerSystemTime();
		}

		// operations against broker
		bool BrokerOperation::unblockingIssueOrder(Order const& order)
		{
			if (!isOperationAllowed())
				THROW(CppUtils::OperationFailed, "exc_BrokerSessionInvalid", "ctx_BrokerOperation", "" );

			bool result =  getBroker().unblockingIssueOrder(getSession(), order);
		
		
			// log this
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL, BRKOPER, "Order was sent", getBroker().getName()  << result << order.toString());

			// and send the respective event
			ISEVENTSUBSCRIBED_BEGIN(HlpStruct::EventData::ET_BrokerOrderEvent)
				HlpStruct::EventData response;
				HlpStruct::XmlParameter xmlparam;

				response.getEventType() = HlpStruct::EventData::ET_BrokerOrderEvent;
				response.setType(HlpStruct::EventData::DT_CommonXml);
			
				order.toXmlParameter(response.getAsXmlParameter(true));

				// set up session
				response.setSource(getSession().getUid().toString());

				//LOG(MSG_INFO, BRKOPER, "Order with ID sent: " << order.ID_m.toString());
			
			ISEVENTSUBSCRIBED_END(response);

			return result;
		}

		bool BrokerOperation::getTickInfo(
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
		)
		{
			if (!isOperationAllowed())
				THROW(CppUtils::OperationFailed, "exc_BrokerSessionInvalid", "ctx_BrokerOperation", "" );

			bool result =  getBroker().getTickInfo(getSession(), symbolNum, symbolDenom);
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL, BRKOPER, "Ask for tick info", getBroker().getName()  << symbolNum << symbolDenom << result);
			return result;		


		}
		
		// subscribe for nes retreival
		bool BrokerOperation::subscribeForNews(
		)
		{
			if (!isOperationAllowed())
				THROW(CppUtils::OperationFailed, "exc_BrokerSessionInvalid", "ctx_BrokerOperation", "" );

			bool result = getBroker().subscribeForNews(getSession());
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL, BRKOPER, "Subscribe for news", getBroker().getName() );
			return result;


		}

		// returns last error information
		bool BrokerOperation::getLastErrorInfo(
		)
		{
			if (!isOperationAllowed())
				THROW(CppUtils::OperationFailed, "exc_BrokerSessionInvalid", "ctx_BrokerOperation", "" );

			bool result = getBroker().getLastErrorInfo(getSession());
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL, BRKOPER, "Ask for last error info", getBroker().getName() );
			return result;

		}

		// subscribe for quotes - they will arrive via onBrokerResponseArrived
		bool BrokerOperation::subscribeForQuotes(
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
		)
		{
			if (!isOperationAllowed())
				THROW(CppUtils::OperationFailed, "exc_BrokerSessionInvalid", "ctx_BrokerOperation", "" );

			bool result=  getBroker().subscribeForQuotes(getSession(), symbolNum, symbolDenom);
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL, BRKOPER, "Subscribe for quotes", getBroker().getName() << symbolNum << symbolDenom);
			return result;


		}

		// returns the history from the server
		bool BrokerOperation::getHistoryInfo(
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom,
			HlpStruct::AggregationPeriods aggrPeriod,
			int const multfactor
		)
		{
			if (!isOperationAllowed())
				THROW(CppUtils::OperationFailed, "exc_BrokerSessionInvalid", "ctx_BrokerOperation", "" );

			bool result= getBroker().getHistoryInfo(getSession(), symbolNum, symbolDenom,aggrPeriod, multfactor) ;
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL, BRKOPER, "Ask for history data", getBroker().getName() << symbolNum << symbolDenom << (const long)aggrPeriod << (const long)multfactor);
			return result;


		}

		void BrokerOperation::setUpMode(int mode)
		{
			if (!isOperationAllowed())
				THROW(CppUtils::OperationFailed, "exc_BrokerSessionInvalid", "ctx_BrokerOperation", "" );

			getBroker().setUpMode(mode);
			
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL, BRKOPER, "Mode set", getBroker().getName() << mode);

		}
		
		void BrokerOperation::getBrokerResponseList() const
		{
			if (!isOperationAllowed())
				THROW(CppUtils::OperationFailed, "exc_BrokerSessionInvalid", "ctx_BrokerOperation", "" );

			getBroker().getBrokerResponseList(getSession());
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL, BRKOPER, "Ask for response list data", getBroker().getName() );


		}
		
		void BrokerOperation::getBrokerResponseList(OperationResult const or) const
		{
			if (!isOperationAllowed())
				THROW(CppUtils::OperationFailed, "exc_BrokerSessionInvalid", "ctx_BrokerOperation", "" );

			getBroker().getBrokerResponseList(getSession(), or);
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL, BRKOPER, "Ask for response list data", getBroker().getName() << (long const)or);


		}

		
		
		void BrokerOperation::getBrokerPositionList(int const posStateQuery, PositionList &posList) const
		{
			if (!isOperationAllowed())
				THROW(CppUtils::OperationFailed, "exc_BrokerSessionInvalid", "ctx_BrokerOperation", "" );

			getBroker().getBrokerPositionList(posStateQuery, getSession(), posList);
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL, BRKOPER, "Returned position list", getBroker().getName() << HlpStruct::serializeList<BrkLib::Position>(posList) );

		}

		

		void BrokerOperation::registerDelayedOrder(DelayedOrderType const type, double const& execPrice, Order const& order, CppUtils::String const& comment)
		{
			if (!isOperationAllowed())
				THROW(CppUtils::OperationFailed, "exc_BrokerSessionInvalid", "ctx_BrokerOperation", "" );

			getBroker().registerDelayedOrder(getSession(), type, execPrice, order, comment );
		}

		bool BrokerOperation::removeDelayedOrder( CppUtils::Uid const& uid)
		{
			if (!isOperationAllowed())
				THROW(CppUtils::OperationFailed, "exc_BrokerSessionInvalid", "ctx_BrokerOperation", "" );

			return getBroker().removeDelayedOrder( getSession(), uid );
		}

		void BrokerOperation::removeAllDelayedOrders()
		{
			if (!isOperationAllowed())
				THROW(CppUtils::OperationFailed, "exc_BrokerSessionInvalid", "ctx_BrokerOperation", "" );

			getBroker().removeAllDelayedOrders(getSession());
		}


};