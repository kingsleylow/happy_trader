#ifndef _HLPSTRUCT_PRICEDATA_INCLUDED
#define _HLPSTRUCT_PRICEDATA_INCLUDED

namespace HlpStruct {
	

	

	// this is a trivial structure containing OHCL
	struct HLPSTRUCT_EXP CandleData {
		float open_m;
		
		float high_m;
		
		float low_m;
		
		float close_m;
	};

// ------------------------------
// price data
#pragma pack(push, 1)

	 struct HLPSTRUCT_EXP PriceData {
		 

		PriceData(bool finished) {
			clear();
			if ( finished )
				setPeriodFinished();
		}

		PriceData() {
			clear();
			setPeriodFinished();
		}

		inline void clear() {
			open_m = -1;
			high_m = -1;
			low_m = -1;
			close_m = -1;
			open2_m = -1;
			high2_m = -1;
			low2_m = -1;
			close2_m = -1;
			volume_m = -1;
			time_m = -1;
			

			flag_m = 0;

		
		};

		// ---------
		// some helpers

		
		bool isBidPriceValid() const
		{
			return open2_m > 0 && close2_m > 0 && high2_m > 0 && low2_m > 0;
		}

		bool isAskPriceValid() const
		{
			return open_m > 0 && close_m > 0 && high_m > 0 && low_m > 0;
		}


		CppUtils::String toCSVAskString() const
		{
			// time, o,h,c,l, volume
			CppUtils::String result= (time_m>0?CppUtils::getGmtTime2(time_m):"N/A")+","+
				CppUtils::float2String(open_m,-1, 5)+","+
				CppUtils::float2String(high_m,-1,5)+","+
				CppUtils::float2String(low_m,-1,5)+","+
				CppUtils::float2String(close_m,-1,5)+","+
				CppUtils::float2String(volume_m,-1,5);

			return result;

		}

		CppUtils::String toCSVBidString() const
		{
			// time, o,h,c,l, volume
			CppUtils::String result= (time_m>0?CppUtils::getGmtTime2(time_m):"N/A")+","+
				CppUtils::float2String(open2_m,-1,5)+","+
				CppUtils::float2String(high2_m,-1,5)+","+
				CppUtils::float2String(low2_m,-1,5)+","+
				CppUtils::float2String(close2_m,-1,5)+","+
				CppUtils::float2String(volume_m,-1,5);

			return result;

		}

		CppUtils::String toString() const
		{
			CppUtils::String result;
			if (isPeriodFinished())
				result= CppUtils::String("( ") + (time_m>0?CppUtils::getGmtTime(time_m):"N/A")+ " ) - CLOSED ";
			else
				result= CppUtils::String("( ") + (time_m>0?CppUtils::getGmtTime(time_m):"N/A")+ " ) - UNCLOSED ";

			if (isNewPeriod())
				result += " (NEW PERIOD SET) ";
			else
				result += " (NEW PERIOD NOT SET) ";

			if (isImportedFromPast())	{
					result += " (PAST) ";
			}

			result += "open="+CppUtils::float2String(open_m,-1,5)+", ";
			result += "high="+CppUtils::float2String(high_m,-1,5)+", ";
			result += "low="+CppUtils::float2String(low_m,-1,5)+", ";
			result += "close="+CppUtils::float2String(close_m,-1,5)+", ";

			result += "open2="+CppUtils::float2String(open2_m,-1,5)+", ";
			result += "high2="+CppUtils::float2String(high2_m,-1,5)+", ";
			result += "low2="+CppUtils::float2String(low2_m,-1,5)+", ";
			result += "close2="+CppUtils::float2String(close2_m,-1,5)+", ";

			result += "volume="+CppUtils::float2String(volume_m,-1,5);
			

			return result;
		}

		// flag_m 2 & 3 bits
		// 2 - is first
		// 3 - is last
		inline bool isFirstInChain() const
		{
			// 0100
			return ((flag_m & 0x04) !=0 );
		}

