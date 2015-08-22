#include "commonhelperstructs.hpp"
#include "../inqueue/commontypes.hpp"
#include "../math/mathengines.hpp"

namespace HlpStruct {
	
	ParseDateTimeParameter::ParseDateTimeParameter():
		secs_m(-1.0)
	{
	}

	void ParseDateTimeParameter::setUp(CppUtils::String const& paramValue )
	{
		CppUtils::StringList reslst;
		CppUtils::tokenize(paramValue,reslst,":");

		int hours, minutes;
		if (reslst.size() ==2) {
			hours = atoi(reslst[0].c_str());
			minutes = atoi(reslst[1].c_str());
		}
		else
			THROW(CppUtils::OperationFailed, "exc_InvalidDateTimeString", "ctx_parseDateTimeParameter", paramValue);

		secs_m = (double)hours * 60.0 * 60.0 + (double)minutes*60.0;
	}

	double const& ParseDateTimeParameter::getSecs() const
	{
		return secs_m;
	}


	// --------------------------------

	WorkTimeHolder::WorkTimeHolder()
	{
	}

	void WorkTimeHolder::setUp(HlpStruct::XmlParameter const& tradeParam, char const* begin_time_param, char const* end_time_param)
	{
		HlpStruct::ParametersLoader<CppUtils::String> strPar;
		beginTimeParam_m.setUp(strPar(tradeParam, begin_time_param, true));
		endTimeParam_m.setUp(strPar(tradeParam, end_time_param, true));

	}

	bool WorkTimeHolder::isWithinTime(double const ttime) const
	{
		double diffFromDayBegin = ttime - CppUtils::roundToBeginningOfTheDay(ttime);

		if (diffFromDayBegin < beginTimeParam_m.getSecs() || diffFromDayBegin > endTimeParam_m.getSecs())
			return false;

		return true;
	}

	// ------------------------------------

	CppUtils::String TradingParameters::dumpParameter(HlpStruct::XmlParameter const& parameter )
	{
			CppUtils::String result;

			CppUtils::StringList keys;
			parameter.getValueKeys(keys);
			
			result += "Parameters as follows:\n";

			for(int i = 0; i < keys.size(); i++) {
				result += keys[i] + " = ";
				if (parameter.getParameter(keys[i]).getType() == CppUtils::Value::VT_String) {
					result += "[" + parameter.getParameter(keys[i]).getAsString() + "]";
				}
				else {
					result += "[ N/A ]";
				}

				result += "\n";
			}


			return result;
	}

	// ---------------------------------------------

	CppUtils::String AlgBrokerDescriptor::dump( ) const
	{
			CppUtils::String result;
			result += "\n--------------------------------------\n";
			result += "Name: [" + name_m + "]\n";
			result += "Priority: [" + CppUtils::long2String(priorityId_m) + "]\n";
			result += "Algorithm lib: [" + algLib_m + "]\n";
			result += "Algorithm Parameters: \n";
			result += TradingParameters::dumpParameter(	initialAlgBrkParams_m.algParams_m );
			
			result += "Broker lib: [" + brokerLib_m + "]\n";
			result += "Broker Parameters: \n";
			result += TradingParameters::dumpParameter(	initialAlgBrkParams_m.brokerParams_m );

			result += "Broker 2 lib: [" + brokerLib2_m + "]\n";
			result += "Broker 2 Parameters: \n";
			result += TradingParameters::dumpParameter(	initialAlgBrkParams_m.brokerParams2_m );
			result += "\n--------------------------------------\n";
			
			
			return result;

	}

	// ---------------------------------

	ProviderSymbolPair::ProviderSymbolPair()
	{
	}

	ProviderSymbolPair::ProviderSymbolPair(CppUtils::String const& provider, CppUtils::String const& symbol):
		provider_m(provider),
		symbol_m(symbol)
	{
	}

	// ------------------------------


	svector& svector::operator << ( CppUtils::String const& _Val )
	{
		//container_m.push_back(CppUtils::prepareValidXml(_Val));
		container_m.append("\r\n");
		container_m.append(_Val);
		return *this;
	};


	svector& svector::operator << ( const long _Val )
	{
		container_m.append("\r\n");
		container_m.append(CppUtils::long2String(_Val));
		return *this;
	};

	svector& svector::operator << ( const int _Val )
	{
		container_m.append("\r\n");
		container_m.append(CppUtils::long2String((long)_Val));
		return *this;
	};

	svector& svector::operator << ( const double _Val )
	{
		container_m.append("\r\n");
		container_m.append(CppUtils::double2String(_Val));
		return *this;
	};

