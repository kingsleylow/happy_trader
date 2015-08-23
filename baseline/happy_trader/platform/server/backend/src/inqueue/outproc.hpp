#ifndef _BACKEND_INQUEUE_OUTPROC_INCLUDED
#define _BACKEND_INQUEUE_OUTPROC_INCLUDED

#include "inqueuedefs.hpp"
#include "commontypes.hpp"
#include "histcontext.hpp"
#include "comproc.hpp"

#include "transportbase.hpp"
#include "transporttcpip.hpp"



/*
	The macros belows create EventData object of eventtype = ET_CommonLog
  and data type = DT_Log
*/
//  level, context, content
#define LOGEVENT( L, C, X)	{	\
	if (L <= Inqueue::OutProcessor::getLogLevel() && Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_CommonLog)) {	\
		ostrstream sstr; sstr << X << ends; \
		Inqueue::OutProcessor::sendLogEvent(HlpStruct::EventData::ET_CommonLog, L, Inqueue::InQueueController::resolveSessionName(), sstr.str(), C);	\
		sstr.freeze( false ); \
	}	\
}

// level, context, content, attributes
#define LOGEVENTA( L, C, X, A)	{	\
	if (L <= Inqueue::OutProcessor::getLogLevel() && Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_CommonLog)) {	\
		ostrstream sstr; sstr << X << "\r\n"; \
		HlpStruct::svector sv;	\
		sv << A;	\
		sstr << sv.getContent() << ends; \
		Inqueue::OutProcessor::sendLogEvent(HlpStruct::EventData::ET_CommonLog, L, Inqueue::InQueueController::resolveSessionName(), sstr.str(), C);	\
		sstr.freeze( false ); \
	}	\
}

/*
	The macros belows create EventData object of eventtype = ET_CommonLog
  and data type = DT_Log,
	getting as a parameter EventData object and turnig this into XML
*/

/*
// helper macro to log common event
// level, context, event
#define LOGCOMMONEVENT( L, C, E)	{	\
	if (L <= Inqueue::OutProcessor::getLogLevel() && Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_CommonLog)) {	\
		HlpStruct::XmlParameter xmlparam;	\
		CppUtils::String xmlcontent;	\
			\
		HlpStruct::convertEventToXmlParameter(E, xmlparam);	\
		HlpStruct::serializeXmlParameter(xmlparam, xmlcontent);	\
			\
		Inqueue::OutProcessor::sendLogEvent(HlpStruct::EventData::ET_CommonLog, L, Inqueue::InQueueController::resolveSessionName(), xmlcontent.c_str(), C);	\
	}	\
}

// helper macro to log common event
// level, context, event, attributes
#define LOGCOMMONEVENTA( L, C, E, A)	{	\
	if (L <= Inqueue::OutProcessor::getLogLevel() && Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_CommonLog) ) {	\
		HlpStruct::XmlParameter xmlparam;	\
		CppUtils::String xmlcontent;	\
			\
		HlpStruct::convertEventToXmlParameter(E, xmlparam);	\
		HlpStruct::serializeXmlParameter(xmlparam, xmlcontent);	\
			\
		Inqueue::svector sv;	\
		sv << A;	\
		Inqueue::OutProcessor::sendLogEvent(HlpStruct::EventData::ET_CommonLog, L, Inqueue::InQueueController::resolveSessionName(), xmlcontent.c_str(), C, sv);	\
	}	\
}
*/

// helper macro to send event if subscribed event exists 
// used to avoid unnecessary activity if there is no sibscription to the event
// Event, 
#define PUSHEVENT(E) {	\
	if (Inqueue::OutProcessor::isEventAllowed(E.getEventType())) {	\
		E.setEventDate(CppUtils::getTime());	\
		Inqueue::OutProcessor::sendEvent(E);	\
	}	\
}	

// other helper macro to help create events if necessary
#define ISEVENTSUBSCRIBED_BEGIN(T)	\
	if (Inqueue::OutProcessor::isEventAllowed(T)) {

#define ISEVENTSUBSCRIBED_END(E)	\
		E.setEventDate(CppUtils::getTime());	\
		Inqueue::OutProcessor::sendEvent(E);	\
	}
		


