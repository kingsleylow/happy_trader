#include "xmldrawableobj.hpp"

namespace HlpStruct {
	HLPSTRUCT_EXP void convertDrawableObjToXmlParameter(RtDrawableObj const& rtdrawableobj, XmlParameter &xmlparameter)
	{
		xmlparameter.clear();

		xmlparameter.getName() = "drawobj";
		xmlparameter.getParameter("type").getAsInt(true) = rtdrawableobj.getType();
		xmlparameter.getParameter("name").getAsString(true) = rtdrawableobj.getName();
		xmlparameter.getParameter("run_name").getAsString(true) = rtdrawableobj.getRunName();

		double sec = rtdrawableobj.getTime();
		if (sec <= 0)
			xmlparameter.getParameter("time").getAsDate(true)  =-1;
		else
			xmlparameter.getParameter("time").getAsDate(true) = sec * 1000.0;

		xmlparameter.getParameter("symbol").getAsString(true) = rtdrawableobj.getSymbol();
		xmlparameter.getParameter("provider").getAsString(true) = rtdrawableobj.getProvider();

		HlpStruct::XmlParameter::XmlParameterList& lineIndicParam = xmlparameter.getXmlParameterList("line_indic");

		if (rtdrawableobj.getType() == RtDrawableObj::DO_LINE_INDIC) {

			for(int i = 0; i < rtdrawableobj.getAsLineIndicator().size(); i++) {

				HlpStruct::XmlParameter xmlPram_i;
				RtDrawableObj::LineIndicator const& lineIndic_i = rtdrawableobj.getAsLineIndicator()[ i ];
				xmlPram_i.getParameter("val").getAsDouble(true) =  lineIndic_i.value_m;
				xmlPram_i.getParameter("color").getAsInt(true) =  lineIndic_i.color_m;
				xmlPram_i.getParameter("name").getAsString(true) =  lineIndic_i.name_m;

				lineIndicParam.push_back(xmlPram_i);

			}

			
			
		}
		else if (rtdrawableobj.getType() == RtDrawableObj::DO_PRINT) {
			xmlparameter.getParameter("print_ask").getAsDouble(true) = rtdrawableobj.getAsPrint().ask_m;
			xmlparameter.getParameter("print_bid").getAsDouble(true) = rtdrawableobj.getAsPrint().bid_m;
			xmlparameter.getParameter("print_volume").getAsDouble(true) = rtdrawableobj.getAsPrint().volume_m;
			xmlparameter.getParameter("print_color").getAsInt(true) = rtdrawableobj.getAsPrint().color_m;
			xmlparameter.getParameter("print_operation").getAsInt(true) = rtdrawableobj.getAsPrint().operation_m;
		}
		else if (rtdrawableobj.getType() == RtDrawableObj::DO_OHLC) {
			xmlparameter.getParameter("ohlc_open").getAsDouble(true) = rtdrawableobj.getAsOHLC().open_m;
			xmlparameter.getParameter("ohlc_high").getAsDouble(true) = rtdrawableobj.getAsOHLC().high_m;
			xmlparameter.getParameter("ohlc_low").getAsDouble(true) = rtdrawableobj.getAsOHLC().low_m;
			xmlparameter.getParameter("ohlc_close").getAsDouble(true) = rtdrawableobj.getAsOHLC().close_m;
			xmlparameter.getParameter("ohlc_volume").getAsDouble(true) = rtdrawableobj.getAsOHLC().volume_m;
		}
		else if (rtdrawableobj.getType() == RtDrawableObj::DO_TEXT) {
			xmlparameter.getParameter("priority").getAsInt(true) = rtdrawableobj.getAsText().priority_m;
			xmlparameter.getParameter("short_text").getAsString(true) = CppUtils::wrapToCDataTags(rtdrawableobj.getAsText().shortText_m ? rtdrawableobj.getAsText().shortText_m : "");
			xmlparameter.getParameter("text").getAsString(true) = CppUtils::wrapToCDataTags(rtdrawableobj.getAsText().text_m ? rtdrawableobj.getAsText().text_m : "");
		
		}
		else if (rtdrawableobj.getType() == RtDrawableObj::DO_LEVEL1) {
			xmlparameter.getParameter("level1_best_bid_price").getAsDouble(true) = rtdrawableobj.getAsLevel1().best_bid_price_m;
			xmlparameter.getParameter("level1_best_ask_price").getAsDouble(true) = rtdrawableobj.getAsLevel1().best_ask_price_m;
			xmlparameter.getParameter("level1_best_bid_volume").getAsDouble(true) = rtdrawableobj.getAsLevel1().best_bid_volume_m;
			xmlparameter.getParameter("level1_best_ask_volume").getAsDouble(true) = rtdrawableobj.getAsLevel1().best_ask_volume_m;
		
		}
		else
			HTASSERT(false);


	}
		
