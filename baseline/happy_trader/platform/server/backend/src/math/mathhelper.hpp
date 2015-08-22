#ifndef _MATH_MATHHELPER_INCLUDED
#define _MATH_MATHHELPER_INCLUDED

#include "mathdecl.hpp"
#include "mathdefs.hpp"




namespace Math {
	
	
		
template<Math::CandlePrices const typeOfPrice> 
double calcCorrelation(
		HlpStruct::PriceDataList const& xList, 
		HlpStruct::PriceDataList const& yList
	)
{
	float result = MC_NPOS;
	
	if (xList.size() != yList.size())
		return result;

	
	double x_avr = calcSimpleAvr<typeOfPrice>(xList);
	double y_avr = calcSimpleAvr<typeOfPrice>(yList);


	float c_numerator = 0;
	float c_denom1 = 0;
	float c_denom2 = 0;

	
	for(int i = 0; i < xList.size(); i++) {

		float const& price_xi = GetPrice2<typeOfPrice>::getCandlePrice(xList[ i ]);
		float const& price_yi = GetPrice2<typeOfPrice>::getCandlePrice(yList[ i ]);

		c_numerator += (price_xi - x_avr)*(price_yi-y_avr);

		c_denom1 += (price_xi - x_avr)*(price_xi - x_avr);
		c_denom2 += (price_yi-y_avr)*(price_yi-y_avr);


	};

	c_denom1 = sqrt(c_denom1);
	c_denom2 = sqrt(c_denom2);
	
	result = c_numerator/(c_denom1*c_denom2);

	
	
	return result;

};

template<Math::CandlePrices const typeOfPrice> 
double calcSimpleAvr(HlpStruct::PriceDataList const& priceList)
{
	
	float result = MC_NPOS;
	
	if (priceList.empty())
		return -1;

	float prev_result = -1;
	int idx = 1;
	for(int i = 0; i <  priceList.size(); i++) {
		float price_i = GetPrice2<typeOfPrice>::getCandlePrice(priceList[ i ]);

		if (i==0) {
			result = price_i;
			prev_result = result;
			continue;
		};

		result = (prev_result * idx + price_i ) / (idx + 1);

		prev_result = result;
		idx++;
		
	}

	return result;
}


// ------------------------

template<Math::CandlePrices> 
struct GetPrice2
{
	inline static float const& getCandlePrice(HlpStruct::PriceData const& priceData) 
	{
		THROW(CppUtils::OperationFailed, "exc_InvalidTemplateComiplation", "ctx_getCandlePrice", "");
	}

	inline static float const& getClosePrice(HlpStruct::PriceData const& priceData) 
	{
		THROW(CppUtils::OperationFailed, "exc_InvalidTemplateComiplation", "ctx_getClosePrice", "");
	}

	inline static float const& getHighPrice(HlpStruct::PriceData const& priceData) 
	{
		THROW(CppUtils::OperationFailed, "exc_InvalidTemplateComiplation", "ctx_getHighPrice", "");
	}

	inline static float const& getLowPrice(HlpStruct::PriceData const& priceData) 
	{
		THROW(CppUtils::OperationFailed, "exc_InvalidTemplateComiplation", "ctx_getLowPrice", "");
	}

	inline static float const& getOpenPrice(HlpStruct::PriceData const& priceData) 
	{
		THROW(CppUtils::OperationFailed, "exc_InvalidTemplateComiplation", "ctx_getOpenPrice", "");
	}

	
};

template<> 
struct GetPrice2<Math::P_Volume> 
{ 
    inline static float const& getCandlePrice(HlpStruct::PriceData const& priceData) 
    {
			return priceData.volume_m;
    } 

		inline static float const& getClosePrice(HlpStruct::PriceData const& priceData) 
		{
			THROW(CppUtils::OperationFailed, "exc_InvalidTemplateComiplation", "ctx_getClosePrice", "");
		}

		inline static float const& getHighPrice(HlpStruct::PriceData const& priceData) 
		{
			THROW(CppUtils::OperationFailed, "exc_InvalidTemplateComiplation", "ctx_getHighPrice", "");
		}

		inline static float const& getLowPrice(HlpStruct::PriceData const& priceData) 
		{
			THROW(CppUtils::OperationFailed, "exc_InvalidTemplateComiplation", "ctx_getLowPrice", "");
		}

		inline static float const& getOpenPrice(HlpStruct::PriceData const& priceData) 
		{
			THROW(CppUtils::OperationFailed, "exc_InvalidTemplateComiplation", "ctx_getOpenPrice", "");
		}
		
};


template<> 
struct GetPrice2<Math::P_CloseAsk> 
{ 
    inline static float const& getCandlePrice(HlpStruct::PriceData const& priceData) 
    {
			return priceData.close_m;
    } 

