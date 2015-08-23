#ifndef _BACKEND_INQUEUE_COMMONTYPES_INCLUDED
#define _BACKEND_INQUEUE_COMMONTYPES_INCLUDED

/**
Here we defined common types participating in the project
*/

#include "inqueuedefs.hpp"

namespace Inqueue {

	typedef map<double, HlpStruct::PriceDataMapPtr* > RowPriceDataMap;

	class AggregatedCache;
	class AggregationHolderList;
	
	// this is a special collback to pass to the enxt class
	class INQUEUE_BCK_EXP CalculatedFieldCallbackCallback
	{
		friend class AggregatedCache;
	public:
		
		~CalculatedFieldCallbackCallback();

		// it locates previous valid data if they exists
		HlpStruct::PriceData const& locatePrevValidData( int const scookie ) const;	

		HlpStruct::PriceData const& locateCurrentData( int const scookie ) const;	

		AggregationHolderList const& getAggregationHolderList(int const scookie) const;

		// check if symbol cached
		void checkSymbolCached (
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			int &cookie
		);

	private:
		
		CalculatedFieldCallbackCallback(
			AggregatedCache& alist, 
			RowPriceDataMap::iterator const& curRowIt,
			int const multfactor,
			HlpStruct::AggregationPeriods const aggrperiod
		);

		RowPriceDataMap::iterator const& curRowIt_m;

		AggregatedCache& alist_m;

		int const multfactor_m;

		HlpStruct::AggregationPeriods const aggrperiod_m;

	};

	class INQUEUE_BCK_EXP OnCalculatedFieldCallback
		{
			public:
				// should we need to insert calculated data we 
				// need to set willInsert = true
				// and update with dataToInsert
				virtual void onNewRowAdded(
					CalculatedFieldCallbackCallback& callback,
					HlpStruct::GlobalPriceDataList const& row, 
					double const& ttime,
					HlpStruct::PriceData const& justUpdated,
					HlpStruct::GlobalPriceDataList& dataToInsert
				) = 0;
		};
	
	

}; // end of namespace







#endif
