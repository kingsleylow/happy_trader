#ifndef _HLPSTRUCT_XMLRTDATADISKBUF_INCLUDED
#define _HLPSTRUCT_XMLRTDATADISKBUF_INCLUDED

#include "xmlrtdata.hpp"
namespace HlpStruct {
	#pragma pack(push, 1)


	/**
	* This is the buffer to store RtData class in the disk buffer - to avoid unnecessary storage consumptions
	*/

	struct HLPSTRUCT_EXP RtBufferData {
		friend HLPSTRUCT_EXP void bufferToRtData(RtBufferData const& rtbufdata, RtData &rtdata);
		
		friend HLPSTRUCT_EXP void bufferFromRtData(RtData const &rtdata, RtBufferData & rtbufdata);

	
	public:
		inline double const& getTime()
		{
			return time_m;
		}

	private:

		RtData::TickerType tickdata_m; 
		
		CppUtils::Byte type_m;
		
		double time_m;
		
		CppUtils::Byte aflag_m;
	};
	#pragma pack (pop)



// non member functions
HLPSTRUCT_EXP void bufferToRtData(RtBufferData const& rtbufdata, RtData &rtdata);
		
HLPSTRUCT_EXP void bufferFromRtData(RtData const &rtdata, RtBufferData & rtbufdata);

};

#endif