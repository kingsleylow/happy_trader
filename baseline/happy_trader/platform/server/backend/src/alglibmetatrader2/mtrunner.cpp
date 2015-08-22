#include "mtrunner.hpp"
#include "shared.hpp"
#include "algorithmmt.hpp"

#define MTRUNNER "MTRUNNER"

namespace AlgLib {

	
	MtRunManager::MtRunManager( AlgorithmMt& base, SettingsManager const& settingsManager ):
		settingsManager_m(settingsManager),
		base_m(base)
	{
	}

	MtRunManager::~MtRunManager()
	{
		

		LOG(MSG_INFO, MTRUNNER, "Destructor call...");

	}



	void MtRunManager::prepare(CppUtils::String const& mtInstance)
	{
		curPid_m=-1;
		
		// get a copy
		// probbaly many need to return a ref later

		MTSettings const& set_i = settingsManager_m.resolveSettingsViaInstanceName(mtInstance);

		//if (!set_i.getStartUpSettings().useChildMtToReconnect_m)
		//	THROW(CppUtils::OperationFailed, "exc_IncorrectSettingsEntry_useChildMtToReconnect", "ctx_MtRunnerParepare", mtInstance);

		// copy 


		childTerminalPath_m = set_i.getStartUpSettings().mtPath + CppUtils::pathDelimiter() +MTRUNNER_BASE_DIR+ CppUtils::pathDelimiter();
		CppUtils::makeDir(childTerminalPath_m);

		childTerminalExecutable_m = childTerminalPath_m + "terminal.exe";
		mq4InitConf_ChildTerm_m = childTerminalPath_m  +PATH_MT4_CONF + NAME_MT4_MQL_INIT_CONFIG;
		mq4SetFile_ChildTerm_m = childTerminalPath_m + PATH_MT4_PRESETS + NAME_MT4_MQL_INIT_PARAMS;



		//CppUtils::copyFile
		function< bool(char const*) > filterFun = [](char const* fileName) {

			if (fileName != NULL && strlen(fileName) > 0) {
				CppUtils::String filaName_u = CppUtils::toLowerCase(fileName);

				if (filaName_u.find(MTRUNNER_BASE_DIR) != CppUtils::String::npos ) {
					// found - do not copy
					return false;
				}
				else if (filaName_u.find(".log") != CppUtils::String::npos ) {
					return false;
				}
			}

			// nothing - copy
			return true;
		};

		if (!CppUtils::copyDirFilter(set_i.getStartUpSettings().mtPath, childTerminalPath_m,  true, filterFun))
			THROW(CppUtils::OperationFailed, "exc_CannotCopydir", "ctx_createAndRunMTInstanceCopy", set_i.getStartUpSettings().mtPath << " - " << childTerminalPath_m );

		// log that copied
		LOG(MSG_INFO, MTRUNNER, "Created temp MT instance at: " << childTerminalPath_m << " from instance: " << set_i.getStartUpSettings().mtName);
		LOG(MSG_INFO, MTRUNNER, "Startup dir resolved: " << set_i.getStartUpSettings().mtPath);
		LOG(MSG_INFO, MTRUNNER, "Child terminal executable: " << childTerminalExecutable_m);
		LOG(MSG_INFO, MTRUNNER, "Child terminal config file: " << mq4InitConf_ChildTerm_m);
		LOG(MSG_INFO, MTRUNNER, "Child terminal set file: " << mq4SetFile_ChildTerm_m);


		//
		mtInstance_m = mtInstance;

		if (mtInstance_m.size() <=0)
			THROW(CppUtils::OperationFailed, "exc_MTInstanceNameNotValid", "ctx_MtRunManager_prepare", "");


		// tmp result path
		outputDir_m = set_i.getStartUpSettings().tmpResultPath_m;
		LOG(MSG_INFO, MTRUNNER, "Temp output dir: " << outputDir_m);
	}

	void MtRunManager::run(
		CppUtils::String const& user, 
		CppUtils::String const& password, 
		CppUtils::String const& server 
	)
	{
		
		if (mtInstance_m.size() <=0)
			THROW(CppUtils::OperationFailed, "exc_MTInstanceNameNotValid", "ctx_RunMtInstaceCopy", "");

		// need to create settings file
		CppUtils::String inifile_cont = AlgLib::MtCommonHelpers::createMTIniContent(user.c_str(), password.c_str(), server.c_str());;
		if(!CppUtils::saveContentInFile2(mq4InitConf_ChildTerm_m, inifile_cont ))
			THROW(CppUtils::OperationFailed, "exc_CannotSaveFile", "ctx_RunMtInstaceCopy", mq4InitConf_ChildTerm_m << " - " << CppUtils::getOSError());
		
		LOG(MSG_DEBUG, MTRUNNER, "Created settings file for MT temp instance: " << mq4InitConf_ChildTerm_m );

		//
		//CppUtils::Uid tmp; tmp.generate();
		//CppUtils::String tmpResFile = outputDir_m + CppUtils::pathDelimiter() + tmp.toString() + "_tmp_mt.txt";

		// get a copy
		// probbaly many need to return a ref later
		//MTSettings  const& set_i = settingsManager_m.resolveSettingsViaInstanceName(mtInstance_m);

		// parameters
		CppUtils::String setFile_dest_content = AlgLib::MtCommonHelpers::createMtParameterContent(
			settingsManager_m.getSettingsFile().c_str(),
			CHILD_MT_LOG_LEVEL, 
			outputDir_m.c_str() 
		);



		if(!CppUtils::saveContentInFile2(mq4SetFile_ChildTerm_m, setFile_dest_content ))
			THROW(CppUtils::OperationFailed, "exc_CannotSaveFile", "ctx_RunMtInstaceCopy", mq4SetFile_ChildTerm_m << " - " << CppUtils::getOSError());

	
		DEBUG_LOG(base_m, "[" << user << "] of [" << server << "] - ["<< mtInstance_m << "] - STARTED OK");
		LOG(MSG_DEBUG, MTRUNNER, "[" << user << "] of [" << server << "] - ["<< mtInstance_m << "] - started OK" );
		
		CppUtils::Process ps(childTerminalExecutable_m, START_MT4_CMD_PARAMS, childTerminalPath_m, false, false, false );
		curPid_m = ps.getPid();
		
		if (!ps.waitForProcess( WAIT_CHILDMT_COMPLETION_SEC )) {
			
			LOG(MSG_ERROR, MTRUNNER, "Trying to kill MT process manually after timeout of: " << WAIT_CHILDMT_COMPLETION_SEC << ", pid: " << ps.getPid() );
			ps.killProcess();
			ps.waitForProcess();
			LOG(MSG_ERROR, MTRUNNER, "Killed MT process manually, pid: " << ps.getPid() );

			DEBUG_LOG(base_m, "MT temp: " << mtInstance_m << " KILLED, PID: " << ps.getPid());
			//THROW(CppUtils::OperationFailed, "exc_CannotWaitForprocessCompletion", "ctx_RunMtInstaceCopy", childTerminalExecutable_m);
		} else {
			LOG(MSG_DEBUG, MTRUNNER, "MT temp instance finished ok: " << childTerminalExecutable_m << " - " << ps.getPid() );
			DEBUG_LOG(base_m, "[" << user << "] of [" << server << "] - ["<< mtInstance_m << "] - FINISHED OK PROCESS: " <<  ps.getPid() );
		}
		
		curPid_m = -1;

	}

	
};
