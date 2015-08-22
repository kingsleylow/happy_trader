#ifndef _BACKEND_INQUEUE_CONTEXTACCESSOR_INCLUDED
#define _BACKEND_INQUEUE_CONTEXTACCESSOR_INCLUDED

#include "inqueuedefs.hpp"
#include "commontypes.hpp"
#include "../math/mathdecl.hpp"




namespace Math {
class MathEngineInterface;

}

namespace Inqueue {
	
	
	class HistoryContext;

	/**
	 Helper class for iteration over data
	*/

	class INQUEUE_BCK_EXP DataPtr
	{
		friend class EngineThread;
		friend class AggregatedCache;
	public:

		DataPtr():
				//cutOffTime_m(cutofftime),
				idx_m(-1),
				valid_m(false),
				firsttime_m(true)
		{
		}

		inline bool isValid() const
		{
			return valid_m && !firsttime_m; 
		}

		

private:
		//double cutOffTime_m;

		long idx_m;

		bool valid_m;

		bool firsttime_m;
	};

	/**
	 Helper class for rows navigation
	*/
	class INQUEUE_BCK_EXP DataRowPtr
	{
		friend class EngineThread;
		friend class AggregatedCache;
	public:

		DataRowPtr():
				//cutOffTime_m(cutofftime),
				valid_m(false),
				firsttime_m(true)
		{
		}

		inline bool isValid() const
		{
			return valid_m && !firsttime_m;
		}

		
private:
		//double cutOffTime_m;

		RowPriceDataMap::iterator rowIt_m;

		RowPriceDataMap::reverse_iterator rRowIt_m;

		bool valid_m;

		bool firsttime_m;

		bool isForward_m;

						
	};


	/**
		Class incapsulating access to history data
	*/

	


	class INQUEUE_BCK_EXP ContextAccessor {
		friend class EngineThread;
		friend class HistoryContext;
	public:
	
		// this will make blocking call to java server to run external jar
		void requestRunExternalPackage(
			CppUtils::String const& package_name,
			bool const do_not_wait_response,
			bool &result,
			CppUtils::String& returned_log
		);

		// this clears history cache and then loads it
		// blocks until operation completes
		bool loadDataFromMaster(
			HlpStruct::ProviderSymbolList const& providerSymbolList,
			CppUtils::String const& dataProfile,
			bool const keepCacheAlive,
			double const& beginTime = -1,
			double const& endTime = -1
		);



		// load all data from master
		bool loadDataFromMaster(
			CppUtils::String const& dataProfile,
			bool const keepCacheAlive,
			double const& beginTime = -1,
			double const& endTime = -1
		);

		// register callback to perform calc fields
		int registerCallBack(OnCalculatedFieldCallback* cbPtr);

		// remove callback to perform calc fields
		void removeCallBack(int const cookie);
		
		// prepare aggregate information for symbol
		// period and multfactor - thiese are aggregation parameters
		// it returns aggregation cookie
		// data is cached so if called multiple time just return the necessary cookie
		// begindate - this is timestamp from which history aggregation begins
		int cacheSymbol(
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			HlpStruct::AggregationPeriods const period,
			int const multfactor
		);

		// the same as upper but filter out workinh hours
		int cacheSymbolWithDateFilteringHours(
			int const begworkhours,
			int const endworkhours,
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			HlpStruct::AggregationPeriods const period,
			int const multfactor
		);

		// delete aggregated symbol from the cache
		void clearCache(int const cookie);

		// retusrns the last candle (probably unclosed)
		HlpStruct::PriceData const& getUnclosedCandle(int const cookie) const;

		long getSize(int const cookie);

		CppUtils::String const& getSymbol(int const cookie) const;

		CppUtils::String const& getProvider(int const cookie) const;

		DataPtr dataFirst(int const cookie);

		bool dataNext(int const cookie, DataPtr& dataPtr);

		DataPtr dataLast(int const cookie);

		DataPtr dataOffsetFromLast(int const cookie, long const offset);

		bool dataPrev(int const cookie, DataPtr& dataPtr);

		bool dataTimeFind(double const& timestamp, int const cookie, DataPtr& dataPtr);

		

		// returns the lits of prices from the dataptr
		// returns false if not enough prices
		bool getBackWardPriceList(const int cookie, HlpStruct::PriceDataList& priceList, DataPtr const& dataPtr, int count);

