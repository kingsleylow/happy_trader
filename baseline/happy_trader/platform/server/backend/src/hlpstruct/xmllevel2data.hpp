#ifndef _HLPSTRUCT_XMLLEVEL2DATA_INCLUDED
#define _HLPSTRUCT_XMLLEVEL2DATA_INCLUDED


#include "hlpstructdefs.hpp"
#include "pricedata.hpp"
#include "xmlparameter.hpp"


#define MAX_SYMB_LENGTH  32

namespace HlpStruct {

// helper structure for level 2
struct HLPSTRUCT_EXP Level2Entry
{
public:

	enum Type
	{
		IS_BID = -1,
		IS_ASK = 1,
		IS_DUMMY = 0
	};

	Level2Entry( float const& volume, float const& price, bool const self, Type const entryType ):
		volume_m(volume),
		self_m( self ),
		price_m (price),
		type_m(entryType)
	{
	}

	Level2Entry():
		volume_m(-1),
		self_m(false),
		price_m(-1),
		type_m(IS_DUMMY)
	{
	}

	inline float& getVolume()
	{
		return volume_m;
	}

	inline float const& getVolume() const
	{
		return volume_m;
	}

	inline float& getPrice()
	{
		return price_m;
	}

	inline float const& getPrice() const
	{
		return price_m;
	}

	inline bool& isSelf()
	{
		return self_m;
	}

	inline bool const& isSelf() const
	{
		return self_m;
	}

	inline int& getType()
	{
		return type_m;
	}

	inline int const& getType() const
	{
		return type_m;
	}

private:

	float volume_m;

	float price_m;

	bool self_m;

	int type_m;
};





struct HLPSTRUCT_EXP RtLevel2Data {
	
		

		friend HLPSTRUCT_EXP void convertToBinary(RtLevel2Data const& rtlevel2data, CppUtils::MemoryChunk& buffer);
			
		friend HLPSTRUCT_EXP void convertFromBinary(RtLevel2Data &rtlevel2data, CppUtils::MemoryChunk const& buffer);

private:
		Level2Entry* data_m;

		int dataBinarySize_m;

		int dataSize_m;

		double time_m;
		
		char symbol_m[MAX_SYMB_LENGTH + 1];
		
		char provider_m[MAX_SYMB_LENGTH + 1];

		// indexes of best bid & ask prices
		int idxBestBid_m;

		int idxBestAsk_m;

public:

	

	RtLevel2Data():
	data_m(NULL),
	dataSize_m(0),
	dataBinarySize_m(0),
	time_m(-1),
	idxBestBid_m(-1),
	idxBestAsk_m(-1)
	{
		symbol_m[0] = '\0';
		provider_m[0] = '\0';
	};


	virtual ~RtLevel2Data()
	{
		if (data_m != NULL) {
			delete [] data_m;
		}

	}

	/*	
	static void copyData(Level2Entry* src, Level2Entry* trg, int const size)
	{
		for(int i = 0; i < size; i++) {
			trg[i] = src[i];
		}
	}
	*/

	inline static void copyData2(Level2Entry* src, Level2Entry* trg, int const size)
	{
			memcpy(	trg, src, size); 
	}
		
	RtLevel2Data( RtLevel2Data const& src):
	time_m(src.time_m)
	{
		
		if (src.dataSize_m > 0) {
				data_m = new Level2Entry[src.dataSize_m];
				dataSize_m = src.dataSize_m;
				dataBinarySize_m = src.dataBinarySize_m;
				copyData2(src.data_m, data_m,src.dataBinarySize_m );
				
		}
		else {
				data_m = NULL;
				dataSize_m = 0;
				dataBinarySize_m = 0;
		}

		idxBestBid_m = src.idxBestBid_m;
		idxBestAsk_m = src.idxBestAsk_m;
		memcpy(symbol_m, src.symbol_m, MAX_SYMB_LENGTH + 1);
		memcpy(provider_m, src.provider_m, MAX_SYMB_LENGTH + 1);
	};

	RtLevel2Data& operator = (RtLevel2Data const& src) {
			time_m = src.time_m;
			
	
			if (data_m != NULL) {
				delete [] data_m;
			}

			if (src.dataSize_m > 0) {
				data_m = new Level2Entry[src.dataSize_m];
				dataSize_m = src.dataSize_m;
				dataBinarySize_m = src.dataBinarySize_m;
				
				copyData2(src.data_m, data_m,src.dataBinarySize_m );
				
			}
			else {
				data_m = NULL;
				dataSize_m = 0;
				dataBinarySize_m = 0;
			
			}

			idxBestBid_m = src.idxBestBid_m;
			idxBestAsk_m = src.idxBestAsk_m;
			memcpy(symbol_m, src.symbol_m, MAX_SYMB_LENGTH + 1);
			memcpy(provider_m, src.provider_m, MAX_SYMB_LENGTH + 1);
			return *this;
		}

		// accessors
		inline char const* getSymbol() const {
			return symbol_m;
		}

		inline char const* getProvider() const {
			return provider_m;
		}

