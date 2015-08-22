#include "impl.hpp"
#include "../inqueue/commontypes.hpp"
#include "../math/math.hpp"

//#include <sstream>

#define ALG_RETRACE "ALG_RETRACE"



namespace AlgLib {

	/**
	* Global trading parameters
	*/




	/**
	*/

	static int id_m = 0;

	AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new Retrace1(*algholder);
	}

	void terminator (AlgorithmBase *pthis)
	{
		HTASSERT(pthis);
		delete pthis;
	}

	AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

	AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;


	// intreface to be implemented

	Retrace1::Retrace1( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder)
	{


	}

	Retrace1::~Retrace1()
	{
	}

	void Retrace1::onLibraryInitialized(
		HlpStruct::AlgBrokerDescriptor const& descriptor, 
		BrkLib::BrokerBase* brokerlib, 
		BrkLib::BrokerBase* brokerlib2,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

		provider_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "PROVIDER");
		baseDirectory_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "BASE_DIRECTORY");

	
	
		
	}

	void Retrace1::onLibraryFreed()
	{

	}

	void Retrace1::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
		)
	{

	}


	void Retrace1::onDataArrived(
		HlpStruct::RtData const& rtdata, 
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

	}    

	void Retrace1::onRtProviderSynchEvent (
		Inqueue::ContextAccessor& contextaccessor,
		HlpStruct::RtData::SynchType const synchFlg,
		double const& synchTime,
		CppUtils::String const& providerName,
		CppUtils::String const& message
		)
	{
	}


	void Retrace1::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{
	}

	void Retrace1::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{



	}


	void Retrace1::onEventArrived(
		Inqueue::ContextAccessor& contextaccessor, 
		HlpStruct::CallingContext& callContext,
		bool& result,
		CppUtils::String &customOutputData
		)
	{
	
		HlpStruct::XmlParameter out_result;
		out_result.getParameter("status_string").getAsString(true) = "OK";
		out_result.getParameter("status").getAsString(true) = "0";
		

		try {
			customOutputData = "FAILED";
			CppUtils::String const& command = callContext.getCallCustomName();
			LOG(MSG_INFO, ALG_RETRACE, "Command: " << command);

			CppUtils::String const& data_hex = callContext.getParameter<BrkLib::String_Proxy>("custom_data").get();
			CppUtils::String data = CppUtils::unhexlify(data_hex);

			LOG(MSG_INFO, ALG_RETRACE, "Data before hex received: [" << data_hex << "]");
			LOG(MSG_INFO, ALG_RETRACE, "Data received: [" << data << "]");

			// get input parameter
			HlpStruct::XmlParameter in_param;
			HlpStruct::deserializeXmlParameter(	in_param, data );


			CppUtils::String user_id = in_param.getParameter("user_id").getAsString();
			CppUtils::String session_id = in_param.getParameter("session_id").getAsString();

			
			CppUtils::makeDir( resolveTmpDirectory(user_id));
			CppUtils::makeDir( resolveDownloadDirectory(user_id));
			CppUtils::makeDir( resolveCompletedDirectory(user_id));


			CppUtils::Uid session_uid(session_id);
			if (!session_uid.isValid())
				 THROW(CppUtils::OperationFailed, "exc_InvalidSessionUid", "ctx_onEventArrived", session_id);
		

			// resolve directory

			if (command=="start_download_torrent") {
				// start download
				CppUtils::String torrent_url = in_param.getParameter("torrent_url").getAsString();
				LOGEVENT(HlpStruct::CommonLog::LL_INFO, ALG_RETRACE, "Starting download - url: " << torrent_url << " user_id: " << user_id << " session _id: " << session_id );
				
				CppUtils::sleep(20.0);

			}
			else if (command=="query_status") {
				// query status
			}
			else if (command=="cancel_download_torrent") {
				// cancel download
			}
			else
				THROW(CppUtils::OperationFailed, "exc_InvalidCommand", "ctx_onEventArrived", command);

			
			
		}
		catch(CppUtils::Exception& e)
		{
			
			out_result.getParameter("status_string").getAsString(true) = "Exception: " + e.message() + " - " + e.arg();
		}
		catch(...)
		{
			out_result.getParameter("status_string").getAsString(true) = "Unknown exception";
		}


		CppUtils::String out_result_str;
		HlpStruct::serializeXmlParameter(out_result, out_result_str );

		customOutputData = CppUtils::hexlify(out_result_str);

		LOG(MSG_INFO, ALG_RETRACE, "Data output before: [" << out_result_str << "]");
		LOG(MSG_INFO, ALG_RETRACE, "Data output: [" << customOutputData << "]");

		// command to stop working thread
		// it's secure to do here
		// because thread cannot be stopped
		stopWorkingThread(contextaccessor);

	}

	void Retrace1::onThreadStarted(Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib) {


	};



	CppUtils::String const& Retrace1::getName() const
	{
		static const CppUtils::String name("[ Retrace algorithm ]");
		return name;
	}

	/**
	* Helpers
	*/


	void Retrace1::stopWorkingThread(Inqueue::ContextAccessor& contextaccessor)
	{
		HlpStruct::XmlParameter xml_param;
		xml_param.getName() = "custom_alg_event_stop_thread";
		xml_param.getParameter("tid_to_stop").getAsInt(true) = contextaccessor.getThreadId();

		Inqueue::OutProcessor::send_custom_event(xml_param);
	}


	CppUtils::String Retrace1::resolveBaseDirectory(CppUtils::String const& user_id)
	{
		return baseDirectory_m + "\\" + CppUtils::createValidFileName(user_id) + "\\";
	}

	CppUtils::String Retrace1::resolveDownloadDirectory(CppUtils::String const& user_id)
	{
		return resolveBaseDirectory(user_id) + "download\\completed\\";
	}

	CppUtils::String Retrace1::resolveCompletedDirectory(CppUtils::String const& user_id)
	{
		return resolveBaseDirectory(user_id) + "download\\in_process\\";
	}

	CppUtils::String Retrace1::resolveTmpDirectory(CppUtils::String const& user_id)
	{
		return resolveBaseDirectory(user_id) + "tmp\\";
	}


} // end of namespace