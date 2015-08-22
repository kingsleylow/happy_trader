#include "xmleventdata.hpp"

namespace HlpStruct {
	
	// initialize availble evnents map
	vector<EventData::EType> EventData::availableEvents_m;

	


	vector<EventData::EType> const& EventData::getAllAllowedEvents()
	{
		if (availableEvents_m.size()==0) {
			availableEvents_m.push_back(EventData::ET_CommonLog);
			availableEvents_m.push_back(EventData::ET_AlgorithmEvent);
			availableEvents_m.push_back(EventData::ET_AlgorithmBrokerEventResp_FromAlgorithm);
			availableEvents_m.push_back(EventData::ET_AlgorithmBrokerEventResp_FromBroker);
			
			availableEvents_m.push_back(EventData::ET_BrokerResponseEventResp);
			availableEvents_m.push_back(EventData::ET_RtProviderTicker);
			availableEvents_m.push_back(EventData::ET_InternalImportResultsLog);
			availableEvents_m.push_back(EventData::ET_InternalExportResultsLog);
			availableEvents_m.push_back(EventData::ET_BrokerOrderEvent);
			availableEvents_m.push_back(EventData::ET_BrokerSessionEvent);
			availableEvents_m.push_back(EventData::ET_AlgorithmAlert);
			availableEvents_m.push_back(EventData::ET_Level2Data);
			availableEvents_m.push_back(EventData::ET_Level1Data);
			availableEvents_m.push_back(EventData::ET_DrawableObject);
			availableEvents_m.push_back(EventData::ET_CustomEvent);
			availableEvents_m.push_back(EventData::ET_RtProviderSynchronizationEvent);
			availableEvents_m.push_back(EventData::ET_AlertPluginSynchronizationEvent);

			availableEvents_m.push_back(EventData::ET_BrokerEvent);
			availableEvents_m.push_back(EventData::ET_Notification);
	
		}
		
		return availableEvents_m;
	}

	CppUtils::String EventData::getEventTypeName(EType const ll) {
			if (ll==ET_CommonLog) 
				return "ET_CommonLog";
			else if (ll==ET_AlgorithmEvent)
				return "ET_AlgorithmEvent";
			else if (ll==ET_AlgorithmBrokerEventResp_FromAlgorithm) 
				return "ET_AlgorithmBrokerEventResp_FromAlgorithm";
			else if (ll==ET_AlgorithmBrokerEventResp_FromBroker) 
				return "ET_AlgorithmBrokerEventResp_FromBroker";
			else if (ll==ET_BrokerResponseEventResp) 
				return "ET_BrokerResponseEventResp";
			else if (ll==ET_RtProviderTicker) 
				return "ET_RtProviderTicker";
			else if (ll==ET_InternalImportResultsLog)
				return "ET_InternalImportResultsLog";
			else if (ll==ET_InternalExportResultsLog) 
				return "ET_InternalExportResultsLog";
			else if (ll==ET_BrokerOrderEvent) 
				return "ET_BrokerOrderEvent";
			else if (ll==ET_BrokerSessionEvent) 
				return "ET_BrokerSessionEvent";
			else if (ll==ET_AlgorithmAlert) 
				return "ET_AlgorithmAlert";
			else if (ll==ET_Level2Data) 
				return "ET_Level2Data";
			else if (ll==ET_Level1Data) 
				return "ET_Level1Data";
			else if (ll==ET_DrawableObject) 
				return "ET_DrawableObject";
			else if (ll==ET_CustomEvent) 
				return "ET_CustomEvent";
			else if (ll==ET_RtProviderSynchronizationEvent) 
				return "ET_RtProviderSynchronizationEvent";
			else if (ll==ET_AlertPluginSynchronizationEvent) 
				return "ET_AlertPluginSynchronizationEvent";
			else if (ll==ET_BrokerEvent)
				return "ET_BrokerEvent";
			else if (ll==ET_Notification)
				return "ET_Notification";
			else
				THROW(CppUtils::OperationFailed, "exc_InvalidEventType", "ctx_getEventTypeName", (long)ll);
			
			
		}

