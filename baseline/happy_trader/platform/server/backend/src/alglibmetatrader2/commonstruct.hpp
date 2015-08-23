#ifndef _ALGLIBMT2_COMMONSTRUCT_INCLUDED
#define _ALGLIBMT2_COMMONSTRUCT_INCLUDED

#include "alglibmetatrader2defs.hpp"
#include "shared.hpp"

#define HEARTBEAT_INTERVAL_DEAFULT 5.0 // how often heartbeats are sent to Java HT layer
#define READ_MT_OUTPUT_INTERVAL 2 // default how often we read MT output
#define ZOMBIE_MT4_KILLER_INTERVAL 3

namespace AlgLib {
	

	#define COMMONSTRUCT "COMMONSTRUCT"

	template<class BASE_ELEM, int n>
	class ScopedSoap
	{
		typedef BASE_ELEM* (*INST_FUN)(struct soap *, int);
		public:
			ScopedSoap(INST_FUN instFun):
				soap_m(NULL), 
				baseEntry_m(NULL)
			{
				soap_m = soap_new(); 
				baseEntry_m  =instFun(soap_m, n);
				soap_set_imode(soap_m, SOAP_XML_IGNORENS);
				
			}

			~ScopedSoap()
			{
				soap_destroy(soap_m); // deletes p too
				soap_end(soap_m);
				soap_done(soap_m); 
			}

			void loadXmlFromString(CppUtils::String const& stringContent)
			{
				if (stringContent.size() <=0)
					THROW(CppUtils::OperationFailed, "exc_InvalidXMLString", "ctx_loadXmlFromString", "");
				stringstream istr;
				istr.str(stringContent);

				baseEntry_m->soap->is = &istr;
				
				soap_begin_recv(baseEntry_m->soap);
				if (SOAP_OK != soap_m->error)
					THROW(CppUtils::OperationFailed, "exc_OperationResult", "ctx_loadXmlFromString", soap_m->error);

				baseEntry_m->soap_in(baseEntry_m->soap, NULL, NULL);
				if (SOAP_OK != soap_m->error)
					THROW(CppUtils::OperationFailed, "exc_OperationResult", "ctx_loadXmlFromString", soap_m->error);

				soap_end_recv(baseEntry_m->soap);
				if (SOAP_OK != soap_m->error)
					THROW(CppUtils::OperationFailed, "exc_OperationResult", "ctx_loadXmlFromString", soap_m->error);

			}

			void loadXmlFromFile(CppUtils::String const& fileName)
			{
				ifstream fstreamIN(fileName);
				baseEntry_m->soap->is = &fstreamIN;
				
				soap_begin_recv(baseEntry_m->soap);
				if (SOAP_OK != soap_m->error)
					THROW(CppUtils::OperationFailed, "exc_OperationResult", "ctx_loadXmlFromFile",  soap_m->error);

				baseEntry_m->soap_in(baseEntry_m->soap, NULL, NULL);
				if (SOAP_OK != soap_m->error)
					THROW(CppUtils::OperationFailed, "exc_OperationResult", "ctx_loadXmlFromFile",  soap_m->error);

				soap_end_recv(baseEntry_m->soap);
				if (SOAP_OK != soap_m->error)
					THROW(CppUtils::OperationFailed, "exc_OperationResult", "ctx_loadXmlFromFile",  soap_m->error);
				
				
			}

			soap *getSoap()
			{
				return soap_m;
			}

		

			BASE_ELEM* getBaseEntry()
			{
				return baseEntry_m;
			}

		private:
			soap *soap_m;
			BASE_ELEM* baseEntry_m;
	};

	struct MtStartUp
	{
		MtStartUp():
			autorecover(false),
			loadBalanceEnabled(true),
			//useChildMtToReconnect_m(false),
			autoInstall_m(false)
		{
		}

		CppUtils::String mtName;
		CppUtils::String mtPath;
		//CppUtils::String mtPipeName;

		// read all queue data the last system worked on
		bool autorecover; 

		// automatically select data to add
		bool loadBalanceEnabled;

