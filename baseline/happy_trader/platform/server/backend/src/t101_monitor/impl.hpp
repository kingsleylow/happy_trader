#ifndef _T101_IMPL_INCLUDED
#define _T101_IMPL_INCLUDED


#include "../brklib/brklib.hpp"
#include "t101_monitordefs.hpp"
#include <algorithm>

namespace Inqueue {
	class AlgorithmHolder;
	struct AlgBrokerDescriptor;
};

namespace AlgLib {

	// boot manager
	class T101_MONITOR_EXP BootManager {
	public:
		static int boot ( );
		static const int foo_scm;
	};

	struct T101_MONITOR_EXP Symbol
	{
		// ctor
		Symbol(
			CppUtils::String const& symbol, 
			double const& income, 
			double const& cutTranslRate,
			double const& priceDiffIncome
		);

		Symbol();

		void initialize(
			CppUtils::String const& symbol, 
			double const& income, 
			double const& cutTranslRate,
			double const& priceDiffIncome
		);

		bool operator == (Symbol const& rhs) const;

		bool operator != (Symbol const& rhs) const;

			
		// static

		static bool isValid();

		static void getAnchorIndexes(int& anchor1, int& anchor2);
		
		static void getZeroLevelIndexes(int& zero1, int& zero2);

		static void getShortSymbolList(CppUtils::StringList& symbols);

		static void getLongSymbolList(CppUtils::StringList& symbols);

		static bool isLong(CppUtils::String const& symbol);


		// -----------------

		int id_m;

		double oldIncome_m;

		double income_m;

		CppUtils::String symbol_m;

		bool isLong_m;

		// current translation rate
		double curTranslRate_m;

		// income in price difference
		double priceDiffIncome_m;

		static map<CppUtils::String, pair<int, bool> > symbolIdentifiers_m;

		static CppUtils::Mutex symbolIdentifiersMtx_m;

				
	};

	// this is the bastek structure
	struct T101_MONITOR_EXP BasketStructure
	{
		enum Event
		{
			E_Nothing =					0x0000,
			E_Changed =					0x0010,
			E_WrongSize	=		0x0020
		};

		BasketStructure();

		void updateNewSymbol(
			CppUtils::String const& symbol, 
			double const& newincome, 
			double const& updateTime, 
			double const& translRate,
			double const& priceDiffIncome
		);

		void compare(BasketStructure const& src, int& event, CppUtils::IntList& changes) const;

		bool isNormalSize() const;

		// converts this structure to HTML string
		static void toHTML2Baskets(
			BasketStructure const& firstOne, 
			BasketStructure const& secondOne, 
			CppUtils::IntList const& changes,
			int& priority,
			CppUtils::String& buffer,
			CppUtils::String& small_buffer,
			double const& milisecs_time
		);

		static CppUtils::String toHTML2Baskets(
			BasketStructure const& firstOne,
			double const& milisecs_time
		);
	

		// last update time
		double lasttime_m;

		CppUtils::String lastSymbol_m;

		map<CppUtils::String, Symbol> regSymbols_m;
	
		multimap<double, Symbol> sortedSymbols_m;
	
	};

	// ---------------------------

	struct BasketEntry
	{
		BasketStructure basket_m;

		CppUtils::IntList changes_m;
	};

	// ---------------------------

	class T101_MONITOR_EXP AlgorithT101Indic: public AlgorithmBase {
	public:
		AlgorithT101Indic( Inqueue::AlgorithmHolder& algHolder);

		virtual ~AlgorithT101Indic();

		// --------------

		// interfaces to be implemented
		virtual void onLibraryInitialized(Inqueue::AlgBrokerDescriptor const& descriptor, BrkLib::BrokerBase* brokerlib, CppUtils::String const& runName, CppUtils::String const& comment);

		virtual void onLibraryFreed();

		virtual void onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLevel1DataArrived(
			HlpStruct::RtLevel1Data const& level1data,
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onBrokerResponseArrived(
			BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
		);

		virtual bool onEventArrived(HlpStruct::EventData const &eventdata, Inqueue::ContextAccessor& contextaccessor, HlpStruct::EventData& response);
	
		virtual bool onThreadStarted(
			Inqueue::ContextAccessor& contextaccessor
		);


		virtual CppUtils::String const& getName() const;

	private:

		// ----------------------
		// helpers

		// returns the first price, -1 == failure
		void calculateFirstPrice(
			double const& timestamp,  
			HlpStruct::PriceData& pdata_out, 
			CppUtils::String const& provider,
			CppUtils::String const& symbol, 
			Inqueue::ContextAccessor& historyaccessor
		);

		void sendAlert(double const rt_time, CppUtils::String const& strtag, BasketEntry const& newEntry, BasketEntry const& oldEntry, CppUtils::String const& runName );

		void sendAlert(double const rt_time, BasketEntry const& newEntry, CppUtils::String const& runName);

		// ----------------------
			
		double milisecs_time_m;

		// force to execute each
		Inqueue::AggregationPeriods executePeriod_m;

		int executeMultFactor_m;


		//CppUtils::String pathForAlerts_m;

		// new entries are added if ther is a signifivant event
		vector<BasketEntry> basketList_m;

	

		// open prices
		map<CppUtils::String, HlpStruct::PriceData> openCandles_m;

		bool isReady_m;

		// whether the first was sent
		bool theFirstSent_m;

		// these are maps containing last prices to convert to USD income
		map<CppUtils::String, HlpStruct::RtData> lastPrices_m;

		set<CppUtils::String> periodFinishSet_m;

		map<CppUtils::String, double> usdIncome_m;

		map<CppUtils::String, double> income_m;

		map<CppUtils::String, double> translRates_m;


	};

};

#endif