#include "aggregatedcache.hpp"
#include "histcontext.hpp"
#include <time.h>
#include "enginethread.hpp"


#define MAX_KEY_BUF 128
#define AGGRCACHE "AGGRCACHE"
namespace Inqueue {

int AggregatedCache::fieldCallbackForThreadId_m	= 0;

const HlpStruct::PriceData AggregatedCache::dummyPriceData_m;

AggregatedCache::AggregatedCache(HistoryContext* histcontext):
	hstContextP_m( histcontext ),
	beginDayTimeWork_m(-1),
	endDayTimeWork_m(-1)
{
}

AggregatedCache::~AggregatedCache()
{
	cleanupAllCache();
}


int AggregatedCache::registerCallBack(OnCalculatedFieldCallback* cbPtr)
{
	LOCK_FOR_SCOPE(aggrMutex_m);
	calcCallbackMap_m[++fieldCallbackForThreadId_m] = cbPtr;

	return fieldCallbackForThreadId_m;
}

void AggregatedCache::removeCallBack(int const cookie)
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	map<int, OnCalculatedFieldCallback*>::iterator it = calcCallbackMap_m.find(cookie);
	if (it != calcCallbackMap_m.end())
		calcCallbackMap_m.erase(it);
	else
		THROW(CppUtils::OperationFailed, "exc_InvalidCookie", "ctx_removeCallBack", cookie );
}

// locates current data if any
HlpStruct::PriceData const& AggregatedCache::locateCurrentData( 
	int const scookie, RowPriceDataMap::iterator const& rowIt ) const
{
	if (rowIt == allSymbolsData_m.end())
		return dummyPriceData_m;

	HlpStruct::PriceDataMapPtr::iterator it2 =	rowIt->second->find(scookie);

	if (it2 != rowIt->second->end() )
		return *(it2->second);

	return dummyPriceData_m;

}

HlpStruct::PriceData const& AggregatedCache::locatePrevValidData( 
	int const scookie, RowPriceDataMap::iterator const& rowIt ) const
{
	
	// try to locate previous valid
	RowPriceDataMap::const_iterator itmp = rowIt;
	
	// nothing to return if we are at the beginning
	if (rowIt == allSymbolsData_m.begin()) 
		return dummyPriceData_m;

	if (rowIt == allSymbolsData_m.end())
		return dummyPriceData_m;
	
	while(true) {
		itmp--;

		HlpStruct::PriceDataMapPtr::iterator it2 =	itmp->second->find(scookie);

		if (it2 != itmp->second->end() )
			return *(it2->second);
		
		if (itmp == allSymbolsData_m.begin()) 
			break;
		
	}

	return dummyPriceData_m;
	
}

AggregationHolderList const& AggregatedCache::getAggregationHolderList(int const scookie) const
{
	map<int, AggregationHolderList* >::const_iterator it = aggregatedData_m.find(scookie);

	if (it == aggregatedData_m.end())
		THROW(CppUtils::OperationFailed, "exc_CookieNotFound", "ctx_getAggregationHolderList", (long)scookie );

	return *it->second;
}


AggregationHolderList* AggregatedCache::createNewAggregationList(CppUtils::String const& key)
{
	AggregationHolderList* aggregationList = new AggregationHolderList();
	int resolvedkey  = ++AggregationHolderList::id_m;

	aggregatedData_m[resolvedkey] = aggregationList;
	aggregationList->getCookie() = resolvedkey;

	return aggregationList;

}

void AggregatedCache::resolveAggregationData(
	bool const initialize,
	CppUtils::String const& provider,
	CppUtils::String const& symbol,
	int const multfactor,
	HlpStruct::AggregationPeriods const period,
	AggregationHolderList** resolvedListPP
	)
{
	*resolvedListPP = NULL;


	//CppUtils::String key = provider+symbol;


	char key[MAX_KEY_BUF];
	strcpy(key, provider.c_str());
	strcat(key, symbol.c_str());

	map<CppUtils::String, map<int, map<HlpStruct::AggregationPeriods, int> > >::iterator it = aggregatedKeys_m.find(key);

	if (it != aggregatedKeys_m.end()) {

		// look for another part of map
		map<int, map<HlpStruct::AggregationPeriods, int> >& multFactAggrPeriodCookieMap = it->second;
		map<int, map<HlpStruct::AggregationPeriods, int> >::iterator it2 = multFactAggrPeriodCookieMap.find(multfactor);

		if (it2 != multFactAggrPeriodCookieMap.end()) {

			map<HlpStruct::AggregationPeriods, int>&aggrPerMap = it2->second;
			map<HlpStruct::AggregationPeriods, int>::iterator it3 = aggrPerMap.find(period);

			if (it3 != aggrPerMap.end()) {


				map<int, AggregationHolderList* >::iterator it_a = aggregatedData_m.find(it3->second);
				if (it_a != aggregatedData_m.end() ) {

					// ok
					*resolvedListPP = it_a->second;
					return;
				}

			}
		}

	}

	// not found
	if (initialize) {
		*resolvedListPP = createNewAggregationList(key);
		aggregatedKeys_m[key][multfactor][period] = (*resolvedListPP)->getCookie();

		// reverse map
		allKeysForProviderSymbol_m[key].insert( (*resolvedListPP)->getCookie() );

	}


}

int AggregatedCache::cacheSymbol(
																 CppUtils::String const& provider, 
																 CppUtils::String const& symbol, 
																 HlpStruct::AggregationPeriods const period,
																 int const multfactor,
																 double const& beginData,
																 double const& endData
																 )
{
	// can be called from multiple threads concurrently

	// first reap history data - using profile to map
	// then reap RT data - profile is ignored


	LOCK_FOR_SCOPE(aggrMutex_m);


	AggregationHolderList* alistP = NULL;
	resolveAggregationData(	true, provider, symbol,	multfactor, period, &alistP); 
	HTASSERT(alistP != NULL);

	AggregationHolderList& alist = *alistP;

	if (!alist.isValid()) {

		alist.initAggrPeriodMultFactor(period, multfactor);
		alist.getProvider() = provider;
		alist.getSymbol() = symbol;

		LOG( MSG_DEBUG, AGGRCACHE, "Caching request of symbol \"" << symbol << "\" provider \"" << provider << "\" aggregation period \""	
			<< period << "\" multfactor \"" << multfactor << " \"begin Data \"" << beginData << "\"end Data \"" << endData << "\" created new memory entry " );

		// update from file
		if (hstContextP_m)
			hstContextP_m->readDataToMemoryCache<AggregatedCache>(*this, alist, beginData, endData);
	}
	//else {
	//LOG( MSG_INFO, getStringId().c_str(), "Caching request of symbol \"" << symbol << "\" provider \"" << provider << "\" aggregation period \""	
	//		<< period << "\" multfactor \"" << multfactor << "\" just returned the old entry" );
	//}



	return alistP->getCookie();

}

