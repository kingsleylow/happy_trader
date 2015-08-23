#include "algholder.hpp"
#include "../alglib/alglib.hpp"
#include "outproc.hpp"
#include "comproc.hpp"






#define ALGHOLDER "ALGBRK HOLDER"



namespace Inqueue {

	AlgorithmHolder::AlgorithmHolder(
		HlpStruct::AlgBrokerDescriptor const& descriptor, 
		CppUtils::String const& runName, 
		CppUtils::String const& runComment
	):
	descriptor_m(descriptor),
	status_m(AS_Free),
	refCount_m(0),
	alg_m(NULL),
	algLib_m(NULL),
	brk_m(NULL),
	brk2_m(NULL),
	brkLib_m(NULL),
	brkLib2_m(NULL),
	//btestLib_m(false),
	outproc_m(*CppUtils::getRootObject<Inqueue::OutProcessor>())
	{
		loadBrokerLibrary();
		loadBrokerLibrary2();
		loadAlgorithmLibrary(runName, runComment);
	}

	AlgorithmHolder::~AlgorithmHolder()
	{
		unloadAlgorithmLibrary();
		unloadBrokerLibrary2();
		unloadBrokerLibrary();
		
	}

	void AlgorithmHolder::onDataArrived(
		HlpStruct::RtData const& rtdata, 
		ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& tradeParams,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
	{
		//LOG(MSG_DEBUG, ALGHOLDER, "Ticker arrived on algorithm \"" << getDescriptor().name_m << "\" : " << rtdata.toString() );
		//if (alg_m != NULL) {
			
			alg_m->onDataArrived(rtdata, historyaccessor, tradeParams, brk_m, runName, comment);
		//}
	}

	void AlgorithmHolder::processRtProviderSynchEvent (
			ContextAccessor& contextaccessor,
			HlpStruct::RtData::SynchType const synchFlg,
			double const& synchTime,
			CppUtils::String const& providerName,
			CppUtils::String const& message
		)
	{
		alg_m->onRtProviderSynchEvent (
			contextaccessor,
			synchFlg,
			synchTime,
			providerName,
			message
		);
	}

	void AlgorithmHolder::event_OnThreadStarted(ContextAccessor& historyaccessor, bool const firstLib, bool const lastLib)
	{
		
		alg_m->onThreadStarted(historyaccessor, firstLib, lastLib);
		
		if (brk_m != NULL)
			brk_m->onThreadStarted(firstLib, lastLib);	

		if (brk2_m != NULL)
			brk2_m->onThreadStarted(firstLib, lastLib);
	}

	void AlgorithmHolder::event_OnThreadFinished(ContextAccessor& historyaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib)
	{
		
		alg_m->onThreadFinished(historyaccessor, tradeParams, firstLib, lastLib);
		
		if (brk_m != NULL)
			brk_m->onThreadFinished(tradeParams, firstLib, lastLib);

		if (brk2_m != NULL)
			brk2_m->onThreadFinished(tradeParams, firstLib, lastLib);
	}

	void AlgorithmHolder::processAlgorithmRelatedEvent(
		HlpStruct::EventData const &eventdata, 
		Inqueue::ContextAccessor& contextaccessor, 
		InQueueController& inqueueController
	)
	{
		HlpStruct::EventData response_alg_layer;
		bool res  = alg_m->processAlgorithmRelatedEvent(eventdata, contextaccessor, response_alg_layer);
	
		LOGEVENTA( HlpStruct::CommonLog::LL_DEBUGDETAIL, ALGHOLDER, 
					"Logging ET_AlgorithmEvent",
					"Request done to algorithm layer" <<	eventdata << 
					"Response arrived from algorithm layer" << response_alg_layer <<
					"Result" << (int)res
		);
				
		if (res ) {
			// send this back
			PUSHEVENT(response_alg_layer);
			inqueueController.onAlgBrkEventArrive(eventdata, response_alg_layer );
		}

	}

	void AlgorithmHolder::processBrokerRelatedEvent(
		int const broker_seq_num, 
		HlpStruct::EventData const &eventdata, 
		InQueueController& inqueueController
	)
	{
		bool res;
		// broker_seq_num ==1 or broker_seq_num == 2
		//  - it is dispatched
		// otherwise sent to both layers
		
		

		if (brk_m != NULL && (broker_seq_num==1 || broker_seq_num <= 0)) {
			HlpStruct::EventData response_brk_1;
			res = brk_m->processBrokerRelatedEvent(eventdata, response_brk_1);

			LOGEVENTA( HlpStruct::CommonLog::LL_DEBUGDETAIL, ALGHOLDER, 
					"Logging ET_BrokerEvent",
					"Request done to broker layer: " <<	eventdata << 
					"Broker sequence num: " << broker_seq_num << 
					"Response arrived from broker layer" << response_brk_1 <<
					"Result" << (int)res
			);

			if (res) {
				PUSHEVENT(response_brk_1);
				inqueueController.onAlgBrkEventArrive(eventdata, response_brk_1 );
			}
		}
		
		if (brk2_m != NULL && (broker_seq_num == 2 || broker_seq_num <= 0)) {
			HlpStruct::EventData response_brk_2;
			res = brk2_m->processBrokerRelatedEvent(eventdata, response_brk_2);

			LOGEVENTA( HlpStruct::CommonLog::LL_DEBUGDETAIL, ALGHOLDER, 
					"Logging ET_BrokerEvent",
					"Request done to broker layer 2: " <<	eventdata << 
					"Broker sequence num: " << broker_seq_num << 
					"Response arrived from broker layer" << response_brk_2 <<
					"Result" << (int)res
			);

			if (res) {
				PUSHEVENT(response_brk_2);
				inqueueController.onAlgBrkEventArrive(eventdata, response_brk_2 );
			}
		}
		
	}
	

	void AlgorithmHolder::processLevel2Data(
		HlpStruct::RtLevel2Data const& level2data,
		ContextAccessor& historyaccessor,
		HlpStruct::TradingParameters const& tradeParams,
		CppUtils::String const& runName,
		CppUtils::String const& runComment
	)
	{
		//if (alg_m != NULL) {
			// 
			alg_m->onLevel2DataArrived(level2data, historyaccessor, tradeParams, brk_m, runName, runComment );

		//}
		
	}

	void AlgorithmHolder::processLevel1Data(
		HlpStruct::RtLevel1Data const& level1data,
		ContextAccessor& historyaccessor,
		HlpStruct::TradingParameters const& tradeParams,
		CppUtils::String const& runName,
		CppUtils::String const& runComment
	)
	{
		//if (alg_m != NULL) {
			// 
			alg_m->onLevel1DataArrived(level1data, historyaccessor, tradeParams, brk_m, runName, runComment );

		//}
		
	}
	
	

	void AlgorithmHolder::setStatus(AlgorithmStatus const status)
	{
		LOCK_FOR_SCOPE(*this);
		status_m = status;
	}

	AlgorithmStatus AlgorithmHolder::getStatus() const
	{
		LOCK_FOR_SCOPE(*this);
		return status_m;
	}
		
	int& AlgorithmHolder::getRefCount()
	{
		return refCount_m;
	}

	void AlgorithmHolder::initAlgorithmLibrary(CppUtils::String const& runName, CppUtils::String const& comment)
	{
		// notify
		try {
			alg_m->onLibraryInitialized(descriptor_m, brk_m, brk2_m, runName, comment);
		}
		catch(CppUtils::Exception &e) {
			LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER, "Exception on initializing algorithm library", alg_m->getName() << GET_LANG_STRING(e.message()) << " " + e.context() << " " << e.arg() << " "  << e.addInfo()  );
			THROW(CppUtils::OperationFailed, "exc_Exception", "ctx_loadAlgLib", EXC2STR(e));
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			// as this should be plugin do need this to crash
			LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER, "MSVC Exception on initializing algorithm library", alg_m->getName() << GET_LANG_STRING(e.message()) );
			THROW(CppUtils::OperationFailed, "exc_MSVCException", "ctx_loadAlgLib", e.message());
		}
#endif
		catch(...) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER, "Unknown exception", alg_m->getName() );
				THROW(CppUtils::OperationFailed, "exc_UnknownException", "ctx_loadAlgLib", "");
		}

		LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER, "Algorithm successfully initialized", alg_m->getName() << runName );
		LOG(MSG_DEBUG, ALGHOLDER, "Successfully initialized algorithm library " << descriptor_m.algLib_m << " of the name " << alg_m->getName());

	}

	void AlgorithmHolder::initBrokerLibrary2()
	{
		if (brkLib2_m != NULL) {

			// notify
			try {
				brk2_m->onLibraryInitialized(descriptor_m);
				//btestLib_m = brk_m->isBacktestLibrary();
			}
			catch(CppUtils::Exception &e) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER, "Exception on initializing broker library 2 ",  brk2_m->getName() << GET_LANG_STRING(e.message()) << " " + e.context() << " " << e.arg() << " " << e.addInfo());
				THROW(CppUtils::OperationFailed, "exc_Exception", "ctx_loadBrkLib2",EXC2STR(e));
			}
		#ifdef HANDLE_NATIVE_EXCEPTIONS
			catch(Testframe::Exception& e) {
				// as this should be plugin do need this to crash
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ALGHOLDER,  "MSVC Exception on initializing broker library2 ",  brk2_m->getName() << GET_LANG_STRING(e.message()) );
				THROW(CppUtils::OperationFailed, "exc_MSVCException", "ctx_loadBrkLib2", e.message());
			}
		#endif
			catch(...) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER,  "Unknown exception on initializing broker library 2",  brk2_m->getName() );
				THROW(CppUtils::OperationFailed, "exc_UnknownException", "ctx_loadBrkLib", "");
			}

			LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER,  "Broker library 2 successfully initialized",  brk2_m->getName() );
			LOG(MSG_DEBUG, ALGHOLDER, "Successfully initialized broker library 2 " + descriptor_m.brokerLib2_m + " of the name " + brk2_m->getName());

		}
	}

	void AlgorithmHolder::initBrokerLibrary()
	{
		if (brkLib_m != NULL) {

			// notify
			try {
				brk_m->onLibraryInitialized(descriptor_m);
				//btestLib_m = brk_m->isBacktestLibrary();
			}
			catch(CppUtils::Exception &e) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER, "Exception on initializing broker library",  brk_m->getName() << GET_LANG_STRING(e.message()) << " " + e.context() << " " << e.arg() << " " << e.addInfo());
				THROW(CppUtils::OperationFailed, "exc_Exception", "ctx_loadBrkLib",EXC2STR(e));
			}
		#ifdef HANDLE_NATIVE_EXCEPTIONS
			catch(Testframe::Exception& e) {
				// as this should be plugin do need this to crash
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ALGHOLDER,  "MSVC Exception on initializing broker library",  brk_m->getName() << GET_LANG_STRING(e.message()) );
				THROW(CppUtils::OperationFailed, "exc_MSVCException", "ctx_loadBrkLib", e.message());
			}
		#endif
			catch(...) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER,  "Unknown exception on initializing broker library",  brk_m->getName() );
				THROW(CppUtils::OperationFailed, "exc_UnknownException", "ctx_loadBrkLib", "");
			}

			LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER,  "Broker successfully initialized",  brk_m->getName() );
			LOG(MSG_DEBUG, ALGHOLDER, "Successfully initialized broker library " + descriptor_m.brokerLib_m + " of the name " + brk_m->getName());

		}
	}

	void AlgorithmHolder::deinitAlgorithmLibrary()
	{
		// notify  
		try {
			alg_m->onLibraryFreed();
			LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER,  "Algorithm successfully released", alg_m->getName() );
		}
		catch(CppUtils::Exception &e) {
			LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER,  "Exception on releasing algorithm library", alg_m->getName() << GET_LANG_STRING(e.message()) << " " << e.context() << " " << e.arg() );
			LOG(MSG_WARN, ALGHOLDER, "Exception on releasing algorithm library " << alg_m->getName() << " "  << GET_LANG_STRING(e.message()) << " " << e.context() << " " << e.arg());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			// as this should be plugin do need this to crash
			LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER,  "MSVC Exception on releasing algorithm library" ,alg_m->getName() << GET_LANG_STRING(e.message()) );
			LOG(MSG_WARN, ALGHOLDER, "MSVC Exception on releasing algorithm library " << alg_m->getName() << " "  << GET_LANG_STRING(e.message()));
		}