		inline static float const& getClosePrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.close_m;
		}

		inline static float const& getHighPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.high_m;
		}

		inline static float const& getLowPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.low_m;
		}

		inline static float const& getOpenPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.open_m;
		}
		
};

template<> 
struct GetPrice2<Math::P_LowAsk> 
{ 
    inline static float const& getCandlePrice(HlpStruct::PriceData const& priceData) 
    {
			return priceData.low_m;
    } 
		 
		inline static float const& getClosePrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.close_m;
		}

		inline static float const& getHighPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.high_m;
		}

		inline static float const& getLowPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.low_m;
		}

		inline static float const& getOpenPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.open_m;
		}

		
};

template<> 
struct GetPrice2<Math::P_HighAsk> 
{ 
    inline static float const& getCandlePrice(HlpStruct::PriceData const& priceData) 
    {
			return priceData.high_m;
    } 

		inline static float const& getClosePrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.close_m;
		}

		inline static float const& getHighPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.high_m;
		}

		inline static float const& getLowPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.low_m;
		}

		inline static float const& getOpenPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.open_m;
		}

		
};



template<> 
struct GetPrice2<Math::P_OpenAsk> 
{ 
    inline static float const& getCandlePrice(HlpStruct::PriceData const& priceData) 
    {
			return priceData.open_m;
    } 

		inline static float const& getClosePrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.close_m;
		}

		inline static float const& getHighPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.high_m;
		}

		inline static float const& getLowPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.low_m;
		}

		inline static float const& getOpenPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.open_m;
		}

		
};


template<> 
struct GetPrice2<Math::P_CloseBid> 
{ 
    inline static float const& getCandlePrice(HlpStruct::PriceData const& priceData) 
    {
			return priceData.close2_m;
    } 

		inline static float const& getClosePrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.close2_m;
		}

		inline static float const& getHighPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.high2_m;
		}

		inline static float const& getLowPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.low2_m;
		}

		inline static float const& getOpenPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.open2_m;
		}
		
};

template<> 
struct GetPrice2<Math::P_LowBid> 
{ 
    inline static float const& getCandlePrice(HlpStruct::PriceData const& priceData) 
    {
			return priceData.low2_m;
    } 

		inline static float const& getClosePrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.close2_m;
		}

		inline static float const& getHighPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.high2_m;
		}

		inline static float const& getLowPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.low2_m;
		}

		inline static float const& getOpenPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.open2_m;
		}
		
};

template<> 
struct GetPrice2<Math::P_HighBid> 
{ 
    inline static float const& getCandlePrice(HlpStruct::PriceData const& priceData) 
    {
			return priceData.high2_m;
    } 

		inline static float const& getClosePrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.close2_m;
		}

		inline static float const& getHighPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.high2_m;
		}

		inline static float const& getLowPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.low2_m;
		}
		
		inline static float const& getOpenPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.open2_m;
		}
};



template<> 
struct GetPrice2<Math::P_OpenBid> 
{ 
    inline static float const& getCandlePrice(HlpStruct::PriceData const& priceData) 
    {
			return priceData.open2_m;
    } 

		inline static float const& getClosePrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.close2_m;
		}

		inline static float const& getHighPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.high2_m;
		}

		inline static float const& getLowPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.low2_m;
		}

		inline static float const& getOpenPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.open2_m;
		}
		
};

template<> 
struct GetPrice2<Math::P_Bid> 
{ 
    inline static float const& getCandlePrice(HlpStruct::PriceData const& priceData) 
    {
			THROW(CppUtils::OperationFailed, "exc_InvalidTemplateComiplation", "ctx_getCandlePrice", "");
    } 

		inline static float const& getClosePrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.close2_m;
		}

		inline static float const& getHighPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.high2_m;
		}

		inline static float const& getLowPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.low2_m;
		}

		inline static float const& getOpenPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.open2_m;
		}
		
};

template<> 
struct GetPrice2<Math::P_Ask> 
{ 
    inline static float const& getCandlePrice(HlpStruct::PriceData const& priceData) 
    {
			THROW(CppUtils::OperationFailed, "exc_InvalidTemplateComiplation", "ctx_getCandlePrice", "");
    } 

		inline static float const& getClosePrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.close_m;
		}

		inline static float const& getHighPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.high_m;
		}

		inline static float const& getLowPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.low_m;
		}

		inline static float const& getOpenPrice(HlpStruct::PriceData const& priceData) 
		{
			return priceData.open_m;
		}
		
};



}

#endif