// special macro for alerts - just to shortage the lines of code
// P- priority
// S - small data
// T - text
// C - BrokerConnect
// R - run name
// 
#define ALERTEVENT(R,C,P,S,T)		\
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_AlgorithmAlert)) {	\
			HlpStruct::EventData response;	\
\
			response.getEventType() = HlpStruct::EventData::ET_AlgorithmAlert;	\
			response.setType(HlpStruct::EventData::DT_CommonXml);	\
\
			response.getAsXmlParameter(true).getParameter("id").getAsUid(true).generate();	\
			response.getAsXmlParameter().getParameter("alg_brk_pair").getAsString(true) = getAlgHolder().getAlgBrokerDescriptor().name_m;	\
			response.getAsXmlParameter().getParameter("thread").getAsInt(true) = CppUtils::getThreadHandle();	\
			response.getAsXmlParameter().getParameter("priority").getAsInt(true) = P;	\
			response.getAsXmlParameter().getParameter("run_name").getAsString(true) = R;	\
			response.getAsXmlParameter().getParameter("small_data").getAsString(true) = CppUtils::wrapToCDataTags(S);	\
\
			ostrstream sstr; sstr << T << ends; \
			response.getAsXmlParameter().getParameter("data").getAsString(true) = CppUtils::wrapToCDataTags(sstr.str());	\
			sstr.freeze(false); \
\
			response.setSource(C.getSession().getUid().toString());	\
			response.setEventDate(CppUtils::getTime());	\
\
			Inqueue::OutProcessor::sendEvent(response);	\
	}

	// vesrion without brokerConnect class
	// S- small data
	#define ALERTEVENTNC(R,P,S,T)		\
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_AlgorithmAlert)) {	\
			HlpStruct::EventData response;	\
\
			response.getEventType() = HlpStruct::EventData::ET_AlgorithmAlert;	\
			response.setType(HlpStruct::EventData::DT_CommonXml);	\
\
			response.getAsXmlParameter(true).getParameter("id").getAsUid(true).generate();	\
			response.getAsXmlParameter().getParameter("alg_brk_pair").getAsString(true) = getAlgHolder().getAlgBrokerDescriptor().name_m;	\
			response.getAsXmlParameter().getParameter("thread").getAsInt(true) = CppUtils::getThreadHandle();	\
			response.getAsXmlParameter().getParameter("priority").getAsInt(true) = P;	\
			response.getAsXmlParameter().getParameter("run_name").getAsString(true) = R;	\
			response.getAsXmlParameter().getParameter("small_data").getAsString(true) = CppUtils::wrapToCDataTags(S);	\
\
			ostrstream sstr; sstr << T << ends; \
			response.getAsXmlParameter().getParameter("data").getAsString(true) = CppUtils::wrapToCDataTags(sstr.str());	\
			sstr.freeze(false); \
\
			response.setEventDate(CppUtils::getTime());	\
\
			Inqueue::OutProcessor::sendEvent(response);	\
	}


// ------------------------------------
// MACROS FOR SENDING DRAWBLE OBJECTS
// TIME
#define DRAWOBJ_LINEINDIC_1(RUN_NAME, TIME, NAME, SYMBOL, PROVIDER, NAME_1, VALUE_1, COLOR_1) \
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_DrawableObject)) {	\
		HlpStruct::EventData drawobj_event;	\
		drawobj_event.getEventType() = HlpStruct::EventData::ET_DrawableObject;	\
		HlpStruct::RtDrawableObj& drawobj = drawobj_event.getAsDrawableObject(true);	\
		drawobj.getRunName() = RUN_NAME;	\
		drawobj.getName() = NAME;	\
		drawobj.getTime() = TIME;	\
		drawobj.getProvider() = PROVIDER;	\
		drawobj.getSymbol() = SYMBOL;	\
		drawobj.getType() = HlpStruct::RtDrawableObj::DO_LINE_INDIC; \
		HlpStruct::RtDrawableObj::LineIndicator indic_1(NAME_1, COLOR_1, VALUE_1);	\
		drawobj.getAsLineIndicator().push_back(indic_1);	\
		/*drawobj_event.setEventDate(CppUtils::getTime());*/	\
		Inqueue::OutProcessor::sendEvent(drawobj_event);	\
	}

#define DRAWOBJ_LINEINDIC_2(RUN_NAME,TIME, NAME, SYMBOL, PROVIDER, NAME_1, VALUE_1, COLOR_1, NAME_2, VALUE_2, COLOR_2) \
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_DrawableObject)) {	\
		HlpStruct::EventData drawobj_event;	\
		drawobj_event.getEventType() = HlpStruct::EventData::ET_DrawableObject;	\
		HlpStruct::RtDrawableObj& drawobj = drawobj_event.getAsDrawableObject(true);	\
		drawobj.getRunName() = RUN_NAME;	\
		drawobj.getName() = NAME;	\
		drawobj.getTime() = TIME;	\
		drawobj.getProvider() = PROVIDER;	\
		drawobj.getSymbol() = SYMBOL;	\
		drawobj.getType() = HlpStruct::RtDrawableObj::DO_LINE_INDIC; \
		HlpStruct::RtDrawableObj::LineIndicator indic_1(NAME_1, COLOR_1, VALUE_1);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_2(NAME_2, COLOR_2, VALUE_2);	\
		drawobj.getAsLineIndicator().push_back(indic_1);	\
		drawobj.getAsLineIndicator().push_back(indic_2);	\
		/*drawobj_event.setEventDate(CppUtils::getTime());*/	\
		Inqueue::OutProcessor::sendEvent(drawobj_event);	\
	}