int AggregatedCache::cacheSymbolWithDateFilteringHours(
	int const begworkhours,
	int const endworkhours,
	CppUtils::String const& provider, 
	CppUtils::String const& symbol, 
	HlpStruct::AggregationPeriods const period,
	int const multfactor,
	double const& beginData,
	double const& endData
	)
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	int hoursb = (int)(begworkhours/100);
	int minutesb = begworkhours - hoursb*100;

	int hourse = (int)(endworkhours/100);
	int minutese = endworkhours - hourse*100;

	// initialize 
	beginDayTimeWork_m = hoursb * 60* 60 + minutesb*60;
	endDayTimeWork_m = hourse * 60*60 + minutese*60;


	AggregationHolderList* alistP = NULL;
	resolveAggregationData(	true, provider, symbol,	multfactor, period, &alistP); 
	HTASSERT(alistP != NULL);

	AggregationHolderList& alist = *alistP;

	if (!alist.isValid()) {

		alist.initAggrPeriodMultFactor(period, multfactor);
		alist.getProvider() = provider;
		alist.getSymbol() = symbol;

		LOG( MSG_DEBUG, AGGRCACHE, "Caching request of symbol \"" << symbol << "\" provider \"" << provider << "\" aggregation period \""	
			<< period << "\" multfactor \"" << multfactor << " \"begin Data \"" << beginData << "\"end Data \"" << endData << "\" created new memory entry " );

		// update from file
		if (hstContextP_m)
			hstContextP_m->readDataToMemoryCache<AggregatedCache>(*this, alist, beginData, endData);
	}



	return alistP->getCookie();
}


void AggregatedCache::checkSymbolCached (
	CppUtils::String const& provider, 
	CppUtils::String const& symbol, 
	HlpStruct::AggregationPeriods const period,
	int const multfactor,
	int &cookie,
	double & beginData,
	double & endData
	)
{
	LOCK_FOR_SCOPE(aggrMutex_m);
	beginData = -1;
	endData = -1;
	cookie = -1;

	AggregationHolderList* alistP = NULL;

	resolveAggregationData(	false, provider, symbol, multfactor, period, &alistP); 

	if (alistP) {
		// found
		beginData = alistP->getBeginDate();
		endData = alistP->getEndDate();
		cookie = alistP->getCookie();
	}

}

HlpStruct::PriceData const& AggregatedCache::getUnclosedCandle(int const cookie) const
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	map<int, AggregationHolderList* >::const_iterator it = aggregatedData_m.find(cookie);

	if (it == aggregatedData_m.end())
		THROW(CppUtils::OperationFailed, "exc_CookieNotFound", "ctx_getUnclosedCandle", (long)cookie );		

	AggregationHolderList* alistP = it->second;
	HTASSERT(alistP != NULL && alistP->isValid());

	return alistP->getLastPriceData();
}

void AggregatedCache::clearCache(int const cookie)
{

	LOCK_FOR_SCOPE(aggrMutex_m);

	map<int, AggregationHolderList* >::iterator it = aggregatedData_m.find(cookie);

	if (it == aggregatedData_m.end())
		THROW(CppUtils::OperationFailed, "exc_CookieNotFound", "ctx_ClearCache", (long)cookie );		

	AggregationHolderList* alistP = it->second;
	HTASSERT(alistP != NULL && alistP->isValid());
	//

	//
	char key[MAX_KEY_BUF];
	strcpy(key, alistP->getProvider().c_str());
	strcat(key, alistP->getSymbol().c_str());

	//CppUtils::String key = alistP->getProvider()+alistP->getSymbol();

	map<CppUtils::String, map<int, map<HlpStruct::AggregationPeriods, int> > >::iterator kt = aggregatedKeys_m.find(key);

	if (kt != aggregatedKeys_m.end()) {

		// look for another part of map
		map<int, map<HlpStruct::AggregationPeriods, int> >& multFactPeriodCookieMap = kt->second;
		map<int, map<HlpStruct::AggregationPeriods, int> >::iterator kt2 = multFactPeriodCookieMap.find(alistP->getMultFactor());

		if (kt2 != multFactPeriodCookieMap.end()) {
			map<HlpStruct::AggregationPeriods, int>& perCookMap = kt2->second;
			map<HlpStruct::AggregationPeriods, int>::iterator kt3 = perCookMap.find(alistP->getAggregationPeriod());

			if (kt3 != perCookMap.end()) {

				// found
				perCookMap.erase(alistP->getAggregationPeriod());

				if (perCookMap.size()==0) {
					multFactPeriodCookieMap.erase(alistP->getMultFactor());

					if (multFactPeriodCookieMap.size()==0) {
						aggregatedKeys_m.erase( key );
					}
				}

			}

		}

	}


	// clear reverse entry
	map<CppUtils::String, set<int> >::iterator tt = allKeysForProviderSymbol_m.find(key);
	if (tt != allKeysForProviderSymbol_m.end()) {
		set<int>& allCookies = tt->second;

		allCookies.erase(alistP->getCookie());

		if (allCookies.size() == 0) {
			allKeysForProviderSymbol_m.erase( key );
		}
	}



	// clear cache itself
	alistP->clear();

	delete alistP;

	// 
	aggregatedData_m.erase(cookie);



	// remove row data
	cleanupAggregatedData(cookie);


}

/**
Helpers
*/


/*
void AggregatedCache::aggregatePassedData2(
	HlpStruct::RtData& newrtdata, 
	int const multfactor, 
	HlpStruct::AggregationPeriods const aggrperiod, 
	AggregationHolderList &alist
	)
{
	// whether symbol and provider mathes
	HTASSERT(alist.getProvider() == newrtdata.getProvider() && alist.getSymbol() == newrtdata.getSymbol());


	// filter out our working times
	if (beginDayTimeWork_m > 0 && endDayTimeWork_m > 0) {
		double timefromdaybegin = newrtdata.getTime()-CppUtils::roundToBeginningOfTheDay(newrtdata.getTime());
		if (timefromdaybegin < beginDayTimeWork_m || timefromdaybegin > endDayTimeWork_m) {
			return;
		}
	}
}
*/

