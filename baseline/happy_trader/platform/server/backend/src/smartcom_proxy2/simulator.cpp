#include "simulator.hpp"
#include "platform\server\backend\src\inqueue\inqueue.hpp"

#define SIMULATOR "SIMULATOR"

namespace SmartComHelperLib {

	Simulator::Simulator():
		numberOfMinutesSinceSimulation_m(0),
		maxtimestamp_m(-1)
	{
		 srand((unsigned) time(NULL));
	}

	Simulator::~Simulator()
	{
	}

	void Simulator::init(CppUtils::StringSet const& simSymbols, CppUtils::String const& rtprovider)
	{
	
		simulationSymbols_m = simSymbols;
		rtProvider_m = rtprovider;
		numberOfMinutesSinceSimulation_m = 0;
		lastGeneratedData_m.clear();
	
	}

	

	void Simulator::addSymbolCandle(CppUtils::String const& symbol, Candle const& data)
	{
		LOCK_FOR_SCOPE(*this)
		lastHistoryTimestamps_m[symbol] = data;

		if (maxtimestamp_m <0)
			maxtimestamp_m= data.datetime_m;
		else {
			if (maxtimestamp_m < data.datetime_m)
				 maxtimestamp_m = data.datetime_m;
		}
	}

	double Simulator::getLastTimeStamp() const
	{
		LOCK_FOR_SCOPE(*this)
		return maxtimestamp_m;
	}

	void Simulator::generateNewCandle()
	{
			Candle new_data;

			// advance the number of minutes
			new_data.datetime_m = CppUtils::roundToMinutes(maxtimestamp_m) + (numberOfMinutesSinceSimulation_m++)*60;

			{
				LOCK_FOR_SCOPE(*this)
			

				for(CppUtils::StringSet::const_iterator it = simulationSymbols_m.begin(); it != simulationSymbols_m.end(); it++) {
					CppUtils::String const& symbol = *it;
					
					map<CppUtils::String, Candle>::const_iterator i2 = lastHistoryTimestamps_m.find(symbol);
					if (i2 == lastHistoryTimestamps_m.end())
						THROW(CppUtils::OperationFailed, "exc_NoHistoryData", "ctx_generateNewCandle", symbol);
				

					// generate data
					// resolve previous data
					map<CppUtils::String, Candle>::const_iterator i3 = lastGeneratedData_m.find(symbol);
					if (i3 != lastGeneratedData_m.end()) {
						// old data are saves
						generateNewCandle(new_data, i3->second);
					}	else {
						// no old data - get what we have stored
						generateNewCandle(new_data, i2->second);
					}


					// save
					lastGeneratedData_m[symbol] = new_data;

					// send
					Inqueue::OutProcessor::send_rt_data_ohcl(
						new_data.datetime_m, 
						rtProvider_m.c_str(), 
						symbol.c_str(), 
						new_data.open_m,
						new_data.high_m,
						new_data.low_m,
						new_data.close_m,
						new_data.volume_m
					);				  


					
				}
			}
			
			
	}

	void Simulator::generateNewCandle(Candle& new_data, Candle const& old_data)
	{
		// generate random advance
		double old_range = old_data.high_m - old_data.low_m;	// the size of the candle
		int n_iter = 10;
		double iter_delta = old_range / sqrt((double)n_iter);
		
		new_data.open_m  =	old_data.close_m;
		new_data.close_m =  old_data.close_m;
		new_data.high_m = old_data.close_m;
		new_data.low_m = old_data.close_m;
		new_data.volume_m = old_data.volume_m / n_iter; 
		
		double cur_price = new_data.open_m;
		//LOG(MSG_INFO, SIMULATOR, "Price 0: " << cur_price );
		
		double b1 = -iter_delta;
		double b2 = iter_delta;
		for(int i = 0; i < n_iter; i++) {
			
			double new_rand_step = b1 + (b2 - b1) * (double)rand() / (double)RAND_MAX;
			//LOG(MSG_INFO, SIMULATOR, "Step: " << new_rand_step );

			double new_cur_price = cur_price  +	 new_rand_step;
			cur_price =  (new_cur_price <=0 ? cur_price : new_cur_price );
			//LOG(MSG_INFO, SIMULATOR, "Price next: " << cur_price );

			if (i == (n_iter-1))
				// last
				new_data.close_m = cur_price;

			if (new_data.high_m < cur_price)
				new_data.high_m = cur_price;

			if (new_data.low_m > cur_price)
				new_data.low_m = cur_price;

		}

		//LOG(MSG_INFO, SIMULATOR, "Generated new candle: " << new_data.toString() );

	}

}; // end of namepsace