#define DRAWOBJ_LINEINDIC_3(RUN_NAME,TIME, NAME, SYMBOL, PROVIDER, NAME_1, VALUE_1, COLOR_1, NAME_2, VALUE_2, COLOR_2, NAME_3, VALUE_3, COLOR_3) \
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_DrawableObject)) {	\
		HlpStruct::EventData drawobj_event;	\
		drawobj_event.getEventType() = HlpStruct::EventData::ET_DrawableObject;	\
		HlpStruct::RtDrawableObj& drawobj = drawobj_event.getAsDrawableObject(true);	\
		drawobj.getRunName() = RUN_NAME;	\
		drawobj.getName() = NAME;	\
		drawobj.getTime() = TIME;	\
		drawobj.getProvider() = PROVIDER;	\
		drawobj.getSymbol() = SYMBOL;	\
		drawobj.getType() = HlpStruct::RtDrawableObj::DO_LINE_INDIC; \
		HlpStruct::RtDrawableObj::LineIndicator indic_1(NAME_1, COLOR_1, VALUE_1);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_2(NAME_2, COLOR_2, VALUE_2);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_3(NAME_3, COLOR_3, VALUE_3);	\
		drawobj.getAsLineIndicator().push_back(indic_1);	\
		drawobj.getAsLineIndicator().push_back(indic_2);	\
		drawobj.getAsLineIndicator().push_back(indic_3);	\
		/*drawobj_event.setEventDate(CppUtils::getTime());*/	\
		Inqueue::OutProcessor::sendEvent(drawobj_event);	\
	}

#define DRAWOBJ_LINEINDIC_4(RUN_NAME,TIME, NAME, SYMBOL, PROVIDER, NAME_1, VALUE_1, COLOR_1, NAME_2, VALUE_2, COLOR_2, NAME_3, VALUE_3, COLOR_3, NAME_4, VALUE_4, COLOR_4) \
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_DrawableObject)) {	\
		HlpStruct::EventData drawobj_event;	\
		drawobj_event.getEventType() = HlpStruct::EventData::ET_DrawableObject;	\
		HlpStruct::RtDrawableObj& drawobj = drawobj_event.getAsDrawableObject(true);	\
		drawobj.getRunName() = RUN_NAME;	\
		drawobj.getName() = NAME;	\
		drawobj.getTime() = TIME;	\
		drawobj.getProvider() = PROVIDER;	\
		drawobj.getSymbol() = SYMBOL;	\
		drawobj.getType() = HlpStruct::RtDrawableObj::DO_LINE_INDIC; \
		HlpStruct::RtDrawableObj::LineIndicator indic_1(NAME_1, COLOR_1, VALUE_1);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_2(NAME_2, COLOR_2, VALUE_2);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_3(NAME_3, COLOR_3, VALUE_3);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_4(NAME_4, COLOR_4, VALUE_4);	\
		drawobj.getAsLineIndicator().push_back(indic_1);	\
		drawobj.getAsLineIndicator().push_back(indic_2);	\
		drawobj.getAsLineIndicator().push_back(indic_3);	\
		drawobj.getAsLineIndicator().push_back(indic_4);	\
		/*drawobj_event.setEventDate(CppUtils::getTime());*/	\
		Inqueue::OutProcessor::sendEvent(drawobj_event);	\
	}

#define DRAWOBJ_LINEINDIC_5(RUN_NAME,TIME, NAME, SYMBOL, PROVIDER, \
NAME_1, VALUE_1, COLOR_1,	\
NAME_2, VALUE_2, COLOR_2,	\
NAME_3, VALUE_3, COLOR_3, \
NAME_4, VALUE_4, COLOR_4,	\
NAME_5, VALUE_5, COLOR_5 \
) \
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_DrawableObject)) {	\
		HlpStruct::EventData drawobj_event;	\
		drawobj_event.getEventType() = HlpStruct::EventData::ET_DrawableObject;	\
		HlpStruct::RtDrawableObj& drawobj = drawobj_event.getAsDrawableObject(true);	\
		drawobj.getRunName() = RUN_NAME;	\
		drawobj.getName() = NAME;	\
		drawobj.getTime() = TIME;	\
		drawobj.getProvider() = PROVIDER;	\
		drawobj.getSymbol() = SYMBOL;	\
		drawobj.getType() = HlpStruct::RtDrawableObj::DO_LINE_INDIC; \
		HlpStruct::RtDrawableObj::LineIndicator indic_1(NAME_1, COLOR_1, VALUE_1);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_2(NAME_2, COLOR_2, VALUE_2);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_3(NAME_3, COLOR_3, VALUE_3);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_4(NAME_4, COLOR_4, VALUE_4);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_5(NAME_5, COLOR_5, VALUE_5);	\
		drawobj.getAsLineIndicator().push_back(indic_1);	\
		drawobj.getAsLineIndicator().push_back(indic_2);	\
		drawobj.getAsLineIndicator().push_back(indic_3);	\
		drawobj.getAsLineIndicator().push_back(indic_4);	\
		drawobj.getAsLineIndicator().push_back(indic_5);	\
		/*drawobj_event.setEventDate(CppUtils::getTime());*/	\
		Inqueue::OutProcessor::sendEvent(drawobj_event);	\
	}

