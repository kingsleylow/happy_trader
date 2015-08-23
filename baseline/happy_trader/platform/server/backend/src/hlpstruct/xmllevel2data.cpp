#include "xmllevel2data.hpp"

namespace HlpStruct {
	
	

	void convertToBinary(RtLevel2Data const& rtlevel2data, CppUtils::MemoryChunk& buffer)
	{
		
		
	
		int global_size = MAX_SYMB_LENGTH + 1 + MAX_SYMB_LENGTH + 1 + sizeof(rtlevel2data.idxBestAsk_m) + sizeof(rtlevel2data.idxBestBid_m) +
			sizeof(rtlevel2data.time_m) +	
			sizeof(rtlevel2data.dataBinarySize_m) +
			sizeof(rtlevel2data.dataSize_m) + rtlevel2data.dataBinarySize_m;

		
		size_t ptr = 0;
		buffer.length(global_size);
		
		memcpy(buffer.data(), rtlevel2data.symbol_m,MAX_SYMB_LENGTH + 1 );
		ptr += (MAX_SYMB_LENGTH + 1);
		
		memcpy(buffer.data() + ptr, rtlevel2data.provider_m,  MAX_SYMB_LENGTH + 1 );
		ptr += (MAX_SYMB_LENGTH + 1);

		
		memcpy(buffer.data() + ptr, &rtlevel2data.time_m,sizeof(rtlevel2data.time_m));
		ptr += sizeof(rtlevel2data.time_m);

		// idx best bid and ask
		memcpy(buffer.data() + ptr, &rtlevel2data.idxBestBid_m,sizeof(rtlevel2data.idxBestBid_m));
		ptr += sizeof(rtlevel2data.idxBestBid_m);

		memcpy(buffer.data() + ptr, &rtlevel2data.idxBestAsk_m,sizeof(rtlevel2data.idxBestAsk_m));
		ptr += sizeof(rtlevel2data.idxBestAsk_m);

		// save whole size
		memcpy(buffer.data() + ptr, &rtlevel2data.dataBinarySize_m,sizeof(rtlevel2data.dataBinarySize_m));
		ptr += sizeof(rtlevel2data.dataBinarySize_m);

		// store just number of elements
		memcpy(buffer.data() + ptr, &rtlevel2data.dataSize_m,sizeof(rtlevel2data.dataSize_m));
		ptr += sizeof(rtlevel2data.dataSize_m);
		
		if (rtlevel2data.dataSize_m > 0) {

			memcpy(buffer.data() + ptr, &rtlevel2data.data_m[0], rtlevel2data.dataBinarySize_m);

			/*
			// loop to store consequently
			for(int i=0; i < rtlevel2data.dataSize_m; i++) {
				memcpy(buffer.data() + ptr, &rtlevel2data.data_m[i], sizeof(Level2Entry));
				ptr += sizeof(Level2Entry);
			}
			*/
		}
		
		
		
	}
			
	void convertFromBinary(RtLevel2Data &rtlevel2data, CppUtils::MemoryChunk const& buffer)
	{
		 // begin reading

		rtlevel2data.clear();

		
		size_t ptr = 0;
		memcpy(rtlevel2data.symbol_m, buffer.data(), MAX_SYMB_LENGTH + 1 );
		ptr += (MAX_SYMB_LENGTH + 1);

		memcpy(rtlevel2data.provider_m, buffer.data()+ptr, MAX_SYMB_LENGTH + 1 );
		ptr += (MAX_SYMB_LENGTH + 1);

		memcpy(&rtlevel2data.time_m, buffer.data()+ptr, sizeof(rtlevel2data.time_m) );
		ptr += sizeof(rtlevel2data.time_m);

		// bid & ask
		memcpy(&rtlevel2data.idxBestBid_m, buffer.data()+ptr, sizeof(rtlevel2data.idxBestBid_m) );
		ptr += sizeof(rtlevel2data.idxBestBid_m);

		memcpy(&rtlevel2data.idxBestAsk_m, buffer.data()+ptr, sizeof(rtlevel2data.idxBestAsk_m) );
		ptr += sizeof(rtlevel2data.idxBestAsk_m);

		// read whole size
		memcpy(&rtlevel2data.dataBinarySize_m, buffer.data()+ptr,sizeof(rtlevel2data.dataBinarySize_m));
		ptr += sizeof(rtlevel2data.dataBinarySize_m);


		// read number of elemnst
		memcpy(&rtlevel2data.dataSize_m, buffer.data()+ptr,sizeof(rtlevel2data.dataSize_m));		
		ptr += sizeof(rtlevel2data.dataSize_m);

		if (rtlevel2data.dataBinarySize_m > 0) {
			rtlevel2data.data_m = new Level2Entry[rtlevel2data.dataSize_m];
			memcpy(&rtlevel2data.data_m[0], buffer.data()+ptr, rtlevel2data.dataBinarySize_m);
			
			/*
			for(int i = 0; i <rtlevel2data.dataSize_m; i++) { 
				memcpy(	&rtlevel2data.data_m[i], buffer.data()+ptr, sizeof(Level2Entry)  );
				ptr += sizeof(Level2Entry);
			}
			*/

		}
		
		
		
	}

}; // end of namespace