	EventData & EventData::operator = (	EventData const &	rhs)
	{
		clear();

		// Copy data
		dataType_m = rhs.dataType_m;
		eventType_m = rhs.eventType_m;
		eventDate_m = rhs.eventDate_m;
		uid_m = rhs.uid_m;
		parentUid_m = rhs.parentUid_m;
		eventSource_m = rhs.eventSource_m;


		switch( dataType_m ) {
		case DT_Log:
			data_m.commonLog_m = new CommonLog(*rhs.data_m.commonLog_m);
			break;

		case DT_String:
			data_m.string_m = new CppUtils::String(*rhs.data_m.string_m);
			break;

		case DT_StringList:
			data_m.stringList_m = new CppUtils::StringList(*rhs.data_m.stringList_m);
			break;

		case DT_StringMap:
			data_m.stringMap_m = new CppUtils::StringMap(*rhs.data_m.stringMap_m);
			break;


		case DT_Ticker:
			data_m.ticker_m = new RtData(*rhs.data_m.ticker_m);
			break;

		case DT_TickerList:
			data_m.tickerList_m = new RtDataList(*rhs.data_m.tickerList_m);
			break;

		case DT_CommonXml:
			data_m.commonXml_m = new XmlParameter(*rhs.data_m.commonXml_m);
			break;
		case DT_Level2Data:
			data_m.level2Data_m = new RtLevel2Data(*rhs.data_m.level2Data_m);
			break;

		case DT_Level1Data:
			data_m.level1Data_m = new RtLevel1Data(*rhs.data_m.level1Data_m);
			break;

		case DT_DrawableObject:
			data_m.drawableObj_m = new RtDrawableObj(*rhs.data_m.drawableObj_m);
			break;
		
		}

		// Done
		return *this;
	};

		
	bool EventData::operator == (	EventData const &	rhs) const
	{
		if( dataType_m != rhs.dataType_m )
			return false;

		switch(dataType_m) {
		case DT_Log:
			HTASSERT(false);
			return false;
			break;

		case DT_String:
			return *data_m.string_m == *rhs.data_m.string_m;
			break;

		case DT_StringList:
			return *data_m.stringList_m == *rhs.data_m.stringList_m;
			break;

		case DT_StringMap:
			return *data_m.stringMap_m == *rhs.data_m.stringMap_m;
			break;


		case DT_Ticker:
			HTASSERT(false);
			return false;
			break;

		case DT_TickerList:
			HTASSERT(false);
			return false;
			break;

		case DT_CommonXml:
			return *data_m.commonXml_m == *rhs.data_m.commonXml_m;
			break;
		
		case DT_Level2Data:
			HTASSERT(false);
			return false;
			break;

		case DT_Level1Data:
			HTASSERT(false);
			return false;
			break;

		case DT_DrawableObject:
			HTASSERT(false);
			return false;
			break;
		
		}

		return false;

	};

	
		
