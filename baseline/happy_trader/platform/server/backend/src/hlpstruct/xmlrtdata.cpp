#include "xmlrtdata.hpp"


namespace HlpStruct {

	void convertToXmlParameter(RtData const& rtdata, XmlParameter &xmlparameter)
	{
		xmlparameter.clear();

		xmlparameter.getName() = "rt_packet";
		xmlparameter.getParameter("symbol").getAsString(true) = rtdata.symbol_m;
		xmlparameter.getParameter("provider").getAsString(true) = rtdata.provider_m;
		xmlparameter.getParameter("time").getAsDate(true) = rtdata.time_m*1000.0;
		xmlparameter.getParameter("type").getAsInt(true) = rtdata.type_m;
		xmlparameter.getParameter("aflag").getAsInt(true) = rtdata.aflag_m;

		if (rtdata.type_m==RtData::RT_Type) {
			xmlparameter.getParameter("bid").getAsDouble(true) = rtdata.tickdata_m.rt_m.bid_m;
			xmlparameter.getParameter("ask").getAsDouble(true) = rtdata.tickdata_m.rt_m.ask_m;
			xmlparameter.getParameter("volume").getAsDouble(true) = rtdata.tickdata_m.rt_m.volume_m;
			xmlparameter.getParameter("color").getAsInt(true) = rtdata.tickdata_m.rt_m.color_m;
			xmlparameter.getParameter("operation").getAsInt(true) = rtdata.tickdata_m.rt_m.operation_m;
		}
		else if (rtdata.type_m==RtData::HST_Type) {
			
			xmlparameter.getParameter("open").getAsDouble(true) = rtdata.tickdata_m.hist_m.open_m;
			xmlparameter.getParameter("high").getAsDouble(true) = rtdata.tickdata_m.hist_m.high_m;
			xmlparameter.getParameter("low").getAsDouble(true) = rtdata.tickdata_m.hist_m.low_m;
			xmlparameter.getParameter("close").getAsDouble(true) = rtdata.tickdata_m.hist_m.close_m;

			xmlparameter.getParameter("open2").getAsDouble(true) = rtdata.tickdata_m.hist_m.open2_m;
			xmlparameter.getParameter("high2").getAsDouble(true) = rtdata.tickdata_m.hist_m.high2_m;
			xmlparameter.getParameter("low2").getAsDouble(true) = rtdata.tickdata_m.hist_m.low2_m;
			xmlparameter.getParameter("close2").getAsDouble(true) = rtdata.tickdata_m.hist_m.close2_m;

			xmlparameter.getParameter("volume").getAsDouble(true) = rtdata.tickdata_m.hist_m.volume_m;
		}
		//else
			//HTASSERT(false);
	}

	void convertFromXmlParameter(RtData &rtdata, XmlParameter const &xmlparameter_const)
	{
		// cast const
		XmlParameter &xmlparameter = const_cast<XmlParameter&>(xmlparameter_const);

		HTASSERT(xmlparameter.getName()=="rt_packet");
			
		
		int len = xmlparameter.getParameter("symbol").getAsString().size();
		if (len > MAX_SYMB_LENGTH)
			len = MAX_SYMB_LENGTH;
		strncpy(rtdata.symbol_m, xmlparameter.getParameter("symbol").getAsString().c_str(), len);
		rtdata.symbol_m[len] = '\0';

		len = xmlparameter.getParameter("provider").getAsString().size();
		if (len > MAX_SYMB_LENGTH)
			len = MAX_SYMB_LENGTH;

		strncpy(rtdata.provider_m, xmlparameter.getParameter("provider").getAsString().c_str(), len);
		rtdata.provider_m[len] = '\0';
		//we  ned to have a number of seconds!
		double const& miliss = xmlparameter.getParameter("time").getAsDate();
		if (miliss > 0)
			rtdata.time_m = miliss/ 1000.0;
		else
			rtdata.time_m = -1.0;

		
		rtdata.type_m = (CppUtils::Byte)xmlparameter.getParameter("type").getAsInt();
		rtdata.aflag_m = (CppUtils::Byte)xmlparameter.getParameter("aflag").getAsInt(); 

		if (rtdata.type_m==RtData::RT_Type) {
			rtdata.tickdata_m.rt_m.bid_m = xmlparameter.getParameter("bid").getAsDouble();
			rtdata.tickdata_m.rt_m.ask_m = xmlparameter.getParameter("ask").getAsDouble();
			rtdata.tickdata_m.rt_m.volume_m = xmlparameter.getParameter("volume").getAsDouble();
			rtdata.tickdata_m.rt_m.color_m = xmlparameter.getParameter("color").getAsInt();
			rtdata.tickdata_m.rt_m.operation_m = xmlparameter.getParameter("operation").getAsInt();
			
				
		}
		else if (rtdata.type_m==RtData::HST_Type) {
			rtdata.tickdata_m.hist_m.open_m = xmlparameter.getParameter("open").getAsDouble();
			rtdata.tickdata_m.hist_m.high_m = xmlparameter.getParameter("high").getAsDouble();
			rtdata.tickdata_m.hist_m.low_m = xmlparameter.getParameter("low").getAsDouble();
			rtdata.tickdata_m.hist_m.close_m = xmlparameter.getParameter("close").getAsDouble();

			rtdata.tickdata_m.hist_m.open2_m = xmlparameter.getParameter("open2").getAsDouble();
			rtdata.tickdata_m.hist_m.high2_m = xmlparameter.getParameter("high2").getAsDouble();
			rtdata.tickdata_m.hist_m.low2_m = xmlparameter.getParameter("low2").getAsDouble();
			rtdata.tickdata_m.hist_m.close2_m = xmlparameter.getParameter("close2").getAsDouble();

			rtdata.tickdata_m.hist_m.volume_m = xmlparameter.getParameter("volume").getAsDouble();
		}
		//else 
			//HTASSERT(false);

		
	}

	void convertToBinary(RtData const& rtdata, CppUtils::MemoryChunk& buffer)
	{
		buffer.length(sizeof(RtData));
		memcpy(buffer.data(), (void*)&rtdata, sizeof(RtData));
	}
		
	void convertFromBinary(RtData &rtdata, CppUtils::MemoryChunk const& buffer)
	{
		if (buffer.length() == sizeof(RtData)) {
			memcpy((void*)&rtdata, buffer.data(), sizeof(RtData));
		}
		else
			THROW(CppUtils::OperationFailed, "exc_InvalidRtDataBuffer", "ctx_convertRtDataFromBinary", "" );
	}

	

};// end of namepspace
