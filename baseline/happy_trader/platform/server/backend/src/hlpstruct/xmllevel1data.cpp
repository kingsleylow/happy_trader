#include "xmllevel1data.hpp"

namespace HlpStruct {

	void convertLevel1ToXmlParameter(RtLevel1Data const& rtlevel1data, XmlParameter &xmlparameter)
	{
		xmlparameter.getName() = "level1_packet";
		xmlparameter.getParameter("symbol").getAsString(true) = rtlevel1data.symbol_m;
		xmlparameter.getParameter("provider").getAsString(true) = rtlevel1data.provider_m;

		if (rtlevel1data.time_m <=0)
			xmlparameter.getParameter("time").getAsDate(true)= -1;
		else
			xmlparameter.getParameter("time").getAsDate(true) = rtlevel1data.time_m * 1000.0;
		

		xmlparameter.getParameter("best_bid_price").getAsDouble(true) = rtlevel1data.best_bid_price_m;
		xmlparameter.getParameter("best_ask_price").getAsDouble(true) = rtlevel1data.best_ask_price_m;
		xmlparameter.getParameter("best_bid_volume").getAsDouble(true) = rtlevel1data.best_bid_volume_m;
		xmlparameter.getParameter("best_ask_volume").getAsDouble(true) = rtlevel1data.best_ask_volume_m;
		xmlparameter.getParameter("open_interest").getAsDouble(true) = rtlevel1data.open_interest_m;

	}

	void convertLevel1FromXmlParameter(RtLevel1Data &rtlevel1data, XmlParameter const &xmlparameter_const)
	{
		// cast const
		XmlParameter &xmlparameter = const_cast<XmlParameter&>(xmlparameter_const);

		HTASSERT(xmlparameter.getName()=="level1_packet");
		rtlevel1data.clear();
		
		int len = xmlparameter.getParameter("symbol").getAsString().size();
		if (len > MAX_SYMB_LENGTH)
			len = MAX_SYMB_LENGTH;
		strncpy(rtlevel1data.symbol_m, xmlparameter.getParameter("symbol").getAsString().c_str(), len);
		rtlevel1data.symbol_m[len] = '\0';

		len = xmlparameter.getParameter("provider").getAsString().size();
		if (len > MAX_SYMB_LENGTH)
			len = MAX_SYMB_LENGTH;

		strncpy(rtlevel1data.provider_m, xmlparameter.getParameter("provider").getAsString().c_str(), len);
		rtlevel1data.provider_m[len] = '\0';
		//we  ned to have a number of seconds!
		double miliss = xmlparameter.getParameter("time").getAsDate();
		if (miliss > 0)
			rtlevel1data.time_m = miliss/ 1000.0;
		else
			rtlevel1data.time_m = -1;

		//
		rtlevel1data.best_ask_price_m = xmlparameter.getParameter("best_ask_price").getAsDouble();
		rtlevel1data.best_bid_price_m = xmlparameter.getParameter("best_bid_price").getAsDouble();
		rtlevel1data.best_bid_volume_m = xmlparameter.getParameter("best_bid_volume").getAsDouble();
		rtlevel1data.best_ask_volume_m = xmlparameter.getParameter("best_ask_volume").getAsDouble();
		rtlevel1data.open_interest_m = xmlparameter.getParameter("open_interest").getAsDouble();
		

	}
	
	void convertToBinary(RtLevel1Data const& rtlevel1data, CppUtils::MemoryChunk& buffer)
	{
		buffer.length(sizeof(RtLevel1Data));
		memcpy(buffer.data(), (void*)&rtlevel1data, sizeof(RtLevel1Data));
	}
		
	void convertFromBinary(RtLevel1Data &rtlevel1data, CppUtils::MemoryChunk const& buffer)
	{
		if (buffer.length() == sizeof(RtLevel1Data)) {
			memcpy((void*)&rtlevel1data, buffer.data(), sizeof(RtLevel1Data));
		}
		else
			THROW(CppUtils::OperationFailed, "exc_InvalidRtLevel1DataBuffer", "ctx_convertRtLevel1DataFromBinary", "" );
	}

}; // end of namespace