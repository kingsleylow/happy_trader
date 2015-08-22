#include "multcache.hpp"

#include "../math/mathengines.hpp"


namespace HlpStruct {
	MultiSymbolCache::MultiSymbolCache(CallBack* callback, int const maxSize, bool const removeLatest):
		maxSize_m( maxSize ),
		removeLatest_m(removeLatest),
		callbackPtr_m(callback),
		physicalFieldsNumber_m(0),
		calculatedFieldsNumber_m(0)
	{
	}

	MultiSymbolCache::~MultiSymbolCache()
	{
	}

	void MultiSymbolCache::setUpParameters(int const maxSize, bool const removeLatest)
	{
		maxSize_m = (maxSize > 0 ? maxSize : -1 );
		removeLatest_m = removeLatest; 

		if (cache_m.size() == 0 || maxSize_m < 0 ) 
			return;

		if (cache_m.size() <= maxSize_m)
			return;

		// must adopt existing number of rows
		map<CppUtils::Int64, RowType >::iterator it_first = cache_m.begin();
		map<CppUtils::Int64, RowType >::iterator it_last = cache_m.end();

		map<CppUtils::Int64, RowType >::iterator it_remove = CppUtils::advanceContainer< map<CppUtils::Int64, RowType >::iterator >(it_first, maxSize_m ); 

		if (removeLatest_m) {
			// remove last elements
			cache_m.erase(it_remove, cache_m.end());
		}
		else {
			// remove first elements
			cache_m.erase(cache_m.begin(), it_remove);
		}

		if (maxSize_m != cache_m.size())
			THROW(CppUtils::IntegrityViolation, "exc_InvalidRemovalFromCache", "MultiSymbolCacheSetup", "");
	}

	void MultiSymbolCache::registerSymbol(CppUtils::String const& symbol)
	{
		SymbolDescriptor descr(symbol, ST_Physical);
		if (symbols_m.find(descr) != symbols_m.end())
			 THROW(CppUtils::OperationFailed, "exc_SymbolAlreadyRegistered", "ctx_registerSymbol", symbol);

		symbols_m.insert(	descr );
		updateFieldsNumber();
	}

	void MultiSymbolCache::unregisterSymbol(CppUtils::String const& symbol)
	{
		SymbolDescriptor descr(symbol, ST_Physical);
		if (symbols_m.find(descr) == symbols_m.end())
			 THROW(CppUtils::OperationFailed, "exc_SymbolNotRegistered", "ctx_unregisterSymbol", symbol);

		symbols_m.erase(	descr );
		updateFieldsNumber();
	}

	void MultiSymbolCache::registerCalculatedSymbol(CppUtils::String const& symbol)
	{
		SymbolDescriptor descr(symbol, ST_Calculated);
		if (symbols_m.find(descr) != symbols_m.end())
			 THROW(CppUtils::OperationFailed, "exc_SymbolAlreadyRegistered", "ctx_registerSymbol", symbol);

		symbols_m.insert(	descr );
		updateFieldsNumber();
	}

	void MultiSymbolCache::unregisterCalculatedSymbol(CppUtils::String const& symbol)
	{
		SymbolDescriptor descr(symbol, ST_Calculated);
		if (symbols_m.find(descr) == symbols_m.end())
			 THROW(CppUtils::OperationFailed, "exc_SymbolNotRegistered", "ctx_unregisterSymbol", symbol);

		symbols_m.erase(	descr );
		updateFieldsNumber();
	}

	void MultiSymbolCache::clear()
	{
		cache_m.clear();
		symbols_m.clear();

	}

	void MultiSymbolCache::clearData()
	{
		cache_m.clear();
	
	}

	void MultiSymbolCache::addPriceDataAsImportedFromPast(double const& new_ttime, CppUtils::String const& symbol, float const& price_value)
	{
		HlpStruct::PriceData pdata_prev;

		pdata_prev.open2_m = price_value;
		pdata_prev.close2_m = price_value;
		pdata_prev.high2_m = price_value;
		pdata_prev.low2_m = price_value;

		pdata_prev.open_m = price_value;
		pdata_prev.close_m = price_value;
		pdata_prev.high_m = price_value;
		pdata_prev.low_m = price_value;

		pdata_prev.setImportedFromPast();

		pdata_prev.time_m = new_ttime;
		pdata_prev.volume_m = 0;
																							   

		addPriceData(symbol, pdata_prev);
	}

