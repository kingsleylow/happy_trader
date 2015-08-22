#ifndef _PAIR_TRADE_COMMONSTRUCT_INCLUDED
#define _PAIR_TRADE_COMMONSTRUCT_INCLUDED

#include "pair_tradedefs.hpp"

namespace AlgLib {

	struct SignalContext
	{
		SignalContext():
			curTime_m(-1),
			prevTime_m(-1),
			curSlowMa_m(-1),
			curFastMa_m(-1),
			prevSlowMa_m(-1),
			prevFastMa_m(-1)
		{
		}

		void clear()
		{
			curTime_m = -1;
			prevTime_m = -1;
			curSlowMa_m = -1;
			curFastMa_m = -1;
			prevSlowMa_m = -1;
			prevFastMa_m = -1;
		}

		BrkLib::TradeDirection update(double const &curTime, double const& slowMa, double const& fastMa)
		{
			prevTime_m = curTime_m;
			curTime_m = curTime;

			prevSlowMa_m =curSlowMa_m;
			curSlowMa_m = slowMa;

			prevFastMa_m = curFastMa_m;
			curFastMa_m = fastMa;

			if (prevTime_m > 0) {
				if (prevFastMa_m < prevSlowMa_m && curFastMa_m >= curSlowMa_m) {
					// signal up
					return BrkLib::TD_LONG;
				}

				if (prevFastMa_m > prevSlowMa_m && curFastMa_m <= curSlowMa_m) {
					// signal down
					return BrkLib::TD_SHORT;
				}
			}

			return BrkLib::TD_NONE;
		}

		double curTime_m;
		double prevTime_m;
		double curSlowMa_m;
		double curFastMa_m;
		double prevSlowMa_m;
		double prevFastMa_m;
	};
	
	
	struct CalcContext
	{
		CalcContext():
			historyaccessorPtr(NULL), tradeParameters_m(NULL)	
		{
		}

		Inqueue::ContextAccessor* historyaccessorPtr;
		HlpStruct::TradingParameters *tradeParameters_m;
	};
	

 	struct MultuSymbolRequestEntry
	{
		MultuSymbolRequestEntry():
			volume_m(0),
			volumeToFill_m(0),
			direction_m(BrkLib::TD_NONE)
		{
		}

		

		CppUtils::String toString() const
		{
			CppUtils::StringConcat sr;
			sr << STRINGCONB << 
				STRINGCONTPAIR(symbol_m) << 
				STRINGCONTPAIR(volume_m) <<
				STRINGCONTPAIR(volumeToFill_m) << 
				STRINGCONTPAIR_FUN(BrkLib::tradeDirection2String, direction_m)
			<< STRINGCONE;

			return sr.getString();
		}

		CppUtils::String symbol_m;
		double volume_m;
		BrkLib::TradeDirection direction_m;
		// this is when we track the completion
		// this is volume to fill
		double volumeToFill_m;
	};

	typedef vector<MultuSymbolRequestEntry>	 MultuSymbolRequestEntryList;

	// this reads this list form JSON string
	MultuSymbolRequestEntryList fromJsonString(CppUtils::String const& value);	

}; // end of ns

namespace HlpStruct {
// continue specialization
	template<>
	class HlpStruct::Member<AlgLib::MultuSymbolRequestEntryList>: public MemberBase<AlgLib::MultuSymbolRequestEntryList>
	{
		public:
		Member()
		{
				
		};
		
		CAST_OPS(AlgLib::MultuSymbolRequestEntryList)

		virtual CppUtils::String toString() const
		{
			// 
			CppUtils::String r;
			r.append("[\n");
			for(int i = 0; i < get().size(); i++) {
				AlgLib::MultuSymbolRequestEntry const& entry_i = get().at(i);
			
				r.append(" ( ").append( entry_i.toString() ).append(" ) ");

			}
			r.append("\n]\n");

			return r;
			
		}
	};

	// --------------------------------------


}

#endif
