#ifndef _ALGLIBMT2_SETTINGSMANAGER_INCLUDED
#define _ALGLIBMT2_SETTINGSMANAGER_INCLUDED

#include "alglibmetatrader2defs.hpp"
#include "shared.hpp"
#include "commonstruct.hpp"



namespace AlgLib {

	typedef map<CppUtils::Uid, CppUtils::String> UidStringMap;

	class AlgorithmMt;
	class SettingsManager: private CppUtils::Mutex
	{
	public:
		SettingsManager(AlgorithmMt& base);

		~SettingsManager();

		void parse(CppUtils::String const& file);

		// call install if necessary
		void installMt();

		inline GlobalSettingsEntry const& getSettings() const
		{
			return sesttings_m;
		}

		inline GlobalSettingsEntry& getSettings()
		{
			return (GlobalSettingsEntry&)sesttings_m;
		}

		inline CppUtils::String const& getSettingsFile() const
		{
			return settingsFile_m;
		};

		
		MTSettings const& resolveSettingsViaInstanceName(CppUtils::String const& name) const;


	private:
		

		GlobalSettingsEntry sesttings_m;

		CppUtils::String settingsFile_m;

		AlgorithmMt& base_m;


	};

};

#endif