	HLPSTRUCT_EXP void convertDrawableObjFromXmlParameter(RtDrawableObj &rtdrawableobj, XmlParameter const &xmlparameter_const)
	{
		rtdrawableobj.clear();

		double miliss = xmlparameter_const.getParameter("time").getAsDate();
		if (miliss > 0)
			rtdrawableobj.setTime( miliss/ 1000.0 );
		else
			rtdrawableobj.setTime(-1);

		rtdrawableobj.getSymbol() = xmlparameter_const.getParameter("symbol").getAsString() ;
		rtdrawableobj.getProvider() =  xmlparameter_const.getParameter("provider").getAsString();
		rtdrawableobj.getName() = xmlparameter_const.getParameter("name").getAsString();
		rtdrawableobj.getRunName() = xmlparameter_const.getParameter("run_name").getAsString();

		RtDrawableObj::DOType type = (RtDrawableObj::DOType)xmlparameter_const.getParameter("type").getAsInt();
		if (type==RtDrawableObj::DO_LINE_INDIC) {

			HlpStruct::XmlParameter::XmlParameterList const& xmlParamlst =  xmlparameter_const.getXmlParameterList("line_indic");

			rtdrawableobj.getAsLineIndicator().clear();
			for(int i = 0; i < xmlParamlst.size(); i++) {
				HlpStruct::XmlParameter const& xmlParam_i  = xmlParamlst[i];
			
				RtDrawableObj::LineIndicator indic_i(xmlParam_i.getParameter("name").getAsString(), xmlParam_i.getParameter("color").getAsInt(), xmlParam_i.getParameter("val").getAsDouble() );
				rtdrawableobj.getAsLineIndicator().push_back(indic_i);
			}

			

		}
		else if (type==RtDrawableObj::DO_PRINT)
		{
			rtdrawableobj.getAsPrint().ask_m = xmlparameter_const.getParameter("print_ask").getAsDouble();
			rtdrawableobj.getAsPrint().bid_m = xmlparameter_const.getParameter("print_bid").getAsDouble();
			rtdrawableobj.getAsPrint().volume_m = xmlparameter_const.getParameter("print_volume").getAsDouble();
			rtdrawableobj.getAsPrint().color_m = xmlparameter_const.getParameter("print_color").getAsInt();
			rtdrawableobj.getAsPrint().operation_m = xmlparameter_const.getParameter("print_operation").getAsInt();
		}
		else if (type==RtDrawableObj::DO_OHLC)
		{
			rtdrawableobj.getAsOHLC().open_m = xmlparameter_const.getParameter("ohlc_open").getAsDouble();
			rtdrawableobj.getAsOHLC().high_m = xmlparameter_const.getParameter("ohlc_high").getAsDouble();
			rtdrawableobj.getAsOHLC().low_m = xmlparameter_const.getParameter("ohlc_low").getAsDouble();
			rtdrawableobj.getAsOHLC().close_m = xmlparameter_const.getParameter("ohlc_close").getAsDouble();
			rtdrawableobj.getAsOHLC().volume_m = xmlparameter_const.getParameter("ohlc_volume").getAsDouble() ;
		}
		else if (type==RtDrawableObj::DO_TEXT) {
			rtdrawableobj.getAsText().priority_m = xmlparameter_const.getParameter("priority").getAsInt();
			
			
			rtdrawableobj.getAsText().shortText_m[0] = '\0';
			strncat(rtdrawableobj.getAsText().shortText_m, xmlparameter_const.getParameter("short_text").getAsString().c_str(),RtDrawableObj::MAX_SHORT_TEXT_LENGTH) ;
			
			rtdrawableobj.getAsText().text_m[0] = '\0';
			strncat(rtdrawableobj.getAsText().text_m, xmlparameter_const.getParameter("text").getAsString().c_str(), RtDrawableObj::MAX_LONG_TEXT_LENGTH);
		}
		else if (type==RtDrawableObj::DO_LEVEL1) {
			rtdrawableobj.getAsLevel1().best_bid_price_m = xmlparameter_const.getParameter("level1_best_bid_price").getAsDouble();
			rtdrawableobj.getAsLevel1().best_ask_price_m = xmlparameter_const.getParameter("level1_best_ask_price").getAsDouble();
			rtdrawableobj.getAsLevel1().best_bid_volume_m = xmlparameter_const.getParameter("level1_best_bid_volume").getAsDouble();
			rtdrawableobj.getAsLevel1().best_ask_volume_m = xmlparameter_const.getParameter("level1_best_ask_volume").getAsDouble();
		}
		else
			HTASSERT(false);



	}

	void RtDrawableObj::clear()
	{
		symbol_m = "";
		provider_m = "";
		time_m = -1;
		type_m = RtDrawableObj::DO_DUMMY;

		lineIndicList_m.clear();
		print_m.clear();
		ohcl_m.clear();
		level1_m.clear();
		text_m.clear();

	

	}


	
};

