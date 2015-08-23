#ifndef _EVENT_BASED_MATH_IMPL_INCLUDED
#define _EVENT_BASED_MATH_IMPL_INCLUDED

 // Many thanks to Zoubok Victor

#include "event_based_mathdefs.hpp"
#define EVENT_ABSED_MATH "EVENT_BASED_MATH"

#define N_MIN 30
#define N_MAX 100

namespace AlgLib {

	CppUtils::String dumpPrices(CppUtils::DoubleList const & priceList  )
	{
		CppUtils::String res;
		for(int i = 0; i < priceList.size(); i++) {
			if (i < (priceList.size()-1))
				res += CppUtils::float2String( priceList[i]) + ",\n";
			else
				res += CppUtils::float2String( priceList[i]);
		}
		return res;
	}
	
	class SymbolContext
		{
		public:
			SymbolContext():
				daysDepth_m(0),
				lastDay_m(-1)
			{
			}

			int daysDepth_m;
			double lastDay_m;

	};

	class ProbabilityCalculatorLess
	{
	public:
		static double calculate(bool const equal, double const thrshold, CppUtils::DoubleList const& series);
	};
	
	class ProbabilityCalculatorMore
	{
	public:
		// calculate probability less than thershold
		static double calculate(bool const equal, double const thrshold, CppUtils::DoubleList const& series);
	};


	//
	class EVENT_BASED_MATH_EXP CEventBasedMath :public AlgorithmBase {
		//
	public:
		
		//
		// ctor & dtor
		CEventBasedMath( Inqueue::AlgorithmHolder& algHolder);

		virtual ~CEventBasedMath();
		// intreface to be implemented

		virtual void onLibraryInitialized(
			HlpStruct::AlgBrokerDescriptor const& descriptor, 
			BrkLib::BrokerBase* brokerlib, 
			BrkLib::BrokerBase* brokerlib2,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLibraryFreed();

		virtual void onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);
		
	virtual void onRtProviderSynchEvent (
		Inqueue::ContextAccessor& contextaccessor, HlpStruct::RtData::SynchType const synchFlg,
		double const& synchTime,
		CppUtils::String const& providerName,
		CppUtils::String const& message
	);

		virtual void onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLevel1DataArrived(
			HlpStruct::RtLevel1Data const& level1data,
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onBrokerResponseArrived(
			BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
		);

		virtual void onEventArrived(
			Inqueue::ContextAccessor& contextaccessor,
			HlpStruct::CallingContext& callContext,
			bool& result,
			CppUtils::String& customOutputData
		);

		virtual void onThreadStarted(
			Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
		);

	virtual void onThreadFinished(
		Inqueue::ContextAccessor& contextaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
	);



		virtual CppUtils::String const& getName() const;

		// -----------------------
	public:

	private:

	

		void addDetailsLogEntry(CppUtils::String const& etry);

		void prepareData(
			Inqueue::ContextAccessor& contextaccessor,
			int const depth,
			CppUtils::String const& symbol, 
			CppUtils::String const& provider,
			CppUtils::DoubleList & closeDiff,
			CppUtils::DoubleList & closeOpenDiff,
			CppUtils::DoubleList & highCloseDiff,
			CppUtils::DoubleList & lowCloseDiff,
			HlpStruct::PriceData & last_data
		);

		void calculateProbabilityBasedOnLevels(
			CppUtils::String const& symbol, double const &levelhigh, double const& levellow,
			double &problevhigh, double &problevlow	);

		void prepareSymbolsListFromCache();

		void createHistohram(CppUtils::IntList& hist, CppUtils::DoubleList const& series, CppUtils::DoubleList const& edges);

		void createEdges(CppUtils::DoubleList& edges, double const& min_val, double const& max_val, int const count);

		void estimateNumberOfBeans(CppUtils::DoubleList const& series, CppUtils::IntList& histTrg, CppUtils::DoubleList& histEdges);

		
		// this get as series and calculate level based on probability
		template< class T >
		void calcLevelFromProbablility(CppUtils::DoubleList const& series, double const& pct, double& level);

	
		// another method
		void calcLevel(CppUtils::DoubleList const& series, double const& pct_min, double const& pct_max, double& level_min, double& level_max);
		
		CppUtils::String historyDataProfile_m;


		// yesterday
		double yesterday_m;

		map<CppUtils::String, SymbolContext> symbols_m;

		int maxDaysToLoad_m;

		CppUtils::String provider_m;

		CppUtils::Event loadFinished_m;

		CppUtils::Mutex dataLoadedMtx_m;

		CppUtils::StringSet readSymbols_m;
			
	};

}; // end of namespace 


#endif // _EVENT_BASED_MATH_IMPL_INCLUDED