	void MultiSymbolCache::addPriceData(CppUtils::String const& symbol, PriceData const& pdata)
	{
	
		SymbolDescriptor sdescr(symbol, ST_Physical);

		if (symbols_m.find(sdescr) == symbols_m.end())
			 THROW(CppUtils::OperationFailed, "exc_SymbolNotRegistered", "ctx_addPriceData", symbol);

		if (!pdata.isValid())
			THROW(CppUtils::OperationFailed, "exc_InvalidPriceData", "ctx_addPriceData", "");

		

		// we have only milliseconds
		Math::DoubleLongHolder<3> lholder( pdata.time_m);
			
		if (maxSize_m != 0) {

			bool inserted = false;
			map<CppUtils::Int64, RowType >::iterator itr = cache_m.find(lholder.getLongInternalValue());
			if (itr == cache_m.end()) {
				// not found
	
				pair< map<CppUtils::Int64, RowType >::iterator, bool> ins_res = 
					cache_m.insert( pair<CppUtils::Int64, RowType >(lholder.getLongInternalValue(), RowType()));

				HTASSERT(ins_res.second);
				map<CppUtils::Int64, RowType >::iterator& inserted_it= ins_res.first;
				RowType &new_row = inserted_it->second;

				new_row[symbol] = pdata;
				inserted = true;

				// notify
				propagate_onUpdateCaclulatedField(lholder.getLongInternalValue(), pdata.time_m, new_row);

			}
			else {
				// already found
				RowType& row = itr->second;
				row[symbol] = pdata;

				// notify
				propagate_onUpdateCaclulatedField(lholder.getLongInternalValue(), pdata.time_m, row);
			}

			
			if (inserted) {
				if (cache_m.size() <= maxSize_m || maxSize_m < 0)
					return;

				map<CppUtils::Int64, RowType >::iterator it_first = cache_m.begin();
				map<CppUtils::Int64, RowType >::iterator it_last = cache_m.end();
				it_last--;

				// actual insert
				if (removeLatest_m) {
					cache_m.erase(it_last);		
				}
				else {
					cache_m.erase(it_first);	
				}
			}
		}
		else {
			// do nothing
		}

	}

	bool MultiSymbolCache::first()
	{
		if (cache_m.empty())
			return false;

		it_m = cache_m.begin();
		return true;
	}

	bool MultiSymbolCache::next()
	{
		// we must invalidate iterator
		// on the next call after the last element
		if (cache_m.empty())
			return false;

		if (++it_m == cache_m.end()) {
			// if after advancing the element we see it ponts to the end, return false
			return false;
		}

		return true;
	}

	bool MultiSymbolCache::last()
	{

		if (cache_m.empty())
			return false;

		// points to the last element
		it_m = cache_m.end();
		it_m--;

		return true;
	}

	bool MultiSymbolCache::prev()
	{
		if (cache_m.empty())
			return false;

		if (it_m == cache_m.begin()) {
			it_m = cache_m.end();
			return false;
		}
		

		it_m--;
		return true;
	}

	

	bool MultiSymbolCache::setOffsetFromLast(int const offset)
	{
		if (offset < 0 )
			THROW(CppUtils::OperationFailed, "exc_InvalidOffset", "ctx_setOffsetFromLast", offset );
		
		if (cache_m.empty())
			return false;

		if (cache_m.size() <= offset)
			return false;

		// if zero - set to the last element
		int idx = offset;
		it_m = cache_m.end();
				
		while(idx-- >= 0) {
			it_m--;	
		}

		return true;
	}

	bool MultiSymbolCache::isPtrValid() const
	{
		if (cache_m.empty())
			return false;

		if (it_m == cache_m.end())
			return false;

		return true;
	}


	int MultiSymbolCache::size() const
	{
		return cache_m.size();
	}

	PriceData const& MultiSymbolCache::get(CppUtils::String const& symbol) const
	{
		if (cache_m.size() == 0)
			THROW(CppUtils::OperationFailed, "exc_EmptyList", "ctx_get", "");

		if (it_m == cache_m.end())
			THROW(CppUtils::OperationFailed, "exc_IteratorNotValid", "ctx_get", "");

		static const PriceData dummy;

		// now need to make sure that our row is valid
		RowType const& pdataMap = it_m->second;
		RowType::const_iterator it2 = pdataMap.find( symbol );

		if ( it2 == pdataMap.end() )
			return dummy;


		return it2->second;
	}

	PriceData const& MultiSymbolCache::getOffsetFromLastData(CppUtils::String const& symbol, int const offset) const
	{
		if (offset < 0 )
			THROW(CppUtils::OperationFailed, "exc_InvalidOffset", "ctx_getOffsetFromLastData", offset );

		if (cache_m.empty())
			THROW(CppUtils::OperationFailed, "exc_EmptyList", "ctx_getOffsetFromLastData", "");

		static const PriceData dummy;

		if (cache_m.size() <= offset)
			return dummy;

		int idx = offset;
		map<CppUtils::Int64, RowType >::const_iterator it = cache_m.end();
			
		while(idx-- >= 0) {
			it--;	
		}

		RowType const& pdataMap =	it->second;
		RowType::const_iterator it2 = pdataMap.find( symbol );

		if ( it2 == pdataMap.end() )
			return dummy;


		return it2->second;

	}

	MultiSymbolCache::RowType const& MultiSymbolCache::getRow() const
	{
		if (cache_m.size() == 0)
			THROW(CppUtils::OperationFailed, "exc_EmptyList", "ctx_getRow", "");

		if (it_m == cache_m.end())
			THROW(CppUtils::OperationFailed, "exc_IteratorNotValid", "ctx_getRow", "");

		return  it_m->second;

	}