void AggregatedCache::aggregatePassedData(
	HlpStruct::RtData& newrtdata, 
	int const multfactor, 
	HlpStruct::AggregationPeriods const aggrperiod, 
	AggregationHolderList &alist
	)
{

	// whether symbol and provider mathes
	HTASSERT(alist.getProvider() == newrtdata.getProvider() && alist.getSymbol() == newrtdata.getSymbol());


	// filter out our working times
	if (beginDayTimeWork_m > 0 && endDayTimeWork_m > 0) {
		double timefromdaybegin = newrtdata.getTime()-CppUtils::roundToBeginningOfTheDay(newrtdata.getTime());
		if (timefromdaybegin < beginDayTimeWork_m || timefromdaybegin > endDayTimeWork_m) {
			return;
		}
	}


	// if previous storage is empty
	if (alist.getListSize() ==0) {
		// first iteration

		HlpStruct::PriceData newPriceData(false);
		createNewPriceDataFromRtData(newPriceData, newrtdata);
		
		// NO need as by default
		//newPriceData.resetPeriodFinished();
	 

		if (aggrperiod==HlpStruct::AP_NoAggr) {
			// if no aggregation - that's it
			newPriceData.setPeriodFinished();
			alist.addPriceDataToEnd(newPriceData);
			RowPriceDataMap::iterator rowIter = insertAggregatedData(alist.getCookie(), &newPriceData);
			onNewRow(	rowIter, multfactor, aggrperiod, newPriceData, alist );
		
			return;
		}

		alist.addPriceDataToEnd(newPriceData);
		insertAggregatedData(alist.getCookie(), &newPriceData);
		
		
		//LOG( MSG_DEBUG, "HISTCONTEXT", "Inserted first ticker: " << newPriceData.toString());

		return;
	} 

	// there are existing data


	bool norm_insert = true;	
	HlpStruct::PriceData& lastStorage = alist.getLastPriceData();

	// initialize last sequentila data
	getPeriodSequenceNum(aggrperiod, multfactor, (long)lastStorage.time_m, alist.lastSequentionNum_m, alist.lastTimeOfCandle_m);


	if (newrtdata.getTime() <= lastStorage.time_m) {

		LOG(MSG_WARN, AGGRCACHE, "New ticker time: " << alist.getProvider() << " - " << alist.getSymbol() << " - " << CppUtils::getGmtTime(newrtdata.getTime()) <<" is LESS or EQUAL to the stored: " <<
			CppUtils::getGmtTime(lastStorage.time_m) << " will ignore this...");

		norm_insert = false;

	}


	if (aggrperiod==HlpStruct::AP_NoAggr) {

		// no aggregation
		HlpStruct::PriceData newPriceData(true);
		createNewPriceDataFromRtData(newPriceData, newrtdata);
		
		alist.addPriceDataToEnd(newPriceData);
		RowPriceDataMap::iterator rowIter = insertAggregatedData( alist.getCookie(), &newPriceData);

		onNewRow(	rowIter, multfactor, aggrperiod, newPriceData, alist );
		
		return;
	}


	// check
	CppUtils::UnsignedLongLong newSequentialNum;
	tm newTimeOfCandle;
	getPeriodSequenceNum(aggrperiod, multfactor, (long)newrtdata.getTime(), newSequentialNum, newTimeOfCandle);


	if (norm_insert) {
		// check whether period is overcame
		if (newSequentialNum > alist.lastSequentionNum_m) {


			// mark last entry as closed
			// period finished
			lastStorage.setPeriodFinished();

			// and update time
			// besides this is not the last candle - this is not from new period
			lastStorage.resetNewPeriod();

			lastStorage.time_m =  CppUtils::returnGmtFromTm(alist.lastTimeOfCandle_m);

			// inserted with updated time- e.i create new entry from prev storage - just have good time
			RowPriceDataMap::iterator rowIter = insertAggregatedData(alist.getCookie(), &lastStorage);

			// need to notify here that we inserted new data - already finished
			onNewRow(	rowIter, multfactor, aggrperiod, lastStorage, alist );

			// insert new unfinished data
			HlpStruct::PriceData newPriceData(false);

			createNewPriceDataFromRtData(newPriceData, newrtdata);
			alist.addPriceDataToEnd(newPriceData);

			//LOG( MSG_DEBUG, "HISTCONTEXT", "Inserted consequent ticker: " << newPriceData.toString());

		}
		else {
			// newSequentialNum == alist.lastSequentionNum_m

			// period is not finished, update with later data
			updatePriceDataWithLaterTime(lastStorage, newrtdata );
			
			// update time
			lastStorage.time_m = newrtdata.getTime();

			

		}


	} 

}

void AggregatedCache::onNewRow(
	RowPriceDataMap::iterator const& rowIt,
	int const multfactor, 
	HlpStruct::AggregationPeriods const aggrperiod, 
	HlpStruct::PriceData const& justUpdated,
	AggregationHolderList const& origAlist
	)
{
	// this function when new RT data is inserted or upadted
	// thus we are goinf to tranfer last row reference there

	//
	
	if (!calcCallbackMap_m.empty()) {
	
		
		HlpStruct::GlobalPriceDataList dataToInsert, currentRow;

		for(map<int, OnCalculatedFieldCallback*>::const_iterator it = calcCallbackMap_m.begin(); it != calcCallbackMap_m.end(); it++) {
			OnCalculatedFieldCallback& f = *it->second;

		
			dataToInsert.clear();

			// create current row
			for(HlpStruct::PriceDataMapPtr::const_iterator i4 = rowIt->second->begin(); i4 != rowIt->second->end(); i4 ++) {
				HlpStruct::GlobalPriceData gdata_i(i4->first, origAlist.getProvider(), origAlist.getSymbol(), *i4->second);
				currentRow.push_back(gdata_i);
			}

			CalculatedFieldCallbackCallback callback(*this, rowIt, multfactor, aggrperiod);
			f.onNewRowAdded(callback, currentRow, justUpdated.time_m, justUpdated, dataToInsert );
			
			for(int k = 0; k < dataToInsert.size(); k++) {

				// create new cache - leaving the same provider
				AggregationHolderList* alistP = NULL;
				resolveAggregationData(	true, dataToInsert[k].provider_m, dataToInsert[k].symbol_m,	multfactor, aggrperiod, &alistP); 
				HTASSERT(alistP != NULL);

				if (!alistP->isValid()) {

					alistP->initAggrPeriodMultFactor(aggrperiod, multfactor);
					alistP->getProvider() = dataToInsert[k].provider_m;
					alistP->getSymbol() = dataToInsert[k].symbol_m;
				}
				//

				HlpStruct::PriceData& calcPriceData = dataToInsert[k].pdata_m;
				calcPriceData.setPeriodFinished();
				calcPriceData.resetNewPeriod();
				calcPriceData.time_m = justUpdated.time_m;

			
				alistP->addOrUpdatePriceData(calcPriceData);
				// insert aggregated data for new symbol
				insertAggregatedData(alistP->getCookie(), &alistP->getLastPriceData() );
			}

			
		}
	}

	// if we insert calculated data need to make sure we have cached list for the symbol and resolve it

	

}



