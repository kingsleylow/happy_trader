#ifndef _MATH_MATHDECL_INCLUDED
#define _MATH_MATHDECL_INCLUDED

namespace Math {


	// to avoid overflows 
	#define MATH_PRACTICAL_ZERO 0.00001

	// what price to use from CppUtils::PriceData
	enum CandlePrices {
		P_OpenAsk = 1,
		P_HighAsk = 2,
		P_LowAsk = 3,
		P_CloseAsk = 4,
		P_OpenBid = 5,
		P_HighBid = 6,
		P_LowBid = 7,
		P_CloseBid = 8,
		// if we need whole candles...
		P_Bid = 9,
		P_Ask = 10,

		// volume
		P_Volume = 11
	
	};
	
	enum ResultConst {
		RESULT_INVALID = -99999
	};

	enum MathConst {
		MC_NPOS = -9999
	};

	
	
}



#endif
