#ifndef _PAIR_TARDE_SOLVERIMPL_INCLUDED
#define _PAIR_TARDE_SOLVERIMPL_INCLUDED

#include "pair_tradedefs.hpp"
#include "statemachinecontext.hpp"
#include "commonstruct.hpp"
#include "event.hpp"






namespace AlgLib {
	class TradingSequenceRobot; 
	class LevelCalculator: public HlpStruct::CalculatorBase<CalcContext>, public HlpStruct::MultiSymbolCache::CallBack
	{
		public:

			LevelCalculator(TradingSequenceRobot& base);
			
			virtual ~LevelCalculator();

			virtual void onRtData(HlpStruct::RtData const& rtdata, CalcContext* ctx);

			virtual void onLevel2Data(HlpStruct::RtLevel2Data const& level2data, CalcContext* ctx);

			virtual void initialized();

			virtual void deinitialized();

			virtual void onThreadStarted(CalcContext* ctx);

			virtual void onThreadFinished(CalcContext* ctx);

			virtual void onUpdateCaclulatedField(
				HlpStruct::MultiSymbolCache const& base,
				HlpStruct::MultiSymbolCache::RowType const& physical_row, 
				HlpStruct::MultiSymbolCache::RowType const& calculated_row, 
				CppUtils::String const& calcFieldName, 
				double const& key_ttime,
				HlpStruct::PriceData& calcPriceData,
				bool& calculated); 
		
			inline MultuSymbolRequestEntryList const& getEntryListShortSingleVolume() const
			{ 
				return entryListShort_SingleVolume_m; 
			};

			inline MultuSymbolRequestEntryList const& getEntryListLongSingleVolume() const
			{ 
				return entryListLong_SingleVolume_m; 
			};

			inline MultuSymbolRequestEntryList const& getEntryListShortDoubleVolume() const
			{ 
				return entryListShort_DoubleVolume_m; 
			};

			inline MultuSymbolRequestEntryList const& getEntryListLongDoubleVolume() const
			{ 
				return entryListLong_DoubleVolume_m; 
			};

			inline	CppUtils::FifoQueue<BrkLib::TradeDirection, 200>& getPendingSignals()
			{
				return pendingSignals_m;
			}

			inline	CppUtils::FifoQueue<BrkLib::TradeDirection, 200> const& getPendingSignals()	const
			{
				return pendingSignals_m;
			}

			void pushPendingSingal(BrkLib::TradeDirection const td);

			int sendPendingSignals();

			void heartBeat();

	private:

		inline TradingSequenceRobot& getBase()
		{
			return base_m;
		}

		inline TradingSequenceRobot const& getBase() const
		{
			return base_m;
		}

		// this goes through the dataset and fill in the gaps using previos data
		// if it can't rais an exception
		void fixGapsInDataset(HlpStruct::MultiSymbolCache &newdataset, int const depth);

		// this calculate synthetic symbols
		void calculateSynthetic(HlpStruct::MultiSymbolCache &newdataset, HlpStruct::MultiSymbolCache &synthetic_dataset);

		template<Math::CandlePrices const typeOfPrice>
		double calculate(CppUtils::Parser& parser,HlpStruct::MultiSymbolCache::RowType const& physical_row);

		// perform export of symbols
		void makeSymbolsExport(CalcContext* ctx) const;

		// from synthetic symbol prepare request to work with pos
		void getTradeRequestFromSynthSymbol(
			MultuSymbolRequestEntryList& reqList, 
			CppUtils::String const& synthSymbol, 
			BrkLib::TradeDirection const td,
			int const volume
		);

	
				
		TradingSequenceRobot& base_m;

		map<CppUtils::String, int> cache_ids_m;

		HlpStruct::MultiSymbolCache symbCache_m;

		// serve as a holder of ma
		HlpStruct::MultiSymbolCache maCache_m;

		bool initialized_m;

		// parsers map based on synthetic symbol list
		map<CppUtils::String, CppUtils::Parser*> parsers_m;

		// multisymbol dataset
		HlpStruct::MultiSymbolCache dataset_m;

		// map for signal context
		map<CppUtils::String, SignalContext> signalContextMap_m;

		// stored commands to open close synth position
		MultuSymbolRequestEntryList entryListShort_SingleVolume_m;

		MultuSymbolRequestEntryList entryListLong_SingleVolume_m;

		MultuSymbolRequestEntryList entryListShort_DoubleVolume_m;

		MultuSymbolRequestEntryList entryListLong_DoubleVolume_m;

		// queue to process orders
		CppUtils::FifoQueue<BrkLib::TradeDirection, 200> pendingSignals_m;

		int flag_m;

		
		
	};
};

#endif