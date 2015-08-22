#ifndef _HLPSTRUCT_MULTCACHE_INCLUDED
#define _HLPSTRUCT_MULTCACHE_INCLUDED

#include "hlpstructdefs.hpp"
#include "pricedata.hpp"

namespace HlpStruct {

class HLPSTRUCT_EXP MultiSymbolCache
{
	public:

		typedef map<CppUtils::String, PriceData> RowType;

		class CallBack
		{
		public:
		  virtual void onUpdateCaclulatedField(
				MultiSymbolCache const& base, 
				RowType const& physical_row, 
				RowType const& calculated_row, 
				CppUtils::String const& calcFieldName, 
				double const& key_ttime,
				PriceData& calcPriceData, 
				bool& calculated
			) = 0; 
		};

		enum FieldType
		{
				ST_Dummy			=	0,
				ST_Physical		= 1,
				ST_Calculated =	2
		};

		struct SymbolDescriptor
		{
			
			SymbolDescriptor():
				ftype_m(ST_Dummy)
			{
			}

			SymbolDescriptor(CppUtils::String const& symbol, FieldType const& ftype): 
				symbol_m(symbol), ftype_m(ftype)
			{
			}

			bool operator < (SymbolDescriptor const& rhs) const 
			{  
				return this->symbol_m < rhs.symbol_m;  
			} 
			
			static CppUtils::String fieldType2String(FieldType const& ftype)
			{
				switch (ftype) {
					case ST_Dummy: return "ST_Dummy";
					case ST_Physical: return "ST_Physical";
					case ST_Calculated: return "ST_Calculated";
					default: return "N/A";
				};
			}


			CppUtils::String symbol_m;

			FieldType ftype_m;

		};
				
		MultiSymbolCache(CallBack* callback = NULL, int const maxSize = -1, bool const removeLatest = true);

		~MultiSymbolCache();

		void setUpParameters(int const maxSize, bool const removeLatest); 

		void registerSymbol(CppUtils::String const& symbol);

		void unregisterSymbol(CppUtils::String const& symbol);

		void registerCalculatedSymbol(CppUtils::String const& symbol);

		void unregisterCalculatedSymbol(CppUtils::String const& symbol);

		void clear();

		// clear data only
		void clearData();

		void addPriceData(CppUtils::String const& symbol, PriceData const& pdata);

		// special function to add porice data as it was imported from past time
		// adopt time, price and voilume
		void addPriceDataAsImportedFromPast(double const& new_ttime, CppUtils::String const& symbol, float const& price_value);

		bool first();

		bool next();

		bool last();

		bool prev();

		bool setOffsetFromLast(int const offset);

		bool isPtrValid() const;

		int size() const;

		PriceData const& get(CppUtils::String const& symbol) const;

		PriceData const& getOffsetFromLastData(CppUtils::String const& symbol, int const offset) const;

		RowType const& getRow() const;

		RowType& getRow();

		CppUtils::Int64 getKey() const;

		double getTimeKey() const;

		// it looks in the past and returns the data from the past for the defined column
		bool getSomePrevValid(CppUtils::String const& symbol, int const maxdepth, PriceData& result) const;

		CppUtils::String dump();

		inline set<SymbolDescriptor> const& getRegisteredSymbols() const
		{
			return symbols_m;
		};

		inline int getPhysicalFieldsNumber() const
		{
			return physicalFieldsNumber_m;
		}

		inline int getCalculatedFieldsNumber() const
		{
			return calculatedFieldsNumber_m;
		}
				

	private:

		// propagate calculated fields
		void propagate_onUpdateCaclulatedField(CppUtils::Int64 const& key, double const& key_ttime, RowType& row) const;

		void updateFieldsNumber();

		// 2 iterators to iterate 2 directions
		map<CppUtils::Int64, RowType >::iterator it_m; 


		map<CppUtils::Int64, RowType > cache_m;

		set<SymbolDescriptor> symbols_m;
			 
		

	
		int maxSize_m;

		// if we have max size installed 
		// should we remove latest elements or earliest elements
		bool removeLatest_m;

		CallBack* callbackPtr_m;

		int physicalFieldsNumber_m;

		int calculatedFieldsNumber_m;

	
};



}; // end of ns

#endif