		HlpStruct::PriceData const& getCurrentPriceData(const int cookie, DataPtr const& dataPtr) const;

		// get offseted price data 
		HlpStruct::PriceData const& getBackwardPriceData(const int cookie, DataPtr const& dataptr, long const offset ) const;
		    
		// row related iteration functions
		
		long getRowsCount();
	
		DataRowPtr dataRowFirst();

		DataRowPtr dataRowLast();
		
		bool dataRowNext(DataRowPtr& rowPtr);

		bool dataRowPrev(DataRowPtr& rowPtr);

		
		HlpStruct::PriceData const& getCurrentDataRow(int const cookie, DataRowPtr const& rowPtr) const;

		HlpStruct::PriceData const& getCurrentDataRowTryLocatePrevValid( int const scookie, DataRowPtr const& rowPtr ) const;
    
		HlpStruct::PriceData const&  findDataRow(int const cookie, double const& ttime, DataRowPtr const& rowPtr) const;

		double getCurrentDataRowTime(DataRowPtr const& rowPtr) const;
		
		long getThreadId() const;

		// data access functions unique for each thread,
		//this access Gloabal to engine thread
		HlpStruct::GlobalStorage& getThreadGlobal();

		//

		/*
		bool getCurrentUpdateCacheFlag() const {
			return updateCacheFlag_m;
		}
		*/

		inline bool isLast() const
		{
			return isLast_m;
		}

		inline bool isFirst() const
		{
			return isFirst_m;
		}

		inline bool isSimulation() const
		{
			return isSumulation_m;
		}

		

		// some debug functions
		// dumps whole cache to STRING
		// GMT time only !!!
		// can be time/resource consuming
		CppUtils::String dump(int const cookie, bool const write_warn);

		static CppUtils::String dump(AggregatedCache& aggregatedCache, int const cookie, bool const write_warn);


		CppUtils::String dump_reverse(int const cookie);

		// dump before datetime
		CppUtils::String dump_reverse(int const cookie, double& datetime_before);
#ifdef _DEBUG
		// detailed dump of how this was creating
		CppUtils::String very_detailed_dump(int const cookie);

#endif
		// special functions to provide convinient access for claculating functions
		// R-result
		template<typename R, Math::CandlePrices const typeOfPrice, class T>
		R callMathEngine (const int cookie, T& mathEngine)
		{
			mathEngine.initialize(*this, cookie);
			return mathEngine.performCalculation<typeOfPrice>();
		}

		// special functions to provide convinient access for claculating functions
		template<typename R, Math::CandlePrices const typeOfPrice, class T>
		R callMathEngineWithArbitrarySeries ( deque<HlpStruct::PriceData> &valuesList, T& mathEngine)
		{
			mathEngine.initialize(valuesList);
			return mathEngine.performCalculation<typeOfPrice>();
		}

		// special function to provider convinient access when we have MultiSymbolCache
		template<typename R, Math::CandlePrices const typeOfPrice, class T>
		R callMathEngineWithMultiSymbolCacheSeries ( HlpStruct::MultiSymbolCache& multSymbolCache, CppUtils::String const& symbol, T& mathEngine)
		{
			mathEngine.initialize(multSymbolCache, symbol);
			return mathEngine.performCalculation<typeOfPrice>();
		}

		// -------------

		CppUtils::String makeDebugDump(bool const very_detailed);

		CppUtils::String dumpAllRows() const;

	
		
		
	private:

		EngineThread& engThread_m;
		
		// private
		ContextAccessor(
			AggregatedCache& aggregatedCache,
			EngineThread& engThread,
			//double const cutOffTime,
			bool const isSimulation,
			bool const isLast,
			bool const isFirst
		);

		~ContextAccessor();

			
		// whether this is called after start RT command
		//bool updateCacheFlag_m;
		
		// current ticker time -e.i. cutoff time
		//double const cutOffTime_m;

		// whether we are simulating
		bool isSumulation_m;

		// whether this is last data (always false in RT trading but contains true if this is last ticker in simulation loop)
		bool isLast_m;

		// whether this is first (always false in RT trading but contains true if this is the first ticker in simulation loop)
		bool isFirst_m;

		// who is actually stores data
		AggregatedCache& aggregatedCache_m;

		

	};


};

#endif