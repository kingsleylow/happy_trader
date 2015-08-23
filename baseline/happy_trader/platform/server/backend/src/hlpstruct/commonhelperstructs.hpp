#ifndef _HLPSTRUCT_COMMONHELPERSTRUCTS_INCLUDED
#define _HLPSTRUCT_COMMONHELPERSTRUCTS_INCLUDED

#include "hlpstructdefs.hpp"
#include "xmlparameter.hpp"
#include "xmleventdata.hpp"


namespace HlpStruct {

class HLPSTRUCT_EXP ParseDateTimeParameter
{
public:

	ParseDateTimeParameter();
	void setUp(CppUtils::String const& paramValue );
	double const& getSecs() const;
	
private:
	double secs_m;
	
};

class HLPSTRUCT_EXP WorkTimeHolder
{
public:
	WorkTimeHolder();
	
	void setUp(HlpStruct::XmlParameter const& tradeParam, char const* begin_time_param, char const* end_time_param);
	
	bool isWithinTime(double const ttime) const;
	
private:

	ParseDateTimeParameter beginTimeParam_m;
	
	ParseDateTimeParameter endTimeParam_m;


};

//  ----------------------------------

// structure contains trading parameters
// which are either initially passed or passed at every onDataArrived(...) call
struct HLPSTRUCT_EXP TradingParameters
{
		HlpStruct::XmlParameter brokerParams_m;

		HlpStruct::XmlParameter brokerParams2_m;

		HlpStruct::XmlParameter algParams_m;

		static CppUtils::String dumpParameter(HlpStruct::XmlParameter const& parameter );
		
};

typedef vector<TradingParameters> TradingParametersList;


// helper class to return parameters and check for existance
template<typename T>
class ParametersLoader;


template<>
class ParametersLoader<long> {
	public:
	

	long  operator() (HlpStruct::XmlParameter const& tradeParam, char const* paramName, bool const required = false )
	{
		
		if (tradeParam.isKeyExists(paramName)) {
			return atoi(tradeParam.getParameter(paramName).getAsString().c_str());
		}
		else {
			if (required) {
				THROW(CppUtils::OperationFailed, "exc_LongParameterNotExist", "ctx_ParametersLoader", paramName );
			}
			else
				return -1;
		}
	
		
	}
};

template<>
class ParametersLoader<CppUtils::String> {
	public:
	

	CppUtils::String  operator() (HlpStruct::XmlParameter const& tradeParam, char const* paramName, bool const required = false )
	{
		
		if (tradeParam.isKeyExists(paramName)) {
			return tradeParam.getParameter(paramName).getAsString();
		}
		else {
			if (required) {
				THROW(CppUtils::OperationFailed, "exc_StringParameterNotExist", "ctx_ParametersLoader", paramName );
			}
			else
				return "";
		}
	
		
	}
};

template<>
class ParametersLoader<double> {
	public:
	

	double  operator() (HlpStruct::XmlParameter const& tradeParam, char const* paramName, bool const required = false )
	{
		
		if (tradeParam.isKeyExists(paramName)) {
			return atof(tradeParam.getParameter(paramName).getAsString().c_str());
		}
		else {
			if (required) {
				THROW(CppUtils::OperationFailed, "exc_DoubleParameterNotExist", "ctx_ParametersLoader", paramName );
			}
			else
				return -1.0;
		}
	
		
	}
};

template<>
class ParametersLoader<bool> {
public:
	
	bool  operator() (HlpStruct::XmlParameter const& tradeParam, char const* paramName, bool const required = false )
	{
		
		if (tradeParam.isKeyExists(paramName)) {
			return _stricmp(tradeParam.getParameter(paramName).getAsString().c_str(), "true")==0;
		}
		else {
			if (required) {
				THROW(CppUtils::OperationFailed, "exc_BoolParameterNotExist", "ctx_ParametersLoader", paramName );
			}
			else
				return false;
		}
	
		
	}
};

// ---------------------------------


// some usefull macro
// required params
#define GET_ALG_TRADE_INT(TP, NAME)	\
	HlpStruct::ParametersLoader<long>()( TP.algParams_m, NAME, true )

#define GET_BRK_TRADE_INT(TP, NAME)	\
	HlpStruct::ParametersLoader<long>()( TP.brokerParams_m ,NAME, true )
	
#define GET_ALG_TRADE_STRING(TP, NAME)	\
	HlpStruct::ParametersLoader<CppUtils::String>()( TP.algParams_m, NAME, true )

#define GET_BRK_TRADE_STRING(TP, NAME)	\
	HlpStruct::ParametersLoader<CppUtils::String>()( TP.brokerParams_m, NAME, true )

#define GET_ALG_TRADE_BOOL(TP, NAME)	\
	HlpStruct::ParametersLoader<bool>()( TP.algParams_m, NAME, true )

#define GET_BRK_TRADE_BOOL(TP, NAME)	\
	HlpStruct::ParametersLoader<bool>()( TP.brokerParams_m, NAME, true )

#define GET_ALG_TRADE_DOUBLE(TP, NAME)	\
	HlpStruct::ParametersLoader<double>()( TP.algParams_m, NAME, true )

#define GET_BRK_TRADE_DOUBLE(TP, NAME)	\
	HlpStruct::ParametersLoader<double>()( TP.brokerParams_m, NAME, true )


// not required params
// ---------------------------------------
#define GET_ALG_TRADE_INT_NR(TP, NAME)	\
	HlpStruct::ParametersLoader<long>()( TP.algParams_m, NAME, false );

#define GET_BRK_TRADE_INT_NR(TP, NAME)	\
	HlpStruct::ParametersLoader<long>()( TP.brokerParams_m ,NAME, false );
	
#define GET_ALG_TRADE_STRING_NR(TP, NAME)	\
	HlpStruct::ParametersLoader<CppUtils::String>()( TP.algParams_m, NAME, false );

#define GET_BRK_TRADE_STRING_NR(TP, NAME)	\
	HlpStruct::ParametersLoader<CppUtils::String>()( TP.brokerParams_m, NAME, false );

#define GET_ALG_TRADE_BOOL_NR(TP, NAME)	\
	HlpStruct::ParametersLoader<bool>()( TP.algParams_m, NAME, false );

#define GET_BRK_TRADE_BOOL_NR(TP, NAME)	\
	HlpStruct::ParametersLoader<bool>()( TP.brokerParams_m, NAME, false );

#define GET_ALG_TRADE_DOUBLE_NR(TP, NAME)	\
	HlpStruct::ParametersLoader<double>()( TP.algParams_m, NAME, false );

#define GET_BRK_TRADE_DOUBLE_NR(TP, NAME)	\
	HlpStruct::ParametersLoader<double>()( TP.brokerParams_m, NAME, false );




// ---------------------------------------


