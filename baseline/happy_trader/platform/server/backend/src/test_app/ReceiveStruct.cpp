#include "ReceiveStruct.hpp"


WLSendStructure::WLSendStructure()
{
		clear();
}

LPVOID WLSendStructure::getData()
{
	return buff_m;
}

void WLSendStructure::clear()
{
	memset(buff_m, '\0', WL_STRUCT_LENGTH);
}


// returns double value from offset (8 bytes)
double WLSendStructure::getDoubleValue(int offset) const
{
	double result = -1;
	memcpy(&result, buff_m + offset, 8);
	return result;
}

// returns long value (4 bytes)
long WLSendStructure::getLongValue(int offset) const
{
	long result = -1;
	memcpy(&result, buff_m + offset, 4);
	return result;
}