void AggregatedCache::createNewPriceDataFromRtData(HlpStruct::PriceData &storage, HlpStruct::RtData const& newrtdata)
{

	// shows that this is new perios
	storage.setNewPeriod();


	storage.time_m = newrtdata.getTime();


	if ( newrtdata.getType() ==  HlpStruct::RtData::HST_Type) {
		if (newrtdata.getHistData().volume_m > 0)
			storage.volume_m = newrtdata.getHistData().volume_m;
	}
	else if (newrtdata.getType() ==  HlpStruct::RtData::RT_Type ) {
		if (newrtdata.getRtData().volume_m > 0)
			storage.volume_m = newrtdata.getRtData().volume_m;
	}

	// new period has begun
	if (newrtdata.getType()==HlpStruct::RtData::HST_Type) {
		storage.open_m = newrtdata.getHistData().open_m;
		storage.high_m = newrtdata.getHistData().high_m;
		storage.low_m = newrtdata.getHistData().low_m;
		storage.close_m = newrtdata.getHistData().close_m;

		storage.open2_m = newrtdata.getHistData().open2_m;
		storage.high2_m = newrtdata.getHistData().high2_m;
		storage.low2_m = newrtdata.getHistData().low2_m;
		storage.close2_m = newrtdata.getHistData().close2_m;
	}
	else if (newrtdata.getType()==HlpStruct::RtData::RT_Type) {
		// rt data are expected to have 2 storages - bid & ask
		storage.open_m = newrtdata.getRtData().ask_m;
		storage.high_m = newrtdata.getRtData().ask_m;
		storage.low_m = newrtdata.getRtData().ask_m;
		storage.close_m = newrtdata.getRtData().ask_m;

		storage.open2_m = newrtdata.getRtData().bid_m;
		storage.high2_m = newrtdata.getRtData().bid_m;
		storage.low2_m = newrtdata.getRtData().bid_m;
		storage.close2_m = newrtdata.getRtData().bid_m;

	}
	else
		HTASSERT(false);


}

void AggregatedCache::updatePriceDataWithLaterTime(HlpStruct::PriceData &storage, HlpStruct::RtData const& newrtdata)
{
	// this was aggregated - e.i. this is not new period occured
	storage.resetNewPeriod();

	// later time - e.i.

	
	// common
	if ( newrtdata.getType() ==  HlpStruct::RtData::HST_Type) {
		if (newrtdata.getHistData().volume_m > 0 && storage.volume_m > 0)
			storage.volume_m += newrtdata.getHistData().volume_m;
	}
	else if (newrtdata.getType() ==  HlpStruct::RtData::RT_Type ) {
		if (newrtdata.getRtData().volume_m > 0 && storage.volume_m > 0)
			storage.volume_m += newrtdata.getRtData().volume_m;
	}

	// new period has begun
	if (newrtdata.getType()==HlpStruct::RtData::HST_Type) {
		// agregate
		if (storage.close_m > 0 && newrtdata.getHistData().close_m > 0)
			storage.close_m = newrtdata.getHistData().close_m;

		if (storage.high_m < newrtdata.getHistData().high_m && storage.high_m>0 && newrtdata.getHistData().high_m>0)
			storage.high_m = newrtdata.getHistData().high_m;

		if (storage.low_m > newrtdata.getHistData().low_m && storage.low_m>0 && newrtdata.getHistData().low_m>0)
			storage.low_m = newrtdata.getHistData().low_m;

		////
		if (storage.close2_m >0 && newrtdata.getHistData().close2_m>0)
			storage.close2_m = newrtdata.getHistData().close2_m;

		if (storage.high2_m < newrtdata.getHistData().high2_m && storage.high2_m>0 && newrtdata.getHistData().high2_m>0 )
			storage.high2_m = newrtdata.getHistData().high2_m;

		if (storage.low2_m > newrtdata.getHistData().low2_m && storage.low2_m>0 && newrtdata.getHistData().low2_m>0)
			storage.low2_m = newrtdata.getHistData().low2_m;

	}
	else if (newrtdata.getType()==HlpStruct::RtData::RT_Type) {
		// ask
		if (storage.close_m>0 && newrtdata.getRtData().ask_m>0)
			storage.close_m = newrtdata.getRtData().ask_m;

		if (storage.high_m < newrtdata.getRtData().ask_m && storage.high_m >0 && newrtdata.getRtData().ask_m>0)
			storage.high_m = newrtdata.getRtData().ask_m;

		if (storage.low_m > newrtdata.getRtData().ask_m && storage.low_m>0 && newrtdata.getRtData().ask_m>0)
			storage.low_m = newrtdata.getRtData().ask_m;

		// bid
		if (storage.close2_m>0 && newrtdata.getRtData().bid_m>0)
			storage.close2_m = newrtdata.getRtData().bid_m;

		if (storage.high2_m < newrtdata.getRtData().bid_m && storage.high2_m>0 && newrtdata.getRtData().bid_m>0)
			storage.high2_m = newrtdata.getRtData().bid_m;

		if (storage.low2_m > newrtdata.getRtData().bid_m && storage.low2_m>0 && newrtdata.getRtData().bid_m>0)
			storage.low2_m = newrtdata.getRtData().bid_m;
	}
	else
		HTASSERT(false);


}



void AggregatedCache::cleanupAggregatedData()
{
	// remove all data for te consistancy
	for(RowPriceDataMap::iterator it = allSymbolsData_m.begin(); it != allSymbolsData_m.end(); it++)	{
		HTASSERT(it->second != NULL);
		delete it->second;
	};

	allSymbolsData_m.clear();
}

void AggregatedCache::cleanupAggregatedData(int const cookie)
{
	CppUtils::DoubleList candidates;
	for(RowPriceDataMap::iterator it = allSymbolsData_m.begin(); it != allSymbolsData_m.end(); it++)	{
		HTASSERT(it->second != NULL);

		HlpStruct::PriceDataMapPtr::iterator it2 = it->second->find(cookie);
		if (it2 != it->second->end()) {
			//found, delete object
			it->second->erase(it2);

			if (it->second->empty()) {
				delete it->second;
				candidates.push_back(it->first);
			}

		}
	}

	for(int  i =0; i < candidates.size(); i++)	{
		allSymbolsData_m.erase(candidates[i]);
	}
}

