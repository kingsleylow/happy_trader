#ifndef _WININET_RECEIVESTRUCT_HPP
#define _WININET_RECEIVESTRUCT_HPP

#include "wininetwrap.hpp"

class WLSendStructure 
{
public:
	
	// length of packet
	static const int WL_STRUCT_LENGTH = 2+8*12;

	// ---------------------

	// types of packet
	static const BYTE FLAG_TYPE_DUMMY = 0;

	// the history data packet
	static const BYTE FLAG_TYPE_HISTORY = 1;

	// the candle is unclosed (history)
	static const BYTE FLAG_TYPE_UNCLOSED = 2;
	
	// the candle is unclosed (RT)
	static const BYTE FLAG_TYPE_UNCLOSED_RT = 3;

	// the candle is unclosed
	static const BYTE FLAG_TYPE_BIDASK = 4;

	// new candle arrived
	static const BYTE FLAG_TYPE_NEW_CANDLE = 5;
	
	// the next packets will be RT
	static const BYTE FLAG_TYPE_HISTORYSEND_FINISHED = 6;

	// error happend
	static const BYTE FLAG_TYPE_ERROR = 7;

	// ---------------------------

	WLSendStructure();
	

	inline BYTE getPacketType() const
	{
		return buff_m[0];
	}

	inline double getUnixTime() const
	{
		return getDoubleValue(2);
	
	}

	// ask...
	inline double getOpen() const
	{
		return getDoubleValue(2+8);
	}

	inline double getHigh() const
	{
		return getDoubleValue(2+8*2);
	}

	inline double getLow() const
	{
		return getDoubleValue(2+8*3);
	}

	inline double getClose() const
	{
		return getDoubleValue(2+8*4);
	}

	// bid...
	inline double getOpen2() const
	{
		return getDoubleValue(2+8*5);
	}

	inline double getHigh2() const
	{
		return getDoubleValue(2+8*6);
	}

	inline double getLow2() const
	{
		return getDoubleValue(2+8*7);
	}

	inline double getClose2() const
	{
		return getDoubleValue(2+8*8);
	}

	// 

	inline double getVolume() const
	{
		return getDoubleValue(2+8*9) ;
	}

	// 
	inline double getBid() const
	{
		return getDoubleValue(2+8*10);
	}
	
	inline double getAsk() const
	{
		return getDoubleValue(2+8*11);
	}

	LPVOID getData();
	
	void clear();
		
private:

	// returns double value from offset (8 bytes)
	double getDoubleValue(int offset) const;


	// returns long value (4 bytes)
	long getLongValue(int offset) const;


	BYTE buff_m[WL_STRUCT_LENGTH];
};

#endif