		inline bool isLastInChain() const
		{
			// 1000
			return ((flag_m & 0x08) != 0);
		}

		inline void setFirstInChain() const
		{
			// 0100
			flag_m |= 0x04;

			// reset 1000
			flag_m &= (~0x08);
		}

		inline void setLastInChain() const
		{
			// set 1000
			flag_m |= 0x08;

			// reset 0100
			flag_m &= (~0x04);

		}

		

		inline bool isValid() const
		{
			return time_m > 0;
		}

		
		inline bool isNewPeriod() const
		{
			// the last flag
			// 0001
			//return notAggregated_m;
			return ((flag_m & 0x01) !=0 );
		}
		
		inline void setNewPeriod() 
		{
			// set 0001
			flag_m |= 0x01;
		}

		inline void resetNewPeriod()
		{
			// reset 0001
			flag_m &= (~0x01);
		}

		/**
		*/

		// ----------------------
		inline bool isImportedFromPast() const
		{
			// 0010
			return ((flag_m & 0x10) !=0 );
		}

		inline void setImportedFromPast()
		{
			// set 0010
			flag_m |= 0x10;
		}

		inline void resetImportedFromPast()
		{
			// reset 0010
			flag_m &= (~0x10);
		}

		// ----------------------

		inline bool isPeriodFinished() const
		{
			// 0010
			return ((flag_m & 0x02) !=0 );
		}

		inline void setPeriodFinished()
		{
			// set 0010
			flag_m |= 0x02;
		}

		inline void resetPeriodFinished()
		{
			// reset 0010
			flag_m &= (~0x02);
		}

		// SYSTEM DEPENDENT!!!!
		inline void getAskCandle(CandleData &candle) const 
		{
			memcpy(&candle, &open_m, sizeof(float)*4);
		}

		inline void getBidCandle(CandleData &candle) const
		{
			memcpy(&candle, &open2_m, sizeof(float)*4);
		}
		
		/**
		*/
		
		// ask prices
		float open_m;
		
		float high_m;
		
		float low_m;
		
		float close_m;


		// bid prices
		float open2_m;
		
		float high2_m;
		
		float low2_m;
		
		float close2_m;
		
		// sure - volume :)
		float volume_m;

		// do not hold milliseconds - just only seconds
		double time_m;
		

		

		// whether this is pure candle - happens after period is switched
	private:

		
	
		mutable CppUtils::Byte flag_m;


	};

	typedef map<int, PriceData*> PriceDataMapPtr;

	typedef vector<PriceData> PriceDataList;

	typedef vector<PriceData*> PriceDataPtrList;

	// 
	


#pragma pack (pop)

	struct HLPSTRUCT_EXP GlobalPriceData
	{
		GlobalPriceData():
		cookie_m(-1)
		{
		};

		GlobalPriceData(int const cookie, CppUtils::String const& provider, CppUtils::String const& symbol, HlpStruct::PriceData const& pdata):
			cookie_m(cookie),
			pdata_m(pdata),
			symbol_m(symbol),
			provider_m(provider)
		{
		}

		// without cookie
		GlobalPriceData(CppUtils::String const& provider, CppUtils::String const& symbol, HlpStruct::PriceData const& pdata):
			cookie_m(-1),
			pdata_m(pdata),
			symbol_m(symbol),
			provider_m(provider)
		{
		}

		CppUtils::String toString()	const
		{
			CppUtils::StringConcat sr;
			sr << STRINGCONB << 
				STRINGCONTPAIR(cookie_m) << 
				STRINGCONTPAIR(symbol_m) << 
				STRINGCONTPAIR(provider_m) << 
				STRINGCONTPAIR_MEMBERFUN(toString, pdata_m) <<
				STRINGCONE;

			return sr.getString();
		}

		int cookie_m;	

		PriceData pdata_m;

		CppUtils::String symbol_m;
		
		CppUtils::String provider_m;

	};

	typedef vector<GlobalPriceData> GlobalPriceDataList;

}; // end of ns

#endif
