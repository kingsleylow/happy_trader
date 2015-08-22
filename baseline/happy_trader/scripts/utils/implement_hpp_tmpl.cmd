@echo off
echo #ifndef %IMPLEMENT_FILE_HEADER%
echo #define %IMPLEMENT_FILE_HEADER%
echo:
echo: // Many thanks to Zoubok Victor
echo:
echo #include "%DEFS_HEADER_FILE_NAME%"
echo:
echo namespace AlgLib {
echo 	//
echo 	class %DEFS_HEADER_FILE_EXPORT_PRFX% %CLASS_NAME% :public AlgorithmBase {
echo 		//
echo 	public:
echo 		//
echo 		// ctor ^& dtor
echo 		%CLASS_NAME%( Inqueue::AlgorithmHolder^& algHolder);
echo:
echo 		virtual ~%CLASS_NAME%();
echo 		// intreface to be implemented
echo:
echo 		virtual void onLibraryInitialized(
echo 			Inqueue::AlgBrokerDescriptor const^& descriptor, 
echo 			BrkLib::BrokerBase* brokerlib, 
echo 			CppUtils::String const^& runName,
echo 			CppUtils::String const^& comment
echo 		);
echo:
echo 		virtual void onLibraryFreed();
echo:
echo 		virtual void onDataArrived(
echo 			HlpStruct::RtData const^& rtdata, 
echo 			Inqueue::ContextAccessor^& historyaccessor, 
echo 			Inqueue::TradingParameters const^& descriptor,
echo 			BrkLib::BrokerBase* brokerlib,
echo 			CppUtils::String const^& runName,
echo 			CppUtils::String const^& comment
echo 		);
echo:		
echo		virtual void onRtProviderSynchEvent (
echo			Inqueue::ContextAccessor^& contextaccessor, HlpStruct::RtData::SynchType const synchFlg,
echo			double const^& synchTime,
echo			CppUtils::String const^& providerName,
echo			CppUtils::String const^& message
echo		);
echo:
echo 		virtual void onLevel2DataArrived(
echo 			HlpStruct::RtLevel2Data const^& level2data,
echo 			Inqueue::ContextAccessor^& historyaccessor, 
echo 			Inqueue::TradingParameters const^& descriptor,
echo 			BrkLib::BrokerBase* brokerlib,
echo 			CppUtils::String const^& runName,
echo 			CppUtils::String const^& comment
echo 		);
echo:
echo 		virtual void onLevel1DataArrived(
echo 			HlpStruct::RtLevel1Data const^& level1data,
echo 			Inqueue::ContextAccessor^& historyaccessor, 
echo 			Inqueue::TradingParameters const^& descriptor,
echo 			BrkLib::BrokerBase* brokerlib,
echo 			CppUtils::String const^& runName,
echo 			CppUtils::String const^& comment
echo 		);
echo:
echo 		virtual void onBrokerResponseArrived(
echo 			BrkLib::BrokerConnect ^&brkConnect,
echo 			BrkLib::BrokerResponseBase const^& bresponse
echo 		);
echo:
echo 		virtual void onEventArrived(Inqueue::ContextAccessor^& contextaccessor,
echo			HlpStruct::CallingContext^& callContext,
echo			bool^& result,
echo			CppUtils::String^& customOutputData
echo		);
echo:
echo 		virtual void onThreadStarted(
echo 			Inqueue::ContextAccessor^& contextaccessor, bool const firstLib, bool const lastLib
echo 		);
echo:
echo		virtual void onThreadFinished(
echo			Inqueue::ContextAccessor^& contextaccessor, bool const firstLib, bool const lastLib
echo		);
echo:
echo:
echo:
echo 		virtual CppUtils::String const^& getName() const;
echo:
echo 		// -----------------------
echo 	public:
echo:
echo 	private:
echo:

echo 	};
echo:
echo }; // end of namespace 

echo:
echo:
echo #endif // %IMPLEMENT_FILE_HEADER%