#define DRAWOBJ_LINEINDIC_6(RUN_NAME,TIME, NAME, SYMBOL, PROVIDER, \
NAME_1, VALUE_1, COLOR_1,	\
NAME_2, VALUE_2, COLOR_2,	\
NAME_3, VALUE_3, COLOR_3, \
NAME_4, VALUE_4, COLOR_4,	\
NAME_5, VALUE_5, COLOR_5, \
NAME_6, VALUE_6, COLOR_6	\
	) \
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_DrawableObject)) {	\
		HlpStruct::EventData drawobj_event;	\
		drawobj_event.getEventType() = HlpStruct::EventData::ET_DrawableObject;	\
		HlpStruct::RtDrawableObj& drawobj = drawobj_event.getAsDrawableObject(true);	\
		drawobj.getRunName() = RUN_NAME;	\
		drawobj.getName() = NAME;	\
		drawobj.getTime() = TIME;	\
		drawobj.getProvider() = PROVIDER;	\
		drawobj.getSymbol() = SYMBOL;	\
		drawobj.getType() = HlpStruct::RtDrawableObj::DO_LINE_INDIC; \
		HlpStruct::RtDrawableObj::LineIndicator indic_1(NAME_1, COLOR_1, VALUE_1);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_2(NAME_2, COLOR_2, VALUE_2);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_3(NAME_3, COLOR_3, VALUE_3);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_4(NAME_4, COLOR_4, VALUE_4);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_5(NAME_5, COLOR_5, VALUE_5);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_6(NAME_6, COLOR_6, VALUE_6);	\
		drawobj.getAsLineIndicator().push_back(indic_1);	\
		drawobj.getAsLineIndicator().push_back(indic_2);	\
		drawobj.getAsLineIndicator().push_back(indic_3);	\
		drawobj.getAsLineIndicator().push_back(indic_4);	\
		drawobj.getAsLineIndicator().push_back(indic_5);	\
		drawobj.getAsLineIndicator().push_back(indic_6);	\
		/*drawobj_event.setEventDate(CppUtils::getTime());*/	\
		Inqueue::OutProcessor::sendEvent(drawobj_event);	\
	}


#define DRAWOBJ_LINEINDIC_7(RUN_NAME,TIME, NAME, SYMBOL, PROVIDER, \
NAME_1, VALUE_1, COLOR_1,	\
NAME_2, VALUE_2, COLOR_2,	\
NAME_3, VALUE_3, COLOR_3, \
NAME_4, VALUE_4, COLOR_4,	\
NAME_5, VALUE_5, COLOR_5, \
NAME_6, VALUE_6, COLOR_6,	\
NAME_7, VALUE_7, COLOR_7	\
	) \
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_DrawableObject)) {	\
		HlpStruct::EventData drawobj_event;	\
		drawobj_event.getEventType() = HlpStruct::EventData::ET_DrawableObject;	\
		HlpStruct::RtDrawableObj& drawobj = drawobj_event.getAsDrawableObject(true);	\
		drawobj.getRunName() = RUN_NAME;	\
		drawobj.getName() = NAME;	\
		drawobj.getTime() = TIME;	\
		drawobj.getProvider() = PROVIDER;	\
		drawobj.getSymbol() = SYMBOL;	\
		drawobj.getType() = HlpStruct::RtDrawableObj::DO_LINE_INDIC; \
		HlpStruct::RtDrawableObj::LineIndicator indic_1(NAME_1, COLOR_1, VALUE_1);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_2(NAME_2, COLOR_2, VALUE_2);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_3(NAME_3, COLOR_3, VALUE_3);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_4(NAME_4, COLOR_4, VALUE_4);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_5(NAME_5, COLOR_5, VALUE_5);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_6(NAME_6, COLOR_6, VALUE_6);	\
		HlpStruct::RtDrawableObj::LineIndicator indic_7(NAME_7, COLOR_7, VALUE_7);	\
		drawobj.getAsLineIndicator().push_back(indic_1);	\
		drawobj.getAsLineIndicator().push_back(indic_2);	\
		drawobj.getAsLineIndicator().push_back(indic_3);	\
		drawobj.getAsLineIndicator().push_back(indic_4);	\
		drawobj.getAsLineIndicator().push_back(indic_5);	\
		drawobj.getAsLineIndicator().push_back(indic_6);	\
		drawobj.getAsLineIndicator().push_back(indic_7);	\
		/*drawobj_event.setEventDate(CppUtils::getTime());*/	\
		Inqueue::OutProcessor::sendEvent(drawobj_event);	\
	}


