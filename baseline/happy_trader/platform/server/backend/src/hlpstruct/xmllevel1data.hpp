#ifndef _HLPSTRUCT_XMLLEVEL1DATA_INCLUDED
#define _HLPSTRUCT_XMLLEVEL1DATA_INCLUDED


#include "hlpstructdefs.hpp"
#include "pricedata.hpp"
#include "xmlparameter.hpp"



namespace HlpStruct {

/*
This is level1 data - e.i. best bid and ask prices with volumes
please not that these are only offers, for real prices (real deals please refer to xmlrtdata.hpp
*/

struct HLPSTRUCT_EXP RtLevel1Data {
	
		friend HLPSTRUCT_EXP void convertLevel1ToXmlParameter(RtLevel1Data const& rtlevel1data, XmlParameter &xmlparameter);
		
		friend HLPSTRUCT_EXP void convertLevel1FromXmlParameter(RtLevel1Data &rtlevel1data, XmlParameter const &xmlparameter_const);

		friend HLPSTRUCT_EXP void convertToBinary(RtLevel1Data const& rtlevel1data, CppUtils::MemoryChunk& buffer);
		
		friend HLPSTRUCT_EXP void convertFromBinary(RtLevel1Data &rtlevel1data, CppUtils::MemoryChunk const& buffer);


public:
		double best_bid_price_m;

		double best_ask_price_m;

		double best_bid_volume_m;

		double best_ask_volume_m;

		double open_interest_m;
    
		double time_m;

private:
		
		char symbol_m[MAX_SYMB_LENGTH + 1];
		
		char provider_m[MAX_SYMB_LENGTH + 1];

public:

		RtLevel1Data() {
			clear();
		};

		RtLevel1Data(
			double const& best_bid_price, 
			double const& best_ask_price, 
			double const& best_bid_volume, 
			double const& best_ask_volume, 
			double const& open_interest,
			double const& time,
			char const* symbol,
			char const* provider
		):
			time_m(time),
			best_bid_price_m(best_bid_price),
			best_ask_price_m(best_ask_price),
			best_bid_volume_m(best_bid_volume),
			best_ask_volume_m(best_ask_volume),
			open_interest_m(open_interest)
		{
			setSymbol(symbol);
			setProvider(provider);
		}

		
		RtLevel1Data( RtLevel1Data const& src):
			time_m(src.time_m),
			best_bid_price_m(src.best_bid_price_m),
			best_ask_price_m(src.best_ask_price_m),
			best_bid_volume_m(src.best_bid_volume_m),
			best_ask_volume_m(src.best_ask_volume_m),
			open_interest_m(src.open_interest_m)
		{

			memcpy(symbol_m, src.symbol_m, MAX_SYMB_LENGTH + 1);
			memcpy(provider_m, src.provider_m, MAX_SYMB_LENGTH + 1);
		};

		RtLevel1Data& operator = (RtLevel1Data const& src) {
			time_m = src.time_m;
			best_bid_price_m = src.best_bid_price_m;
			best_ask_price_m = src.best_ask_price_m;
			best_bid_volume_m = src.best_bid_volume_m;
			best_ask_volume_m = src.best_ask_volume_m;
			open_interest_m = src.open_interest_m;

			memcpy(symbol_m, src.symbol_m, MAX_SYMB_LENGTH + 1);
			memcpy(provider_m, src.provider_m, MAX_SYMB_LENGTH + 1);
			return *this;
		}

		// accessors
		inline char const* getSymbol() const {
			return symbol_m;
		}

		inline CppUtils::String getSymbol2() const {
			return CppUtils::String(symbol_m);
		}

		inline char const* getProvider() const {
			return provider_m;
		}

		inline CppUtils::String getProvider2() const {
			return  CppUtils::String(provider_m);
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

		void setProvider(CppUtils::String const& provider) {
			setProvider(provider.c_str());
		
		}


		void clear() {
			memset(this, 0, sizeof(RtLevel1Data));
		}

		bool isValid() const {
			return (time_m > 0);
		}

		// -----------------
		CppUtils::String toString() const {
			CppUtils::String result = "< ";
			result += CppUtils::String(provider_m)+ " > - < ";
			result += CppUtils::String(symbol_m) + " > - ";
			result += "best bid/volume@ask/volume: ";
			result += CppUtils::float2String((float)best_bid_price_m,-1,4);
			result += "/";
			result += CppUtils::float2String((float)best_bid_volume_m,-1,4);
			result += "@";
			result += CppUtils::float2String((float)best_ask_price_m,-1,4);
			result += "/";
			result += CppUtils::float2String((float)best_ask_volume_m,-1,4);
			result += " - ";
			result += CppUtils::getGmtTime(time_m);
			result += " [ ";
			result += CppUtils::float2String((float)open_interest_m,-1,4);
			result += " ] ";
			

			return result;
		}

};

HLPSTRUCT_EXP void convertLevel1ToXmlParameter(RtLevel1Data const& rtlevel1data, XmlParameter &xmlparameter);
		
HLPSTRUCT_EXP void convertLevel1FromXmlParameter(RtLevel1Data &rtlevel1data, XmlParameter const &xmlparameter_const);

HLPSTRUCT_EXP void convertToBinary(RtLevel1Data const& rtlevel1data, CppUtils::MemoryChunk& buffer);
		
HLPSTRUCT_EXP void convertFromBinary(RtLevel1Data &rtlevel1data, CppUtils::MemoryChunk const& buffer);


}; // end of namespace

#endif