	EventData::EventData (	):
	dataType_m(DT_Dummy),
	eventDate_m(-1),
	eventType_m(ET_Dummy)
	{
			//count(true);
	};

	
	EventData::EventData (	EventData const &	rhs):
	dataType_m(DT_Dummy),
	eventDate_m(-1),
	eventType_m(ET_Dummy)
	{ 
		// Use assignment operator
		*this = rhs;
		
	};

		
	EventData::~EventData(	)
	{
		clear();
		
	};

		
	void EventData::clear()
	{
		switch(dataType_m) {
			case DT_Log:
				HTASSERT(data_m.commonLog_m);
				delete data_m.commonLog_m;
				break;
			case DT_String:
				HTASSERT(data_m.string_m);
				delete data_m.string_m;
				break;
			case DT_StringList:
				HTASSERT(data_m.stringList_m);
				delete data_m.stringList_m;
				break;
			case DT_StringMap:
				HTASSERT(data_m.stringMap_m);
				delete data_m.stringMap_m;
				break;
			case DT_Ticker:
				HTASSERT(data_m.ticker_m);
				delete data_m.ticker_m;
				break;
			case DT_TickerList:
				HTASSERT(data_m.tickerList_m);
				delete data_m.tickerList_m;
				break;
			case DT_CommonXml:
				HTASSERT(data_m.commonXml_m);
				delete data_m.commonXml_m;
				break;
			case DT_Level2Data:
				HTASSERT(data_m.level2Data_m);
				delete data_m.level2Data_m;
				break;
			case DT_Level1Data:
				HTASSERT(data_m.level1Data_m);
				delete data_m.level1Data_m;
				break;

			case DT_DrawableObject:
				HTASSERT(data_m.drawableObj_m);
				delete data_m.drawableObj_m;
				break;
		};

		data_m.commonLog_m=NULL;
		dataType_m = DT_Dummy;
		eventDate_m = -1;
		uid_m.invalidate();
		parentUid_m.invalidate();
		eventSource_m.clear();
		
	};

	void EventData::setType(EventData::DType const type)
	{
		// New type?
	//
	if( type == dataType_m )
		// No; bail out
		return;


	// Get rid of previous data
	//
	clear();


	// List new type
	//
	dataType_m = type;

	// Generate UID
	//
	uid_m.generate();


	// Allocate data if needed
	//
	switch( dataType_m ) {
		case DT_Log:
				
				data_m.commonLog_m = new CommonLog;
				break;
			case DT_String:
				
				data_m.string_m = new CppUtils::String;
				break;
			case DT_StringList:
				
				data_m.stringList_m = new CppUtils::StringList;
				break;
			case DT_StringMap:
				
				data_m.stringMap_m = new CppUtils::StringMap;
				break;
			case DT_Ticker:
				
				data_m.ticker_m = new RtData;
				break;
			case DT_TickerList:
				
				data_m.tickerList_m = new RtDataList;
				break;
			case DT_CommonXml:
				
				data_m.commonXml_m = new XmlParameter;
				break;
			case DT_Level2Data:
				data_m.level2Data_m = new RtLevel2Data;
				break;
			case DT_Level1Data:
				data_m.level1Data_m = new RtLevel1Data;
				break;
			case DT_DrawableObject:
				data_m.drawableObj_m = new RtDrawableObj;
				break;
		}
	};

	

			
	
	void EventData::setParentUid(CppUtils::Uid const& puid)
	{
		HTASSERT(puid.isValid());
		parentUid_m = puid;
	}

	
	void EventData::setSource(CppUtils::String const& source)
	{
		eventSource_m = source;
	}
		
	CommonLog& EventData::getAsLog(bool setType )
	{
		if( setType ) this->setType( DT_Log );
		HTASSERT( dataType_m == DT_Log );
		HTASSERT( data_m.commonLog_m != NULL );

		return *data_m.commonLog_m;
	};

	CommonLog const & EventData::getAsLog() const 
	{
		HTASSERT( dataType_m == DT_Log );
		return *data_m.commonLog_m;
	};

	CppUtils::String& EventData::getAsString(bool setType )
	{
		if( setType ) this->setType( DT_String );
		HTASSERT( dataType_m == DT_String );
		HTASSERT( data_m.string_m != NULL );

		return *data_m.string_m;
	};

	CppUtils::String const & EventData::getAsString() const
	{
		HTASSERT( dataType_m == DT_String);
		return *data_m.string_m;
	};

	CppUtils::StringList& EventData::getAsStringList(bool setType )
	{
		if( setType ) this->setType( DT_StringList );
		HTASSERT( dataType_m == DT_StringList );
		HTASSERT( data_m.stringList_m != NULL );

		return *data_m.stringList_m;
	};