//
void AggregatedCache::getPeriodSequenceNum(
	HlpStruct::AggregationPeriods const aggrperiod, 
	int const multfactor, 
	long const& curTime, 
	CppUtils::UnsignedLongLong& periodNum,
	tm& candleTime
	)
{

	tm tmBuf;
	tm* tmp = gmtime(&curTime);
	memcpy(&tmBuf, tmp, sizeof(tm));

	periodNum = -1;
	candleTime = tmBuf;
	//candleTime.tm_isdst = -1;
	candleTime.tm_isdst = 0;
	candleTime.tm_yday=-1;
	candleTime.tm_wday = -1;

	if (aggrperiod==HlpStruct::AP_Minute) {
		div_t tmp = div(tmBuf.tm_min, multfactor);
		HTASSERT(multfactor >= 1 && multfactor <= 60);

		periodNum = (tmBuf.tm_year + 1900) * (CppUtils::UnsignedLongLong)10000000 + tmBuf.tm_yday*10000 + tmBuf.tm_hour * 100 + tmp.quot;
		candleTime.tm_min = tmp.quot * multfactor;
		candleTime.tm_sec = 0;
	}
	else if (aggrperiod==HlpStruct::AP_Hour) {
		/*
		HTASSERT(multfactor ==1	 ||
		multfactor ==2  ||
		multfactor ==3  ||
		multfactor ==4  ||
		multfactor ==6  ||
		multfactor ==8  ||
		multfactor ==12  ||
		multfactor ==24 
		);  
		*/

		div_t tmp = div(tmBuf.tm_hour, multfactor);
		HTASSERT(multfactor >=1 && multfactor <= 24);
		periodNum = (tmBuf.tm_year + 1900) * 100000 + tmBuf.tm_yday*100 + tmp.quot;

		candleTime.tm_hour = tmp.quot * multfactor;
		candleTime.tm_min = 0;
		candleTime.tm_sec = 0;

	}
	else if (aggrperiod==HlpStruct::AP_Day) {
		HTASSERT(multfactor==1);
		// claculate the order of the day basing on current time
		periodNum = (tmBuf.tm_year + 1900) * 1000 + tmBuf.tm_yday;

		candleTime.tm_hour = 0;
		candleTime.tm_min = 0;
		candleTime.tm_sec = 0;

	}
	else if (aggrperiod==HlpStruct::AP_Week) {
		HTASSERT(multfactor==1);
		periodNum = (tmBuf.tm_year + 1900) * 1000 + calculateWeekOfYear(tmBuf);

		candleTime.tm_wday = 0;
		candleTime.tm_hour = 0;
		candleTime.tm_min = 0;
		candleTime.tm_sec = 0;

	}
	else if (aggrperiod==HlpStruct::AP_Month) {
		HTASSERT(multfactor==1);
		periodNum = (tmBuf.tm_year + 1900) * 1000 + tmBuf.tm_mon;

		candleTime.tm_mday = 0;
		candleTime.tm_hour = 0;
		candleTime.tm_min = 0;
		candleTime.tm_sec = 0;

	}
	else if (aggrperiod==HlpStruct::AP_Year) {
		HTASSERT(multfactor==1);
		periodNum = (tmBuf.tm_year + 1900) * 1000;

		candleTime.tm_mon = 0;
		candleTime.tm_mday = 0;
		candleTime.tm_hour = 0;
		candleTime.tm_min = 0;
		candleTime.tm_sec = 0;
	}
	else
		HTASSERT(false);
}

int AggregatedCache::calculateWeekOfYear(tm const& timestamp )
{
	// day of week for the 1st January of this year
	int curDOW = timestamp.tm_wday;

	tm firstjanuary, *fisrtjanuaryp;

	// 1st january
	firstjanuary.tm_year = timestamp.tm_year;
	firstjanuary.tm_mon = 0;
	firstjanuary.tm_isdst = 0;  // ?
	firstjanuary.tm_mday =1;
	firstjanuary.tm_hour = 0;
	firstjanuary.tm_min = 0;
	firstjanuary.tm_sec = 0;

	time_t fisrtjanuarytime = mktime(&firstjanuary) - _timezone;
	fisrtjanuaryp = localtime( &fisrtjanuarytime );


	int baseDOW = fisrtjanuaryp->tm_wday;

	int curDOY = timestamp.tm_yday;

	int baseWeek = baseDOW<4 ? 1 : 0;
	// грубо: номер нашей недели - это 
	// номер недели 1 января + количество целых недель
	int isoWeek = baseWeek + curDOY / 7;
	// коррекция: если данный день раньше в неделе, чем 1 января, 
	// то на самом деле неделя уже другая (следующая)
	if( curDOW < baseDOW )
		++isoWeek;

	return isoWeek;
}


HlpStruct::PriceDataMapPtr* AggregatedCache::resolveAggregatedData(double const ttime)
{
		RowPriceDataMap::iterator it = allSymbolsData_m.find(ttime);
		if (it!=allSymbolsData_m.end()) 
			return it->second;

		THROW(CppUtils::OperationFailed, "exc_CannotResolveAggregationData", "ctx_resolveAggregatedData", CppUtils::getGmtTime(ttime));

}

RowPriceDataMap::iterator AggregatedCache::insertAggregatedData(int const cookie, HlpStruct::PriceData* pricedata)
{
	// inserting invalidates no iterators tus leave it_m as it is


	// this must be inserted aotomatically if any
	RowPriceDataMap::iterator it = allSymbolsData_m.find(pricedata->time_m);

	if (it==allSymbolsData_m.end()) {
		pair< RowPriceDataMap::iterator, bool> it_ins =	allSymbolsData_m.insert( pair<double, HlpStruct::PriceDataMapPtr*>(pricedata->time_m, new HlpStruct::PriceDataMapPtr));
		HTASSERT(it_ins.second);
		
		it = it_ins.first;
		
	}

	(*it->second)[cookie] = pricedata;
	
	//LOG( MSG_DEBUG, "HISTCONTEXT", "Row data identified by cookie " << cookie << " was inserted to row cache " << (*allSymbolsData_m[ttime])[cookie]->toString() );
	
	return it;
};



/**
Accessors to aggregated data
*/
long AggregatedCache::getSize(int const cookie) const
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	// resolve  list
	map<int, AggregationHolderList* >::const_iterator it = aggregatedData_m.find(cookie);

	if (it == aggregatedData_m.end())
		THROW(CppUtils::OperationFailed, "exc_CookieNotFound", "ctx_getFirst", (long)cookie );		

	AggregationHolderList const& alist = (AggregationHolderList&)(*it->second);

	return alist.getListSize();

}

CppUtils::String const& AggregatedCache::getSymbol(int const cookie) const
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	// resolve  list
	map<int, AggregationHolderList* >::const_iterator it = aggregatedData_m.find(cookie);

	if (it == aggregatedData_m.end())
		THROW(CppUtils::OperationFailed, "exc_CookieNotFound", "ctx_getSymbol", (long)cookie );		

	AggregationHolderList const& alist = (AggregationHolderList&)(*it->second);

	return alist.getSymbol();
}

CppUtils::String const& AggregatedCache::getProvider(int const cookie) const
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	// resolve  list
	map<int, AggregationHolderList* >::const_iterator it = aggregatedData_m.find(cookie);

	if (it == aggregatedData_m.end())
		THROW(CppUtils::OperationFailed, "exc_CookieNotFound", "ctx_getProvider", (long)cookie );		

	AggregationHolderList const& alist = (AggregationHolderList&)(*it->second);

	return alist.getProvider();
}

void AggregatedCache::getAllCookiesList(CppUtils::IntList& cookies) const
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	cookies.clear();
	CppUtils::getKeys<int, AggregationHolderList* >(cookies, aggregatedData_m );

}

void AggregatedCache::dataFirst(int const cookie, DataPtr& dataptr)
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	// resolve  list
	map<int, AggregationHolderList* >::const_iterator it = aggregatedData_m.find(cookie);

	if (it == aggregatedData_m.end())
		THROW(CppUtils::OperationFailed, "exc_CookieNotFound", "ctx_dataFirst", (long)cookie );		

	dataptr.idx_m = -1;
	dataptr.valid_m = true;
	dataptr.firsttime_m = true;

}


