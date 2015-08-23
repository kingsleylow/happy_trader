#ifndef _MGERCHIK_SLOGGER_INCLUDED
#define _MGERCHIK_SLOGGER_INCLUDED

#include "../brklib/brklib.hpp"



namespace AlgLib {

	class SLogger: private CppUtils::Mutex{
	public:

		enum Level {
			SL_ERROR =	0,
			SL_INFO =		1,
			SL_DEBUG =	2
		};

		SLogger();

		~SLogger();

		void initialize(CppUtils::String const& basePath, Level const level);

		void deinitialize();

		// add log entry and flushes it
		void addLogEntry(Level const level, CppUtils::String const& provider, CppUtils::String const& symbol, CppUtils::String const& data);

		void addAccountEntry(Level const level, BrkLib::BrokerResponseEquity const& requity);

		// write context
		void addLogEntry(
			Level const level,
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			CppUtils::String const& data,
			CppUtils::String const& context
		);

		// add log entry and flushes it without provider
		void addLogEntryNoProvider(Level const level, CppUtils::String const& symbol, CppUtils::String const& data);

		// write context without provider
		void addLogEntryNoProvider(
			Level const level,
			CppUtils::String const& symbol, 
			CppUtils::String const& data,
			CppUtils::String const& context
		);

		// add to common file
		void addLogEntry(Level const level, CppUtils::String const& data);

		inline int getLogLevel() const
		{
			return curLevel_m;
		}
  

	private:

				
		char const* getLogLevelString(Level const level) const;
		
		CppUtils::String basePath_m;

		ofstream* common_m; // common will be always opened, but not the separate files as there are a lot of them

		ofstream* symbol_m; // the only one file for symbols

		ofstream* account_m; // account & equity...

		int curLevel_m;

		bool not_initialized_m;
		

	};

}

#endif