#define DRAWOBJ_OHCL(RUN_NAME,TIME, NAME, SYMBOL, PROVIDER, OPEN, HIGH, LOW, CLOSE, VOLUME) \
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_DrawableObject)) {	\
		HlpStruct::EventData drawobj_event;	\
		drawobj_event.getEventType() = HlpStruct::EventData::ET_DrawableObject;	\
		HlpStruct::RtDrawableObj& drawobj = drawobj_event.getAsDrawableObject(true);	\
		drawobj.getRunName() = RUN_NAME;	\
		drawobj.getName() = NAME;	\
		drawobj.getTime() = TIME;	\
		drawobj.getProvider() = PROVIDER;	\
		drawobj.getSymbol() = SYMBOL;	\
		drawobj.getType() = HlpStruct::RtDrawableObj::DO_OHLC; \
		drawobj.getAsOHLC().open_m = OPEN;		\
		drawobj.getAsOHLC().high_m = HIGH;		\
		drawobj.getAsOHLC().low_m = LOW;			\
		drawobj.getAsOHLC().close_m = CLOSE;	\
		drawobj.getAsOHLC().volume_m = VOLUME;	\
		/*drawobj_event.setEventDate(CppUtils::getTime());*/	\
		Inqueue::OutProcessor::sendEvent(drawobj_event);	\
	}

#define DRAWOBJ_TEXT(RUN_NAME,TIME, NAME, SYMBOL, PROVIDER, ALERT_PRIORITY, SHORT_TEXT, T) \
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_DrawableObject)) {	\
		HlpStruct::EventData drawobj_event;	\
		drawobj_event.getEventType() = HlpStruct::EventData::ET_DrawableObject;	\
		HlpStruct::RtDrawableObj& drawobj = drawobj_event.getAsDrawableObject(true);	\
		drawobj.getRunName() = RUN_NAME;	\
		drawobj.getName() = NAME;	\
		drawobj.getTime() = TIME;	\
		drawobj.getProvider() = PROVIDER;	\
		drawobj.getSymbol() = SYMBOL;	\
		drawobj.getType() = HlpStruct::RtDrawableObj::DO_TEXT; \
		drawobj.getAsText().priority_m = (BrkLib::AlertPriorities)ALERT_PRIORITY;		\
		drawobj.getAsText().setShortText(SHORT_TEXT);		\
		ostrstream sstr; sstr << T << ends; \
		drawobj.getAsText().setText( sstr.str() );			\
		sstr.freeze(false); \
		/*drawobj_event.setEventDate(CppUtils::getTime());*/	\
		Inqueue::OutProcessor::sendEvent(drawobj_event);	\
	}

#define DRAWOBJ_PRINT_OPERATION(RUN_NAME,TIME, NAME, SYMBOL, PROVIDER, BID, ASK, VOLUME, COLOR, OPERATION) \
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_DrawableObject)) {	\
		HlpStruct::EventData drawobj_event;	\
		drawobj_event.getEventType() = HlpStruct::EventData::ET_DrawableObject;	\
		HlpStruct::RtDrawableObj& drawobj = drawobj_event.getAsDrawableObject(true);	\
		drawobj.getRunName() = RUN_NAME;	\
		drawobj.getName() = NAME;	\
		drawobj.getTime() = TIME;	\
		drawobj.getProvider() = PROVIDER;	\
		drawobj.getSymbol() = SYMBOL;	\
		drawobj.getType() = HlpStruct::RtDrawableObj::DO_PRINT; \
		drawobj.getAsPrint().bid_m = BID;		\
		drawobj.getAsPrint().ask_m = ASK;		\
		drawobj.getAsPrint().volume_m = VOLUME;		\
		drawobj.getAsPrint().color_m = COLOR;		\
		drawobj.getAsPrint().operation_m = OPERATION; \
		/*drawobj_event.setEventDate(CppUtils::getTime());*/	\
		Inqueue::OutProcessor::sendEvent(drawobj_event);	\
	}