		// 
		CppUtils::String autoLoadPath_m; // this is to autoload
		//bool autostart; // if autostart the instance
		CppUtils::String companyId_m; // company ID (any unique string identifying company)
		AlgLib::AutoLoadFileStruct fstruct_m;
		//bool useChildMtToReconnect_m; // of we enforce using child MT instance to reconnect instead of script reconnect
		bool autoInstall_m; // if autoinstall

		// tmp result export path
		CppUtils::String tmpResultPath_m;



	};

	struct MtRunTime
	{
		
	};

	class MTSettings
	{
	public:
		
		MTSettings()
		{
		}
	
		~MTSettings()
		{
		}

		inline bool hasAutoLoadLib() const
		{
			return startUpSettings_m.autoLoadPath_m.size() > 0;
		}


		inline MtStartUp const& getStartUpSettings() const
		{
			return startUpSettings_m;
		}

		inline void setStartUpSettings(MtStartUp const& entry)
		{
			startUpSettings_m = entry;
		}

		inline MtRunTime& getRunTimeSettings()
		{
			return runTimeSettings_m;
		}

		inline MtRunTime const& getRunTimeSettings() const
		{
			return runTimeSettings_m;
		}

		


		/*
		CppUtils::String getDescriptor() const
		{
			CppUtils::String r;
			r.append("ID:").append(startUpSettings_m.mtName)
				.append(", COMPANY ID: ").append(startUpSettings_m.companyId_m)
				.append(", UID: ").append(runTimeSettings_m.mTterminalUid.toString())
				.append(", STATE: ").append(CppUtils::long2String(runTimeSettings_m.state))
				.append(", LAST HEARBEAT: ").append(CppUtils::getGmtTime(runTimeSettings_m.lastHeartBeat))
				.append(", PID: ").append(CppUtils::long2String(runTimeSettings_m.pid));
			
			return r;
		}
		*/

	private:
		MtStartUp startUpSettings_m;
		MtRunTime runTimeSettings_m;

	};


	typedef map<CppUtils::String, MTSettings> MtSettingsMap;

	//typedef map<CppUtils::Uid, MTSettings> MtSettingsUidMap;

	typedef vector<MTSettings> MtSetttingsList;


	struct GlobalSettingsEntry
	{
	
		GlobalSettingsEntry():
			heartBeatJavaHTSec_m(HEARTBEAT_INTERVAL_DEAFULT),
			//heartBeatMtproxySec_m(-1),
			//connectionAssumedLostSec_m(-1),
			allowdebugEvents_m(false),
			mt4datareadintrvalsec_m(READ_MT_OUTPUT_INTERVAL),
			zombiemt4killersec_m(ZOMBIE_MT4_KILLER_INTERVAL)
		{
		};

		void clear()
		{
			//serverPipe_m = "";
			mtSettingsMap_m.clear();
			eventPluginParams_m.clear();
			heartBeatJavaHTSec_m = HEARTBEAT_INTERVAL_DEAFULT;
			//heartBeatMtproxySec_m = -1;
			//connectionAssumedLostSec_m = -1;
			allowdebugEvents_m = false;
			mt4datareadintrvalsec_m = READ_MT_OUTPUT_INTERVAL;
			zombiemt4killersec_m = ZOMBIE_MT4_KILLER_INTERVAL;
		}

		//CppUtils::String serverPipe_m;
		MtSettingsMap mtSettingsMap_m;
		// to be passed to event as paraeters
		CppUtils::StringMap eventPluginParams_m;		
		int heartBeatJavaHTSec_m; // how often heartbeats are sent to Java HT layer
		//int heartBeatMtproxySec_m; // this is heart beat period  regularly obtained from MT instances (see lastHeartBeat_m)
		//int connectionAssumedLostSec_m; // how many seconds to elapse when connections is assumed lost
		bool allowdebugEvents_m; // see DEBUG_LOG macro
		int mt4datareadintrvalsec_m; // how often we read what MT instnaces output
		int zombiemt4killersec_m;
		CppUtils::String baseTmpMtPath_m;
		CppUtils::String baseTmpMtPathFileMatch_m;
	};


	


	// ---------------------
	
	//  --------------------
	struct User
	{
		CppUtils::String user_m;

		CppUtils::String password_m;
	};

	
	

};


#endif