	svector& svector::operator << ( const char* _Val )
	{
		container_m.append("\r\n");
		container_m.append(_Val);
		return *this;
	};
	
	svector& svector::operator << ( const bool _Val )
	{
		container_m.append("\r\n");
		container_m.append(_Val ? "true" : "false");
		return *this;
	};

	
	svector& svector::operator << ( CppUtils::StringList const& _Val )
	{

		for(int i = 0; i < _Val.size(); i++) {
			container_m.append("\r\n");
			container_m.append(_Val[i]);
		}
		return *this;
	};
	

	svector& svector::operator << ( HlpStruct::EventData const& _Val )
	{
		CppUtils::String xmlcontent;	
		HlpStruct::XmlParameter xmlparam;		
		HlpStruct::convertEventToXmlParameter(_Val, xmlparam);	
		HlpStruct::serializeXmlParameter(xmlparam, xmlcontent);	

		//container_m.push_back(CppUtils::prepareValidXml(xmlcontent));
		container_m.append("\r\n");
		container_m.append(xmlcontent);

		return *this;
	}

	// ----------------------------------------------------------
	AggregationPeriods aggrPeriodFromString::operator ()(char const* aggrPeriodStr)
		{
			if (_stricmp(aggrPeriodStr, "AP_Dummy")==0) return AP_Dummy;
			if (_stricmp(aggrPeriodStr, "AP_NoAggr")==0) return AP_NoAggr;
			if (_stricmp(aggrPeriodStr, "AP_Minute")==0) return AP_Minute;
			if (_stricmp(aggrPeriodStr, "AP_Hour")==0) return AP_Hour;
			if (_stricmp(aggrPeriodStr, "AP_Day")==0) return AP_Day;
			if (_stricmp(aggrPeriodStr, "AP_Week")==0) return AP_Week;
			if (_stricmp(aggrPeriodStr, "AP_Month")==0) return AP_Month;
			if (_stricmp(aggrPeriodStr, "AP_Year")==0) return AP_Year;

			return AP_Dummy;
		};

	// ----------------------------------------------------------

	CppUtils::String aggrPeriodToString::operator ()(AggregationPeriods const aggrper)
	{
			if (aggrper==AP_Dummy) return "AP_Dummy";
			if (aggrper==AP_NoAggr) return "AP_NoAggr";
			if (aggrper==AP_Minute) return "AP_Minute";
			if (aggrper==AP_Hour) return "AP_Hour";
			if (aggrper==AP_Day) return "AP_Day";
			if (aggrper==AP_Week) return "AP_Week";
			if (aggrper==AP_Month) return "AP_Month";
			if (aggrper==AP_Year) return "AP_Year";

			return "";
	};

	// ----------------------------------------------------------
	TickGenerator genTicksFromString::operator ()(char const* genTickStr)
	{
			if (_stricmp(genTickStr, "TG_Uniform")==0) return TG_Uniform;
			if (_stricmp(genTickStr, "TG_CloseOnly")==0) return TG_CloseOnly;
			if (_stricmp(genTickStr, "TG_Dummy")==0) return TG_Dummy;
		
			return TG_Dummy;
	};

	// ----------------------------------------------------------
	CppUtils::String genTicksToString::operator ()(TickGenerator const tg)
	{
			if (tg==TG_Uniform) return "TG_Uniform";
			if (tg==TG_Dummy) return "TG_Dummy";
			if (tg==TG_CloseOnly) return "TG_CloseOnly";
		
			return "";
	};

	// ----------------------------------------------------------
	TickUsage  tickUsageFromString::operator() (char const* tickUsage)
	{
			if (tickUsage==NULL) return PC_USE_DUMMY;
			if (_stricmp(tickUsage, "BOTH")==0) return PC_USE_BOTH;
			if (_stricmp(tickUsage, "BID")==0) return PC_USE_BID;
			if (_stricmp(tickUsage, "ASK")==0) return PC_USE_ASK;

			return PC_USE_DUMMY;
	}

	// ----------------------------------------------------------
	CppUtils::String  tickUsageToString::operator() (TickUsage const tu)
	{
			if (tu==PC_USE_DUMMY) return "PC_USE_DUMMY";
			if (tu==PC_USE_BID) return "PC_USE_BID";
			if (tu==PC_USE_ASK) return "PC_USE_ASK";
			if (tu==PC_USE_BOTH) return "PC_USE_BOTH";

			return "";
	}

	// ----------------------------------------------------------
	


	// ----------------------------------------------------------



};  // end of ns