		inline CppUtils::String getProvider2() const 
		{
			return  CppUtils::String(provider_m);
		}

		inline CppUtils::String getSymbol2() const 
		{
			return  CppUtils::String(symbol_m);
		}

		
		inline double& getTime()
		{
			return time_m;
		}

		inline double const& getTime() const
		{
			return (double const&)time_m;
		}

		

		//
		void setSymbol(char const* symbol) {
			symbol_m[0] = '\0';
			if (strlen(symbol)<=MAX_SYMB_LENGTH) {
				strcpy(symbol_m, symbol);
			}
			else {
				strncpy(symbol_m, symbol, MAX_SYMB_LENGTH);
			}
				
		}

		void setProvider(char const* provider) {
			provider_m[0] = '\0';
			if (strlen(provider_m)<=MAX_SYMB_LENGTH) {
				strcpy(provider_m, provider);
			}
			else {
				strncpy(provider_m, provider, MAX_SYMB_LENGTH);
			}
		}


		void clear() {
			if (data_m != NULL) {
				delete [] data_m;
				data_m = NULL;
				dataSize_m = 0;
				dataBinarySize_m = 0;
			}

			idxBestAsk_m = -1;
			idxBestBid_m = -1;
			time_m  =-1;
			symbol_m[0] = '\0';
			provider_m[0] = '\0';
		}

		
		inline bool isValid() const {
			return (time_m > 0);
		}

		inline int const getSize() const
		{
			return dataSize_m;
		}

		void init(int const dataSize )
		{
			if (data_m != NULL) {
				delete [] data_m;
			}

			idxBestAsk_m = -1;
			idxBestBid_m = -1;

			if (dataSize > 0)	{
				data_m = new Level2Entry[dataSize];
				dataSize_m = dataSize;
				dataBinarySize_m = sizeof(Level2Entry) * dataSize_m;
			}
			else {
				dataSize_m = 0;
				dataBinarySize_m = 0;
				data_m = NULL;
			}

		}

		void init(int const dataSize, int const idxBestBid, int const idxBestAsk )
		{
			if (data_m != NULL) {
				delete [] data_m;
			}

			idxBestAsk_m = idxBestAsk;
			idxBestBid_m = idxBestBid;

			if (dataSize > 0)	{
				data_m = new Level2Entry[dataSize];
				dataSize_m = dataSize;
				dataBinarySize_m = sizeof(Level2Entry) * dataSize_m;
			}
			else {
				dataSize_m = 0;
				dataBinarySize_m = 0;
				data_m = NULL;
			}

		}

		inline int getBestBidIndex() const
		{
			return idxBestBid_m;
		}

		inline int getBestAskIndex() const
		{
			return idxBestAsk_m;
		}

		inline void setBestBidIndex(int const idxBestBid) {
			idxBestBid_m = idxBestBid;
		}

		inline void setBestAskIndex(int const idxBestAsk) {
			idxBestAsk_m = idxBestAsk;
		}

		void setBestBidaskIndexes(int const idxBestBid, int const idxBestAsk) {
			idxBestBid_m = idxBestBid;
			idxBestAsk_m = idxBestAsk;
		}

		inline Level2Entry& getLevel2Entry(int const idx)
		{
			HTASSERT(dataSize_m > idx);
			return data_m[idx];
		}

		inline Level2Entry const& getLevel2Entry(int const idx) const
		{
			HTASSERT(dataSize_m > idx);
			return data_m[idx];
		}


		// -----------------
		CppUtils::String toString() const {
			CppUtils::String result = "[ ";
			CppUtils::String typeString;
			result += CppUtils::String(getProvider()) + " - " + getSymbol() + "] ["+ CppUtils::getGmtTime(getTime())+ "] ->\n";
			if (dataSize_m > 0) {
				for(int i = 0; i < dataSize_m; i++) {
					Level2Entry const& e = getLevel2Entry(i);
					if (e.getType() == Level2Entry::IS_BID) {
						if (getBestBidIndex() >=0 && getBestBidIndex() == i)
							typeString="BID (BEST)";
						else
							typeString="BID";
					}
					else if (e.getType() == Level2Entry::IS_ASK) {
						if (getBestAskIndex() >=0 && getBestAskIndex() == i)
							typeString = "ASK (BEST)";
						else
							typeString = "ASK";
					}
					else {
						typeString = "DUMMY";
					}

					

					result += " price=" + CppUtils::float2String(e.getPrice(), -1, 4);
					result += " volume=" + CppUtils::float2String(e.getVolume(), -1, 4);
					result += " type=" +typeString;
					result += " is self=" + CppUtils::String((e.isSelf() ? "true":"false"));
					result += "\n";
				}
			}

			return result;
		}

};



HLPSTRUCT_EXP void convertToBinary(RtLevel2Data const& rtlevel2data, CppUtils::MemoryChunk& buffer);
		
HLPSTRUCT_EXP void convertFromBinary(RtLevel2Data &rtlevel2data, CppUtils::MemoryChunk const& buffer);




}; // end of namespace

#endif