	CppUtils::StringList const & EventData::getAsStringList() const
	{
		HTASSERT( dataType_m == DT_StringList);
		return *data_m.stringList_m;
	};

	CppUtils::StringMap& EventData::getAsStringMap(bool setType )
	{
		if( setType ) this->setType( DT_StringMap );
		HTASSERT( dataType_m == DT_StringMap );
		HTASSERT( data_m.stringMap_m != NULL );

		return *data_m.stringMap_m;
	};

	CppUtils::StringMap const & EventData::getAsStringMap() const
	{
		HTASSERT( dataType_m == DT_StringMap);
		return *data_m.stringMap_m;
	};

	RtData& EventData::getAsRtData(bool setType )
	{
			
		if( setType ) this->setType( DT_Ticker );
		HTASSERT( dataType_m == DT_Ticker );
		HTASSERT( data_m.ticker_m != NULL );

		return *data_m.ticker_m;
	};

	RtData const & EventData::getAsRtData() const
	{
		HTASSERT( dataType_m == DT_Ticker);
		return *data_m.ticker_m;
	};

	RtDataList& EventData::getAsRtDataList(bool setType )
	{
		if( setType ) this->setType( DT_TickerList );
		HTASSERT( dataType_m == DT_TickerList );
		HTASSERT( data_m.tickerList_m != NULL );

		return *data_m.tickerList_m;
	};

	RtDataList const & EventData::getAsRtDataList() const
	{
		HTASSERT( dataType_m == DT_TickerList);
		return *data_m.tickerList_m;
	};

	XmlParameter& EventData::getAsXmlParameter(bool setType )
	{
		if( setType ) this->setType( DT_CommonXml );
		HTASSERT( dataType_m == DT_CommonXml );
		HTASSERT( data_m.commonXml_m != NULL );

		return *data_m.commonXml_m;
	}
		
	XmlParameter const& EventData::getAsXmlParameter() const
	{
		HTASSERT( dataType_m == DT_CommonXml);
		return *data_m.commonXml_m;
	}

	//
	RtLevel2Data& EventData::getAsLevel2Data(bool setType )
	{
		if( setType ) this->setType( DT_Level2Data );
		HTASSERT( dataType_m == DT_Level2Data );
		HTASSERT( data_m.level2Data_m != NULL );

		return *data_m.level2Data_m;
	}

	RtLevel2Data const& EventData::getAsLevel2Data( ) const
	{
		HTASSERT( dataType_m == DT_Level2Data );
		return *data_m.level2Data_m;
	}

	//
	RtLevel1Data& EventData::getAsLevel1Data(bool setType )
	{
		if( setType ) this->setType( DT_Level1Data );
		HTASSERT( dataType_m == DT_Level1Data );
		HTASSERT( data_m.level1Data_m != NULL );

		return *data_m.level1Data_m;
	}

	RtLevel1Data const& EventData::getAsLevel1Data( ) const
	{
		HTASSERT( dataType_m == DT_Level1Data );
		return *data_m.level1Data_m;
	}

	RtDrawableObj& EventData::getAsDrawableObject(bool setType )
	{
		if( setType ) this->setType( DT_DrawableObject );
		HTASSERT( dataType_m == DT_DrawableObject );
		HTASSERT( data_m.drawableObj_m != NULL );

		return *data_m.drawableObj_m;
	}

