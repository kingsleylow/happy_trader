#ifndef _SMARTCOM_SIMULATOR_INCLUDED
#define _SMARTCOM_SIMULATOR_INCLUDED

#include "smartcom_proxy2defs.h"
#include "helperstruct.h"

namespace SmartComHelperLib {
	class Simulator: public CppUtils::Mutex
	{
	public:

		Simulator();

		~Simulator();

		void init(CppUtils::StringSet const& simSymbols, CppUtils::String const& rtprovider);

		
		void addSymbolCandle(CppUtils::String const& symbol, Candle const& data);

		void generateNewCandle();

		double getLastTimeStamp() const;

	private:

		void generateNewCandle(Candle& new_data, Candle const& old_data);

		map<CppUtils::String, Candle> lastHistoryTimestamps_m;	

		int numberOfMinutesSinceSimulation_m;

		CppUtils::StringSet simulationSymbols_m;

		CppUtils::String rtProvider_m;

		map<CppUtils::String, Candle> lastGeneratedData_m;	

		double maxtimestamp_m;


	};
};

#endif