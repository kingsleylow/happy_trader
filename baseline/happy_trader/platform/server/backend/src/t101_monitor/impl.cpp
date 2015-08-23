#include "impl.hpp"
#include "../inqueue/commontypes.hpp"
#include "../math/math.hpp"
#include <sstream> 
	using namespace std;

#define T101 "T101"

namespace AlgLib {

// -------------------------------

	


// -------------------------------

	map<CppUtils::String, pair<int, bool> > Symbol::symbolIdentifiers_m = map<CppUtils::String, pair<int, bool> >();

	CppUtils::Mutex Symbol::symbolIdentifiersMtx_m;

	const int BootManager::foo_scm = BootManager::boot( );
	
// --------------------------------
int BootManager::boot ()
{
	LOCK_FOR_SCOPE(Symbol::symbolIdentifiersMtx_m);
	LOGEVENT(HlpStruct::CommonLog::LL_INFO,  T101, "Booting...");

	// longs
	Symbol::symbolIdentifiers_m["USDJPY"] = pair<int, bool>(1, true);
	Symbol::symbolIdentifiers_m["CADJPY"] = pair<int, bool>(2, true);
	Symbol::symbolIdentifiers_m["GBPUSD"] = pair<int, bool>(3, true);
	Symbol::symbolIdentifiers_m["GBPJPY"] = pair<int, bool>(4, true);
	Symbol::symbolIdentifiers_m["EURGBP"] = pair<int, bool>(5, true);
	Symbol::symbolIdentifiers_m["AUDUSD"] = pair<int, bool>(6, true);
	Symbol::symbolIdentifiers_m["EURJPY"] = pair<int, bool>(7, true);

	// shorts
	Symbol::symbolIdentifiers_m["USDCAD"] = pair<int, bool>(8, false);
	Symbol::symbolIdentifiers_m["CHFJPY"] = pair<int, bool>(9, false);
	Symbol::symbolIdentifiers_m["GBPCHF"] = pair<int, bool>(10, false);
	Symbol::symbolIdentifiers_m["EURCHF"] = pair<int, bool>(11, false);
	Symbol::symbolIdentifiers_m["USDCHF"] = pair<int, bool>(12, false);
	Symbol::symbolIdentifiers_m["AUDJPY"] = pair<int, bool>(13, false);
	Symbol::symbolIdentifiers_m["EURUSD"] = pair<int, bool>(14, false);


	return 42;

};

// ------------------------------------------------------------
// Symbol
	Symbol::Symbol(
		CppUtils::String const& symbol, 
		double const& income, 
		double const& cutTranslRate,
		double const& priceDiffIncome
	):
			id_m(-1),
			symbol_m(symbol),
			income_m( income ),
			isLong_m( false ),
			oldIncome_m(-1),
			curTranslRate_m( cutTranslRate ),
			priceDiffIncome_m(priceDiffIncome)
		{
			LOCK_FOR_SCOPE(Symbol::symbolIdentifiersMtx_m);
			
			map<CppUtils::String, pair<int, bool> >::const_iterator it = symbolIdentifiers_m.find(symbol);

			if (it!=symbolIdentifiers_m.end()) {
				id_m = it->second.first;
				isLong_m = it->second.second;
			}
			
		}

		Symbol::Symbol():
			id_m(-1),
			income_m( -1 ),
			isLong_m(false),
			oldIncome_m(-1),
			curTranslRate_m(-1),
			priceDiffIncome_m(-1)
		{
		}

		void Symbol::initialize(
			CppUtils::String const& symbol, 
			double const& income, 
			double const& cutTranslRate,
			double const& priceDiffIncome
		)
		{

			symbol_m = symbol;
			income_m = income;
			curTranslRate_m = cutTranslRate;
			priceDiffIncome_m = priceDiffIncome;

			{
				LOCK_FOR_SCOPE(Symbol::symbolIdentifiersMtx_m);

				map<CppUtils::String, pair<int, bool> >::const_iterator it = symbolIdentifiers_m.find(symbol);
				if (it!=symbolIdentifiers_m.end()) {
					id_m = it->second.first;
					isLong_m = it->second.second;
				}
				else {
					id_m = -1;
					isLong_m = false;
				}
			}

		}

		bool Symbol::operator == (Symbol const& rhs) const
		{
			return id_m > 0 && rhs.id_m > 0 && id_m==rhs.id_m;
		}

		bool Symbol::operator != (Symbol const& rhs) const
		{
			return !(*this==rhs);
		}

			
		//