	// this describes the pair to be loaded
	struct HLPSTRUCT_EXP AlgBrokerDescriptor {
		
		// initial trading parameters
		TradingParameters initialAlgBrkParams_m;

		CppUtils::String algLib_m;

		CppUtils::String brokerLib_m;

		CppUtils::String brokerLib2_m;

		CppUtils::String name_m;

		int priorityId_m;

		CppUtils::String dump( ) const;
		
	};

	typedef vector<AlgBrokerDescriptor> AlgBrokerDescriptorList;


	// ------------------------------------------

	// this is the pair of provider - symbol
	struct HLPSTRUCT_EXP ProviderSymbolPair 
	{
		ProviderSymbolPair();
		
		ProviderSymbolPair(CppUtils::String const& provider, CppUtils::String const& symbol);
		
		CppUtils::String provider_m;

		CppUtils::String symbol_m;
	};

	typedef vector<ProviderSymbolPair> ProviderSymbolList;


	// -------------------------------

class HLPSTRUCT_EXP svector
{
public:
	inline long size() const
	{
		return container_m.size();
	}

	svector& operator << ( CppUtils::String const& _Val );
	
	svector& operator << ( const long _Val );
	
	svector& operator << ( const int _Val );
	
	svector& operator << ( const double _Val );
	
	svector& operator << ( const char* _Val );
		
	svector& operator << ( const bool _Val );
		
	svector& operator << ( CppUtils::StringList const& _Val );
	
	svector& operator << ( HlpStruct::EventData const& _Val );
	
	inline CppUtils::String & getContent()
	{
		return container_m;
	}

	inline CppUtils::String const& getContent() const
	{
		return (CppUtils::String const&)container_m;
	}

private:
	CppUtils::String container_m;


};


// -----------------------------------------------------

///////////////////////////////////////
	// enumeration of aggregation types
	enum AggregationPeriods {
		AP_Dummy	=	0,
		AP_NoAggr	= 1,
		AP_Minute = 2,
		AP_Hour		=	3,
		AP_Day		=	4,
		AP_Week		=	5,
		AP_Month	= 6,
		AP_Year		=	7
	};

	// convert this from string
	class HLPSTRUCT_EXP aggrPeriodFromString {
	public:
		AggregationPeriods operator ()(char const* aggrPeriodStr);
	};

	// convert this from string
	class HLPSTRUCT_EXP aggrPeriodToString {
	public:
		CppUtils::String operator ()(AggregationPeriods const aggrper);
	};


	/////////////////////////////////////
	// enumaration if tick generator

	enum TickGenerator {
		TG_Dummy			= 0,
		TG_Uniform		= 1, // uniform distribution
		TG_CloseOnly	= 2,
		TG_OHLC =				3 // just 4 ticks
	};

	// convert this from string
	class HLPSTRUCT_EXP genTicksFromString {
	public:
		TickGenerator operator ()(char const* genTickStr);
	};

	// convert this from string
	class HLPSTRUCT_EXP genTicksToString {
	public:

		CppUtils::String operator ()(TickGenerator const tg);
		
	};

	////////////////////////////////////
	// tick usage for tick generator
	enum TickUsage {
			PC_USE_DUMMY = 0,
			PC_USE_BID = 1,
			PC_USE_ASK = 2,
			PC_USE_BOTH = 3
		};

	class HLPSTRUCT_EXP tickUsageFromString {
	public:
		TickUsage  operator() (char const* tickUsage);
	};

	class HLPSTRUCT_EXP tickUsageToString {
	public:
		CppUtils::String  operator() (TickUsage const tu);
	};
	

	/**
	* This is what describes simulation process
	*/
	
	
// ----------------------------------
// base class for solver
template<typename CONTEXT>
class CalculatorBase {
		public:
			// ctor & dror
			CalculatorBase()
			{
			}
		 
			virtual ~CalculatorBase()
			{
			}

			virtual void onRtData(HlpStruct::RtData const& rtdata, CONTEXT* ctx) = 0;

			virtual void onLevel2Data(HlpStruct::RtLevel2Data const& level2data, CONTEXT* ctx) = 0;

			virtual void onThreadStarted(CONTEXT* ctx) = 0;

			virtual void onThreadFinished(CONTEXT* ctx) = 0;

			virtual void initialized() = 0;

			virtual void deinitialized() = 0;


		

	};

	
	
};  // end of ns

#endif