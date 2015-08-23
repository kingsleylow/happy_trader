#ifndef _BACKEND_INQUEUE_AGGREGATEDCACHE_INCLUDED
#define _BACKEND_INQUEUE_AGGREGATEDCACHE_INCLUDED

#include "inqueuedefs.hpp"
#include "commontypes.hpp"
#include "algholder.hpp"
#include "engine.hpp"
#include "aholder.hpp"
#include "tickgenerator.hpp"

namespace Inqueue {

	/**
	 This is helper class containg cached data
	*/

	class AggregatedCache
	{
		friend class CalculatedFieldCallbackCallback;
	public:

		
		AggregatedCache(HistoryContext* histcontext);

		~AggregatedCache();

		int registerCallBack(OnCalculatedFieldCallback* cbPtr);

		void removeCallBack(int const cookie);

		void resolveAggregationData(
			bool const initialize,
			CppUtils::String const& provider,
			CppUtils::String const& symbol,
			int const multfactor,
			HlpStruct::AggregationPeriods const period, // how we could forget this !!!
			AggregationHolderList** resolvedListPP
		);

		int cacheSymbol(
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			HlpStruct::AggregationPeriods const period,
			int const multfactor,
			double const& beginData = -1,
			double const& endData = -1
		);

		// work hors in the format 
		// HHMM

		int cacheSymbolWithDateFilteringHours(
			int const begworkhours,
			int const endworkhours,
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			HlpStruct::AggregationPeriods const period,
			int const multfactor,
			double const& beginData = -1,
			double const& endData = -1
		);

		// this checks whether this symbol is really cached and retursn the cached period
		void checkSymbolCached (
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			HlpStruct::AggregationPeriods const period,
			int const multfactor,
			int &cookie,
			double & beginData,
			double & endData
		);
		
		HlpStruct::PriceData const& getUnclosedCandle(int const cookie) const;

		void clearCache(int const cookie);

		void cleanupAllCache();

		long getSize(int const cookie) const;

		// get symbol and provider based on cookie
		CppUtils::String const& getSymbol(int const cookie) const;

		CppUtils::String const& getProvider(int const cookie) const;

		void getAllCookiesList(CppUtils::IntList& cookies) const;


		void dataFirst(int const cookie, DataPtr& dataptr);

		// point to the most recent candle not including unclosed
		void dataLast(int const cookie, DataPtr& dataptr);

		// set up data as specified offset from last data
		// the pointer returned is exepcted to be valid
		// thus navigation can happen to both sides - e.i dataNext()... or dataPrev()
		void dataOffsetFromLast(int const cookie, long const offset, DataPtr& dataptr);

		void dataNext(int const cookie, DataPtr& dataptr);

		// navigate to previous data
		void dataPrev(int const cookie, DataPtr& dataptr);

		
		// search upon timestamp
		// returns the ptr equal the time or the first less than passed time
		// returns -1 if cannot find
		void dataTimeFind(double const& timestamp, int const cookie, DataPtr& dataptr);


		HlpStruct::PriceData const& getCurrentPriceData(const int cookie, DataPtr const& dataptr) const;

		// it returns offseted price data
		// it substructs offset
		HlpStruct::PriceData const& getBackwardPriceData(const int cookie, DataPtr const& dataptr, long const offset ) const;


	  void pushRtData(HlpStruct::RtData & rtdata, EngineThread* engineThread, bool const& tickerInsideExistingTime);


		/**
		 rows functions
		*/

		
		// returns the last index for the whole data
		long getRowsCount();

		// begins iteration 
		void dataRowFirst(DataRowPtr& rowPtr);

		// points the data row to the last
		void dataRowLast(DataRowPtr& rowPtr);

		// whethere there are other rows
		void dataRowNext(DataRowPtr& rowPtr);

		// navigate to backward direction
		void dataRowPrev(DataRowPtr& rowPtr);

		// gets the price data, whereas scookei - the cookie for the cached symbol
		// time - is the ticker time return
		// if pointer is invalid throws an exception
		HlpStruct::PriceData const& getCurrentDataRow(int const scookie, DataRowPtr const& rowPtr) const;