		bool Symbol::isValid()
		{
			// must be even
			{
				LOCK_FOR_SCOPE(Symbol::symbolIdentifiersMtx_m);
				if ( !(symbolIdentifiers_m.size() & 0x0001) ) {
					if (symbolIdentifiers_m.size() >=2)
						return true;
				}
			}

			return false;
		}

		void Symbol::getAnchorIndexes(int& anchor1, int& anchor2)
		{
			if (!isValid()) {
				anchor1 = -1;
				anchor2 = -1;
			}
			else {
				{
					LOCK_FOR_SCOPE(Symbol::symbolIdentifiersMtx_m);

					anchor1 = 0;
					anchor2 = symbolIdentifiers_m.size() - 1;
				}
			}
		}

		void Symbol::getZeroLevelIndexes(int& zero1, int& zero2)
		{
			if (!isValid()) {
				zero1 = -1;
				zero2 = -1;

			}
			else {
				{
					LOCK_FOR_SCOPE(Symbol::symbolIdentifiersMtx_m);
					zero2 = symbolIdentifiers_m.size() / 2;
					zero1 = zero2-1;
				}
			}

		}

		void Symbol::getShortSymbolList(CppUtils::StringList& symbols)
		{
			symbols.clear();
			{
				LOCK_FOR_SCOPE(Symbol::symbolIdentifiersMtx_m);
				for(map<CppUtils::String, pair<int, bool> >::iterator it = symbolIdentifiers_m.begin(); it != symbolIdentifiers_m.end(); it++)
				{
					if (it->second.second==false)
						// shorts
						symbols.push_back(it->first);
				}
			}
		}

		void Symbol::getLongSymbolList(CppUtils::StringList& symbols)
		{
			symbols.clear();
			{
				LOCK_FOR_SCOPE(Symbol::symbolIdentifiersMtx_m);
				for(map<CppUtils::String, pair<int, bool> >::iterator it = symbolIdentifiers_m.begin(); it != symbolIdentifiers_m.end(); it++)
				{
					if (it->second.second==true)
						// longs
						symbols.push_back(it->first);
				}
			}
		}