#endif
		catch(...) { 
			LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER,  "Unknown exception on releaseing algorithm library", alg_m->getName() );
			LOG(MSG_WARN, ALGHOLDER, "Unknown exception on releaseing algorithm library " << alg_m->getName() );

		}
		
		LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER, "Algorithm successfully deinitialized", alg_m->getName()  );
		LOG(MSG_DEBUG, ALGHOLDER, "Successfully deinitialized algorithm library " << descriptor_m.algLib_m << " of the name " << alg_m->getName());


	}

	void AlgorithmHolder::deinitBrokerLibrary2()
	{
		if (brkLib2_m != NULL) {
			// notify
			try {
				brk2_m->onLibraryFreed();
				LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER,  "Broker 2 successfully released", brk2_m->getName() );
				
			}
			catch(CppUtils::Exception &e) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER,  "Exception on releasing broker library 2 " , brk2_m->getName() << GET_LANG_STRING(e.message()) << " " << e.context() << " " << e.arg() );
				LOG(MSG_WARN, ALGHOLDER, "Exception on releasing broker library 2 " << brk2_m->getName() << " " <<  GET_LANG_STRING(e.message()) << " " << e.context() << " " << e.arg());
			}
	#ifdef HANDLE_NATIVE_EXCEPTIONS
			catch(Testframe::Exception& e) {
				// as this should be plugin do need this to crash
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER,  " MSVC Exception on releasing broker library 2 ",  brk2_m->getName() <<  GET_LANG_STRING(e.message()) );
				LOG(MSG_WARN, ALGHOLDER, "MSVC Exception on releasing broker library 2 " << brk2_m->getName() << " "  << GET_LANG_STRING(e.message()));
				//THROW(CppUtils::OperationFailed, "exc_MSVCException", "ctx_unloadBrkLib2", e.message());
			}
	#endif
			catch(...) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER,  "Unknown exception on releasing broker library",  brk2_m->getName() );
				LOG(MSG_WARN, ALGHOLDER, "Unknown exception on releaseing broker library 2 " << brk2_m->getName() );
			}

			LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER,  "Broker 2 successfully deinitialized ",  brk2_m->getName() );
			LOG(MSG_DEBUG, ALGHOLDER, "Successfully deinitialized broker library 2 " + descriptor_m.brokerLib_m + " of the name " + brk2_m->getName());
		}

	}

	void AlgorithmHolder::deinitBrokerLibrary()
	{
		if (brkLib_m != NULL) {
			// notify
			try {
				brk_m->onLibraryFreed();
				LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER,  "Broker successfully released", brk_m->getName() );
				
			}
			catch(CppUtils::Exception &e) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER,  "Exception on releasing broker library" , brk_m->getName() << GET_LANG_STRING(e.message()) << " " << e.context() << " " << e.arg() );
				LOG(MSG_WARN, ALGHOLDER, "Exception on releasing broker library " << brk_m->getName() << " " <<  GET_LANG_STRING(e.message()) << " " << e.context() << " " << e.arg());
			}
	#ifdef HANDLE_NATIVE_EXCEPTIONS
			catch(Testframe::Exception& e) {
				// as this should be plugin do need this to crash
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER,  " MSVC Exception on releasing broker library",  brk_m->getName() <<  GET_LANG_STRING(e.message()) );
				LOG(MSG_WARN, ALGHOLDER, "MSVC Exception on releasing broker library " << brk_m->getName() << " "  << GET_LANG_STRING(e.message()));
				//THROW(CppUtils::OperationFailed, "exc_MSVCException", "ctx_unloadBrkLib", e.message());
			}
	#endif
			catch(...) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ALGHOLDER,  "Unknown exception on releasing broker library",  brk_m->getName() );
				LOG(MSG_WARN, ALGHOLDER, "Unknown exception on releaseing broker library " << brk_m->getName() );
			}

			LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER,  "Broker successfully deinitialized",  brk_m->getName() );
			LOG(MSG_DEBUG, ALGHOLDER, "Successfully deinitialized broker library " + descriptor_m.brokerLib_m + " of the name " + brk_m->getName());
		}


	}

	void AlgorithmHolder::loadAlgorithmLibrary(CppUtils::String const& runName, CppUtils::String const& comment)
	{
		if (algLib_m == NULL) {
			LOG(MSG_DEBUG, ALGHOLDER, "Attempting to load library " << descriptor_m.algLib_m );
		
			algLib_m = CppUtils::dynlibOpen( descriptor_m.algLib_m );
			if (algLib_m==NULL)
				THROW(CppUtils::OperationFailed, "exc_cannotLoadLibrary", "ctx_loadAlgorithm", descriptor_m.algLib_m + " - " + CppUtils::getOSError());

			void* ptr =  CppUtils::dynlibGetSym( algLib_m, ENGINE_CREATOR_ALGORITHM_FUNCTION_NAME);
			if (ptr==NULL)
				THROW(CppUtils::OperationFailed, "exc_cannotResolveCreatorFunction", "ctx_loadAlgorithm", descriptor_m.algLib_m + " - " + CppUtils::getOSError());

			AlgLib::AlgorithmBase::CreatorAlgorithmFun cfun = *(AlgLib::AlgorithmBase::CreatorAlgorithmFun*)ptr;
			
			alg_m = cfun(this);
			if (alg_m==NULL)
				THROW(CppUtils::OperationFailed, "exc_cannotCreateAlgorithm", "ctx_loadAlgorithm", descriptor_m.algLib_m);

			

			LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER, "Algorithm successfully loaded", alg_m->getName() << runName );
			LOG(MSG_DEBUG, ALGHOLDER, "Successfully loaded algorithm library " << descriptor_m.algLib_m << " of the name " << alg_m->getName());
		
		}
	} 
	void AlgorithmHolder::loadBrokerLibrary2()
	{
		if (brkLib2_m == NULL) {
			if (descriptor_m.brokerLib2_m.empty()) {
				LOG(MSG_DEBUG, ALGHOLDER, "Broker library 2 is not set" );
				return;
			}

			//btestLib_m = false;
			LOG(MSG_DEBUG, ALGHOLDER, "Attempting to load broker library 2 " << descriptor_m.brokerLib2_m );

					
			brkLib2_m = CppUtils::dynlibOpen( descriptor_m.brokerLib2_m );
			if (brkLib2_m==NULL)
				THROW(CppUtils::OperationFailed, "exc_cannotLoadLibrary", "ctx_loadBroker2", descriptor_m.brokerLib2_m + " - " + CppUtils::getOSError());

			void* ptr =  CppUtils::dynlibGetSym( brkLib2_m, ENGINE_CREATOR_BROKER_FUNCTION_NAME);
			if (ptr==NULL)
				THROW(CppUtils::OperationFailed, "exc_cannotResolveCreatorFunction", "ctx_loadBroker2", descriptor_m.brokerLib2_m + " - " + CppUtils::getOSError());

			BrkLib::BrokerBase::CreatorBrokerFun cfun = *(BrkLib::BrokerBase::CreatorBrokerFun*)ptr;
			
			brk2_m = cfun(this);
			if (brk2_m==NULL)
				THROW(CppUtils::OperationFailed, "exc_cannotCreateBroker", "ctx_loadBroker2", descriptor_m.brokerLib2_m);

			
			LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER,  "Broker 2 successfully loaded",  brk2_m->getName() );
			LOG(MSG_DEBUG, ALGHOLDER, "Successfully loaded broker library 2 " + descriptor_m.brokerLib2_m + " of the name " + brk2_m->getName());
		
		}
	}

	void AlgorithmHolder::loadBrokerLibrary()
	{
		

		if (brkLib_m == NULL) {
			if (descriptor_m.brokerLib_m.empty()) {
				LOG(MSG_DEBUG, ALGHOLDER, "Broker library is not set" );
				return;
			}

			//btestLib_m = false;
			LOG(MSG_DEBUG, ALGHOLDER, "Attempting to load library " << descriptor_m.brokerLib_m );

					
			brkLib_m = CppUtils::dynlibOpen( descriptor_m.brokerLib_m );
			if (brkLib_m==NULL)
				THROW(CppUtils::OperationFailed, "exc_cannotLoadLibrary", "ctx_loadBroker", descriptor_m.brokerLib_m + " - " + CppUtils::getOSError());

			void* ptr =  CppUtils::dynlibGetSym( brkLib_m, ENGINE_CREATOR_BROKER_FUNCTION_NAME);
			if (ptr==NULL)
				THROW(CppUtils::OperationFailed, "exc_cannotResolveCreatorFunction", "ctx_loadBroker", descriptor_m.brokerLib_m + " - " + CppUtils::getOSError());

			BrkLib::BrokerBase::CreatorBrokerFun cfun = *(BrkLib::BrokerBase::CreatorBrokerFun*)ptr;
			
			brk_m = cfun(this);
			if (brk_m==NULL)
				THROW(CppUtils::OperationFailed, "exc_cannotCreateBroker", "ctx_loadBroker", descriptor_m.brokerLib_m);

			
			LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER,  "Broker successfully loaded",  brk_m->getName() );
			LOG(MSG_DEBUG, ALGHOLDER, "Successfully loaded broker library " + descriptor_m.brokerLib_m + " of the name " + brk_m->getName());
		
		}
	}


	void AlgorithmHolder::unloadAlgorithmLibrary()
	{
		if (algLib_m != NULL) {
			LOG(MSG_DEBUG, ALGHOLDER, "Attempting to unload library " << descriptor_m.algLib_m );

	

			//
			AlgLib::AlgorithmBase::DestroyerAlgorithmFun tfun = *(AlgLib::AlgorithmBase::DestroyerAlgorithmFun*)CppUtils::dynlibGetSym( algLib_m, ENGINE_DESTROYER_ALGORITHM_FUNCTION_NAME);
			if (tfun==NULL) {
				LOG(MSG_ERROR, ALGHOLDER, "Cannot resolve terminator function in " << descriptor_m.algLib_m );
			}
			else {
				// destroy this
				tfun(alg_m);
			}

			// unload library
			CppUtils::dynlibClose(algLib_m);
						
			////
			algLib_m = NULL;
			alg_m = NULL;
			//btestLib_m = false;
			LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER,  "Alogorithm successfully unloaded",  descriptor_m.algLib_m  );
			LOG(MSG_DEBUG, ALGHOLDER, "Successfully unloaded algorithm library " << descriptor_m.algLib_m );
			
	
		}
	}

	void AlgorithmHolder::unloadBrokerLibrary2()
	{
		if (brkLib2_m != NULL) {
			LOG(MSG_DEBUG, ALGHOLDER, "Attempting to unload broker library 2 " << descriptor_m.brokerLib2_m );

			//

			BrkLib::BrokerBase::DestroyerBrokerFun tfun = *(BrkLib::BrokerBase::DestroyerBrokerFun*)CppUtils::dynlibGetSym( brkLib2_m, ENGINE_DESTROYER_BROKER_FUNCTION_NAME);
			if (tfun==NULL) {
				LOG(MSG_ERROR, ALGHOLDER, "Cannot resolve terminator function in broker library 2 " << descriptor_m.brokerLib2_m );
			}
			else {
				// destroy this
				tfun(brk2_m);
			}

			// unload library
			CppUtils::dynlibClose(brkLib2_m);
						
			////
			brkLib2_m = NULL;
			brk2_m = NULL;
			
			LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER,  "Broker 2 successfully unloaded",  descriptor_m.brokerLib2_m  );
			LOG(MSG_DEBUG, ALGHOLDER, "Successfully unloaded broker 2 library " << descriptor_m.brokerLib2_m );
		}
	}

	void AlgorithmHolder::unloadBrokerLibrary()
	{
		if (brkLib_m != NULL) {
			LOG(MSG_DEBUG, ALGHOLDER, "Attempting to unload library " << descriptor_m.brokerLib_m );

			//

			BrkLib::BrokerBase::DestroyerBrokerFun tfun = *(BrkLib::BrokerBase::DestroyerBrokerFun*)CppUtils::dynlibGetSym( brkLib_m, ENGINE_DESTROYER_BROKER_FUNCTION_NAME);
			if (tfun==NULL) {
				LOG(MSG_ERROR, ALGHOLDER, "Cannot resolve terminator function in " << descriptor_m.brokerLib_m );
			}
			else {
				// destroy this
				tfun(brk_m);
			}

			// unload library
			CppUtils::dynlibClose(brkLib_m);
						
			////
			brkLib_m = NULL;
			brk_m = NULL;
			
			LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGHOLDER,  "Broker successfully unloaded",  descriptor_m.brokerLib_m  );
			LOG(MSG_DEBUG, ALGHOLDER, "Successfully unloaded broker library " << descriptor_m.brokerLib_m );
		}
	}

	

};