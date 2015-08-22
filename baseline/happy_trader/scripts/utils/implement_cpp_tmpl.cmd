echo #include "%GENERAL_IMPLEMENT_HEADER_FILE_NAME%"
echo: // Many thanks to Zoubok Victor for programming this
echo:
echo namespace AlgLib {
echo	AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
echo	{
echo		return new %CLASS_NAME%(*algholder);
echo	}
echo:
echo	void terminator (AlgorithmBase *pthis)
echo	{
echo		assert(pthis);
echo		delete pthis;
echo	}
echo:
echo	AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;
echo:
echo	AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;
echo:
echo	// intreface to be implemented
echo	%CLASS_NAME%::%CLASS_NAME%( Inqueue::AlgorithmHolder^& algHolder):
echo		AlgorithmBase(algHolder)
echo	{
echo:
echo	}
echo:
echo	%CLASS_NAME%::~%CLASS_NAME%()
echo	{
echo:
echo	}
echo:
echo	void %CLASS_NAME%::onLibraryInitialized(
echo		Inqueue::AlgBrokerDescriptor const^& descriptor, 
echo		BrkLib::BrokerBase* brokerlib, 
echo		CppUtils::String const^& runName,
echo		CppUtils::String const^& comment
echo	)
echo	{
echo:
echo	}
echo:
echo	void %CLASS_NAME%::onLibraryFreed()
echo	{
echo:
echo	}
echo:
echo	void %CLASS_NAME%::onBrokerResponseArrived(
echo			BrkLib::BrokerConnect ^&brkConnect,
echo			BrkLib::BrokerResponseBase const^& bresponse
echo	)
echo	{
echo:
echo	}
echo:
echo	void %CLASS_NAME%::onDataArrived(
echo		HlpStruct::RtData const^& rtdata, 
echo		Inqueue::ContextAccessor^& historyaccessor, 
echo		Inqueue::TradingParameters const^& descriptor,
echo		BrkLib::BrokerBase* brokerlib,
echo		CppUtils::String const^& runName,
echo		CppUtils::String const^& comment
echo	)
echo	{
echo:
echo	}
echo:		
echo	void %CLASS_NAME%::onRtProviderSynchEvent (
echo			Inqueue::ContextAccessor^& contextaccessor, HlpStruct::RtData::SynchType const synchFlg,
echo			double const^& synchTime,
echo			CppUtils::String const^& providerName,
echo			CppUtils::String const^& message
echo	)
echo	{
echo:
echo	}
echo:   
echo	void %CLASS_NAME%::onLevel2DataArrived(
echo			HlpStruct::RtLevel2Data const^& level2data,
echo			Inqueue::ContextAccessor^& historyaccessor, 
echo			Inqueue::TradingParameters const^& descriptor,
echo			BrkLib::BrokerBase* brokerlib,
echo			CppUtils::String const^& runName,
echo			CppUtils::String const^& comment
echo		)
echo	{
echo:
echo	}
echo:
echo	void %CLASS_NAME%::onLevel1DataArrived(
echo			HlpStruct::RtLevel1Data const^& level1data,
echo			Inqueue::ContextAccessor^& historyaccessor, 
echo			Inqueue::TradingParameters const^& descriptor,
echo			BrkLib::BrokerBase* brokerlib,
echo			CppUtils::String const^& runName,
echo			CppUtils::String const^& comment
echo		)
echo	{
echo:
echo	}
echo:
echo	void %CLASS_NAME%::onEventArrived(
echo		Inqueue::ContextAccessor^& contextaccessor, 
echo		HlpStruct::CallingContext^& callContext,
echo		bool^& result,
echo		CppUtils::String^& customOutputData
echo	)
echo	{
echo		result=false;
echo	}
echo:	
echo 	void %CLASS_NAME%::onThreadStarted(
echo 		Inqueue::ContextAccessor^& contextaccessor, bool const firstLib, bool const lastLib
echo 	)
echo	{
echo:		
echo	}
echo:
echo	void %CLASS_NAME%::onThreadFinished(
echo		Inqueue::ContextAccessor^& contextaccessor, Inqueue::TradingParameters const^& tradeParams, bool const firstLib, bool const lastLib
echo	)
echo	{
echo:		
echo	}
echo:
echo	CppUtils::String const^& %CLASS_NAME%::getName() const
echo	{
echo		static const CppUtils::String name("[ alg lib: %CLASS_NAME% ]");
echo		return name;
echo	}
echo:
echo:
echo } // end of namespace