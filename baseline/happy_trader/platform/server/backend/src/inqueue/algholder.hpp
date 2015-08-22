#ifndef _BACKEND_INQUEUE_ALGHOLDER_INCLUDED
#define _BACKEND_INQUEUE_ALGHOLDER_INCLUDED

#include "inqueuedefs.hpp"
#include "commontypes.hpp"
#include "engine.hpp"
#include "contextaccessor.hpp"
#include "../brklib/brklib.hpp"


#define ENGINE_CREATOR_ALGORITHM_FUNCTION_NAME "createAlgorithm" 
#define ENGINE_DESTROYER_ALGORITHM_FUNCTION_NAME "destroyAlgorithm" 

#define ENGINE_CREATOR_BROKER_FUNCTION_NAME "createBroker" 
#define ENGINE_DESTROYER_BROKER_FUNCTION_NAME "destroyBroker" 

namespace AlgLib {
	class AlgorithmBase;
};



namespace Inqueue {
/**
Helper class that provides access to one or more algorithm/broker library pairs
each AlgorithmHoledr object incapsulates access from one thread
*/
	class InQueueController;

	class INQUEUE_BCK_EXP AlgorithmHolder: private CppUtils::Mutex {
		
	public:

		// loads the respective libraries
		AlgorithmHolder(
			HlpStruct::AlgBrokerDescriptor const& descriptor, 
			CppUtils::String const& runName,
			CppUtils::String const& runComment
		);

		// unload library and free resourcses
		~AlgorithmHolder();

		
		// thread access functions 
		// called when enginethread delegates to here
		void onDataArrived(
			HlpStruct::RtData const& rtdata, 
			ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& tradeParams,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		void processRtProviderSynchEvent (
			ContextAccessor& contextaccessor,
			HlpStruct::RtData::SynchType const synchFlg,
			double const& synchTime,
			CppUtils::String const& providerName,
			CppUtils::String const& message
		);

		void event_OnThreadStarted(ContextAccessor& historyaccessor, bool const firstLib, bool const lastLib);

		void event_OnThreadFinished(ContextAccessor& historyaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib);

		// delegates event arrival
		void processAlgorithmRelatedEvent(
			HlpStruct::EventData const &eventdata, 
			Inqueue::ContextAccessor& contextaccessor, 
			InQueueController& inqueueController
		);

		// delegates broker event arrived
		void processBrokerRelatedEvent(
			int const broker_seq_num, 
			HlpStruct::EventData const &eventdata, 
			InQueueController& inqueueController
		);

		// process level 2 data
		// this is for level 2 data
		void processLevel2Data(
			HlpStruct::RtLevel2Data const& level2data,
			ContextAccessor& historyaccessor,
			HlpStruct::TradingParameters const& tradeParams,
			CppUtils::String const& runName,
			CppUtils::String const& runComment
		);

		// process level 1 data
		// this is for level 1 data
		void processLevel1Data(
			HlpStruct::RtLevel1Data const& level1data,
			ContextAccessor& historyaccessor,
			HlpStruct::TradingParameters const& tradeParams,
			CppUtils::String const& runName,
			CppUtils::String const& runComment
		);

		
		// thread access function
		// this is called after simulation ticks are generated to notify simulation broker library
		inline void onSimulationBrokerDataArrived(HlpStruct::RtData const& rtdata)
		{
			// must be ok
			//if (brk_m != NULL ) {
			brk_m->onDataArrived(rtdata);
			//}  
		};

		inline void onSimulationBrokerDataArrived2(HlpStruct::RtData const& rtdata)
		{
			// must be ok
			//if (brk_m != NULL ) {
			brk2_m->onDataArrived(rtdata);
			//}  
		};

		inline void initializeBacktest(
			BrkLib::SimulationProfileData const& simulProfileData, 
			BrkLib::BrokerBase::ProviderSymbolCookieMap const& providerSymbolCookieMap ) 
		{
			HTASSERT (brk_m);
			brk_m->initializeBacktest(simulProfileData, providerSymbolCookieMap);
			
		}

		inline void initializeBacktest(
			BrkLib::SimulationProfileData const& simulProfileData ) 
		{
			HTASSERT(brk_m); 
			brk_m->initializeBacktest(simulProfileData );
			
		}

		inline void initializeBacktest2(
			BrkLib::SimulationProfileData const& simulProfileData, 
			BrkLib::BrokerBase::ProviderSymbolCookieMap const& providerSymbolCookieMap ) 
		{
			HTASSERT (brk2_m);
			brk2_m->initializeBacktest(simulProfileData, providerSymbolCookieMap);
			
		}

		inline void initializeBacktest2(
			BrkLib::SimulationProfileData const& simulProfileData ) 
		{
			HTASSERT(brk2_m); 
			brk2_m->initializeBacktest(simulProfileData );
			
		}

		// update parameters
		//void updateDescriptor(AlgBrokerDescriptor const& descriptor);

		// status
		void setStatus(AlgorithmStatus const status);

		AlgorithmStatus getStatus() const;
		
		// provides access to descriptor object
		//AlgBrokerDescriptor getDescriptor() const;

		int& getRefCount();

	
		// accessors to algorithm and broker
		inline BrkLib::BrokerBase* getBroker()
		{
			return brk_m;
		}

		inline BrkLib::BrokerBase* getBroker2()
		{
			return brk2_m;
		}

		inline AlgLib::AlgorithmBase* getAlgorithm()
		{
			return alg_m;
		}

		// reference to constant broker descriptor
		inline HlpStruct::AlgBrokerDescriptor const& getAlgBrokerDescriptor() const
		{
			return descriptor_m;
		}

		inline bool isBtestLib() const
		{
			if (brk_m) {
				return brk_m->isBacktestLibrary();
			};

			return false;
		}

		inline bool isBtestLib2() const
		{
			if (brk2_m) {
				return brk2_m->isBacktestLibrary();
			};

			return false;
		}
    
		// function to perform explicit initialization - just to separate from others
		void initAlgorithmLibrary(CppUtils::String const& runName, CppUtils::String const& comment);

		void initBrokerLibrary();

		void initBrokerLibrary2();

		// do not throw exc
		void deinitAlgorithmLibrary();

		void deinitBrokerLibrary();

		void deinitBrokerLibrary2();
		
	private:
		
		// helper functions

		
		// loads algorithm librarys
		// called from ctor
		void loadAlgorithmLibrary(CppUtils::String const& runName, CppUtils::String const& comment);

		void loadBrokerLibrary();

		void loadBrokerLibrary2();

		// must do not throw 
		void unloadAlgorithmLibrary();

		void unloadBrokerLibrary();

		void unloadBrokerLibrary2();
		
		

		// algorithm library pointer
		AlgLib::AlgorithmBase* alg_m;

		void* algLib_m;

		// first broker library
		BrkLib::BrokerBase* brk_m;

		// second broker library
		BrkLib::BrokerBase* brk2_m;

		void* brkLib_m;

		void* brkLib2_m;

		
		// descriptor class that cannot change
		HlpStruct::AlgBrokerDescriptor descriptor_m;

		// status of the algorithm broker pair
		AlgorithmStatus status_m;

		// referenbce count
		int refCount_m;

	

		Inqueue::OutProcessor& outproc_m;
		

	};

};

#endif