#define DRAWOBJ_PRINT(RUN_NAME,TIME, NAME, SYMBOL, PROVIDER, BID, ASK, VOLUME, COLOR) \
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_DrawableObject)) {	\
		HlpStruct::EventData drawobj_event;	\
		drawobj_event.getEventType() = HlpStruct::EventData::ET_DrawableObject;	\
		HlpStruct::RtDrawableObj& drawobj = drawobj_event.getAsDrawableObject(true);	\
		drawobj.getRunName() = RUN_NAME;	\
		drawobj.getName() = NAME;	\
		drawobj.getTime() = TIME;	\
		drawobj.getProvider() = PROVIDER;	\
		drawobj.getSymbol() = SYMBOL;	\
		drawobj.getType() = HlpStruct::RtDrawableObj::DO_PRINT; \
		drawobj.getAsPrint().bid_m = BID;		\
		drawobj.getAsPrint().ask_m = ASK;		\
		drawobj.getAsPrint().volume_m = VOLUME;		\
		drawobj.getAsPrint().color_m = COLOR;		\
		/*drawobj_event.setEventDate(CppUtils::getTime());*/	\
		Inqueue::OutProcessor::sendEvent(drawobj_event);	\
	}

#define DRAWOBJ_LEVEL1(RUN_NAME,TIME, NAME, SYMBOL, PROVIDER, BEST_BID_PRICE,BEST_BID_VOLUME, BEST_ASK_PRICE, BEST_ASK_VOLUME) \
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_DrawableObject)) {	\
		HlpStruct::EventData drawobj_event;	\
		drawobj_event.getEventType() = HlpStruct::EventData::ET_DrawableObject;	\
		HlpStruct::RtDrawableObj& drawobj = drawobj_event.getAsDrawableObject(true);	\
		drawobj.getRunName() = RUN_NAME;	\
		drawobj.getName() = NAME;	\
		drawobj.getTime() = TIME;	\
		drawobj.getProvider() = PROVIDER;	\
		drawobj.getSymbol() = SYMBOL;	\
		drawobj.getType() = HlpStruct::RtDrawableObj::DO_LEVEL1; \
		drawobj.getAsLevel1().best_bid_price_m = BEST_BID_PRICE;	\
		drawobj.getAsLevel1().best_ask_price_m = BEST_ASK_PRICE;	\
		drawobj.getAsLevel1().best_bid_volume_m = BEST_BID_VOLUME;	\
		drawobj.getAsLevel1().best_ask_volume_m = BEST_ASK_VOLUME;	\
		/*drawobj_event.setEventDate(CppUtils::getTime());*/	\
		Inqueue::OutProcessor::sendEvent(drawobj_event);	\
	}

// ------------------------------------
// MACROS FOR SENDING RT DATA
// 

