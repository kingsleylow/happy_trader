#include "aholder.hpp"
#include "enginethread.hpp"


#define AGGRHOLDERLIST "AGGRHOLDERLIST"

namespace Inqueue {

	int AggregationHolderList::id_m = 0;

	AggregationHolderList::AggregationHolderList()
	{
		clear();
	};

	void AggregationHolderList::clear() {

		aggrPeriod_m = HlpStruct::AP_Dummy;
		multfactor_m = -1;
		dataList_m.clear();
		provider_m = "";
		symbol_m = "";
		cookie_m = -1;
		lastSequentionNum_m = -1;
		memset(&lastTimeOfCandle_m, 0, sizeof(tm));
	}

	void AggregationHolderList::clearOnlyPriceData()
	{
		dataList_m.clear();
	}

	void AggregationHolderList::initAggrPeriodMultFactor(HlpStruct::AggregationPeriods const aggrperiod, int const multfactor)
	{
		int n_multfactor = multfactor;
		if (aggrperiod==HlpStruct::AP_Minute) {
			div_t tmp = div(60, multfactor);
			if (tmp.rem !=0)
				THROW(CppUtils::OperationFailed, "exc_InvalidMultFactor", "ctx_InitAggregationList", (long)multfactor);
		}
		else if (aggrperiod==HlpStruct::AP_Hour) {
			div_t tmp = div(24, multfactor);
			if (tmp.rem !=0)
				THROW(CppUtils::OperationFailed, "exc_InvalidMultFactor", "ctx_InitAggregationList", (long)multfactor);

		}
		else if (aggrperiod==HlpStruct::AP_Day) {
			if (multfactor!=1)
				THROW(CppUtils::OperationFailed, "exc_InvalidMultFactor", "ctx_InitAggregationList", (long)multfactor);


		}
		else if (aggrperiod==HlpStruct::AP_Week) {
			if (multfactor!=1)
				THROW(CppUtils::OperationFailed, "exc_InvalidMultFactor", "ctx_InitAggregationList", (long)multfactor);


		}
		else if (aggrperiod==HlpStruct::AP_Month) {
			if (multfactor!=1)
				THROW(CppUtils::OperationFailed, "exc_InvalidMultFactor", "ctx_InitAggregationList", (long)multfactor);


		}
		else if (aggrperiod==HlpStruct::AP_Year) {
			if (multfactor!=1)
				THROW(CppUtils::OperationFailed, "exc_InvalidMultFactor", "ctx_InitAggregationList", (long)multfactor);

		}
		else if (aggrperiod == HlpStruct::AP_NoAggr) {
			// just set to one
			n_multfactor = 1;
		}
		else {
			THROW(CppUtils::OperationFailed, "exc_InvalidAggregationPeriod", "ctx_InitAggregationList", (long)aggrperiod);
		}

		aggrPeriod_m = aggrperiod;
		multfactor_m = n_multfactor;
	}

	long AggregationHolderList::find(double const timestamp, bool exact) const
	{

		if (dataList_m.size() ==0)
			return -1;

		//LOG(MSG_DEBUG, AGGRHOLDERLIST, "Binary search in a cache begin, size of list is: " << dataList_m.size() << " exact: " << exact);

		long l = 0;
		long u = dataList_m.size() - 1;

		// ignore last entry if period not finished
		if (!dataList_m[u].isPeriodFinished()) {
			//LOG(MSG_DEBUG, AGGRHOLDERLIST, "Do not care about unclosed candle: " << CppUtils::getGmtTime(dataList_m[u].time_m));
			u--;
		}



		long i = -1;

		while (l <= u)
		{
			i = (l + u) >> 1;

			double const& f = dataList_m[i].time_m;
			//LOG(MSG_DEBUG, AGGRHOLDERLIST, "i=" << i << " l=" << l << " u=" << u);

			//LOG(MSG_DEBUG, AGGRHOLDERLIST, "Compare our searching datetime: " << CppUtils::getGmtTime(timestamp) << " with #" <<
			//	i << " - " << CppUtils::getGmtTime(f) );

			if (timestamp < f) {
				u = i - 1;
			}
			else if (timestamp > f) {
				l = i + 1;
			}
			else {
				return i;
			}

		}


		if (exact)
			return -1;

		if (i >=1)
			return l - 1;
		else
			return 0;

	}

