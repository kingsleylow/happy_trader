#ifndef _HLPSTRUCT_XMLRTDATA_INCLUDED
#define _HLPSTRUCT_XMLRTDATA_INCLUDED


#include "hlpstructdefs.hpp"
#include "pricedata.hpp"



#include "xmlparameter.hpp"



namespace HlpStruct {

	
// structure to hold internally real time ticker data
struct HLPSTRUCT_EXP RtData {

		// serialize to XmlParameter
		friend HLPSTRUCT_EXP void convertToXmlParameter(RtData const& rtdata, XmlParameter &xmlparameter);
		
		friend HLPSTRUCT_EXP void convertFromXmlParameter(RtData &rtdata, XmlParameter const &xmlparameter_const);

		// serialize to binary for performance
		friend HLPSTRUCT_EXP void convertToBinary(RtData const& rtdata, CppUtils::MemoryChunk& buffer);
		
		friend HLPSTRUCT_EXP void convertFromBinary(RtData &rtdata, CppUtils::MemoryChunk const& buffer);

		
		// synchronization types
		enum SynchType {
			SYNC_EVENT_PROVIDER_START =					 1,
			SYNC_EVENT_PROVIDER_FINISH =				 2,
			SYNC_EVENT_PROVIDER_TRANSMIT_ERROR = 3,
			SYNC_EVENT_CUSTOM_MESSAGE = 4
		};

		// ticker type
		enum Type {
			RT_Type = 0,
			HST_Type = 1
		};

		enum AType {
			AT_Closed = 0,
			AT_NonClosed = 1
		};

		enum ColorType {
			CT_NA				=	0,
			CT_Bid_Deal = 1,
			CT_Ask_Deal = 100,
			CT_Neutral	= 50
		};

		enum OperationType {
			OT_NA				=	0,
			OT_Buy			= 1,
			OT_Sell			= 2,
			OT_Short		= 3,
			OT_Cover		= 4
		};

		union TickerType {
			struct RtHistType {
				float open_m;
				float high_m;
				float low_m;
				float close_m;
				
				float open2_m;
				float high2_m;
				float low2_m;
				float close2_m;

				float volume_m;
			} hist_m;

			struct RtInstType {
				/*
				actual bid & ask (best bid or best ask) must be detected in RtLevel1Data
				or in Level2Quot
				here for stocks it must be the same (the price of last deal)
				for artifical forex it can be different reflecting bid and ask prices provided
				for stocks we are goint to add - color - if a deal is goint with bid price, ask price or medium
				*/
				float bid_m;
				float ask_m;
				float volume_m;

				// color - shows if last deal done with bid or ask prices
				// surely optionsl
				CppUtils::Byte color_m;

				// optional operation - as imported from QUIK transaction table. Shows whether limit bids/asks or asks were executed???
				CppUtils::Byte operation_m;
			} rt_m;

			

		};
		
	public:
		
		inline void setProviderSymbolCookie(int const cookie)
		{
			cookie_symb_prov_m = cookie;
		}

		inline int getProviderSymbolCookie() const
		{
			return cookie_symb_prov_m;
		}

	private:
		
		// this is cookie for symbol / provider pair 
		int cookie_symb_prov_m;


	private:
		TickerType tickdata_m; 
		CppUtils::Byte type_m;
		double time_m;
		char symbol_m[MAX_SYMB_LENGTH + 1];
		char provider_m[MAX_SYMB_LENGTH + 1];
		CppUtils::Byte aflag_m;

		// functions
	public:
		inline RtData() {
			clear();
		};
		
		inline RtData( RtData const& src) {
			memcpy(this, &src, sizeof(RtData));
		};

		inline RtData(CppUtils::Byte const aflag, CppUtils::Byte const type )
		{
			clear();
			type_m = type;
			aflag_m = aflag;
		}

		inline RtData(CppUtils::Byte const aflag, CppUtils::Byte const type, double const& time )
		{
			clear();
			type_m = type;
			aflag_m = aflag;
			time_m = time;
		}

		inline RtData(CppUtils::Byte const aflag, CppUtils::Byte const type, double const& time, CppUtils::String const& provider, CppUtils::String const& symbol )
		{
			clear();
			type_m = type;
			aflag_m = aflag;
			time_m = time;

			setProvider(provider);
			setSymbol(symbol);


		}