void AggregatedCache::dataLast(int const cookie, DataPtr& dataptr)
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	dataptr.valid_m = false;
	map<int, AggregationHolderList* >::const_iterator it = aggregatedData_m.find(cookie);

	if (it == aggregatedData_m.end())
		THROW(CppUtils::OperationFailed, "exc_CookieNotFound", "ctx_dataLast", (long)cookie );		

	AggregationHolderList& alist = (AggregationHolderList&)(*it->second);

	dataptr.idx_m = alist.getListSize() - 1;
	if (dataptr.idx_m < 0) 
		return;

	HlpStruct::PriceData const& pricedata = alist.getPriceData(dataptr.idx_m);
	if (!pricedata.isPeriodFinished()) {
		if (--dataptr.idx_m < 0) 
			return;
	}

	//cutoff time
	//if (dataptr.cutOffTime_m > 0 && alist.getPriceData(dataptr.idx_m).time_m > dataptr.cutOffTime_m ) {
	//	return;
	//}

	// point after the last idx as dataPrev will decrement it
	dataptr.idx_m++;
	dataptr.valid_m = true;
	dataptr.firsttime_m = true;

}


void AggregatedCache::dataOffsetFromLast(int const cookie, long const offset, DataPtr& dataptr)
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	dataptr.valid_m = false;
	map<int, AggregationHolderList* >::const_iterator it = aggregatedData_m.find(cookie);

	if (it == aggregatedData_m.end())
		THROW(CppUtils::OperationFailed, "exc_CookieNotFound", "ctx_dataOffsetFromLast", (long)cookie );		

	AggregationHolderList& alist = (AggregationHolderList&)(*it->second);

	dataptr.idx_m = alist.getListSize() - 1- offset;
	if (dataptr.idx_m < 0) 
		return;

	HlpStruct::PriceData const& pricedata = alist.getPriceData(dataptr.idx_m);
	if ( offset == 0 && !pricedata.isPeriodFinished()) {
		if (--dataptr.idx_m < 0) 
			return;
	}


	dataptr.valid_m = true;
	dataptr.firsttime_m = false;

}

void AggregatedCache::dataPrev(int const cookie, DataPtr& dataptr)
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	map<int, AggregationHolderList* >::const_iterator it = aggregatedData_m.find(cookie);
	dataptr.valid_m = false;

	if (it == aggregatedData_m.end())
		THROW(CppUtils::OperationFailed, "exc_CookieNotFound", "ctx_dataPrev", (long)cookie );		

	AggregationHolderList& alist = (AggregationHolderList&)(*it->second);

	// out of upper limit
	dataptr.idx_m--;

	HTASSERT(dataptr.idx_m < alist.getListSize());
	if (dataptr.idx_m < 0) 
		return;


	/*
	if (dataptr.cutOffTime_m > 0 ) {
	while (alist.getPriceData(dataptr.idx_m).time_m > dataptr.cutOffTime_m ) {
	dataptr.idx_m--;

	if (dataptr.idx_m < 0)
	return;
	}
	}
	*/


	dataptr.firsttime_m = false;
	dataptr.valid_m = true;

}


void AggregatedCache::dataTimeFind(double const& timestamp, int const cookie, DataPtr& dataptr)
{

	LOCK_FOR_SCOPE(aggrMutex_m);
	map<int, AggregationHolderList* >::const_iterator it = aggregatedData_m.find(cookie);

	dataptr.valid_m = false;

	if (it == aggregatedData_m.end()) 
		THROW(CppUtils::OperationFailed, "exc_CookieNotFound", "ctx_dataTimeFind", (long)cookie );		

	AggregationHolderList& alist = (AggregationHolderList&)(*it->second);
	dataptr.idx_m = alist.find(timestamp, false );

	if (dataptr.idx_m > 0) {
		dataptr.valid_m = true;
		dataptr.firsttime_m = false;
	}

}


void AggregatedCache::dataNext(int const cookie, DataPtr& dataptr)
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	// resolve  list
	map<int, AggregationHolderList* >::const_iterator it = aggregatedData_m.find(cookie);
	dataptr.valid_m = false;

	if (it == aggregatedData_m.end())
		THROW(CppUtils::OperationFailed, "exc_CookieNotFound", "ctx_dataNext", (long)cookie );		


	AggregationHolderList& alist = (AggregationHolderList&)(*it->second);


	// out of upper limit
	dataptr.idx_m++;

	if (dataptr.idx_m >= alist.getListSize()) {
		return;
	}

	HlpStruct::PriceData const& pricedata = alist.getPriceData(dataptr.idx_m);
	if (!pricedata.isPeriodFinished()) {
		return;
	}

	//cutoff time
	/*
	if (dataptr.cutOffTime_m > 0 && alist.getPriceData(dataptr.idx_m).time_m > dataptr.cutOffTime_m ) {
	return;
	}
	*/

	dataptr.firsttime_m = false;  
	dataptr.valid_m = true;

}

HlpStruct::PriceData const& AggregatedCache::getCurrentPriceData(const int cookie, DataPtr const& dataptr) const
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	// resolve  list
	map<int, AggregationHolderList* >::const_iterator it = aggregatedData_m.find(cookie);

	if (it == aggregatedData_m.end())
		THROW(CppUtils::OperationFailed, "exc_CookieNotFound", "ctx_getCurrentPriceData", (long)cookie );		


	AggregationHolderList const& alist = (AggregationHolderList const&)(*it->second);

	if (!dataptr.isValid() )
		THROW(CppUtils::OperationFailed, "exc_InvalidDataPointer", "ctx_getCurrentPriceData", (long)cookie );

	HTASSERT(alist.getListSize()>=0);

	return alist.getPriceData(dataptr.idx_m);
}

HlpStruct::PriceData const& AggregatedCache::getBackwardPriceData(const int cookie, DataPtr const& dataptr, long const offset ) const
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	// resolve  list
	map<int, AggregationHolderList* >::const_iterator it = aggregatedData_m.find(cookie);

	if (it == aggregatedData_m.end())
		THROW(CppUtils::OperationFailed, "exc_CookieNotFound", "ctx_getBackwardPriceData", (long)cookie );		


	AggregationHolderList const& alist = (AggregationHolderList const&)(*it->second);

	if (!dataptr.isValid() )
		THROW(CppUtils::OperationFailed, "exc_InvalidDataPointer", "ctx_getBackwardPriceData", (long)cookie );

	// backward offset
	long newidx = dataptr.idx_m - offset;

	if (newidx < 0)
		THROW(CppUtils::OperationFailed, "exc_NegativeOffset", "ctx_getBackwardPriceData", (long)cookie );

	if (newidx >= alist.getListSize())
		THROW(CppUtils::OperationFailed, "exc_LargeOffset", "ctx_getBackwardPriceData", (long)cookie );

	HTASSERT(alist.getListSize()>=0);

	return alist.getPriceData(newidx);
}



/*
Rows iteration
*/