	RtDrawableObj const& EventData::getAsDrawableObject( ) const
	{
		HTASSERT( dataType_m == DT_DrawableObject );
		return *data_m.drawableObj_m;
	}


		
	// conversion functions
	void convertEventToXmlParameter(EventData const& eventdata, XmlParameter &xmlparameter)
	{
		xmlparameter.clear();
		xmlparameter.getName() = "event_packet";

		xmlparameter.getParameter("source").getAsString(true) = eventdata.getSource();
		xmlparameter.getParameter("puid").getAsUid(true) = eventdata.getParentEventUid();
		xmlparameter.getParameter("uid").getAsUid(true) = eventdata.getEventUid();
		xmlparameter.getParameter("etype").getAsInt(true) = eventdata.getEventType();
		xmlparameter.getParameter("dtype").getAsInt(true) = eventdata.getType();
		
		double sec = eventdata.getEventDate();
		if (sec <= 0)
			xmlparameter.getParameter("etime").getAsDate(true)  =-1;
		else
			xmlparameter.getParameter("etime").getAsDate(true) = sec * 1000.0;

		if (eventdata.getType()==EventData::DT_Log) {
			XmlParameter logParam;
			CommonLog const& log = eventdata.getAsLog();
			double sec = log.logDate_m;

			if (sec <= 0)
				logParam.getParameter("time").getAsDate(true) = -1;
			else	
				logParam.getParameter("time").getAsDate(true) = sec * 1000.0;
			
			logParam.getParameter("level").getAsInt(true) = log.logLevel_m;
			logParam.getParameter("context").getAsString(true) = log.context_m;
			logParam.getParameter("content").getAsString(true) = CppUtils::wrapToCDataTags(log.content_m);
			logParam.getParameter("session").getAsString(true) = log.session_m;
			logParam.getParameter("thread").getAsInt(true) = log.logThread_m;
			
		
			xmlparameter.getXmlParameter("log") = logParam;
		}
		else if (eventdata.getType()==EventData::DT_CommonXml) {
			xmlparameter.getXmlParameter("commonxml") = eventdata.getAsXmlParameter();
		}
		else if (eventdata.getType()==EventData::DT_String) {
			xmlparameter.getParameter("str").getAsString(true) = eventdata.getAsString();

		}
		else if (eventdata.getType()==EventData::DT_StringList) {
			xmlparameter.getParameter("strlist").getAsStringList(true) = eventdata.getAsStringList();
		}
		else if (eventdata.getType()==EventData::DT_StringMap) {
			XmlParameter strMapParam;
			
			CppUtils::StringMap const& strmap = eventdata.getAsStringMap();
			for(CppUtils::StringMap::const_iterator it = strmap.begin(); it != strmap.end(); it++) {
				strMapParam.getParameter(it->first).getAsString(true) = it->second;
			}

			xmlparameter.getXmlParameter("strmap") = strMapParam;
		}
		else if (eventdata.getType()==EventData::DT_Ticker) {
			convertToXmlParameter(eventdata.getAsRtData(), xmlparameter.getXmlParameter("ticker"));
		}
		else if (eventdata.getType()==EventData::DT_TickerList) {
			XmlParameter tickerParam;
			RtDataList const& rtlist = eventdata.getAsRtDataList();
			for(int i = 0; i < rtlist.size(); i++) {
				convertToXmlParameter(rtlist[ i ], tickerParam);
				xmlparameter.getXmlParameterList("tickerlist").push_back(tickerParam);
			}

		}
		else if (eventdata.getType()==EventData::DT_Dummy) {
			// no-op
		}
		else if (eventdata.getType()==EventData::DT_Level2Data) {
			//convertLevel2ToXmlParameter(eventdata.getAsLevel2Data(), xmlparameter.getXmlParameter("level2data"));
			HTASSERT(false);
		}
		else if (eventdata.getType()==EventData::DT_Level1Data) {
			convertLevel1ToXmlParameter(eventdata.getAsLevel1Data(), xmlparameter.getXmlParameter("level1data"));
		}
		else if (eventdata.getType()==EventData::DT_DrawableObject) {
			convertDrawableObjToXmlParameter(eventdata.getAsDrawableObject(), xmlparameter.getXmlParameter("drawobj"));
		}
		else {
			// dummy
			HTASSERT(false);
		}

	}

