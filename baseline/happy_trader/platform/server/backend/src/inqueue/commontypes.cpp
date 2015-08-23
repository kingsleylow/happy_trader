#include "commontypes.hpp"
#include "aggregatedcache.hpp"
#include "aholder.hpp"

namespace Inqueue {
	CalculatedFieldCallbackCallback::CalculatedFieldCallbackCallback(
			AggregatedCache& alist, 
			map<double,HlpStruct::PriceDataMapPtr* >::iterator const& curRowIt,
			int const multfactor,
			HlpStruct::AggregationPeriods const aggrperiod
	):
		alist_m(alist),
		curRowIt_m(curRowIt),
		multfactor_m(multfactor),
		aggrperiod_m(aggrperiod)
	{
	}

	CalculatedFieldCallbackCallback::~CalculatedFieldCallbackCallback()
	{
	}
	
	HlpStruct::PriceData const& CalculatedFieldCallbackCallback::locatePrevValidData( int const scookie ) const
	{
		return alist_m.locatePrevValidData(scookie, curRowIt_m);
	}

	HlpStruct::PriceData const& CalculatedFieldCallbackCallback::locateCurrentData( int const scookie ) const
	{
		return alist_m.locateCurrentData( scookie, curRowIt_m );
	}

	AggregationHolderList const& CalculatedFieldCallbackCallback::getAggregationHolderList(int const scookie) const
	{
		return alist_m.getAggregationHolderList( scookie );
	}

	void CalculatedFieldCallbackCallback::checkSymbolCached (
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			int &cookie
		)
	{
		double begindata, enddate;
		alist_m.checkSymbolCached(provider, symbol, aggrperiod_m, multfactor_m, cookie, begindata, enddate );
	}


};