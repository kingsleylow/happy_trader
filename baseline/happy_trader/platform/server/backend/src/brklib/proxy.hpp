#ifndef _BRKLIB_PROXY_INCLUDED
#define _BRKLIB_PROXY_INCLUDED

#include "brklibdefs.hpp"
#include "brokerstruct.hpp"
#include "brkconnect.hpp"


namespace BrkLib {

	// int proxy
	class BRKLIB_EXP Int_Proxy : public HlpStruct::ProxyBase<long>
	{
	public:
		virtual void convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter);

		virtual void convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter);

		
	};

	// double proxy
	class BRKLIB_EXP Double_Proxy : public HlpStruct::ProxyBase<long>
	{
	public:
		virtual void convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter);

		virtual void convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter);

		
	};

	// string proxy
	class BRKLIB_EXP String_Proxy : public HlpStruct::ProxyBase<CppUtils::String>
	{
	public:
		virtual void convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter);

		virtual void convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter);
	};

	// bool proxy
	class BRKLIB_EXP Bool_Proxy : public HlpStruct::ProxyBase<bool>
	{
	public:
		virtual void convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter);

		virtual void convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter);
	};

	// bool proxy
	class BRKLIB_EXP BoolList_Proxy : public HlpStruct::ProxyBase< vector<bool> >
	{
	public:
		virtual void convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter);

		virtual void convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter);
	};

	

	// this incapsulates order agains broker server
	class BRKLIB_EXP Order_Proxy :public HlpStruct::ProxyBase<Order>
	{
		public:
		virtual void convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter);

		virtual void convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter);
	};




	// position list proxy
	class BRKLIB_EXP PositionList_Proxy: public HlpStruct::ProxyBase<PositionList>
	{
	public:
		virtual void convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter);

		virtual void convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter);

	};

	// rt data list proxy
	class BRKLIB_EXP RtDataList_Proxy: public HlpStruct::ProxyBase<HlpStruct::RtDataList>
	{
	public:
		virtual void convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter);

		virtual void convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter);

	};

	// position list proxy
	class BRKLIB_EXP Uid_Proxy: public HlpStruct::ProxyBase<CppUtils::Uid>
	{
	public:
		virtual void convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter);

		virtual void convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter);

	};

	
	// RT Data proxy
	class BRKLIB_EXP RtData_Proxy: public HlpStruct::ProxyBase<HlpStruct::RtData>
	{
	public:
		virtual void convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter);

		virtual void convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter);

	};

	// position list proxy
	class BRKLIB_EXP UidList_Proxy: public HlpStruct::ProxyBase<CppUtils::UidList>
	{
	public:
		virtual void convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter);

		virtual void convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter);

	};

	
	// position list proxy
	class BRKLIB_EXP StringList_Proxy: public HlpStruct::ProxyBase< CppUtils::StringList >
	{
	public:
		virtual void convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter);

		virtual void convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter);

	};

	// position list proxy
	class BRKLIB_EXP IntList_Proxy: public HlpStruct::ProxyBase< CppUtils::IntList >
	{
	public:
		virtual void convertToXmlParameter(HlpStruct::XmlParameter &xmlparameter);

		virtual void convertFromXmlparameter(HlpStruct::XmlParameter const &xmlparameter);

	};
	
	
	
	// creator class
	class BrokerProxyBaseCreator : public HlpStruct::ProxyBaseCreator {
		BEGIN_PROXY_CREATOR()
			PROXY_CREATOR("Int_Proxy", Int_Proxy);
			PROXY_CREATOR("Double_Proxy", Double_Proxy);
			PROXY_CREATOR("Bool_Proxy", Bool_Proxy);
			PROXY_CREATOR("BoolList_Proxy", BoolList_Proxy);
			PROXY_CREATOR("String_Proxy", String_Proxy);
			PROXY_CREATOR("Uid_Proxy", Uid_Proxy);
			PROXY_CREATOR("RtData_Proxy", RtData_Proxy);
			PROXY_CREATOR("RtDataList_Proxy", RtDataList_Proxy);
			PROXY_CREATOR("UidList_Proxy", UidList_Proxy);
			PROXY_CREATOR("StringList_Proxy", StringList_Proxy);
			PROXY_CREATOR("IntList_Proxy", IntList_Proxy);
			PROXY_CREATOR("Order_Proxy", Order_Proxy);
			PROXY_CREATOR("PositionList_Proxy", PositionList_Proxy);
		END_PROXY_CREATOR()
	};

};


#endif