		inline RtData& operator = (RtData const& src) {
			memcpy(this, &src, sizeof(RtData));
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

		//
		inline TickerType::RtInstType& getRtData()
		{
			return tickdata_m.rt_m;
		}

		inline TickerType::RtInstType const& getRtData() const
		{
			return (TickerType::RtInstType const&)tickdata_m.rt_m;
		}
		//
		inline TickerType::RtHistType& getHistData()
		{
			return tickdata_m.hist_m;
		}

		inline TickerType::RtHistType const& getHistData() const
		{
			return (TickerType::RtHistType const&)tickdata_m.hist_m;
		}
		
		
		//
		inline CppUtils::Byte& getType() {
			return type_m;
		}

		inline CppUtils::Byte const& getType() const {
			return type_m;
		}

		//
		inline CppUtils::Byte& getAFlag() {
			return aflag_m;
		}

		inline CppUtils::Byte const& getAFlag() const {
			return aflag_m;
		}

		//

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

		void setSymbol(CppUtils::String const& symbol) {
			setSymbol(symbol.c_str());
		}

		void setProvider(CppUtils::String const& provider) {
			setProvider(provider.c_str());
		}




		inline void clear() {
			memset(this, 0, sizeof(RtData));
		}

		inline bool isValid() const {
			return (time_m > 0);
		}

		void assignFromPriceData(HlpStruct::PriceData const& pricedata, char const* provider, char const* symbol)
		{
			clear();

			setProvider(provider);
			setSymbol(symbol);
		
			getType() = HlpStruct::RtData::HST_Type;
			getTime() = pricedata.time_m;
			getHistData().volume_m = pricedata.volume_m;
			
			getHistData().open_m = pricedata.open_m;
			getHistData().high_m = pricedata.high_m;
			getHistData().low_m= pricedata.low_m;
			getHistData().close_m = pricedata.close_m;

			getHistData().open2_m = pricedata.open2_m;
			getHistData().high2_m = pricedata.high2_m;
			getHistData().low2_m= pricedata.low2_m;
			getHistData().close2_m = pricedata.close2_m;


		}

		
		CppUtils::String toString() const {
			CppUtils::String result = CppUtils::String(provider_m);
			result += " - ";
			result += CppUtils::String(symbol_m);
			if (isValid()) {
				result += " Type=";
				result += CppUtils::long2String((long)type_m);
				result += " Time=";
				result += CppUtils::getGmtTime(time_m);
				result += " Aflag=";
				result += CppUtils::long2String((long)aflag_m);

				if (type_m==RT_Type) {
					result += " Bid=" + CppUtils::float2String(getRtData().bid_m, -1, 4);
					result += " Ask=" + CppUtils::float2String(getRtData().ask_m, -1, 4);
					result += " Color=" + CppUtils::float2String(getRtData().color_m, -1, 4);
					result += " Operation=" + CppUtils::float2String(getRtData().operation_m, -1, 4);

					result += " Volume=" + CppUtils::float2String(getRtData().volume_m, -1, 4);
				}
				else if (type_m==HST_Type) {
					result += " Open=" + CppUtils::float2String(getHistData().open_m, -1, 4);
					result += " High=" + CppUtils::float2String(getHistData().high_m, -1, 4);
					result += " Low=" + CppUtils::float2String(getHistData().low_m, -1, 4);
					result += " Close=" + CppUtils::float2String(getHistData().close_m, -1, 4);

					result += " Open2=" + CppUtils::float2String(getHistData().open2_m, -1, 4);
					result += " High2=" + CppUtils::float2String(getHistData().high2_m, -1, 4);
					result += " Low2=" + CppUtils::float2String(getHistData().low2_m, -1, 4);
					result += " Close2=" + CppUtils::float2String(getHistData().close2_m, -1, 4);

					result += " Volume=" + CppUtils::float2String(getHistData().volume_m, -1, 4);
				}
				
				

			}
			else 
				result += " - NOT VALID!";

			return result;
		}
		

	
	};

	
	typedef vector<RtData> RtDataList;

	// non member functions
	HLPSTRUCT_EXP void convertToXmlParameter(RtData const&rtdata, XmlParameter &xmlparameter);

	HLPSTRUCT_EXP void convertFromXmlParameter(RtData &rtdata, XmlParameter const &xmlparameter_const);

	HLPSTRUCT_EXP void convertToBinary(RtData const& rtdata, CppUtils::MemoryChunk& buffer);
		
	HLPSTRUCT_EXP void convertFromBinary(RtData &rtdata, CppUtils::MemoryChunk const& buffer);

}
#endif