	void convertXmlParameterToEvent(EventData &eventdata, XmlParameter const &xmlparameter)
	{
		eventdata.clear();
		
		EventData::DType dtype = (EventData::DType)xmlparameter.getParameter("dtype").getAsInt();
		EventData::EType etype = (EventData::EType)xmlparameter.getParameter("etype").getAsInt();

				
		eventdata.getEventType() = etype;
		eventdata.setType(dtype);

		double miliss = xmlparameter.getParameter("etime").getAsDate();
		if (miliss > 0)
			eventdata.setEventDate( miliss/ 1000.0 );
		else
			eventdata.setEventDate(-1);


		eventdata.eventSource_m = xmlparameter.getParameter("source").getAsString();
		eventdata.parentUid_m = xmlparameter.getParameter("puid").getAsUid();
		eventdata.uid_m = xmlparameter.getParameter("uid").getAsUid();

		if (dtype==EventData::DT_Log) {
		
			XmlParameter const& logParam = xmlparameter.getXmlParameter("log");
			CommonLog& log = eventdata.getAsLog(true);

			double miliss = logParam.getParameter("time").getAsDate();
			if (miliss > 0)
				log.logDate_m = miliss / 1000.0;
			else
				log.logDate_m = -1;

			log.logLevel_m  = logParam.getParameter("level").getAsInt();
			log.context_m = logParam.getParameter("context").getAsString();
			log.content_m = logParam.getParameter("content").getAsString();
			log.session_m = logParam.getParameter("session").getAsString();
			log.logThread_m = logParam.getParameter("thread").getAsInt();

						

		}
		else if (dtype==EventData::DT_CommonXml) {
			eventdata.getAsXmlParameter(true) = xmlparameter.getXmlParameter("commonxml");
		}
		else if (dtype==EventData::DT_String) {
			eventdata.getAsString(true) = xmlparameter.getParameter("str").getAsString();
		}
		else if (dtype==EventData::DT_StringList) {
			eventdata.getAsStringList(true) = xmlparameter.getParameter("strlist").getAsStringList();
		}
		else if (dtype==EventData::DT_StringMap) {
			
			CppUtils::StringMap& strmap = eventdata.getAsStringMap(true);

			XmlParameter const& strMapParam = xmlparameter.getXmlParameter("strmap");
			CppUtils::StringList keys;
			strMapParam.getValueKeys(keys);
			for(int i = 0; i < keys.size(); i++) {
				strmap[ keys[ i ] ] = strMapParam.getParameter(keys[ i ]).getAsString();
			}
			
		}
		else if (dtype==EventData::DT_Ticker) {
			convertFromXmlParameter( eventdata.getAsRtData(), xmlparameter.getXmlParameter("ticker"));
		}
		else if (eventdata.getType()==EventData::DT_TickerList) {
			
			RtDataList& rtlist = eventdata.getAsRtDataList(true);

			XmlParameter::XmlParameterList const& tickerMapParamList = xmlparameter.getXmlParameterList("tickerlist");
			RtData rtdata;

			for(int i = 0; i < tickerMapParamList.size(); i++) {
				convertFromXmlParameter( rtdata, tickerMapParamList[ i ]);
				rtlist.push_back(rtdata);
			}

		}
		else if (dtype==EventData::DT_Level2Data) {
			//convertLevel2FromXmlParameter( eventdata.getAsLevel2Data(), xmlparameter.getXmlParameter("level2data"));
			HTASSERT(false);
		}
		else if (dtype==EventData::DT_Level1Data) {
			convertLevel1FromXmlParameter( eventdata.getAsLevel1Data(), xmlparameter.getXmlParameter("level1data"));
		}
		else if (dtype==EventData::DT_DrawableObject) {
			convertDrawableObjFromXmlParameter( eventdata.getAsDrawableObject(), xmlparameter.getXmlParameter("drawobj"));
		}
		else {
			HTASSERT(false);
		}


	}

	

};