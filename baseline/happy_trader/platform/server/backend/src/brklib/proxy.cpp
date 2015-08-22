#include "proxy.hpp"




namespace BrkLib {

	void Int_Proxy::convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter)
	{
		
		xmlparameter.clear();
		xmlparameter.getParameter("type").getAsString(true) = "Int_Proxy";
		xmlparameter.getParameter("value").getAsInt(true) = get();
	}

	void Int_Proxy::convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter)
	{
		
		get() = xmlparameter.getParameter("value").getAsInt();
	}

	//
	
	void Double_Proxy::convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter)
	{
		
		xmlparameter.clear();
		xmlparameter.getParameter("type").getAsString(true) = "Double_Proxy";
		xmlparameter.getParameter("value").getAsDouble(true) = get();
	}

	void Double_Proxy::convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter)
	{
		
		get() = xmlparameter.getParameter("value").getAsDouble();
	}

	//
	void String_Proxy::convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter)
	{
		xmlparameter.clear();
		xmlparameter.getParameter("type").getAsString(true) = "String_Proxy";
		xmlparameter.getParameter("value").getAsString(true) = CppUtils::hexlify(get());
	}

	void String_Proxy::convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter)
	{
		
		get() = CppUtils::unhexlify( xmlparameter.getParameter("value").getAsString() );
	}

	//
	void Bool_Proxy::convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter)
	{
		xmlparameter.clear();
		xmlparameter.getParameter("type").getAsString(true) = "Bool_Proxy";
		xmlparameter.getParameter("value").getAsInt(true) = get();
	}

	void Bool_Proxy::convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter)
	{
		
		get() = xmlparameter.getParameter("value").getAsInt();
	}

	//
	void BoolList_Proxy::convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter)
	{
		xmlparameter.clear();
		xmlparameter.getParameter("type").getAsString(true) = "BoolList_Proxy";
		
		CppUtils::IntList tmp(get().size());
		for( int i = 0; i < get().size(); i++) 
			tmp[i] = ( get()[i] ? 1:0);
		

		xmlparameter.getParameter("value").getAsIntList(true) = tmp;
	}

	void BoolList_Proxy::convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter)
	{
		CppUtils::IntList const& tmp= xmlparameter.getParameter("value").getAsIntList();
		get().resize(tmp.size());

		for(int i = 0; i < tmp.size(); i++)
			get()[i] = (tmp[i]==1?true:false);
		
	}
		

	//

	void Order_Proxy::convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter)
	{
		xmlparameter.clear();
		xmlparameter.getParameter("type").getAsString(true) = "Order_Proxy";

		HlpStruct::XmlParameter internal;
		get().toXmlParameter(internal);
		xmlparameter.getXmlParameter("value") = internal;

	}

	void Order_Proxy::convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter)
	{
		get().fromXmlParameter(xmlparameter.getXmlParameter("value"));
	}

	

	//
	void PositionList_Proxy::convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter)
	{
		xmlparameter.clear();
		xmlparameter.getParameter("type").getAsString(true) = "PositionList_Proxy";
		PositionList const& poslist = get();

		for(int i = 0; i < poslist.size(); i++) {
			HlpStruct::XmlParameter param;
			poslist[ i ].toXmlParameter(param);

			xmlparameter.getXmlParameterList("value").push_back(param);
		}
	}

	void PositionList_Proxy::convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter)
	{
		
		HlpStruct::XmlParameter::XmlParameterList const& xmlparamlist = xmlparameter.getXmlParameterList("value");
		get().clear();

		for(int i = 0; i < xmlparamlist.size(); i++) {
			
			Position i_pos;
			i_pos.fromXmlParameter(xmlparamlist[i]);
			get().push_back(i_pos);
		}

	}

	// RT Data list proxy
	void RtDataList_Proxy::convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter)
	{
		xmlparameter.clear();
		xmlparameter.getParameter("type").getAsString(true) = "RtDataList_Proxy";
		HlpStruct::RtDataList const& poslist = get();

		for(int i = 0; i < poslist.size(); i++) {
			HlpStruct::XmlParameter param;
		
			HlpStruct::convertToXmlParameter(poslist[ i ], param);

			xmlparameter.getXmlParameterList("value").push_back(param);
		}
	}

	void RtDataList_Proxy::convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter)
	{
		HlpStruct::XmlParameter::XmlParameterList const& xmlparamlist = xmlparameter.getXmlParameterList("value");
		get().clear();

		for(int i = 0; i < xmlparamlist.size(); i++) {
					
			HlpStruct::RtData i_pos;
			HlpStruct::convertFromXmlParameter(i_pos, xmlparamlist[i]);
			get().push_back(i_pos);
		}

	}



	// Uid proxy
	void Uid_Proxy::convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter)
	{
		xmlparameter.clear();
		xmlparameter.getParameter("type").getAsString(true) = "Uid_Proxy";
		xmlparameter.getParameter("value").getAsUid(true) = get();
	}

	void Uid_Proxy::convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter)
	{
		get() =  xmlparameter.getParameter("value").getAsUid();
	}

	
	
	// RT Data
	void RtData_Proxy::convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter)
	{
		xmlparameter.clear();
		xmlparameter.getParameter("type").getAsString(true) = "RtData_Proxy";
		HlpStruct::convertToXmlParameter(get(), xmlparameter.getXmlParameter("value"));
	
	}

	void RtData_Proxy::convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter)
	{
		HlpStruct::XmlParameter const& param_rt=  xmlparameter.getXmlParameter("value");
		HlpStruct::convertFromXmlParameter(get(), param_rt);
	}

	//

	void UidList_Proxy::convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter)
	{
		xmlparameter.clear();
		xmlparameter.getParameter("type").getAsString(true) = "UidList_Proxy";
		
		xmlparameter.getParameter("value").getAsUidList(true) = get();

	
	}
		
	void UidList_Proxy::convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter)
	{
		get() = xmlparameter.getParameter("value").getAsUidList();
		
	}

	//
	
	void StringList_Proxy::convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter)
	{
		
		xmlparameter.clear();
		xmlparameter.getParameter("type").getAsString(true) = "StringList_Proxy";

		// need to decode to avoid any problems with XML wrapping any more!!!

		CppUtils::StringList& dataArray = get();
		for(int i = 0; i < dataArray.size(); i++) {
			dataArray[i] = CppUtils::hexlify(dataArray[i]);
		}
		xmlparameter.getParameter("value").getAsStringList(true) = dataArray;
		
		
		
	}

	void StringList_Proxy::convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter)
	{
		
		get() = xmlparameter.getParameter("value").getAsStringList();
		CppUtils::StringList& dataArray = get();

		for(int i = 0; i < dataArray.size(); i++) {
			dataArray[i] = CppUtils::unhexlify(dataArray[i]);
		}
			
			
	}

	//
	void IntList_Proxy::convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter)
	{
		xmlparameter.clear();
		xmlparameter.getParameter("type").getAsString(true) = "IntList_Proxy";
		xmlparameter.getParameter("value").getAsIntList(true) = get();
	}

	void IntList_Proxy::convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter)
	{
		get() = xmlparameter.getParameter("value").getAsIntList();
	}
	
	
	
};