	MultiSymbolCache::RowType& MultiSymbolCache::getRow()
	{
		if (cache_m.size() == 0)
			THROW(CppUtils::OperationFailed, "exc_EmptyList", "ctx_getRow", "");

		if (it_m == cache_m.end())
			THROW(CppUtils::OperationFailed, "exc_IteratorNotValid", "ctx_getRow", "");

		return  it_m->second;
	}

	CppUtils::Int64 MultiSymbolCache::getKey() const
	{
		if (cache_m.size() == 0)
			THROW(CppUtils::OperationFailed, "exc_EmptyList", "ctx_getKey", "");

		if (it_m == cache_m.end())
			THROW(CppUtils::OperationFailed, "exc_IteratorNotValid", "ctx_getKey", "");

		return it_m->first;
	}

	double MultiSymbolCache::getTimeKey() const
	{
		return (double)(getKey() / 1000.0);
	}

	// it looks in the past and returns the data from the past for the defined column
	bool MultiSymbolCache::getSomePrevValid(CppUtils::String const& symbol, int const maxdepth, PriceData& result) const
	{
		if (it_m == cache_m.end())
			return false;

		map<CppUtils::Int64, RowType >::iterator i2 = it_m;

		// step backward
		i2--;

		if (i2 == cache_m.end())
			return false;
	
		int idx = 0;
		while(true) {

			if (maxdepth >= 0 && (++idx > maxdepth))
				break;

			RowType const& row = i2->second;

			RowType::const_iterator it_row = row.find(symbol);
			if (it_row != row.end()) {
				result = it_row->second;
				return true;
			}

			if (--i2 == cache_m.end())
				break;

			
		}

		return false;
	}

	CppUtils::String MultiSymbolCache::dump()
	{
		CppUtils::String r;
		r.append("Created on: ").append(CppUtils::getGmtTime(CppUtils::getTime())).append("\n");
		if(first()) {
			int idx = 0;
			do {
				CppUtils::Int64 millisec_time = getKey();
				CppUtils::String t_c = CppUtils::getGmtTime((double)(millisec_time / 1000.0));

				r.append("Row#: ").append(CppUtils::long2String(++idx)).append(";").append(t_c).append(";");
				for(set<SymbolDescriptor>::const_iterator it = symbols_m.begin(); it != symbols_m.end() ; it++) {
					CppUtils::String const& symbol_name = it->symbol_m;
					
					r.append(symbol_name).append(" [ ").append(" type: <").append(SymbolDescriptor::fieldType2String( it->ftype_m)).append("> name: <").append(symbol_name).append("> ");
					PriceData const& pdata_i = get(symbol_name);
					
					if (pdata_i.isValid())
						r.append(pdata_i.toString());
					else
						r.append("Not Valid");

					r.append(" ]; ");
				}
				
				r.append("\n");

			} while (next());
		}

		return r;
	}

	/**
	* Helpers
	*/

	

	void MultiSymbolCache::propagate_onUpdateCaclulatedField(CppUtils::Int64 const& key, double const& key_ttime, RowType& row) const
	{
		if (!callbackPtr_m)
			return;

		// split whole row into 2 parts
		RowType physical_row;
		RowType calculated_row;

		for(set<SymbolDescriptor>::const_iterator it = symbols_m.begin(); it != symbols_m.end(); it++) {
			if (it->ftype_m == ST_Calculated)	{
				RowType::const_iterator it2 = row.find( it->symbol_m );
				if (it2 != row.end())
					calculated_row[ it->symbol_m ] = it2->second;
			}
			else if (it->ftype_m == ST_Physical)	{
				RowType::const_iterator it2 = row.find( it->symbol_m );
				if (it2 != row.end())
					physical_row[ it->symbol_m ] = it2->second;
				
			}

			 
		}

		for(set<SymbolDescriptor>::const_iterator it = symbols_m.begin(); it != symbols_m.end(); it++) {
			if (it->ftype_m == ST_Calculated)	{

				bool calculated = false;
				PriceData calculated_data;


			
				callbackPtr_m->onUpdateCaclulatedField(*this, physical_row, calculated_row, it->symbol_m, key_ttime, calculated_data, calculated);
			
				if (calculated) {
					// insert calculated data
					calculated_data.time_m = key_ttime;
					row[it->symbol_m] = calculated_data; 
				}
			}
		}
	}


	void MultiSymbolCache::updateFieldsNumber()
	{
		physicalFieldsNumber_m = 0;
		calculatedFieldsNumber_m = 0;
		for(set<SymbolDescriptor>::const_iterator it = symbols_m.begin(); it != symbols_m.end(); it++) {
			if (it->ftype_m == ST_Calculated)	
				calculatedFieldsNumber_m++;

			if (it->ftype_m == ST_Physical)	
				physicalFieldsNumber_m++;

		}
	}

}; // end of ns
