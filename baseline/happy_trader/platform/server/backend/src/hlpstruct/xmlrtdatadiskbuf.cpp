#include "xmlrtdatadiskbuf.hpp"


namespace HlpStruct {

void bufferToRtData(RtBufferData const& rtbufdata, RtData &rtdata)
{
	// no need to clear this all
	// clear only what we got
	if (rtbufdata.type_m==RtData::RT_Type) {
		memcpy(&rtdata.getRtData(), &rtbufdata.tickdata_m.rt_m, sizeof(RtData::TickerType::RtInstType) );
	}
	else if (rtbufdata.type_m==RtData::HST_Type) {
		memcpy(&rtdata.getHistData(), &rtbufdata.tickdata_m.hist_m,  sizeof(RtData::TickerType::RtHistType) );
	}
	else
		HTASSERT(false);
	rtdata.getAFlag() = rtbufdata.aflag_m;
	rtdata.getType() = rtbufdata.type_m;
	rtdata.getTime() = rtbufdata.time_m;
}
		
void bufferFromRtData(RtData const &rtdata, RtBufferData & rtbufdata)
{
	memset(&rtbufdata, '\0', sizeof(RtBufferData));
	
	rtbufdata.type_m = rtdata.getType();
	rtbufdata.aflag_m = rtdata.getAFlag();
	rtbufdata.time_m = rtdata.getTime();

	if (rtbufdata.type_m==RtData::RT_Type) {
		memcpy(&rtbufdata.tickdata_m.rt_m, &rtdata.getRtData(), sizeof(RtData::TickerType::RtInstType) );
	}
	else if (rtbufdata.type_m==RtData::HST_Type) {
		memcpy(&rtbufdata.tickdata_m.hist_m, &rtdata.getHistData(), sizeof(RtData::TickerType::RtHistType) );

	}
	else
		HTASSERT(false);


}

}; // end of namespace