long AggregatedCache::getRowsCount()
{
	LOCK_FOR_SCOPE(aggrMutex_m);


	if (allSymbolsData_m.size() <=0)
		return -1;

	return allSymbolsData_m.size()-1;

}

/////


void AggregatedCache::dataRowFirst(DataRowPtr& rowPtr)
{
	LOCK_FOR_SCOPE(aggrMutex_m);


	// invalidates iterator
	rowPtr.firsttime_m=true;
	rowPtr.valid_m = true;
	rowPtr.isForward_m  =true;

}

void AggregatedCache::dataRowLast(DataRowPtr& rowPtr)
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	rowPtr.firsttime_m=true;
	rowPtr.valid_m = true;
	rowPtr.isForward_m = false;


}


void AggregatedCache::dataRowNext(DataRowPtr& rowPtr)
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	rowPtr.valid_m=false;

	if (!rowPtr.isForward_m)
		THROW(CppUtils::OperationFailed, "exc_InvalidDirection", "ctx_dataRowNext", "")

		if (rowPtr.firsttime_m) {
			if (allSymbolsData_m.empty()) 
				return;
			rowPtr.rowIt_m = allSymbolsData_m.begin();
		}
		else 
			rowPtr.rowIt_m++;

	if (rowPtr.rowIt_m != allSymbolsData_m.end()) {

		/*
		// check cutoff time
		if (rowPtr.cutOffTime_m > 0 && rowPtr.rowIt_m->first > rowPtr.cutOffTime_m ) 
		return;
		*/

		rowPtr.firsttime_m = false;
		rowPtr.valid_m=true;
		return;
	}

	return;
}

void AggregatedCache::dataRowPrev(DataRowPtr& rowPtr)
{
	LOCK_FOR_SCOPE(aggrMutex_m);
	rowPtr.valid_m=false;

	if (rowPtr.isForward_m)
		THROW(CppUtils::OperationFailed, "exc_InvalidDirection", "ctx_dataRowPrev", "")

		if (rowPtr.firsttime_m) {
			if (allSymbolsData_m.empty()) 
				return;

			rowPtr.rRowIt_m = allSymbolsData_m.rbegin();

			// check cutoff time
			/*
			if (rowPtr.cutOffTime_m > 0) { 
			while (rowPtr.rRowIt_m->first > rowPtr.cutOffTime_m ) {
			// backward
			rowPtr.rRowIt_m++;

			if (rowPtr.rRowIt_m==allSymbolsData_m.rend()) 
			// return - invalid
			return;


			}
			}
			*/


		}
		else 
			rowPtr.rRowIt_m++;

	if (rowPtr.rRowIt_m != allSymbolsData_m.rend()) {	
		rowPtr.firsttime_m = false;
		rowPtr.valid_m=true;
		return;
	}

	return;

}

HlpStruct::PriceData const& AggregatedCache::getCurrentDataRowTryLocatePrevValid(int const scookie, DataRowPtr const& rowPtr) const
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	if (!rowPtr.isValid())
		THROW(CppUtils::OperationFailed, "exc_InvalidRowPointer", "ctx_getCurrentDataRowTryLocatePrevValid", "");

	
	HlpStruct::PriceDataMapPtr::iterator it2;
	RowPriceDataMap::const_iterator itmp;

	if (rowPtr.isForward_m) {

		it2 = rowPtr.rowIt_m->second->find(scookie);

		if (it2==rowPtr.rowIt_m->second->end())	{
			// try to locate previous valid
			RowPriceDataMap::const_iterator itmp = rowPtr.rowIt_m;
			
			// nothing to return if we are at the beginning
			if (itmp == allSymbolsData_m.begin()) 
					return dummyPriceData_m;
			
			while(true) {
				itmp--;

				it2 =	itmp->second->find(scookie);

				if (it2 != itmp->second->end() )
					return *(it2->second);
				
				if (itmp == allSymbolsData_m.begin()) 
					break;
				
			}

			return dummyPriceData_m;
	
		}
		else
			return *(it2->second);

	}
	else {

		it2 = rowPtr.rRowIt_m->second->find(scookie);

		if (it2==rowPtr.rRowIt_m->second->end()) {
			// try to locate previous valid
			RowPriceDataMap::const_reverse_iterator irtmp = rowPtr.rRowIt_m;
						
			while(true) {
				irtmp++;

				if (irtmp == allSymbolsData_m.rend())
					break;

				it2 =	irtmp->second->find(scookie);

				if (it2 != irtmp->second->end() )
					return *(it2->second);
				
			}

			return dummyPriceData_m;
		}
		else
			return *(it2->second);

	}
}

HlpStruct::PriceData const& AggregatedCache::getCurrentDataRow(int const scookie, DataRowPtr const& rowPtr) const
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	if (!rowPtr.isValid())
		THROW(CppUtils::OperationFailed, "exc_InvalidRowPointer", "ctx_getCurrentDataRow", "");

	
	if (rowPtr.isForward_m) {

		HlpStruct::PriceDataMapPtr::iterator it2 = rowPtr.rowIt_m->second->find(scookie);

		if (it2==rowPtr.rowIt_m->second->end())
			return dummyPriceData_m;

		return *(it2->second);

	}
	else {

		HlpStruct::PriceDataMapPtr::iterator it2 = rowPtr.rRowIt_m->second->find(scookie);

		if (it2==rowPtr.rRowIt_m->second->end())
			return dummyPriceData_m;

		return *(it2->second);

	}


}

double AggregatedCache::getCurrentDataRowTime(DataRowPtr const& rowPtr) const
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	if (!rowPtr.isValid())
		THROW(CppUtils::OperationFailed, "exc_InvalidRowPointer", "ctx_getCurrentDataRowTime", "");

	if (rowPtr.isForward_m) 
		return  rowPtr.rowIt_m->first;
	else
		return rowPtr.rRowIt_m->first;

}

HlpStruct::PriceData const&  AggregatedCache::findDataRow(int const scookie, double const& ttime, DataRowPtr const& rowPtr) const
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	/*
	if (rowPtr.cutOffTime_m <0)
	THROW(CppUtils::OperationFailed, "exc_InvalidCutoffTime", "ctx_findDataRow", "");
	*/

	// perfrm serach of necessary time
	// returns equal or greater 
	

	RowPriceDataMap::const_iterator it = allSymbolsData_m.lower_bound(ttime);
	if (it==allSymbolsData_m.end())
		return dummyPriceData_m;


	// strict match
	if (it->first==ttime) {
		HlpStruct::PriceDataMapPtr* const& priceData = it->second;

		HlpStruct::PriceDataMapPtr::iterator it2 = priceData->find(scookie);
		if (it2==priceData->end())
			return dummyPriceData_m;

		return *(it2->second);

	}

	/*
	if (it->first > rowPtr.cutOffTime_m)
	return dummyPriceData_m; 
	*/

	// no strict match and we are in beginning
	if (it==allSymbolsData_m.begin())
		return dummyPriceData_m;

	// go back
	it--;
	HlpStruct::PriceDataMapPtr* const& priceData = it->second;
	HlpStruct::PriceDataMapPtr::iterator it2 = priceData->find(scookie);
	if (it2==priceData->end())
		return dummyPriceData_m;



	return *(it2->second);

}