		bool Symbol::isLong(CppUtils::String const& symbol)
		{

			map<CppUtils::String, pair<int, bool> >::const_iterator it = symbolIdentifiers_m.find(symbol);
			{
				LOCK_FOR_SCOPE(Symbol::symbolIdentifiersMtx_m);
				if (it!=symbolIdentifiers_m.end()) {
					return it->second.second;
				}
				else {
					THROW(CppUtils::OperationFailed, "exc_SymbolNotRegistered", "ctx_IsLong", symbol );
				}
			}

		}


// BasketStructure
//-------------------------------------------------------------
	BasketStructure::BasketStructure():
		lasttime_m(0)
	{
		
	}

void BasketStructure::updateNewSymbol(
		CppUtils::String const& symbol, 
		double const& newincome, 
		double const& updateTime, 
		double const& translRate,
		double const& priceDiffIncome
)
{
	

	map<CppUtils::String, Symbol>::iterator it = regSymbols_m.find(symbol);
	if (it != regSymbols_m.end()) {
		

		// remove from sorted list
		multimap<double, Symbol>::iterator itSrt = sortedSymbols_m.find(it->second.oldIncome_m);

		
		while(itSrt != sortedSymbols_m.end()) {
			if (itSrt->second.symbol_m == symbol) {
				sortedSymbols_m.erase(itSrt);
				break;
			}

			itSrt++;
		}

		// init with new values
		it->second.oldIncome_m = newincome;
		it->second.initialize(symbol, newincome,translRate, priceDiffIncome );


		// insert new to sorted list
		sortedSymbols_m.insert(pair <double, Symbol>(newincome, it->second));

	}
	else {
		// insert new value and update

		Symbol sym(symbol, newincome, translRate, priceDiffIncome);
		sym.oldIncome_m = newincome;

		// insert
		sortedSymbols_m.insert(pair <double, Symbol>(newincome, sym));
		regSymbols_m[symbol] = sym;
	}
	
	
	
	// time
	lasttime_m =updateTime;
	lastSymbol_m = symbol;


};

void BasketStructure::compare(BasketStructure const& src, int& event, CppUtils::IntList& changes) const
{
	event = E_Nothing;
	changes.clear();

	if (!src.isNormalSize()) {
		event |= E_WrongSize;
		return;
	}

	if (!isNormalSize()) {
		event |= E_WrongSize;
		return;
	}
	
	
	if (sortedSymbols_m.size() > 0 ) {


		// first more last

		//int idx = 1;
		multimap<double, Symbol>::const_iterator it_src = src.sortedSymbols_m.begin();

		for(multimap<double, Symbol>::const_iterator it = sortedSymbols_m.begin(); it != sortedSymbols_m.end(); it++ ) {
			Symbol const& smb_i = it->second;

			Symbol const& smb_i_src = it_src->second;
			
			if (smb_i != smb_i_src) {
				// all the same
				changes.push_back(smb_i.id_m);

			}

			it_src++;
			//idx++;
			
		}
		

		//
	}

	if (changes.size() >0)
		event |= E_Changed;
	

}

bool BasketStructure::isNormalSize() const
{
	return (regSymbols_m.size() == Symbol::symbolIdentifiers_m.size());
}



CppUtils::String BasketStructure::toHTML2Baskets(
			BasketStructure const& firstOne,
			double const& milisecs_time
		)
{
		CppUtils::String result;

		int anchor1, anchor2, zero1, zero2;
		Symbol::getAnchorIndexes(anchor1, anchor2);
		Symbol::getZeroLevelIndexes(zero1, zero2);

		if (anchor1 < 0 || anchor2 < 0) {
			result = "<table><tr><td>Invalid anchor indexes</td></tr></table>";
			return result;
		}

		if (zero1 < 0 || zero2 < 0) {
			result = "<table><tr><td>Invalid zero level indexes</td></tr></table>";
			return result;
		}

		//

		std::stringstream out;

		out << "<!-- Initial picture-->";
		out << "<table>";
		out << "<tr>";

		out << "<td>";
		out << "<table style='border: 1px solid black; font-size:smaller' cellpadding='2' cellspacing='2'>";

		// header
		out << "<tr class=x3>";

		out << "<td align=center>";
		out << "ID";
		out << "</td>"; 

		out << "<td align=center>";
		out << "SYMBOL";
		out << "</td>";

		out << "<td align=center>";
		out << "USD income";
		out << "</td>";

		out << "<td align=center>";
		out << "USD Rate";
		out << "</td>";

		out << "<td align=center>";
		out << "Price income";
		out << "</td>";
		
		out << "</tr>";


		//

		int idx = 0;
		for(multimap<double, Symbol>::const_iterator it = firstOne.sortedSymbols_m.begin(); it != firstOne.sortedSymbols_m.end(); it++) {

			if (zero2==idx) {
					out << "<tr height='3px'>";
					out << "<td colspan='5' BGCOLOR='black' BORDERCOLOR='black'>";
					out << "</td>";
					out << "</tr>";
			}

			out << "<tr bgcolor='" << ( it->second.isLong_m ? "limegreen" : "orange" ) << "'>";

			out << "<td>";
			out << CppUtils::long2String(it->second.id_m);
			out << "</td>"; 
	
			out << "<td>";
		  out << it->second.symbol_m;
			out << "</td>";
	
			out << "<td>";
			out << CppUtils::float2String((float)it->second.income_m, -1, 5 );
			out << "</td>";

			out << "<td>";
			out << CppUtils::float2String((float)it->second.curTranslRate_m, -1, 5 );
			out << "</td>";

			out << "<td>";
			out << CppUtils::float2String((float)it->second.priceDiffIncome_m, -1, 5 );
			out << "</td>";


			out << "</tr>";
	  
			idx++;
		}

		out << "</table>";
		out << "</td>";

		out << "</tr>";
		out << "</table>";

		return out.str();

}

void BasketStructure::toHTML2Baskets(
	BasketStructure const& firstOne, 
	BasketStructure const& secondOne,
	CppUtils::IntList const& changes,
	int& priority,
	CppUtils::String& buffer,
	CppUtils::String& small_buffer,
	double const& milisecs_time
)
{
		

			priority = 2;
			CppUtils::String result;

			int anchor1, anchor2, zero1, zero2;
			Symbol::getAnchorIndexes(anchor1, anchor2);
			Symbol::getZeroLevelIndexes(zero1, zero2);

			if (anchor1 < 0 || anchor2 < 0) {
				buffer = "<table><tr><td>Invalid anchor indexes</td></tr></table>";
				return;
			}

			if (zero1 < 0 || zero2 < 0) {
				buffer = "<table><tr><td>Invalid zero level indexes</td></tr></table>";
				return;
			}

			// how we calculate priority
			// firstly if anchor level indexes changed
			map<int, int> beforeIds, afterIds;

			

			CppUtils::String hint;
		

			std::stringstream out;
			
			out << "<table>";
			out << "<tr>";

			out << "<td>";
			out << "<table style='border: 1px solid black; font-size:smaller' cellpadding='2' cellspacing='2'>";

			// header
			out << "<tr class=x3>";

			out << "<td align=center>";
			out << "ID";
			out << "</td>"; 

			out << "<td align=center>";
			out << "SYMBOL";
			out << "</td>";

			out << "<td align=center>";
			out << "USD income";
			out << "</td>";

			out << "<td align=center>";
			out << "USD Rate";
			out << "</td>";

			out << "<td align=center>";
			out << "Price income";
			out << "</td>";
			
			out << "</tr>";



			int idx = 0;
			for(multimap<double, Symbol>::const_iterator it = firstOne.sortedSymbols_m.begin(); it != firstOne.sortedSymbols_m.end(); it++) {

				
				
				if (zero2==idx) {
						out << "<tr height='3px'>";
						out << "<td colspan='5' BGCOLOR='black' BORDERCOLOR='black'>";
						out << "</td>";
						out << "</tr>";
				}

				out << "<tr bgcolor='" << ( it->second.isLong_m ? "limegreen" : "orange" ) << "'>";
				
				
				
				if (CppUtils::contains(changes, it->second.id_m)) {
					// changed
					out << "<td BGCOLOR='buttonface'>";
					beforeIds[it->second.id_m] = idx;
				
				}
				else {
					out << "<td>";
				}

				out << CppUtils::long2String(it->second.id_m);
				out << "</td>"; 
		
				out << "<td>";
		    out << it->second.symbol_m;
				out << "</td>";
		
				out << "<td>";
				out << CppUtils::float2String((float)it->second.income_m, -1, 5 );
				out << "</td>";

				out << "<td>";
				out << CppUtils::float2String((float)it->second.curTranslRate_m, -1, 5 );
				out << "</td>";

				out << "<td>";
				out << CppUtils::float2String((float)it->second.priceDiffIncome_m, -1, 5 );
				out << "</td>";

				out << "</tr>";
	    
				idx++;
			}
			out << "</table>";
			out << "</td>";

			out << "<td width='30px'></td>";

			out << "<td>";
			out << "<table style='border: 1px solid black; font-size:smaller' cellpadding='2' cellspacing='2'>";

			// header
			out << "<tr class=x3>";

			out << "<td align=center>";
			out << "ID";
			out << "</td>"; 

			out << "<td align=center>";
			out << "SYMBOL";
			out << "</td>";

			out << "<td align=center>";
			out << "USD income";
			out << "</td>";

			out << "<td align=center>";
			out << "USD Rate";
			out << "</td>";

			out << "<td align=center>";
			out << "Price income";
			out << "</td>";
			
			out << "</tr>";


			idx = 0;
			for(multimap<double, Symbol>::const_iterator it = secondOne.sortedSymbols_m.begin(); it != secondOne.sortedSymbols_m.end(); it++) {
				
				if (zero2==idx) {
						out << "<tr height='3px'>";
						out << "<td colspan='5' BGCOLOR='black' BORDERCOLOR='black'>";
						out << "</td>";
						out << "</tr>";
				}

				out << "<tr bgcolor='" << ( it->second.isLong_m ? "limegreen" : "orange" ) << "'>";
				
				if (CppUtils::contains(changes, it->second.id_m)) {

					afterIds[it->second.id_m] = idx;

					// changed
					out << "<td BGCOLOR='buttonface'>";

					if (anchor1==idx || anchor2==idx) {
						// changed anchor position
						priority = 0;
						hint += "Anchor change<br>";

						
						small_buffer += " Anchor change ";

						
					}
				
				}
				else {
					out << "<td>";
				}


				out << CppUtils::long2String(it->second.id_m);
				out << "</td>"; 
		
				out << "<td>";
				out << it->second.symbol_m;
				out << "</td>";
		
				out << "<td>";
				out << CppUtils::float2String((float)it->second.income_m, -1, 5 );
				out << "</td>";

				out << "<td>";
				out << CppUtils::float2String((float)it->second.curTranslRate_m, -1, 5 );
				out << "</td>";

				out << "<td>";
				out << CppUtils::float2String((float)it->second.priceDiffIncome_m, -1, 5 );
				out << "</td>";

				out << "</tr>";

				idx++;
			}
			out << "</table>";
			out << "</td>";

			// this is a list of moved indexes

			out << "</tr>";
			out << "</table>";

			// -----------------------------

		
			bool changes = false;
			for(map<int, int>::iterator bit = beforeIds.begin(); bit != beforeIds.end(); bit++) { 

					if (bit->second <= zero1) {
						// begin index is in upper part
						map<int, int>::iterator ait = afterIds.find(bit->first);
						if(ait != afterIds.end()) {
							if (ait->second >= zero2) {
								// end index is at the lower part
								priority = 0;
								changes = true;
								hint += ("Zero position change down of symbol: " + CppUtils::long2String(bit->first) + "<br>");

							}
						}
						else {
							THROW(CppUtils::OperationFailed, "exc_IntegrityViolationsWithIndexes", "ctx_toHTML2Baskets", CppUtils::long2String(bit->first) );
						}

					}
					else if (bit->second >= zero2) {
						map<int, int>::iterator ait = afterIds.find(bit->first);
						if(ait != afterIds.end()) {
							if (ait->second <= zero1) {
								// end index is at the lower part
								changes = true;
								priority = 0;
								hint += ("Zero position change up of symbol: " + CppUtils::long2String(bit->first) + "<br>");

							}
						}
						else {
							THROW(CppUtils::OperationFailed, "exc_IntegrityViolationsWithIndexes", "ctx_toHTML2Baskets", CppUtils::long2String(bit->first) );
						}

					}

					
				
			}

			if (changes) {
				small_buffer += "Zero moved ";
			}
			// -----------------------------
			// other info

			out << "<table width=100% class=x8>";
			
			out << "<tr>";
			out << "<td align=left>";
			out << hint;
			out << "</td>";
			out << "</tr>";
			

			out << "<tr>";
			out << "<td align=left>";
			out << "Detect time: " << CppUtils::getGmtTime(secondOne.lasttime_m);
			out << "</td>";
			out << "</tr>";
			
			out << "<tr>";
			out << "<td align=left>";
			out << "Last symbol: " << secondOne.lastSymbol_m;
			out << "</td>";
			out << "</tr>";

			out << "<tr>";
			out << "<td align=left>";
			out << "Start monitor time: " << CppUtils::getGmtTime(milisecs_time);
			out << "</td>";
			out << "</tr>";
		
			out << "</table>";


			
			
		
			buffer = CppUtils::String(out.str());
}


	
	
