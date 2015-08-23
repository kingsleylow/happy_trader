#include "settingsmanager.hpp"
#include "xsd/auto/empty.nsmap"
#include "algorithmmt.hpp"

#define SETTINGS_MAN "SETTINGS MAN"

namespace AlgLib {

	SettingsManager::SettingsManager(AlgorithmMt& base):
		base_m(base)
	{
	}

	SettingsManager::~SettingsManager()
	{
	}

	void SettingsManager::parse(CppUtils::String const& file)
	{
		LOCK_FOR_SCOPE(*this);
		
		sesttings_m.clear();

		if (!CppUtils::fileExists(file))
			THROW(CppUtils::OperationFailed, "exc_SettingsFileNotFound", "ctx_parseSettings", file);
		
		settingsFile_m = file;


		ScopedSoap< _ns1__settings, -1> sc(soap_new__ns1__settings);
		sc.loadXmlFromFile(file);
		
		LOG(MSG_DEBUG, SETTINGS_MAN, "Settings file loaded: " << file );

		// read even plugin params
		_ns1__settings_generalparams_eventpluginparams const& eventPluginParams = sc.getBaseEntry()->generalparams.eventpluginparams;
		for(int i = 0; i < eventPluginParams.entry.size() ;i++) {

			CppUtils::String const& name = eventPluginParams.entry[i].name;
			CppUtils::String const& value = eventPluginParams.entry[i].value;

			auto itt = sesttings_m.eventPluginParams_m.find(name);
			if (itt != sesttings_m.eventPluginParams_m.end())
				THROW(CppUtils::OperationFailed, "exc_DublicateEntry_EventPluginParams", "ctx_parseSettings", name);

			sesttings_m.eventPluginParams_m[ name ] = value;

			LOG(MSG_DEBUG, SETTINGS_MAN, "Read event plugin parameters: [" << name << "]-[" << value << "]");

		}

		int mt4datareadintrvalsec_i = atoi(sc.getBaseEntry()->mt4datareadintrvalsec.c_str());
		if (mt4datareadintrvalsec_i > 0) {		
			sesttings_m.mt4datareadintrvalsec_m = mt4datareadintrvalsec_i;
		}
		LOG(MSG_DEBUG, SETTINGS_MAN, "Read MT output every: " << sesttings_m.mt4datareadintrvalsec_m << " sec");

		int zombiemt4killersec_i  = atoi(sc.getBaseEntry()->zombiemt4killer.c_str());
		if (zombiemt4killersec_i > 0) {
			sesttings_m.zombiemt4killersec_m = zombiemt4killersec_i;
		}
		LOG(MSG_DEBUG, SETTINGS_MAN, "Zombie MT killer runs every: " << sesttings_m.mt4datareadintrvalsec_m << " sec");

		

		sesttings_m.baseTmpMtPath_m = CppUtils::getTempPath() + CppUtils::pathDelimiter() + "__mt_ht_tmp";
		sesttings_m.baseTmpMtPathFileMatch_m = sesttings_m.baseTmpMtPath_m;
		LOG(MSG_DEBUG, SETTINGS_MAN, "Base tmp MT output path: " << sesttings_m.baseTmpMtPath_m);

		//sesttings_m.serverPipe_m = sc.getBaseEntry()->servercollectorpipe;
		//if (sesttings_m.serverPipe_m.size() <=0)
		//	THROW(CppUtils::OperationFailed, "exc_InvalidPipeName", "ctx_parseSettings", "");

		sesttings_m.allowdebugEvents_m = ComparatorMt<CppUtils::String>()(*sc.getBaseEntry()->generalparams.htserver.debugconsole);
		
		
		int mtinstances = sc.getBaseEntry()->mtinstances.instance.size();

		// read other settings

		for(int i = 0; i < mtinstances; i++) {
			MTSettings mtsetting;

			MtStartUp startUpEntry;

			_ns1__settings_mtinstances_instance const& sentry = sc.getBaseEntry()->mtinstances.instance[i];
			startUpEntry.mtName = sentry.name;
			startUpEntry.mtPath = sentry.path;
			//startUpEntry.mtPipeName = sentry.mtpipe;
			//startUpEntry.useChildMtToReconnect_m = ComparatorMt<CppUtils::String>()(*sentry.usechildmttoreconnect);
			startUpEntry.loadBalanceEnabled = ComparatorMt<CppUtils::String>()(sentry.loadbalanced);

			//LOG(MSG_INFO, SETTINGS_MAN, "[" << startUpEntry.mtName << "] - use MT child for reconect: " << CppUtils::bool2String(startUpEntry.useChildMtToReconnect_m) );
			LOG(MSG_INFO, SETTINGS_MAN, "[" << startUpEntry.mtName << "] - load balanced: " << CppUtils::bool2String(startUpEntry.loadBalanceEnabled) );

			
			startUpEntry.tmpResultPath_m = sesttings_m.baseTmpMtPath_m +  CppUtils::pathDelimiter() + startUpEntry.mtName + CppUtils::pathDelimiter();
			CppUtils::removeDir(startUpEntry.tmpResultPath_m);
			CppUtils::makeDir(startUpEntry.tmpResultPath_m);

			LOG(MSG_INFO, SETTINGS_MAN, "[" << startUpEntry.mtName << "] - base temp path: " << startUpEntry.tmpResultPath_m);


			if (sentry.autoload != NULL) {
				startUpEntry.autoLoadPath_m = *sentry.autoload;


				AlgLib::AutoLoadFileStruct::loadFromFile(startUpEntry.autoLoadPath_m, startUpEntry.fstruct_m);
				LOG(MSG_INFO, SETTINGS_MAN, "Autoload path: " << startUpEntry.autoLoadPath_m << ", read OK" );
			}

			if (sentry.autorecover!= NULL) {
				startUpEntry.autorecover = ComparatorMt<CppUtils::String>()(*sentry.autorecover);


				LOG(MSG_INFO, SETTINGS_MAN, "Autorecover strategy is: " << (startUpEntry.autorecover ?  "true":"false") << " for "<< startUpEntry.mtName );
			}
			// company ID
			startUpEntry.companyId_m =  sentry.companyid;
			if (startUpEntry.companyId_m.size() <=0)
				THROW(CppUtils::OperationFailed, "exc_InvalidCompanyID", "ctx_parseSettings", startUpEntry.mtName);

			LOG(MSG_INFO, SETTINGS_MAN, "Company ID is: \"" << startUpEntry.companyId_m << "\" for \""<< startUpEntry.mtName << "\"" );

			// autonistall
			if (sentry.autoinstall != NULL) {
				startUpEntry.autoInstall_m = ComparatorMt<CppUtils::String>()(*sentry.autoinstall);

				//autoInstLibToMt(startUpEntry);

				LOG(MSG_INFO, SETTINGS_MAN, "Will perform autoinstall of MT for: " << startUpEntry.mtName );
			}

			// autostart
			/*
			if (sentry.autostart!= NULL) {
				startUpEntry.autostart = ComparatorMt<CppUtils::String>()(*sentry.autostart);

				LOG(MSG_INFO, SETTINGS_MAN, "Autostart  is: " << CppUtils::bool2String(startUpEntry.autostart) << " for "<< startUpEntry.mtName );

				//if (!startMtInstance(startUpEntry)) {
				//	THROW(CppUtils::OperationFailed, "exc_CannotStartMtInstance", "ctx_parseSettings", startUpEntry.mtName);
				//}
			}
			*/

			mtsetting.setStartUpSettings(startUpEntry);
		

			auto it_mts = sesttings_m.mtSettingsMap_m.find(startUpEntry.mtName);
			if (it_mts != sesttings_m.mtSettingsMap_m.end())
				THROW(CppUtils::OperationFailed, "exc_DublicateMTNameEntry", "ctx_parseSettings", startUpEntry.mtName);

			sesttings_m.mtSettingsMap_m[ startUpEntry.mtName ] = mtsetting;
			LOG(MSG_INFO, SETTINGS_MAN, "MT setting path: " << startUpEntry.mtPath /* << " pipe: " << startUpEntry.mtPipeName */ << " name: " << startUpEntry.mtName << ", broker layer initialized" );

		} // end loop over instances

		

		// read heartbeat interval
		int hbt_int = atoi(sc.getBaseEntry()->heartbeatjavahtsec.c_str());

		if (hbt_int > 0)
			sesttings_m.heartBeatJavaHTSec_m = hbt_int;

		LOG(MSG_DEBUG, SETTINGS_MAN, "Heartbeat interval to Java HT layer: " << sesttings_m.heartBeatJavaHTSec_m << " seconds" );
		
		
		
	}

