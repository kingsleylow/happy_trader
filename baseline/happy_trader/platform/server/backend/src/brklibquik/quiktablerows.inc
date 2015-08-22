// this is actual implementation of response generators


// --------------------------------------------


// actually not used
template <class T>
class RespGenerator 
{

public: 
	BrkLib::BrokerResponseOrder operator ()(vector<T> const& data, int const rbeg, int const cbeg, int const rend, int const cend);
	
};


// ------------------------------------------
// actual generators
template<>
class RespGenerator<QuikDeals>
{
	public: 
	BrkLib::BrokerResponseOrder operator ()(vector<QuikDeals> const& data, int const rbeg, int const cbeg, int const rend, int const cend)
	{
		BrkLib::BrokerResponseOrder bresp;
		
		return bresp;
	}
};

// ------------------------------------------
template<>
class RespGenerator<QuikExecutedAndCancelledOrders>
{
	public: 
	BrkLib::BrokerResponseOrder operator ()(vector<QuikExecutedAndCancelledOrders> const& data, int const rbeg, int const cbeg, int const rend, int const cend)
	{
		BrkLib::BrokerResponseOrder bresp;
		
		return bresp;
	}
};

// ------------------------------------------
template<>
class RespGenerator<QuikActiveOrders>
{
	public: 
	BrkLib::BrokerResponseOrder operator ()(vector<QuikActiveOrders> const& data, int const rbeg, int const cbeg, int const rend, int const cend)
	{
		BrkLib::BrokerResponseOrder bresp;
		
		return bresp;
	}
};

// ------------------------------------------
template<>
class RespGenerator<QuikStopOrders>
{
	public: 
	BrkLib::BrokerResponseOrder operator ()(vector<QuikStopOrders> const& data, int const rbeg, int const cbeg, int const rend, int const cend)
	{
		BrkLib::BrokerResponseOrder bresp;
		
		return bresp;
	}
};

// ------------------------------------------
template<>
class RespGenerator<QuikCancelledAndExecutedStopOrders>
{
	public: 
	BrkLib::BrokerResponseOrder operator ()(vector<QuikCancelledAndExecutedStopOrders> const& data, int const rbeg, int const cbeg, int const rend, int const cend)
	{
		BrkLib::BrokerResponseOrder bresp;
		
		return bresp;
	}
};