	bool AggregationHolderList::isValid() const {
		return symbol_m.size() > 0 && cookie_m > 0;
	}

	double AggregationHolderList::getBeginDate() const
	{
		if (dataList_m.size() <=0)
			return -1;

		// 0 index
		return dataList_m.at(0).time_m;
	}

	double AggregationHolderList::getEndDate() const
	{
		if (dataList_m.size() <=0)
			return -1;

		return dataList_m.at( dataList_m.size() - 1 ).time_m; 
	}

	HlpStruct::PriceData& AggregationHolderList::addPriceDataToAnyPlace(HlpStruct::PriceData const& pricedata)
	{


		// this performs search
		long l = 0;
		long u = dataList_m.size() - 1;
		long i = -1;

		if (u < 0) {
			// no data

			return *(dataList_m.insert(dataList_m.end(), pricedata ));
		}

		while (l <= u)	{
			i = (l + u) >> 1;

			double const &f = dataList_m[i].time_m;

			if (pricedata.time_m < f) {
				u = i - 1;
			}
			else if (pricedata.time_m > f) {
				l = i + 1;
			}
			else {
				// found exact match pricedata.time_m == f
				return *(dataList_m.insert(dataList_m.begin() + i, pricedata ));
			}

		}

		// this means our data is more ALL elements
		if (l >= dataList_m.size()) {
			return *(dataList_m.insert(dataList_m.end(), pricedata ));
		}

		// this means our data is less than ALL elements
		if (u < 0) {
			return *(dataList_m.insert(dataList_m.begin(), pricedata ));
		}


		// what we have that l element is swapped with u and u < l
		// so l shows the first existing after our being inserted

		return *(dataList_m.insert(dataList_m.begin() + l, pricedata ));

	}

	void AggregationHolderList::addOrUpdatePriceData(HlpStruct::PriceData const& pricedata)
	{
		// this performs search
		long l = 0;
		long u = dataList_m.size() - 1;
		long i = -1;

		if (u < 0) {
			// no data

			dataList_m.insert(dataList_m.end(), pricedata );
			return;
		}

		while (l <= u)	{
			i = (l + u) >> 1;

			double const &f = dataList_m[i].time_m;

			if (pricedata.time_m < f) {
				u = i - 1;
			}
			else if (pricedata.time_m > f) {
				l = i + 1;
			}
			else {
				// found exact match pricedata.time_m == f
				deque<HlpStruct::PriceData>::iterator cur_it = dataList_m.begin() + i;
				*cur_it = pricedata;
				return;
			}

		}

		// this means our data is more ALL elements
		if (l >= dataList_m.size()) {
			dataList_m.insert(dataList_m.end(), pricedata );
			return;
		}

		// this means our data is less than ALL elements
		if (u < 0) {
			dataList_m.insert(dataList_m.begin(), pricedata );
			return;
		}


		// what we have that l element is swapped with u and u < l
		// so l shows the first existing after our being inserted

		deque<HlpStruct::PriceData>::iterator cur_it = dataList_m.begin() + l;
		*cur_it = pricedata;
	}

	HlpStruct::PriceData* AggregationHolderList::exactFindEntry(double const& ttime)
	{
		// this performs search
		long l = 0;
		long u = dataList_m.size() - 1;
		long i = -1;

		if (u < 0) {
			return NULL;
		}


		while (l <= u)	{
			i = (l + u) >> 1;

			double const &f = dataList_m[i].time_m;

			if (ttime < f) {
				u = i - 1;
			}
			else if (ttime > f) {
				l = i + 1;
			}
			else {
				// found exact match pricedata.time_m == f
				return &dataList_m[i];
			}

		}

		// not found
		return NULL;


	}



}; // end of namespace
