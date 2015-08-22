#include "deserial.hpp"

namespace BrkLib {

	void DataRowDescriptor::registerColumn(CppUtils::String const& columnName, FieldData const& fielddata)
	{
		columnMap_m[ columnName ] = fielddata;
	}

	FieldData const& DataRowDescriptor::getFieldData(CppUtils::String const& columnName) const
	{
		map<CppUtils::String, FieldData>::const_iterator it = columnMap_m.find(columnName);
		if (it==columnMap_m.end()) {
			static const FieldData dummy;
			return dummy;
		}

		return it->second;
	}

	
	// -------------------------


}; // end of namespace