		HlpStruct::PriceData const& getCurrentDataRowTryLocatePrevValid( int const scookie, DataRowPtr const& rowPtr ) const;

    // returns data if any for the time equal or less passed time
		// ttime - is the ticker time returned
		HlpStruct::PriceData const&  findDataRow(int const scookie, double const& ttime, DataRowPtr const& rowPtr) const;

		// get current data ro time
		double getCurrentDataRowTime(DataRowPtr const& rowPtr) const;

		// debug function which writes the cache details
		CppUtils::String dump(bool const very_detailed);

		// dump all rowas to see the dataset
		CppUtils::String dumpAllRows() const;


	public:

		// new version
		/*
		void aggregatePassedData2(
			HlpStruct::RtData& newrtdata, 
			int const multfactor, 
			HlpStruct::AggregationPeriods const aggrperiod, 
			AggregationHolderList &alist
		);
		*/

		void aggregatePassedData(
			HlpStruct::RtData& newrtdata, 
			int const multfactor, 
			HlpStruct::AggregationPeriods const aggrperiod, 
			AggregationHolderList &alist
		);

		void cleanupAggregatedData();

		void cleanupAggregatedData(int const cookie);

		
	private:
		// notification when new row is created or updated
		void onNewRow(
			RowPriceDataMap::iterator const& rowIt,
			int const multfactor, 
			HlpStruct::AggregationPeriods const aggrperiod, 
			HlpStruct::PriceData const& justUpdated,
			AggregationHolderList const& origAlist
		);

		HlpStruct::PriceData const& locateCurrentData( int const scookie, map<double, HlpStruct::PriceDataMapPtr* >::iterator const& rowIt ) const;		

		HlpStruct::PriceData const& locatePrevValidData( int const scookie, map<double, HlpStruct::PriceDataMapPtr* >::iterator const& rowIt ) const;	

		AggregationHolderList const& getAggregationHolderList(int const scookie) const;

		AggregationHolderList* createNewAggregationList(CppUtils::String const& key);
		
		// this creates new entry
		void createNewPriceDataFromRtData(HlpStruct::PriceData &storage, HlpStruct::RtData const& newrtdata);

		// if newrtdata is LATER than what we have our storage
		// storage must exists
		// storage time is not updated here
		void updatePriceDataWithLaterTime(HlpStruct::PriceData &storage, HlpStruct::RtData const& newrtdata);
	

		// helper to get the sequence number of the period
		void getPeriodSequenceNum(
			HlpStruct::AggregationPeriods const aggrperiod, 
			int const multfactor, 
			long const& curTime, 
			CppUtils::UnsignedLongLong& periodNum,
			tm& candleTime
		);

		int calculateWeekOfYear(tm const& timestamp );

		// returns the row ptr which was updated
		RowPriceDataMap::iterator insertAggregatedData(int const cookie, HlpStruct::PriceData* pricedata);

		HlpStruct::PriceDataMapPtr* resolveAggregatedData(double const ttime);

		//
		CppUtils::Mutex aggrMutex_m;
		
		map<int, AggregationHolderList* > aggregatedData_m;

		//map<CppUtils::String, int> aggregatedKeys_m;

		// provider_symbol + multfactor <-> aggrperiod --> cookie
		// because cache must be separate for all occasions !!!
		map<CppUtils::String, map<int, map<HlpStruct::AggregationPeriods, int> > > aggregatedKeys_m;

		// provider_symbol --> all cookies for all the mult factors
		map<CppUtils::String, set<int> > allKeysForProviderSymbol_m;

		
				
		// structure making alignment for all the cached data just to access 
		// simultaneusly data with he same timestamp

		// map contains the array of collected together data
		//map < (time) map< (cokie of cached data), (ref to price data)> >
		RowPriceDataMap allSymbolsData_m;


		// history context
		HistoryContext* hstContextP_m;

		// we may set the filter and pass only data filtered by daytime
		long beginDayTimeWork_m;

		long endDayTimeWork_m;

		// map to registre callback for calc field

		
		static int fieldCallbackForThreadId_m;

		map<int, OnCalculatedFieldCallback*> calcCallbackMap_m;

		static const HlpStruct::PriceData dummyPriceData_m;

	};
}

#endif