/*
#define SEND_RT_PROVIDER_SYNC(TIME, PROVIDER, TYPE, MESSAGE)			\
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_RtProviderSynchronizationEvent)) {	\
		HlpStruct::EventData synch_event;	\
		synch_event.getEventType() = HlpStruct::EventData::ET_RtProviderSynchronizationEvent;	\
		synch_event.setEventDate(TIME);	\
		HlpStruct::XmlParameter& synch_param = synch_event.getAsXmlParameter(true);	\
		synch_param.getParameter("server_id").getAsString(true) = Inqueue::InQueueController::resolveSessionName();	\
		synch_param.getParameter("provider_name").getAsString(true) = PROVIDER;	\
		synch_param.getParameter("type").getAsInt(true) = TYPE;	\
		synch_param.getParameter("message").getAsString(true) = MESSAGE;	\
		Inqueue::OutProcessor::sendEvent(synch_event);	\
	};
*/
/*
#define SEND_RT_DATA(TIME, PROVIDER, SYMBOL, BID, ASK, VOLUME, COLOR)			\
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_RtProviderTicker)) {	\
		HlpStruct::EventData print_event;	\
		print_event.getEventType() = HlpStruct::EventData::ET_RtProviderTicker;	\
		HlpStruct::RtData& rtdata = print_event.getAsRtData(true);	\
		rtdata.setProvider(PROVIDER);	\
		rtdata.setSymbol(SYMBOL);	\
		rtdata.getType() = HlpStruct::RtData::RT_Type;	\
		rtdata.getTime() = TIME;	\
		rtdata.getRtData().bid_m = BID;	\
		rtdata.getRtData().ask_m = ASK;	\
		rtdata.getRtData().volume_m = VOLUME;	\
		rtdata.getRtData().color_m =COLOR;	\
		//print_event.setEventDate(CppUtils::getTime());	\
		Inqueue::OutProcessor::sendEvent(print_event);	\
	}
*/
/*
#define SEND_RT_DATA_OPERATION(TIME, PROVIDER, SYMBOL, BID, ASK, VOLUME, COLOR, OPERATION)			\
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_RtProviderTicker)) {	\
		HlpStruct::EventData print_event;	\
		print_event.getEventType() = HlpStruct::EventData::ET_RtProviderTicker;	\
		HlpStruct::RtData& rtdata = print_event.getAsRtData(true);	\
		rtdata.setProvider(PROVIDER);	\
		rtdata.setSymbol(SYMBOL);	\
		rtdata.getType() = HlpStruct::RtData::RT_Type;	\
		rtdata.getTime() = TIME;	\
		rtdata.getRtData().bid_m = BID;	\
		rtdata.getRtData().ask_m = ASK;	\
		rtdata.getRtData().volume_m = VOLUME;	\
		rtdata.getRtData().color_m =COLOR;	\
		rtdata.getRtData().operation_m =OPERATION;	\
		//print_event.setEventDate(CppUtils::getTime());	\
		Inqueue::OutProcessor::sendEvent(print_event);	\
	}
*/
/*
#define SEND_RT_DATA_OHLC(TIME, PROVIDER, SYMBOL, OPEN, HIGH, LOW, CLOSE,VOLUME)			\
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_RtProviderTicker)) {	\
		HlpStruct::EventData print_event;	\
		print_event.getEventType() = HlpStruct::EventData::ET_RtProviderTicker;	\
		HlpStruct::RtData& rtdata = print_event.getAsRtData(true);	\
		rtdata.setProvider(PROVIDER);	\
		rtdata.setSymbol(SYMBOL);	\
		rtdata.getType() = HlpStruct::RtData::HST_Type;	\
		rtdata.getTime() = TIME;	\
		rtdata.getHistData().open_m = OPEN;	\
		rtdata.getHistData().high_m = HIGH;	\
		rtdata.getHistData().low_m = LOW;	\
		rtdata.getHistData().close_m = CLOSE;	\
		rtdata.getHistData().open2_m = OPEN;	\
		rtdata.getHistData().high2_m = HIGH;	\
		rtdata.getHistData().low2_m = LOW;	\
		rtdata.getHistData().close2_m = CLOSE;	\
		rtdata.getHistData().volume_m = VOLUME;	\
		Inqueue::OutProcessor::sendEvent(print_event);	\
	}
*/
/*
#define SEND_RT_DATA_OHLC2(TIME, PROVIDER, SYMBOL, OPEN, HIGH, LOW, CLOSE,OPEN2, HIGH2, LOW2, CLOSE2,VOLUME)			\
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_RtProviderTicker)) {	\
		HlpStruct::EventData print_event;	\
		print_event.getEventType() = HlpStruct::EventData::ET_RtProviderTicker;	\
		HlpStruct::RtData& rtdata = print_event.getAsRtData(true);	\
		rtdata.setProvider(PROVIDER);	\
		rtdata.setSymbol(SYMBOL);	\
		rtdata.getType() = HlpStruct::RtData::HST_Type;	\
		rtdata.getTime() = TIME;	\
		rtdata.getHistData().open_m = OPEN;	\
		rtdata.getHistData().high_m = HIGH;	\
		rtdata.getHistData().low_m = LOW;	\
		rtdata.getHistData().close_m = CLOSE;	\
		rtdata.getHistData().open2_m = OPEN2;	\
		rtdata.getHistData().high2_m = HIGH2;	\
		rtdata.getHistData().low2_m = LOW2;	\
		rtdata.getHistData().close2_m = CLOSE2;	\
		rtdata.getHistData().volume_m = VOLUME;	\
		Inqueue::OutProcessor::sendEvent(print_event);	\
	}
*/
/*
#define SEND_LEVEL1_DATA(TIME, PROVIDER, SYMBOL, BESTBIDPRICE, BESTBIDVOLUME, BESTASKPRICE, BESTASKVOLUME)	\
	if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_Level1Data)) {	\
		HlpStruct::EventData level1_event;	\
		level1_event.getEventType() = HlpStruct::EventData::ET_Level1Data;	\
		level1_event.setType(HlpStruct::EventData::DT_Level1Data);	\
		HlpStruct::RtLevel1Data& level1data = level1_event.getAsLevel1Data(true);	\
		level1data.best_ask_price_m =  BESTASKPRICE;	\
		level1data.best_bid_price_m =  BESTBIDPRICE;	\
		level1data.best_ask_volume_m = BESTASKVOLUME;	\
		level1data.best_bid_volume_m = BESTBIDVOLUME;	\
		level1data.setSymbol(SYMBOL);	\
		level1data.setProvider(PROVIDER);	\
		level1data.getTime() = TIME;	\
		//level1_event.setEventDate(CppUtils::getTime());	\
		Inqueue::OutProcessor::sendEvent(level1_event);	\
	}
*/
namespace Inqueue {

	/**
		Class is responsible for sending many kinds of messages back to the client
	*/

