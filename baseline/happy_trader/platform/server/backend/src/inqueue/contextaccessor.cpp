#include "contextaccessor.hpp"
#include "histcontext.hpp"
#include "enginethread.hpp"
#include "utils.hpp"
#include "../math/math.hpp"


#define CONTEXTACCESSOR "CONTEXTACCESSOR"

namespace Inqueue {



ContextAccessor::ContextAccessor(
				AggregatedCache& aggregatedCache,
				EngineThread& engThread, 
				//double const cutOffTime,
				bool const isSimulation,
				bool const isLast,
				bool const isFirst
	):
	aggregatedCache_m(aggregatedCache),
	engThread_m(engThread),
	//cutOffTime_m(cutOffTime),
	isSumulation_m(isSimulation),
	isLast_m(isLast),
	isFirst_m(isFirst)
{
	
}

ContextAccessor::~ContextAccessor()
{
}

void ContextAccessor::requestRunExternalPackage(
			CppUtils::String const& package_name,
			bool const do_not_wait_response,
			bool &result,
			CppUtils::String& returned_log
		)
{
	requestRunExternalPackageHelper(package_name, do_not_wait_response, result, returned_log);
}

bool ContextAccessor::loadDataFromMaster(
			CppUtils::String const& dataProfile,
			bool const keepCacheAlive,
			double const& beginTime,
			double const& endTime
)
{

	return loadDataFromMaster(HlpStruct::ProviderSymbolList(), dataProfile, keepCacheAlive, beginTime, endTime);
}

bool ContextAccessor::loadDataFromMaster(
			HlpStruct::ProviderSymbolList const& providerSymbolList,
			CppUtils::String const& dataProfile,
			bool const keepCacheAlive,
			double const& beginTime,
			double const& endTime
		)
{
	return loadDataFromMasterHelper(providerSymbolList, dataProfile, keepCacheAlive, beginTime, endTime);
}

// register callback to perform calc fields
int ContextAccessor::registerCallBack(OnCalculatedFieldCallback* cbPtr)
{
	return aggregatedCache_m.registerCallBack(cbPtr);
}

// remove callback to perform calc fields
void ContextAccessor::removeCallBack(int const cookie)
{
	aggregatedCache_m.removeCallBack(cookie);
}

int ContextAccessor::cacheSymbol(
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			HlpStruct::AggregationPeriods const period,
			int const multfactor
)
{
	return aggregatedCache_m.cacheSymbol(
		provider, 
		symbol, 
		period, 
		multfactor
	);
}

int ContextAccessor::cacheSymbolWithDateFilteringHours(
			int const begworkhours,
			int const endworkhours,
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			HlpStruct::AggregationPeriods const period,
			int const multfactor
		)
{
	return aggregatedCache_m.cacheSymbolWithDateFilteringHours(
		begworkhours,
		endworkhours,
		provider,
		symbol,
		period,
		multfactor
	);
}


void ContextAccessor::clearCache(int const cookie)
{
	aggregatedCache_m.clearCache(cookie);
}

HlpStruct::PriceData const& ContextAccessor::getUnclosedCandle(int const cookie) const
{
	return aggregatedCache_m.getUnclosedCandle(cookie);
}
 
long ContextAccessor::getSize(int const cookie)
{
	return aggregatedCache_m.getSize(cookie);
}

CppUtils::String const& ContextAccessor::getSymbol(int const cookie) const
{
	return aggregatedCache_m.getSymbol(cookie);
}

CppUtils::String const& ContextAccessor::getProvider(int const cookie) const
{
	return aggregatedCache_m.getProvider(cookie);
}

DataPtr ContextAccessor::dataFirst(int const cookie)
{
	//DataPtr newPtr(cutOffTime_m);
	DataPtr newPtr;
	aggregatedCache_m.dataFirst(cookie, newPtr);

	return newPtr;
}

bool ContextAccessor::dataNext(int const cookie,  DataPtr& dataPtr)
{
	aggregatedCache_m.dataNext(cookie, dataPtr);
	return dataPtr.isValid();
}

DataPtr ContextAccessor::dataLast(int const cookie)
{
	
	DataPtr newPtr;
	aggregatedCache_m.dataLast(cookie, newPtr);

	return newPtr;
}

DataPtr ContextAccessor::dataOffsetFromLast(int const cookie, long const offset)
{
	DataPtr newPtr;
	aggregatedCache_m.dataOffsetFromLast(cookie, offset, newPtr);
	return newPtr;
}

bool ContextAccessor::dataPrev(int const cookie, DataPtr& dataPtr)
{
		aggregatedCache_m.dataPrev(cookie, dataPtr);
		return dataPtr.isValid();
}

bool ContextAccessor::dataTimeFind(double const& timestamp, int const cookie, DataPtr& dataPtr)
{
	aggregatedCache_m.dataTimeFind( timestamp, cookie, dataPtr );
	return dataPtr.isValid();
}




bool ContextAccessor::getBackWardPriceList(const int cookie, HlpStruct::PriceDataList& priceList, DataPtr const& dataPtr, int count)
{
	DataPtr curDataPtr = dataPtr;

	bool result = false;
	
	int idx = 0;
	if (!curDataPtr.isValid())
		dataPrev(cookie, curDataPtr);

#ifdef _DEBUG
	double begin_t = -1, end_t = -1;
	int cnt = 0;
#endif

	while(dataPtr.isValid()) {
		
		// ---
		HlpStruct::PriceData const& pdata = aggregatedCache_m.getCurrentPriceData(cookie, curDataPtr);				
		priceList.push_back(pdata);
#ifdef _DEBUG
		if (cnt++==0) {
			begin_t = pdata.time_m;
		}
#endif

		// advance counter
		if (++idx >= count) {
			result = true;
#ifdef _DEBUG
			end_t = pdata.time_m;
#endif

			break;
		}

		if (!dataPrev(cookie, curDataPtr)) {
#ifdef _DEBUG
			end_t = pdata.time_m;
#endif
			break;
		}

	}

#ifdef _DEBUG

	LOG( MSG_DEBUG_DETAIL, CONTEXTACCESSOR, "Read backward data from: " << (begin_t > 0 ? CppUtils::getGmtTime(begin_t) : "N/A") << 
		" to: " <<  (end_t > 0 ? CppUtils::getGmtTime(end_t) : "N/A") 
	);
#endif

	return result;
};


HlpStruct::PriceData const& ContextAccessor::getCurrentPriceData(const int cookie,  DataPtr const& dataPtr) const
{
	return aggregatedCache_m.getCurrentPriceData(cookie, dataPtr);
}

HlpStruct::PriceData const& ContextAccessor::getBackwardPriceData(const int cookie, DataPtr const& dataptr, long const offset ) const
{
	return aggregatedCache_m.getBackwardPriceData(cookie, dataptr, offset );
}
		

	
long ContextAccessor::getRowsCount()
{
	return aggregatedCache_m.getRowsCount();
}
	
DataRowPtr ContextAccessor::dataRowFirst()
{
	//DataRowPtr rowPtr(cutOffTime_m);
	DataRowPtr rowPtr;
	aggregatedCache_m.dataRowFirst(rowPtr);

	return rowPtr;
}

DataRowPtr ContextAccessor::dataRowLast()
{
	//DataRowPtr rowPtr(cutOffTime_m);
	DataRowPtr rowPtr;

	aggregatedCache_m.dataRowLast(rowPtr);
	return rowPtr;

}
		
bool ContextAccessor::dataRowNext(DataRowPtr& rowPtr)
{
	aggregatedCache_m.dataRowNext(rowPtr);

	return rowPtr.isValid();
}

bool ContextAccessor::dataRowPrev(DataRowPtr& rowPtr)
{
	aggregatedCache_m.dataRowPrev(rowPtr);

	return rowPtr.isValid();
}
		
HlpStruct::PriceData const& ContextAccessor::getCurrentDataRow(int const cookie, DataRowPtr const& rowPtr) const
{
	return aggregatedCache_m.getCurrentDataRow(cookie, rowPtr);
}

HlpStruct::PriceData const& ContextAccessor::getCurrentDataRowTryLocatePrevValid( int const scookie, DataRowPtr const& rowPtr ) const
{
	return aggregatedCache_m.getCurrentDataRowTryLocatePrevValid(scookie, rowPtr);
}
    
double ContextAccessor::getCurrentDataRowTime(DataRowPtr const& rowPtr) const
{
	return aggregatedCache_m.getCurrentDataRowTime(rowPtr);
}

HlpStruct::PriceData const&  ContextAccessor::findDataRow(int const cookie, double const& ttime, DataRowPtr const& rowPtr) const
{
	return aggregatedCache_m.findDataRow(cookie, ttime, rowPtr);
}

long ContextAccessor::getThreadId() const
{
	return engThread_m.id_m;
}

HlpStruct::GlobalStorage& ContextAccessor::getThreadGlobal()
{
	return engThread_m.getThreadGlobalStorage();
}
	
// ------------------------
// debug
CppUtils::String ContextAccessor::dump(int const cookie, bool const write_warn)
{
	// use static version
	return dump(aggregatedCache_m, cookie,write_warn);
	
}

// static version
CppUtils::String ContextAccessor::dump(AggregatedCache& aggregatedCache, int const cookie, bool const write_warn)
{
	bool normal_order = true;
	CppUtils::String result("FORWARD DUMP\n");
	result += "DATETIME,OPEN ASK,HIGH ASK,LOW ASK,CLOSE ASK,OPEN BID,HIGH BID,LOW BID,CLOSE BID,VOLUME\n";
	DataPtr ptr;
	aggregatedCache.dataFirst(cookie, ptr);


	double prev_time= -1;
	
	while(true) {
		aggregatedCache.dataNext(cookie, ptr);
		if (!ptr.isValid())
			break;

		
		HlpStruct::PriceData const& pdata = aggregatedCache.getCurrentPriceData(cookie, ptr);
		if (prev_time >0 ) {
			if ( pdata.time_m < prev_time) {
				if (write_warn) {
					result += "\n WARNING: previous time is more then current!\n";
				}
			}
			else if ( pdata.time_m == prev_time) {
				if (write_warn) {
					result += "\n WARNING: previous time is equal to current!\n";
				}
			}
		}

		// dump all here
		result += ((pdata.time_m>0?CppUtils::getGmtTime(pdata.time_m):"N/A")+","+
				CppUtils::float2String(pdata.open_m,-1, 5)+","+
				CppUtils::float2String(pdata.high_m,-1,5)+","+
				CppUtils::float2String(pdata.low_m,-1,5)+","+
				CppUtils::float2String(pdata.close_m,-1,5)+","+
				CppUtils::float2String(pdata.open2_m,-1,5)+","+
				CppUtils::float2String(pdata.high2_m,-1,5)+","+
				CppUtils::float2String(pdata.low2_m,-1,5)+","+
				CppUtils::float2String(pdata.close2_m,-1,5)+","+
				CppUtils::float2String(pdata.volume_m,-1,5) );

		result += "\n";

		prev_time = pdata.time_m;
	}

	// unclosed
	HlpStruct::PriceData const& pdata_unclosed= aggregatedCache.getUnclosedCandle(cookie);
	result += ( "* " + (pdata_unclosed.time_m>0?CppUtils::getGmtTime(pdata_unclosed.time_m):"N/A")+","+
				CppUtils::float2String(pdata_unclosed.open_m,-1, 5)+","+
				CppUtils::float2String(pdata_unclosed.high_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.low_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.close_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.open2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.high2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.low2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.close2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.volume_m,-1,5) );

	result += "\n";
	return result;
}

CppUtils::String ContextAccessor::dump_reverse(int const cookie)
{
	CppUtils::String result("BACKWARD DUMP\n");
	result += "DATETIME,OPEN ASK,HIGH ASK,LOW ASK,CLOSE ASK,OPEN BID,HIGH BID,LOW BID,CLOSE BID,VOLUME\n";

	DataPtr ptr = dataLast(cookie);

	// unclosed
	HlpStruct::PriceData const& pdata_unclosed= getUnclosedCandle(cookie);
	result += ( "* " + (pdata_unclosed.time_m>0?CppUtils::getGmtTime2(pdata_unclosed.time_m):"N/A")+","+
				CppUtils::float2String(pdata_unclosed.open_m,-1, 5)+","+
				CppUtils::float2String(pdata_unclosed.high_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.low_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.close_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.open2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.high2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.low2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.close2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.volume_m,-1,5) );

	result += "\n";

	// backward
	if (!ptr.isValid())
		dataPrev(cookie, ptr);

	while(ptr.isValid()) {
		HlpStruct::PriceData const& pdata = aggregatedCache_m.getCurrentPriceData(cookie, ptr);

		// dump all here
		result += ((pdata.time_m>0?CppUtils::getGmtTime2(pdata.time_m):"N/A")+","+
				CppUtils::float2String(pdata.open_m,-1, 5)+","+
				CppUtils::float2String(pdata.high_m,-1,5)+","+
				CppUtils::float2String(pdata.low_m,-1,5)+","+
				CppUtils::float2String(pdata.close_m,-1,5)+","+
				CppUtils::float2String(pdata.open2_m,-1,5)+","+
				CppUtils::float2String(pdata.high2_m,-1,5)+","+
				CppUtils::float2String(pdata.low2_m,-1,5)+","+
				CppUtils::float2String(pdata.close2_m,-1,5)+","+
				CppUtils::float2String(pdata.volume_m,-1,5) );

		result += "\n";

		if (!dataPrev(cookie, ptr))
			break;
	};

	
	return result;

}

#ifdef _DEBUG

CppUtils::String ContextAccessor::very_detailed_dump(int const cookie)
{
	CppUtils::String result("FORWARD DETAILED DUMP\n");
	result += "DATETIME,OPEN ASK,HIGH ASK,LOW ASK,CLOSE ASK,OPEN BID,HIGH BID,LOW BID,CLOSE BID,VOLUME\n";
	DataPtr ptr = dataFirst(cookie);

	while(dataNext(cookie, ptr)) {
		HlpStruct::PriceData const& pdata = aggregatedCache_m.getCurrentPriceData(cookie, ptr);

		// dump all here
		result += ((pdata.time_m>0?CppUtils::getGmtTime2(pdata.time_m):"N/A")+","+
				CppUtils::float2String(pdata.open_m,-1, 5)+","+
				CppUtils::float2String(pdata.high_m,-1,5)+","+
				CppUtils::float2String(pdata.low_m,-1,5)+","+
				CppUtils::float2String(pdata.close_m,-1,5)+","+
				CppUtils::float2String(pdata.open2_m,-1,5)+","+
				CppUtils::float2String(pdata.high2_m,-1,5)+","+
				CppUtils::float2String(pdata.low2_m,-1,5)+","+
				CppUtils::float2String(pdata.close2_m,-1,5)+","+
				CppUtils::float2String(pdata.volume_m,-1,5) );

		result += "\n";

		

	}

	// unclosed
	HlpStruct::PriceData const& pdata_unclosed= getUnclosedCandle(cookie);
	result += ( "* " + (pdata_unclosed.time_m>0?CppUtils::getGmtTime2(pdata_unclosed.time_m):"N/A")+","+
				CppUtils::float2String(pdata_unclosed.open_m,-1, 5)+","+
				CppUtils::float2String(pdata_unclosed.high_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.low_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.close_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.open2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.high2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.low2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.close2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.volume_m,-1,5) );

	result += "\n";

	
	return result;
}

#endif


CppUtils::String ContextAccessor::dump_reverse(int const cookie, double& datetime_before)
{
	CppUtils::String result("BACKWARD DUMP BEFORE TIMESTAMP: " + CppUtils::getGmtTime(datetime_before) + "\n");
	result += "DATETIME,OPEN ASK,HIGH ASK,LOW ASK,CLOSE ASK,OPEN BID,HIGH BID,LOW BID,CLOSE BID,VOLUME\n";

	DataPtr ptr;
	dataTimeFind(datetime_before, cookie, ptr);

	// unclosed
	HlpStruct::PriceData const& pdata_unclosed= getUnclosedCandle(cookie);

	// filter out unclosed
	if (pdata_unclosed.time_m <= datetime_before) {
		result += ( "* " + (pdata_unclosed.time_m>0?CppUtils::getGmtTime2(pdata_unclosed.time_m):"N/A")+","+
				CppUtils::float2String(pdata_unclosed.open_m,-1, 5)+","+
				CppUtils::float2String(pdata_unclosed.high_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.low_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.close_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.open2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.high2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.low2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.close2_m,-1,5)+","+
				CppUtils::float2String(pdata_unclosed.volume_m,-1,5) );

		result += "\n";
	}

	// backward
	if (!ptr.isValid())
		dataPrev(cookie, ptr);

	while(ptr.isValid()) {
		HlpStruct::PriceData const& pdata = aggregatedCache_m.getCurrentPriceData(cookie, ptr);

		// dump all here
		result += ((pdata.time_m>0?CppUtils::getGmtTime2(pdata.time_m):"N/A")+","+
				CppUtils::float2String(pdata.open_m,-1, 5)+","+
				CppUtils::float2String(pdata.high_m,-1,5)+","+
				CppUtils::float2String(pdata.low_m,-1,5)+","+
				CppUtils::float2String(pdata.close_m,-1,5)+","+
				CppUtils::float2String(pdata.open2_m,-1,5)+","+
				CppUtils::float2String(pdata.high2_m,-1,5)+","+
				CppUtils::float2String(pdata.low2_m,-1,5)+","+
				CppUtils::float2String(pdata.close2_m,-1,5)+","+
				CppUtils::float2String(pdata.volume_m,-1,5) );

		result += "\n";

		if (!dataPrev(cookie, ptr))
			break;

	};

	
	return result;
}

CppUtils::String ContextAccessor::makeDebugDump(bool const very_detailed)
{
	return aggregatedCache_m.dump(very_detailed);
}

CppUtils::String ContextAccessor::dumpAllRows() const
{
	return aggregatedCache_m.dumpAllRows();
}



}; // end of namespace