	void SettingsManager::installMt()
	{
		LOCK_FOR_SCOPE(*this);

		CppUtils::String path_s = CppUtils::getStartupDir();

		for(auto it = sesttings_m.mtSettingsMap_m.begin(); it != sesttings_m.mtSettingsMap_m.end(); it++) {
			MTSettings const & settings_i = it->second;

		//for(int i = 0; i < mtinstances; i++) {
			

			
			// autonistall
			if (settings_i.getStartUpSettings().autoInstall_m) {

				CppUtils::String const& path_mt = it->second.getStartUpSettings().mtPath;

				// ------------------

				CppUtils::String mq4LibName = path_s + NAME_MT4_MQL_COMPILED;
				CppUtils::String mq4LibName_dest = path_mt + PATH_MT4_MQL + NAME_MT4_MQL_COMPILED;
				if (!CppUtils::fileExists(mq4LibName))
					THROW(CppUtils::OperationFailed, "exc_FileNotExist", "ctx_installMt", mq4LibName );


				LOG(MSG_DEBUG, SETTINGS_MAN, "Copying file: " << mq4LibName << " to: " << mq4LibName_dest);
				if (!CppUtils::copyFile(mq4LibName, mq4LibName_dest, true ))
					THROW(CppUtils::OperationFailed, "exc_CannotCopyFile", "ctx_installMt", CppUtils::getOSError());
				
				// -------------------

				CppUtils::String mq4LibNameSrc = path_s + NAME_MT4_MQL_SOURCE;
				CppUtils::String mq4LibNameSrc_dest = path_mt + PATH_MT4_MQL + NAME_MT4_MQL_SOURCE;
				if (!CppUtils::fileExists(mq4LibNameSrc))
					THROW(CppUtils::OperationFailed, "exc_FileNotExist", "ctx_installMt", mq4LibNameSrc );

				LOG(MSG_DEBUG, SETTINGS_MAN, "Copying file: " << mq4LibNameSrc << " to: " << mq4LibNameSrc_dest);
				if (!CppUtils::copyFile(mq4LibNameSrc, mq4LibNameSrc_dest, true ))
					THROW(CppUtils::OperationFailed, "exc_CannotCopyFile", "ctx_installMt", CppUtils::getOSError());

		
				// -------------------


				// create and save ini file
				CppUtils::String mq4InitConf_dest = path_mt + CppUtils::pathDelimiter() +PATH_MT4_CONF + NAME_MT4_MQL_INIT_CONFIG;
				CppUtils::String inifile_cont;
		
				if (it->second.getStartUpSettings().fstruct_m.rows_m.size() <= 0) {
					inifile_cont = MtCommonHelpers::createMTIniContent();
				} else {
					auto const& row_first = it->second.getStartUpSettings().fstruct_m.rows_m[0];
					inifile_cont = MtCommonHelpers::createMTIniContent(row_first.user_m.c_str(), row_first.pass_m.c_str(), row_first.srv_m.c_str());
				}

				if(!CppUtils::saveContentInFile2(mq4InitConf_dest, inifile_cont ))
					THROW(CppUtils::OperationFailed, "exc_CannotSaveFile", "ctx_installMt", mq4InitConf_dest << " - " << CppUtils::getOSError());

				LOG(MSG_DEBUG, SETTINGS_MAN, "Saved MT ini file: " << mq4InitConf_dest );
			

				// check dll			
				// ------------

				CppUtils::String fullMt4ProxyDllName = path_s + SOURCE_NAME_MT4_PROXY_DLL_BASE;

				// destination file must be always 
				CppUtils::String fullMt4ProxyDllName_dest = path_mt + CppUtils::pathDelimiter() + TARGET_NAME_MT4_PROXY_DLL_BASE;

				if (!CppUtils::fileExists(fullMt4ProxyDllName))
					THROW(CppUtils::OperationFailed, "exc_FileNotExist", "ctx_installMt", fullMt4ProxyDllName );
				  

				LOG(MSG_DEBUG, SETTINGS_MAN, "Copying file: " << fullMt4ProxyDllName << " to: " << fullMt4ProxyDllName_dest);
				if (!CppUtils::copyFile(fullMt4ProxyDllName, fullMt4ProxyDllName_dest, true ))
					THROW(CppUtils::OperationFailed, "exc_CannotCopyFile", "ctx_installMt", CppUtils::getOSError());

				// ------------------
				//  set file

					
				CppUtils::String setFile_dest = path_mt + PATH_MT4_PRESETS + NAME_MT4_MQL_INIT_PARAMS;
				CppUtils::String setFile_dest_content = MtCommonHelpers::createMtParameterContent(
					settingsFile_m.c_str(), 
					CHILD_MT_LOG_LEVEL, 
					settings_i.getStartUpSettings().tmpResultPath_m.c_str()
				);
	
				if(!CppUtils::saveContentInFile2(setFile_dest, setFile_dest_content ))
					THROW(CppUtils::OperationFailed, "exc_CannotSaveFile", "ctx_autoInstLibToMt", setFile_dest << " - " << CppUtils::getOSError());

				LOG(MSG_DEBUG, SETTINGS_MAN, "Saved MT set file: " << setFile_dest );


			} // END LOOP
		};


		

	}



	MTSettings const& SettingsManager::resolveSettingsViaInstanceName(CppUtils::String const& name) const
	{
		LOCK_FOR_SCOPE(*this);

		if (name.size() <=0)
			THROW(CppUtils::OperationFailed, "exc_WrongMTInstance", "ctx_SetingManager_resolveSettingsViaInstanceName", "");

		auto it = sesttings_m.mtSettingsMap_m.find(name);
		if (it == sesttings_m.mtSettingsMap_m.end())
			THROW(CppUtils::OperationFailed, "exc_CannotFindMTSettingsEntry", "ctx_SetingManager_resolveSettingsViaInstanceName", name);

		return it->second;
	}

	

}; // end of ns