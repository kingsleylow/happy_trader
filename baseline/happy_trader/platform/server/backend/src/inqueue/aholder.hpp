#ifndef _BACKEND_INQUEUE_AHOLDERHDR_INCLUDED
#define _BACKEND_INQUEUE_AHOLDERHDR_INCLUDED

#include "inqueuedefs.hpp"
#include "commontypes.hpp"
#include <deque>


namespace Inqueue {

	class EngineThread;
	// helper structure to store aggregation data information and allowing to work with this in multithread mode
	// it must be locked by extrenal mutex
	class AggregationHolderList {

	public:
		AggregationHolderList();
		
		void clear();

		void clearOnlyPriceData();

		// returns the idx equal the time or the first less than passed time
		// returns -1 if cannot find
		
		long find(double const timestamp, bool exact = false) const;

		bool isValid() const;
		
		double getBeginDate() const;
		
		double getEndDate() const;
				
		// accessors
		inline long getListSize() const
		{
			return dataList_m.size(); 
		}

		inline HlpStruct::PriceData& getPriceData(long const idx)
		{
			//if (idx >=dataList_m.size())
			//	THROW(CppUtils::OperationFailed, "exc_InvalidIndex", "ctx_getHistoryPriceData", idx);

			HTASSERT(idx < dataList_m.size());
			return dataList_m[ idx ];
		}

		inline HlpStruct::PriceData const& getPriceData(long const idx) const
		{
			//if (idx >=dataList_m.size())
			//	THROW(CppUtils::OperationFailed, "exc_InvalidIndex", "ctx_getHistoryPriceData", idx);

			HTASSERT(idx < dataList_m.size());
			return dataList_m[ idx ];
		}

		inline  HlpStruct::PriceData& getFisrtPriceData()
		{
			static HlpStruct::PriceData dummy;
			
			if ( dataList_m.size() == 0)
				return dummy;

			return dataList_m[ 0 ];	
		}

		inline HlpStruct::PriceData& getLastPriceData()
		{
			static HlpStruct::PriceData dummy;

			long idx = dataList_m.size() - 1;
			//if (idx < 0)
			//	THROW(CppUtils::OperationFailed, "exc_ListIsEmpty", "ctx_getHistoryPriceData", "");

			// dummy
			if (idx < 0)
				return dummy;

			return dataList_m[ idx ];
		}

		inline HlpStruct::PriceData const& getLastPriceData() const
		{
			long idx = dataList_m.size() - 1;
			if (idx < 0)
				THROW(CppUtils::OperationFailed, "exc_ListIsEmpty", "ctx_getHistoryPriceData", "");

			return dataList_m[ idx ];
		}

		inline void addPriceDataToEnd(HlpStruct::PriceData const& pricedata)
		{
			dataList_m.push_back(pricedata);
		}

		// add price data to the fromnt
		inline void addPriceDataToFront(HlpStruct::PriceData const& pricedata)
		{
			dataList_m.push_front(pricedata);
		}

		// this add price data to the middle just making a search
		// return a reference to newly inserted entry
		HlpStruct::PriceData& addPriceDataToAnyPlace(HlpStruct::PriceData const& pricedata);

		// this make a search but substitute existiong element
		void addOrUpdatePriceData(HlpStruct::PriceData const& pricedata);

		HlpStruct::PriceData* exactFindEntry(double const& ttime);

		inline HlpStruct::AggregationPeriods getAggregationPeriod() const
		{
			return aggrPeriod_m;
		}

		inline int getMultFactor() const
		{
			return multfactor_m;
		}

		inline CppUtils::String& getProvider() {
			return provider_m;
		}

		inline CppUtils::String& getSymbol() {
			return symbol_m;
		}

		inline CppUtils::String const& getProvider() const {
			return provider_m;
		}

		inline CppUtils::String const& getSymbol() const{
			return symbol_m;
		}

		inline int& getCookie() {
			return cookie_m;
		}

			
		void initAggrPeriodMultFactor(HlpStruct::AggregationPeriods const aggrperiod, int const multfactor);

		// members
	public:

		static int id_m;

		
		// 
		CppUtils::UnsignedLongLong lastSequentionNum_m;

		tm lastTimeOfCandle_m;
				

	private:

		// symbol id
		int cookie_m;

		// symbol and provider
		CppUtils::String provider_m;

		CppUtils::String symbol_m;

		// aggregation parameters
		HlpStruct::AggregationPeriods aggrPeriod_m;

		int multfactor_m;
		
		// good point for optimization
		deque<HlpStruct::PriceData> dataList_m;

		
	};

	
}; // end of namespace

#endif