void AggregatedCache::cleanupAllCache()
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	// free cached data
	CppUtils::IntList cookiesList;
	for(map<CppUtils::String, map<int, map<HlpStruct::AggregationPeriods, int> > >::iterator it = aggregatedKeys_m.begin(); 
		it != aggregatedKeys_m.end(); it++) 
	{
		map<int, map<HlpStruct::AggregationPeriods, int> >& multFactPeriodCookieMap = it->second;

		for(map<int, map<HlpStruct::AggregationPeriods, int> >::iterator it2 = multFactPeriodCookieMap.begin(); 
			it2 != multFactPeriodCookieMap.end(); it2++)
		{

			map<HlpStruct::AggregationPeriods, int>& periodMap = it2->second;
			for(map<HlpStruct::AggregationPeriods, int>::iterator it3 = periodMap.begin(); it3 != periodMap.end(); it3++) {
				cookiesList.push_back(it3->second);
			}
		}


	}

	for(int i = 0; i < cookiesList.size(); i++) {
		clearCache(cookiesList[ i ]);
		LOG( MSG_DEBUG, AGGRCACHE, "Removed cache for cookie " << cookiesList[ i ] );
	}

	cleanupAggregatedData();
	LOG( MSG_DEBUG, AGGRCACHE, "Aggregation data cleaned" );
}


//////

void AggregatedCache::pushRtData(HlpStruct::RtData & rtdata, EngineThread* engineThread, bool const& tickerInsideExistingTime)
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	// here we need to aggregate data if aggregation list is valid

	AggregationHolderList* alistP = NULL;


	//LOG( MSG_DEBUG, "HISTCONTEXT", "-> PUSHED RT DATA: " << rtdata.toString() );

	//CppUtils::String key (rtdata.getProvider());
	//key+=rtdata.getSymbol();


	char key[MAX_KEY_BUF];
	strcpy(key, rtdata.getProvider());
	strcat(key, rtdata.getSymbol());

	map<CppUtils::String, set<int> >::iterator it = allKeysForProviderSymbol_m.find( key );
	if (it != allKeysForProviderSymbol_m.end()) {

		set<int>& keys = it->second;
		for(set<int>::iterator itk = keys.begin(); itk != keys.end(); itk++) {

			// we have cookie here
			alistP = aggregatedData_m[ *itk ];

			// have caching data ??
			HTASSERT (alistP != NULL && alistP->isValid());
			aggregatePassedData(rtdata, alistP->getMultFactor(), alistP->getAggregationPeriod(), *alistP);

			//LOG( MSG_DEBUG, "HISTCONTEXT", "Aggregated RT data - symbol \"" << rtdata.toString() << "\" provider \"" << 
			//	rtdata.getProvider() << "\" aggregation period \""	<< alistP->aggrPeriod_m << "\" multfactor \"" << alistP->multfactor_m << "\"" );

		}

	}

	//CppUtils::String dmp = dump();

	// do not pass it if ticker already exists
	if (engineThread && (!tickerInsideExistingTime))
		engineThread->pushRtDataHelper(rtdata);
}
//


CppUtils::String AggregatedCache::dump(bool const very_detailed)
{
	LOCK_FOR_SCOPE(aggrMutex_m);


	CppUtils::String result("Aggregation Map: \n");


	for(map<CppUtils::String, map<int, map<HlpStruct::AggregationPeriods, int> > >::iterator it = aggregatedKeys_m.begin(); 
		it != aggregatedKeys_m.end(); it++) 
	{
		result += "symbol provider key: '"+it->first+"':\n";

		map<int, map<HlpStruct::AggregationPeriods, int> >& multFactPeriodCookieMap = it->second;

		for(map<int, map<HlpStruct::AggregationPeriods, int> >::iterator it2 = multFactPeriodCookieMap.begin(); 
			it2 != multFactPeriodCookieMap.end(); it2++)
		{

			result += "_ multfactor: "+ CppUtils::long2String(it2->first)+":\n";

			map<HlpStruct::AggregationPeriods, int>& periodMap = it2->second;
			for(map<HlpStruct::AggregationPeriods, int>::iterator it3 = periodMap.begin(); it3 != periodMap.end(); it3++) {
				int cookie = it3->second;
				result += "__ aggregation period: "+HlpStruct::aggrPeriodToString()(it3->first)+" : -> cookie: " + 
					CppUtils::long2String(cookie) + " has list with details: \n";

				result += CppUtils::String("     [\n") + 
					" size: " + CppUtils::long2String(aggregatedData_m[cookie]->getListSize()) + 
					" mult factor: " + CppUtils::long2String(aggregatedData_m[cookie]->getMultFactor()) + 
					" provider: [" + aggregatedData_m[cookie]->getProvider()+ "]"+
					" symbol: [" + aggregatedData_m[cookie]->getSymbol() + "]"+
					(very_detailed ? "\n ALL DATA: (\n" +	ContextAccessor::dump(*this, cookie, true) + "\n ALL DATA END )\n" : "" ) +
					"\n      ]\n"; 

			}	
		}

	}

	return result;


}

CppUtils::String AggregatedCache::dumpAllRows() const
{
	LOCK_FOR_SCOPE(aggrMutex_m);

	CppUtils::String r;

	CppUtils::IntList cookies;
	getAllCookiesList(cookies);

	for(RowPriceDataMap::const_iterator it = allSymbolsData_m.begin(); it != allSymbolsData_m.end(); it++) {
		double const ttime = it->first;
		
		r.append("[").append(CppUtils::getGmtTime(ttime)).append("]");

		HlpStruct::PriceDataMapPtr const& row_i = *it->second;
		
		for(int i =0; i < cookies.size(); i++) {
		
			

			r.append("[");
			int const cookie_i = cookies[i];

			// get alist
			
			HlpStruct::PriceDataMapPtr::const_iterator i2 = row_i.find(cookies[i]);
			if (i2 == row_i.end()) {

				r.append("Cookie is invalid: ").append(CppUtils::long2String(cookie_i)).append(";");
				
			} else {
				map<int, AggregationHolderList* >::const_iterator ita = aggregatedData_m.find(cookie_i);
				HTASSERT(ita != aggregatedData_m.end());

				HlpStruct::PriceData const& pdata_i = *i2->second;
															   
				r.append("Cookie: ").append(CppUtils::long2String(cookie_i)).append(";");
				r.append(ita->second->getProvider()).append(" - ").append(ita->second->getSymbol()).append(": ");
				if (pdata_i.isValid()) {
					r.append(pdata_i.toString());
				}	
				else {
					r.append("Data not valid");
				}
				
				r.append(";");
			}

			
			r.append("] ");
			
		}
		r.append("\n");

	}
	
	return r;

}


} // end of namespace