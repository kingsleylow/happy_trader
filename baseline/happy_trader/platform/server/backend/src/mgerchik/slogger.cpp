#include "slogger.hpp"
#include <fstream>


namespace AlgLib {

	// ------------------------------------

	char const* logLevels_g[] = {"ERROR", "INFO", "DEBUG"};

	// ------------------------------------
	SLogger::SLogger():
	common_m(NULL),
	symbol_m(NULL),
	account_m(NULL),
	curLevel_m(SL_DEBUG),
	not_initialized_m(true)
	{
		

	}

	SLogger::~SLogger()
	{
		deinitialize();
	}

	void SLogger::initialize(CppUtils::String const& basePath, Level const level)
	{
		
		if (!not_initialized_m)
			return;

		common_m = NULL;
		curLevel_m = level;

		if (basePath.size() <=0)	
			THROW(CppUtils::OperationFailed, "exc_InvalidBasePath", "ctx_SLoggerCtr",basePath );

		// create unique file name
		basePath_m = basePath + "\\" + CppUtils::getAscTimeAsFileName(CppUtils::getTime())+"\\";
		if (!CppUtils::makeDir(basePath_m))
			THROW(CppUtils::OperationFailed, "exc_CantCreateDir", "ctx_SLoggerCtr",basePath );

		not_initialized_m = false;
	}

	void SLogger::deinitialize()
	{ 
		if (not_initialized_m)
			return;

		/*
		for(map<CppUtils::String, ofstream*>::iterator it = fileHandlers_m.begin(); it != fileHandlers_m.end(); it++) {
			it->second->close();

			delete it->second;
		}

		fileHandlers_m.clear();

		*/

		if (symbol_m) {
			symbol_m->close();

			delete symbol_m;
			symbol_m = NULL;
		}

		if (common_m){
			common_m->close();
			delete common_m;

			common_m = NULL;
		}

		if (account_m) {
			account_m->close();
			delete account_m;

			account_m = NULL;
		}

		not_initialized_m = true;
	}

	void SLogger::addAccountEntry(Level const level, BrkLib::BrokerResponseEquity const& requity)
	{

		if (not_initialized_m)
			return;

		if (level <= curLevel_m) {

			{
				LOCK_FOR_SCOPE(*this);

				if (account_m==NULL) {
					CppUtils::String fileName = basePath_m + "account.log";

					account_m = new ofstream(fileName.c_str(), ios::out | ios::trunc);

					if (account_m==NULL) 
						THROW(CppUtils::OperationFailed, "exc_CantOpenFileForWriting", "ctx_AddLogEntry",fileName );

				

				}
			}
	  
			// write to file
			(*account_m) << ">>>>>> " << " [ " << CppUtils::getAscTime(CppUtils::getTime()) << " ] " <<
				"[" << getLogLevelString(level) << "]-(" << CppUtils::getTID() << ") " <<
				" Equity: " << requity.curPositionReportProfit_m <<
				" Comment: " << requity.brokerComment_m <<
				endl <<  "<<<<<<" << endl;

			account_m->flush();	
		}
		

	}

	void SLogger::addLogEntry(Level const level, CppUtils::String const& provider, CppUtils::String const& symbol, CppUtils::String const& data)
	{
		if (not_initialized_m)
			return;

		if (level <= curLevel_m) {

			{
				LOCK_FOR_SCOPE(*this);

				if (symbol_m==NULL) {
					CppUtils::String fileName = basePath_m + "symbol.log";

					symbol_m = new ofstream(fileName.c_str(), ios::out | ios::trunc);

					if (symbol_m==NULL) 
						THROW(CppUtils::OperationFailed, "exc_CantOpenFileForWriting", "ctx_AddLogEntry",fileName );

				

				}
			}
	  
			// write to file
			(*symbol_m) << ">>>>>> " << provider << " - " << symbol << " >>>>>>" << " [ " << CppUtils::getAscTime(CppUtils::getTime()) << " ] " <<
			"[" << getLogLevelString(level) << "]-(" << CppUtils::getTID() << ") " <<
			data << endl <<  "<<<<<<" << endl;

			symbol_m->flush();	
		}
		

	}

	void SLogger::addLogEntry(
			Level const level,
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			CppUtils::String const& data,
			CppUtils::String const& context
		)
	{

		addLogEntry(level, provider, symbol, data + "\n CONTEXT (\n" + context + "\n)");
	}

	void SLogger::addLogEntryNoProvider(Level const level, CppUtils::String const& symbol, CppUtils::String const& data)
	{
		if (not_initialized_m)
			return;

		if (level <= curLevel_m) {

			{
				LOCK_FOR_SCOPE(*this);

				if (symbol_m==NULL) {
					CppUtils::String fileName = basePath_m + "symbol.log";

					symbol_m = new ofstream(fileName.c_str(), ios::out | ios::trunc);

					if (symbol_m==NULL) 
						THROW(CppUtils::OperationFailed, "exc_CantOpenFileForWriting", "ctx_AddLogEntry",fileName );

				

				}
			}
	  
			// write to file
			(*symbol_m) << ">>>>>> " << symbol << " >>>>>>"  << " [ " << CppUtils::getAscTime(CppUtils::getTime()) << " ] " <<
			"[" << getLogLevelString(level) << "]-(" << CppUtils::getTID() << ") " <<
			data << endl <<  "<<<<<<" << endl;

			symbol_m->flush();	
		}
		

	}

	void SLogger::addLogEntryNoProvider(
			Level const level,
			CppUtils::String const& symbol, 
			CppUtils::String const& data,
			CppUtils::String const& context
		)
	{

		addLogEntryNoProvider(level, symbol, data + "\nCONTEXT\n(\n" + context + "\n)");
	}


	void SLogger::addLogEntry(Level const level, CppUtils::String const& data)
	{
	
		if (not_initialized_m)
			return;

		if (level <= curLevel_m) {

			{
				LOCK_FOR_SCOPE(*this);

				if (common_m==NULL) {
					CppUtils::String fileName = basePath_m + "common.log";

					common_m = new ofstream(fileName.c_str(), ios::out | ios::trunc);

					if (common_m==NULL) 
						THROW(CppUtils::OperationFailed, "exc_CantOpenFileForWriting", "ctx_AddLogEntry",fileName );

				

				}
			}
	  
			// write to file
			(*common_m) << ">>>>>>" << " [ " << CppUtils::getAscTime(CppUtils::getTime()) << " ] " <<
				"[" << getLogLevelString(level) << "]-(" << CppUtils::getTID() << ") " <<
				data << endl <<  "<<<<<<" << endl;

			common_m->flush();	
		}

	}

	char const* SLogger::getLogLevelString(Level const level) const
	{
		static char const* dummy = "n/a";

		int level_i = (int)level;
		if (level_i <= SL_DEBUG && level_i >= SL_ERROR ) {
			return logLevels_g[level_i];
		}
		
		return dummy;

	}

}