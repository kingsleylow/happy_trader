#ifndef _ALGLIBMT2_MTRUNNER_INCLUDED
#define _ALGLIBMT2_MTRUNNER_INCLUDED


#include "alglibmetatrader2defs.hpp"
#include "settingsmanager.hpp"

namespace AlgLib {

	// class wrapping call to MT and returning results from the file
	
	class AlgorithmMt;
	class MtRunManager
	{
	public:
		MtRunManager( AlgorithmMt& base, SettingsManager const& settingsManager );

		~MtRunManager();

		void prepare(CppUtils::String const& mtInstance);

		void run(
			CppUtils::String const& user, 
			CppUtils::String const& password, 
			CppUtils::String const& server// note that it's going to be the same company as parent terminal
		);

		inline int getMtPid() const
		{
			return curPid_m;
		}

	private:

	private:

		MtRunManager();

		MtRunManager(MtRunManager const& rhs);

		SettingsManager const& settingsManager_m;

		CppUtils::String mtInstance_m;

		CppUtils::String childTerminalPath_m;

		CppUtils::String childTerminalExecutable_m;
		
		CppUtils::String mq4InitConf_ChildTerm_m;
		
		CppUtils::String mq4SetFile_ChildTerm_m;

		CppUtils::String outputDir_m;

		atomic<int> curPid_m;


		AlgorithmMt& base_m;

	};
	
};

#endif