	class INQUEUE_BCK_EXP OutProcessor: 
		public CppUtils::RefCounted, 
		private CppUtils::Mutex, 
		public CppUtils::ProcessManager::Bootable,
		public ServerSocketControllerAceptor
{
	
	public:
		static CppUtils::String const &getRootObjectName ();
		
		OutProcessor();
		
		~OutProcessor();

		// this function starts in a separate thread and establishes client socket connection
		void threadRun();

		// boot function perfroming any kind of initialization
		virtual void boot();

		

		// event data to push, the sending occures in the separate queue
		void pushEventData(HlpStruct::EventData const& eventdata);

		// analog of macro
		static void pushevent(HlpStruct::EventData& eventdata);
		
		

		// static version of pushEventData
		static void sendEvent(
			HlpStruct::EventData const& eventdata
		);

		static void sendLogEvent(
			HlpStruct::EventData::EType const etype, 
			HlpStruct::CommonLog::LogLevel const level, 	
			CppUtils::String const& session,
			CppUtils::String const& content,
			CppUtils::String const& context
		);

		

		// notification sent when thread is started
		static void sendEngineThreadStartedEvent(CppUtils::String const& server, int const tid, CppUtils::String const& algLibPair);

		static void sendEngineThreadFinishedEvent(CppUtils::String const& server, int const tid, CppUtils::String const& algLibPair);
		
		static void sendSimpleCommonLoggingEvent(char const*context, CppUtils::String const& str, char const* arg);
	
		static void sendSimpleErrorLoggingEvent(char const*context, CppUtils::String const& str, char const* arg);

		static void send_rt_data(
			double const& optime, 
			char const* provider, 
			char const* symbol, 
			double const& bidPrice, 
			double const& askPrice, 
			double const& volume, 
			int const& color
		);

		static void send_rt_data_operation(
			double const& optime, 
			char const* provider, 
			char const* symbol, 
			double const& bidPrice, 
			double const& askPrice, 
			double const& volume, 
			int const& color, 
			int const& operation
		);

		static void send_rt_provider_synch(double const& optime, char const* provider, int const& otype, char const* message);

		static void send_rt_data_ohcl(
			double const& optime, 
			char const* provider, 
			char const* symbol, 
			double const& open, 
			double const& high, 
			double const& low, 
			double const& close, 
			double const& volume
		);

		static void send_rt_data_ohcl2(
			double const& optime, 
			char const* provider, 
			char const* symbol, 
			double const& open, 
			double const& high, 
			double const& low, 
			double const& close, 
			double const& open2, 
			double const& high2, 
			double const& low2, 
			double const& close2, 
			double const& volume
		);

		static void send_level1_data(
			double const& optime, 
			char const* provider, 
			char const* symbol, 
			double const& bestbidprice, 
			double const& bestbidvolume, 
			double const& bestaskprice, 
			double const& bestaskvolume,
			double const& open_interest
		);
		
		static void send_level2_data( HlpStruct::RtLevel2Data const& level2data);

		static void send_custom_event(HlpStruct::XmlParameter const& xml_param);
		
		// -----------------------

		static void setLogLevel(HlpStruct::CommonLog::LogLevel const eventLevel);

		static HlpStruct::CommonLog::LogLevel getLogLevel();

		// subscribed for allowed event
		static void setAllowedEvent(HlpStruct::EventData::EType const eventtype);

		// unsubscribe for allowed event
		static void clearAllowedEvent(HlpStruct::EventData::EType const eventtype);

		// check for event
		static bool isEventAllowed(HlpStruct::EventData::EType const eventtype);

		virtual void onSelectedAccept(HtClientSocket& clientsocket);


	private:

		
		
		// event level
		// events only send with lower level than set
		static HlpStruct::CommonLog::LogLevel eventLevel_m;

		static CppUtils::Mutex mtx_eventLevel_m;

		// port and host to connect
		//short port_m;

		// list of events that we are allowed to send
		static set<int> allowedEvents_m;

		static CppUtils::Mutex mtx_allowedEvents_m;

		// just for fast access
		static OutProcessor* thisPtr_m;

		//CppUtils::String host_m;

		// thread object to run in background
		CppUtils::Thread* thread_m;

		// XML parameter handler
		HlpStruct::ParametersHandler paramsProc_m;

		// history context object
		HistoryContext& hstContext_m;
 
		// event data peniding for sending :-)
		// the second parametre manages the chunk of data allocated at once
		CppUtils::FifoQueue<HlpStruct::EventData, 5000> dataToSend_m;
		
		// event telling other therads that data are ready to send
		CppUtils::Event dataPushed_m;

		// event showing that shutdown must occure
		CppUtils::Event shutDownThread_m;

		// started event
		CppUtils::Event started_m;


		// this is fired when a client socket object is ready and the connection was accepted and selected
		CppUtils::Event connectionAccepted_m;

		// client socket
		HtClientSocket* clientsocket_m;

		
	};


}; // end of namespace

#endif