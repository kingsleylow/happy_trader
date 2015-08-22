#ifndef _HLPSTRUCT_SERIALIZEHELPER_INCLUDED
#define _HLPSTRUCT_SERIALIZEHELPER_INCLUDED

#include "xmlparameter.hpp"


namespace HlpStruct {

template<class T>
class SerialImpl {
public:
	typedef T TheClass;
};

template<class T>
class SerialDeriveImpl: public T {
public:
	typedef T TheClass;
};



// helper function to serialize vectors
template <class T>
CppUtils::String serializeList(vector<T> const& list)
{
	HlpStruct::XmlParameter wholeparam;
	for(int i = 0; i < list.size(); i++) {
		HlpStruct::XmlParameter param;
		list[ i ].toXmlParameter(param);

		wholeparam.getXmlParameterList("list").push_back(param);
	}
	CppUtils::String content;	
	HlpStruct::serializeXmlParameter(wholeparam, content);	

	return content;
};

}; // end of namespace

#define BEGIN_SERIALIZE_MAP()	\
	public:	\
	CppUtils::String toString() const {	\
		CppUtils::String content;	\
		HlpStruct::XmlParameter param;	\
		toXmlParameter(param);	\
		HlpStruct::serializeXmlParameter(param, content);	\
		return content;	\
	};	\
	\
	void fromString(CppUtils::String const& content)	\
	{	\
		HlpStruct::XmlParameter param;	\
		HlpStruct::deserializeXmlParameter(param, content);	\
		fromXmlParameter(param);	\
	}	\
	void fromXmlParameter(HlpStruct::XmlParameter const& param)	\
	{	\
		HlpStruct::XmlParameter& param_non_const = const_cast<HlpStruct::XmlParameter&>(param);	\
		processSerial(param_non_const, false);	\
	}	\
	\
	void toXmlParameter(HlpStruct::XmlParameter& param) const	\
	{	\
		TheClass const* this_2 = this;	\
		const_cast<TheClass*>(this_2)->processSerial(param, true);	\
	}	\
	\
	virtual void processSerial(HlpStruct::XmlParameter& param, bool isSerialize){	\
		if (isSerialize) {	\
			param.clear();	\
		};	\
		

#define	END_SERIALIZE_MAP()	\
	};	\
	public:	

#define PROCESS_BASE()	\
	TheClass::processSerial(param, isSerialize);

#define INT_ENTRY(N)	\
	if (isSerialize) {	\
		param.getParameter(#N).getAsInt(true) = (long)N;	\
	}	\
	else {	\
		N = param.getParameter(#N).getAsInt();	\
	};	\

#define DOUBLE_ENTRY(N)	\
	if (isSerialize) {	\
		param.getParameter(#N).getAsDouble(true) = (double)N;	\
	}	\
	else {	\
		N = param.getParameter(#N).getAsDouble();	\
	};	

#define STRING_ENTRY(N)	\
	if (isSerialize) {	\
		param.getParameter(#N).getAsString(true) = N;	\
	}	\
	else {	\
		N = param.getParameter(#N).getAsString();	\
	};	

#define UID_ENTRY(N)	\
	if (isSerialize) {	\
		param.getParameter(#N).getAsUid(true) = N;	\
	}	\
	else {	\
		N = param.getParameter(#N).getAsUid();	\
	};


// versions requiring explicit type cast
#define INT_ENTRY2(N,T)	\
	if (isSerialize) {	\
		param.getParameter(#N).getAsInt(true) = (long)N;	\
	}	\
	else {	\
		N = (T)param.getParameter(#N).getAsInt();	\
	};	\

/**
* This is intended only for special types of context - i.e. string context
*/
#define STRINGCONTEXT_ENTRY(N)	\
	if (isSerialize) {	\
		BrkLib::BrokerContext_String const* ptr = dynamic_cast<BrkLib::BrokerContext_String const*>(N.getContext());		\
		if (ptr) {	\
			param.getParameter(#N).getAsString(true) = ptr->getStringData();	\
		}	\
	}	\
	else {	\
		BrokerContext_String ctx_str;	\
		ctx_str.getStringData() = param.getParameter(#N).getAsString();	\
		N.setContext( ctx_str );	\
	};	\

	
#endif