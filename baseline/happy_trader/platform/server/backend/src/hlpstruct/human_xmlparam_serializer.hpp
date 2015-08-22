#ifndef _HLPSTRUCT_HUMAN_XMLPARAM_SERIALIZER_INCLUDED
#define _HLPSTRUCT_HUMAN_XMLPARAM_SERIALIZER_INCLUDED

#include "xmlparameter.hpp"

namespace HlpStruct {

// base class for performiung simple serialization/deserialization
// strings entries only!
class HLPSTRUCT_EXP HumanSerializer
{
public:
	static void serializeXmlParameter(XmlParameter const& xmlparameter, CppUtils::String& content);

	static void deserializeXmlParameter(CppUtils::String const& content, XmlParameter& xmlparameter);
};

};

#endif