	// --------------
	// createor & terminator functions
	AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new AlgorithT101Indic(*algholder);
	}

	void terminator (AlgorithmBase *pthis)
	{
		assert(pthis);
		delete pthis;
	}

	AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

	AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;



	// --------------------------------------
	// 

	AlgorithT101Indic::AlgorithT101Indic( Inqueue::AlgorithmHolder& algHolder): AlgorithmBase(algHolder)
	{
	}

	AlgorithT101Indic::~AlgorithT101Indic()
	{
	}

	void AlgorithT101Indic::onLibraryInitialized(
		Inqueue::AlgBrokerDescriptor const& descriptor, 
		BrkLib::BrokerBase* brokerlib, 
		CppUtils::String const& runName, 
		CppUtils::String const& comment
	)
	{
			isReady_m = false;

			CppUtils::String startMonitoringTimeS = descriptor.initialAlgBrkParams_m.algParams_m.getParameter("START_MONITOR_TIME").getAsString();
			CppUtils::String startMonitorHoursBackS = descriptor.initialAlgBrkParams_m.algParams_m.getParameter("START_MONITOR_HOURS_BACK").getAsString();
	
			if ((startMonitoringTimeS.size() > 0 && startMonitorHoursBackS.size() > 0) ||
				(startMonitoringTimeS.size() <= 0 && startMonitorHoursBackS.size() <= 0))
			{
				THROW(CppUtils::OperationFailed, "exc_InvalidMonotorTimeOrHoursAgo", "ctx_T101libInitialize", "");
			}
  
		  
			milisecs_time_m = -1;
  
			if (startMonitoringTimeS.size() > 0)
				milisecs_time_m = CppUtils::parseFormattedDateTime("d/M/Y h:m", startMonitoringTimeS, 0 );

			if (startMonitorHoursBackS.size() > 0)
				milisecs_time_m = CppUtils::getTime() - atoi(startMonitorHoursBackS.c_str()) * 60 * 60;

		

			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  T101, "The searching GMT time for the first price is: " << CppUtils::getGmtTime(milisecs_time_m));
			ALERTEVENTNC(runName, BrkLib::AP_HIGH, "T101 <'/\\\"\"> initialized,", "<table class=x8><tr><td>T101 was initialized, but not ready to work</td></tr></table>" );
   
			/////
			CppUtils::String executePeriodS = descriptor.initialAlgBrkParams_m.algParams_m.getParameter("EXECUTE_PERIOD").getAsString();
			CppUtils::String executeMultFactorS = descriptor.initialAlgBrkParams_m.algParams_m.getParameter("EXECUTE_MULT_FACTOR").getAsString();
			

			executePeriod_m = Inqueue::AP_Dummy;
			executeMultFactor_m = -1;

			if (executePeriodS.size() > 0) {
				executePeriod_m = Inqueue::aggrPeriodFromString()(executePeriodS.c_str());
				executeMultFactor_m = atoi(executeMultFactorS.c_str());
			}
			 
	}    

	void AlgorithT101Indic::onLibraryFreed()
	{
		ALERTEVENTNC("",BrkLib::AP_HIGH,"T101 deinitialized", "<table class=x8><tr><td>T101 was deinitialized</td></tr></table>");
	}
  
	void AlgorithT101Indic::onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
	{
		//LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  T101, "RT data arrived" ,	
		//	"Rt data:" << rtdata.toString() );

		/*
		Sell GBPUSD 500 <- Anchor in slot 1
		Sell EURGBP 450
		Sell GBPCHF 400
		Sell CHFJPY 350
		Sell AUDJPY 300
		Sell EURJPY 250
		Sell USDCHF 200
		Buy CADJPY -200
		Buy AUDUSD -250
		Buy USDJPY -300
		Buy EURUSD -350
		Buy EURCHF -400
		Buy GBPJPY -450
		Buy USDCAD -500
		*/  

		
		// store last prices to convert
		lastPrices_m[rtdata.getSymbol2()] =  rtdata;


		// is 5 min period
		// before sending that alert make sure we work for 5 minutes basis
		bool is_cache_exec_period = false;

		if (executePeriod_m != Inqueue::AP_Dummy) {
			int cache_exec_period_id = historyaccessor.cacheSymbol(rtdata.getProvider(), rtdata.getSymbol2(), executePeriod_m, executeMultFactor_m);
			if (cache_exec_period_id > 0) {
				HlpStruct::PriceData const& unclosed = historyaccessor.getUnclosedCandle(cache_exec_period_id);
				//is_cache_exec_period = unclosed.isNewPeriod();

				if (unclosed.isNewPeriod()) {
					periodFinishSet_m.insert(rtdata.getSymbol2());

					if (periodFinishSet_m.size() == Symbol::symbolIdentifiers_m.size()) {

						// reaped 14 symbols
						is_cache_exec_period = true;

						// reset that
						periodFinishSet_m.clear();
					}
				}
			};
		}
		else {
			// each tick
			is_cache_exec_period = true;
		}

						

		if (!isReady_m) {

			theFirstSent_m = false;

			// only once must determine open prices
			CppUtils::StringList shortSymbols, longSymbols;
			Symbol::getLongSymbolList(longSymbols);
			Symbol::getShortSymbolList(shortSymbols);

			HlpStruct::PriceData price;
			for( int i = 0; i < shortSymbols.size(); i++) {
				calculateFirstPrice(milisecs_time_m, price, rtdata.getProvider(), shortSymbols[i], historyaccessor );

				if (!price.isValid()) 
					return;

				openCandles_m[shortSymbols[i]] = price;
				//LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  T101, "Have good short first price", rtdata.getProvider() << shortSymbols[i]  << CppUtils::getGmtTime(price.time_m)  << price.open2_m);

			}

			for( int i = 0; i < longSymbols.size(); i++) {
				calculateFirstPrice(milisecs_time_m, price, rtdata.getProvider(), longSymbols[i], historyaccessor );
				
				if (!price.isValid()) 
					return;

				openCandles_m[longSymbols[i]] = price;
				//LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  T101, "Have good long first price", rtdata.getProvider() << longSymbols[i] << " " << CppUtils::getGmtTime(price.time_m) << " " << price.open2_m);

			}

			
			isReady_m = true;
			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  T101, "Ready to work...");
			
			ALERTEVENTNC(runName, BrkLib::AP_HIGH, "T101 actually ready to work", "T101 actually ready to work" );

		}; //

		if (isReady_m) {
			// ready
			// must work each tick here

		
		
			if (openCandles_m.find(rtdata.getSymbol2())==openCandles_m.end()) {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  T101, "Symbol not in the list " << rtdata.getSymbol2());

				return;
			}

			if (rtdata.getRtData().ask_m <0) {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  T101, "Invalid RT data " << rtdata.toString());
				return;
			}

			double income = -1;
			double hist_price = openCandles_m[rtdata.getSymbol2()].open2_m;
			bool is_direct;

			if (hist_price <=0) {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  T101, "Invalid price data " << rtdata.toString() << " " << hist_price << " for symbol " << rtdata.getSymbol2());
				return;
			}

			// only ask prices are used
			if (Symbol::isLong(rtdata.getSymbol2())) {
				// long
				income = rtdata.getRtData().ask_m - hist_price;
			}
			else {
				// short
				income = hist_price - rtdata.getRtData().ask_m;
			}
			
			// income is calc in the amount of denom
			// need to translate this income to USD
			// symbol AUDJPY, income in JPY, thus need to multiply that 

			// kolvo EUR * EURUSD rate  = kol vo USD
			// kolvo EUR / USDEUR rate = kol vo USD

			// dummy income
			
			CppUtils::String symb_num = rtdata.getSymbol2().substr(0,3);
			CppUtils::String symb_denom = rtdata.getSymbol2().substr(3,3);

			CppUtils::String direct_convert_symbol = symb_denom + "USD";
			CppUtils::String reverse_convert_symbol = "USD" + symb_denom;

			double usd_income = -99999999;
			double transl_rate = -99999999;

			if (symb_denom != "USD") {

				// try to find
				is_direct = true;
				map<CppUtils::String, HlpStruct::RtData>::iterator it = lastPrices_m.find(direct_convert_symbol);

				if (it == lastPrices_m.end()) {
					// try to find the reversed symbol
					it = lastPrices_m.find(reverse_convert_symbol);

					if (it == lastPrices_m.end()) {
						LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  T101, "Can't translate to USD income", rtdata.getProvider() << rtdata.getSymbol()  << "price not found for:" << direct_convert_symbol << "or" << reverse_convert_symbol);
						ALERTEVENTNC(runName, BrkLib::AP_HIGH, "Can't translate to USD income, see log", "Can't translate to USD income, see log" );

						return;
					}

					is_direct = false;
				}

				// found!!!!
				
				
			
				if (is_direct) {
					transl_rate = it->second.getRtData().bid_m;
				}
				else {
					transl_rate = 1/it->second.getRtData().bid_m;
				}

				usd_income = income * transl_rate;

				/*
				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  T101, 
					"Translate symbol:  " << it->second.toString());
				*/
			}
			else {
				// no need to translate
				usd_income = income; 
				transl_rate = 1;
			}
			
			// it is storead upon each tick thus containg LAST valid data
			usdIncome_m[rtdata.getSymbol2()] = usd_income;
			income_m[rtdata.getSymbol2()] = income;
			translRates_m[rtdata.getSymbol2()] = transl_rate;

			/*
			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  T101, 
				"For ticker " << rtdata.toString() << " income: " << income << 
				" USD income: " << usd_income << 
				" transl rate: " << transl_rate <<
				" symb_num: " << symb_num <<
				" symb_denom: " << symb_denom <<
				" is direct: " << is_direct <<
				" direct_convert_symbol: " << direct_convert_symbol <<
				" reverse_convert_symbol: " << reverse_convert_symbol);
			*/

			  

			
			// ---------------
			// this must happen 1 time per our period!
			if (is_cache_exec_period) {

				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  T101, "For ticker period switched, time " << CppUtils::getGmtTime2(rtdata.getTime()));

				
				int event;
			
				BasketEntry newEntry;
				if (basketList_m.size() > 0 ) {
					// list is not empty
					BasketEntry& topEntry = *basketList_m.rbegin();

					// recover from top
					newEntry = topEntry;

					for(map<CppUtils::String, double>::const_iterator it = usdIncome_m.begin(); it != usdIncome_m.end(); it++) {
						CppUtils::String const& symbol_i = it->first;

						newEntry.basket_m.updateNewSymbol(symbol_i, it->second, rtdata.getTime(), translRates_m[symbol_i], income_m[symbol_i] );
					}

					if (!newEntry.basket_m.isNormalSize()) {
						// store as top
						topEntry = newEntry;
					}
					else {
						// compare with old entry
						newEntry.basket_m.compare(topEntry.basket_m, event, newEntry.changes_m);

						if (!theFirstSent_m) {

							// sent the once the first one
							sendAlert(rtdata.getTime(), newEntry, runName);

							theFirstSent_m = true;
						}

						if (event & BasketStructure::E_Changed) {

						
							sendAlert(rtdata.getTime(), "", newEntry, topEntry, runName);

							// significant changes happened
							basketList_m.push_back(newEntry);

						}
						else { 
							// no changes
							//sendAlert(rtdata.getTime(), "NO CHANGE", newEntry, topEntry, runName);

							topEntry = newEntry;
						}

					}

				}
				else {

					// no previous data at all!
					for(map<CppUtils::String, double>::const_iterator it = usdIncome_m.begin(); it != usdIncome_m.end(); it++) {
						CppUtils::String const& symbol_i = it->first;
						newEntry.basket_m.updateNewSymbol(symbol_i, it->second, rtdata.getTime(), translRates_m[symbol_i], income_m[symbol_i] );
					}

				
					// just store
					basketList_m.push_back(newEntry);
				}
				

				// remove unnecessary
				while (basketList_m.size() > 10) {
					basketList_m.erase(basketList_m.begin());
				}


			}; // end if period
		  

		}; // is Ready


	}

	void AlgorithT101Indic::onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
	{
			
	}

	void AlgorithT101Indic::onLevel1DataArrived(
			HlpStruct::RtLevel1Data const& level1data,
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
	{
			
	}

	void AlgorithT101Indic::onBrokerResponseArrived(
			BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
		)
	{
	}

	bool AlgorithT101Indic::onEventArrived(HlpStruct::EventData const &eventdata, Inqueue::ContextAccessor& contextaccessor, HlpStruct::EventData& response)
	{
		return false;
	}

	bool AlgorithT101Indic::onThreadStarted(Inqueue::ContextAccessor& contextaccessor) {
		return false;
	};
	
	CppUtils::String const& AlgorithT101Indic::getName() const
	{
		static const CppUtils::String name("t101 indicator algorithm");
		return name;
	}

	// ---------------------------------

	void AlgorithT101Indic::calculateFirstPrice(
		double const& timestamp, 
		HlpStruct::PriceData& pdata_out, 
		CppUtils::String const& provider,
		CppUtils::String const& symbol, 
		Inqueue::ContextAccessor& historyaccessor)
	{
		
			pdata_out.clear();
			int cached_id = historyaccessor.cacheSymbol(provider, symbol, Inqueue::AP_Minute, 1);
			if (cached_id <0) {
					LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  T101, "Can't cache symbol " << symbol);
					return;
			}

			Inqueue::DataPtr ptr;
			bool found = historyaccessor.dataTimeFind(timestamp , cached_id, ptr );

			if (!found) {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  T101, "Can't find past price data " << symbol << CppUtils::getGmtTime(timestamp));
				return;
			}

			pdata_out = historyaccessor.getCurrentPriceData(cached_id, ptr );

			// ask
			if (pdata_out.open2_m <=0 ) {
					LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  T101, "Invalid past price data " << symbol << " " << CppUtils::getGmtTime(timestamp));
					return;
			}

		
	}

	void AlgorithT101Indic::sendAlert(double const rt_time, BasketEntry const& newEntry, CppUtils::String const& runName)
	{
		// medium priority
		CppUtils::String buffer = BasketStructure::toHTML2Baskets(newEntry.basket_m, milisecs_time_m);
		ALERTEVENTNC(runName, BrkLib::AP_MEDIUM , CppUtils::getGmtTime2(rt_time) + " - Initial picture", buffer);

	}  

	void AlgorithT101Indic::sendAlert(
		double const rt_time, 
		CppUtils::String const& strtag,
		BasketEntry const& newEntry, 
		BasketEntry const& oldEntry, 
		CppUtils::String const& runName
	)
	{
		
		int priority;
		CppUtils::String buffer, small_buffer;
		BasketStructure::toHTML2Baskets(oldEntry.basket_m, newEntry.basket_m, newEntry.changes_m, priority, buffer , small_buffer, milisecs_time_m);
		ALERTEVENTNC(runName, priority , CppUtils::getGmtTime2(rt_time) + " - " + small_buffer +" - " + strtag, buffer);

	}
}; 