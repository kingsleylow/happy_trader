#ifndef _MATH_MATHENGINES_INCLUDED
#define _MATH_MATHENGINES_INCLUDED

#include "mathdecl.hpp"
#include "mathdefs.hpp"
#include "mathinterface.hpp"



namespace Math {

	

	// Min/Max Channel

	template<class T>
	class MinMaxChannel: public T {
	public:
		struct Result
		{
			Result():
				upper_m(-1), lower_m(-1)
			{
			};

			double upper_m;
			double lower_m;
		};

		MinMaxChannel(int const period):
			period_m(period)
		{
		}

		virtual int returnNecessaryCacheSize() const
		{
			return period_m;
		}

		template<Math::CandlePrices const typeOfPrice>
		Result performCalculation()
		{
			Result result;

			setDataOffset(0);

			long idx = 0;
			while(isPtrValid()) {

				HlpStruct::PriceData const& pdata = getCurrentData();
				
				if (idx++==0) {

					result.lower_m  = GetPrice2<typeOfPrice>::getLowPrice(pdata);
					result.upper_m  = GetPrice2<typeOfPrice>::getHighPrice(pdata);

					getPrevData();
					continue;
				}
				

				if (result.lower_m > GetPrice2<typeOfPrice>::getLowPrice(pdata)) {
					result.lower_m = GetPrice2<typeOfPrice>::getLowPrice(pdata);
				}


				if (result.upper_m < GetPrice2<typeOfPrice>::getHighPrice(pdata)) {
					result.upper_m = GetPrice2<typeOfPrice>::getHighPrice(pdata);
				}

				if (idx >= period_m)
					break;
				
				getPrevData();
			}

			if (idx != period_m) {
				result.lower_m = -1.0;
				result.upper_m = -1.0;
			}

			return result;
		}


	private:

		int period_m;

	};
	
	// ----------------------------------------
	// WL version of RSI
	template<class T>
	class WLDRSIMathEngine: public T {
	public:
		WLDRSIMathEngine(int const rsiPer):
		rsiPer_m(rsiPer),
		totChangeAvg_m(-1),
		netChangeAvg_m(-1)
		{
		}

		virtual int returnNecessaryCacheSize() const
		{
			return rsiPer_m+2;
		}

		template<Math::CandlePrices const typeOfPrice>
		double performCalculation()
		{
			double result = RESULT_INVALID;

			if (rsiPer_m < 1)
				return RESULT_INVALID;
		
						
			if (getCacheSize() <= (rsiPer_m+1))
				return RESULT_INVALID;

			// need to have the first data in advance
			setDataOffset(0);

			if (totChangeAvg_m < 0 && netChangeAvg_m <0) {
				// first iteration

				if (isPtrValid()) {
					// first data
					HlpStruct::PriceData const& pdata = getCurrentData();

					// rsi per back
					HlpStruct::PriceData const& pdata_rsiPer = getBackwardOffsetData(rsiPer_m);

					netChangeAvg_m = ( GetPrice2<typeOfPrice>::getCandlePrice(pdata) - GetPrice2<typeOfPrice>::getCandlePrice(pdata_rsiPer) ) / ((double)rsiPer_m);
					

				}
				else 
					return -1.0;
				
				double value = 0;
				long idx = 0;
				while(isPtrValid()) {
					
					HlpStruct::PriceData const& pdata = getCurrentData();
					HlpStruct::PriceData const& pdata_1 = getBackwardOffsetData(1);
					
					double diff = GetPrice2<typeOfPrice>::getClosePrice(pdata) - GetPrice2<typeOfPrice>::getClosePrice(pdata_1);

					value += abs(diff);

					// prev
					getPrevData();  

					// exit if more than rsi per
					if (++idx >= rsiPer_m)
						break;
				}

				totChangeAvg_m = value / ((double)rsiPer_m);

			}
			else {
				// consequent iterations

				HlpStruct::PriceData const& pdata = getCurrentData();
				HlpStruct::PriceData const& pdata_1 = getBackwardOffsetData(1);

				double diff = GetPrice2<typeOfPrice>::getClosePrice(pdata) - GetPrice2<typeOfPrice>::getClosePrice(pdata_1);

				double SF = 1.0/((double)rsiPer_m);
				netChangeAvg_m = netChangeAvg_m + SF * ( diff - netChangeAvg_m );
				totChangeAvg_m = totChangeAvg_m + SF * ( abs( diff ) - totChangeAvg_m );

			}
			double chratio;
			if (totChangeAvg_m !=0)	{
				chratio = netChangeAvg_m/totChangeAvg_m;
			}
			else {
				chratio = 0;
			}

			result = 50* (chratio + 1);
			
			return result;
			
  	}

		
	
		// members
	private:  

		double totChangeAvg_m;

		double netChangeAvg_m;

		int rsiPer_m;

		
	};


	// ----------------------------------------
	// RSI
	template<class T>
	class RSIMathEngine: public T {
	public:
		RSIMathEngine(int const rsiPer):
		rsiPer_m(rsiPer),
		avrDown_m(-1),
		avrUp_m(-1)
		{
		}

		virtual int returnNecessaryCacheSize() const
		{
			return rsiPer_m+2;
		}

		template<Math::CandlePrices const typeOfPrice>
		double performCalculation()
		{
			double result = RESULT_INVALID;

			if (rsiPer_m < 1)
				return RESULT_INVALID;
		
						
			if (getCacheSize() <= (rsiPer_m+1))
				return RESULT_INVALID;

			// need to have the first data in advance
			setDataOffset(0);

			if (avrDown_m < 0 && avrUp_m <0) {
				// begin

				double avrUp = 0;
				double avrDown = 0;

				long idx  = 0;
				while(isPtrValid()) {
					
					HlpStruct::PriceData const& pdata = getCurrentData();
					HlpStruct::PriceData const& pdata_1 = getBackwardOffsetData(1);
					
					double diff = GetPrice2<typeOfPrice>::getClosePrice(pdata) - GetPrice2<typeOfPrice>::getClosePrice(pdata_1);
					if (diff > 0) {
						//avrUp = (avrUp * (idx-1) + GetPrice2<typeOfPrice>::getCandlePrice(pdata) ) / idx;
						avrUp += diff;
					}
					else {
						//avrDown = (avrDown * (idx-1) + GetPrice2<typeOfPrice>::getCandlePrice(pdata) ) / idx;
						avrDown -= diff;
					}

									
					// next
					getPrevData();   
				
					if (++idx >= rsiPer_m)
						break;
				}  

			
				// the first time calculation must be completed!!!
				if (idx < rsiPer_m)
					return -1;

				avrUp /= idx;
				avrDown /= idx;

				avrDown_m = avrDown;
				avrUp_m = avrUp;

			}
			else {
				// continue
				
				if(isPtrValid()) {
					HlpStruct::PriceData const& pdata = getCurrentData();
					HlpStruct::PriceData const& pdata_1 = getBackwardOffsetData(1);

					double diff = GetPrice2<typeOfPrice>::getClosePrice(pdata) - GetPrice2<typeOfPrice>::getClosePrice(pdata_1);

					if (diff > 0) {
						avrUp_m = (avrUp_m * (rsiPer_m-1) + diff) / rsiPer_m;
					}
					else {
						avrDown_m = (avrDown_m * (rsiPer_m-1) - diff) / rsiPer_m;
					}

				}
				else
					return RESULT_INVALID;

			}


			result = 100*avrUp_m/(avrUp_m+avrDown_m);
			return result;
			
  	}

		
	
		// members
	private:  

		
		int rsiPer_m;

		double avrDown_m;

		double avrUp_m;

	};

	// ----------------------------------------
	// simple moving average
template<class T>
class MovAverMathEngine: public T {
	public:
		MovAverMathEngine(int const maPer):
		maPer_m(maPer)
		{
		}

		virtual int returnNecessaryCacheSize() const
		{
			return maPer_m;
		}

		template<Math::CandlePrices const typeOfPrice>
		double performCalculation()
		{
			double result = -1.0;

			setDataOffset(maPer_m-1);

			long idx = 0;
			while(isPtrValid()) {

				HlpStruct::PriceData const& pdata = getCurrentData();
				
				if (idx++==0) {

					result = GetPrice2<typeOfPrice>::getCandlePrice(pdata);

					getNextData();
					continue;
				}
				
				result = (result * (idx-1) + GetPrice2<typeOfPrice>::getCandlePrice(pdata) ) / idx;
				getNextData();
			}

			return result;

		}

private:
		int maPer_m;
};

// ----------------------------------------

template<class T>
class MovAverMathEngineNoNegativePrice: public T {
	public:
		struct Result
		{
			Result():
				isValid_m(false), 
				result_m(-1)
			{
			};

	
			double result_m;
			bool isValid_m;
		};

		MovAverMathEngineNoNegativePrice(int const maPer):
		maPer_m(maPer)
		{
		}

		virtual int returnNecessaryCacheSize() const
		{
			return maPer_m;
		}

		template<Math::CandlePrices const typeOfPrice>
		Result performCalculation()
		{
			Result result;

			setDataOffset(maPer_m-1);

			long idx = 0;
			while(isPtrValid()) {

				HlpStruct::PriceData const& pdata = getCurrentData();
				
				if (idx++==0) {

					result.result_m = GetPrice2<typeOfPrice>::getCandlePrice(pdata);
					if (result.result_m < 0)
						return result;

				
					getNextData();
					continue;
				}
				
				float cp = GetPrice2<typeOfPrice>::getCandlePrice(pdata);
				if (cp < 0)
						return result;

				result.result_m  = (result.result_m * (idx-1) + cp ) / idx;
				getNextData();
			}

			result.isValid_m = true;
			return result;

		}

private:
		int maPer_m;
};

// ----------------------------------------
	
template<class T>
class PivotMathEngine: public T {
	public:
		PivotMathEngine(int const pivotMaPer, int const barsBack):
		pivotMaPer_m(pivotMaPer),
		barsBack_m(barsBack)
		{
		}

		virtual int returnNecessaryCacheSize() const
		{
			return 3+pivotMaPer_m+barsBack_m+4+1;
		}

		template<Math::CandlePrices const typeOfPrice>
		double performCalculation()
		{
			double result = -1;

			if (getCacheSize() < (3+pivotMaPer_m+barsBack_m+4+1))
				return -1.0;

			// set to the last
			setDataOffset(0);


			int cnt = 0;
		
			CppUtils::DoubleList kp8Ser, factorSer, smaSer, pivotSer;
			while(isPtrValid()) {

				// current data
				HlpStruct::PriceData const& pdata = getCurrentData();

				// 4 bars back
				HlpStruct::PriceData const& pdata_4 = getBackwardOffsetData(4);

				double mp = ( GetPrice2<typeOfPrice>::getHighPrice(pdata) + GetPrice2<typeOfPrice>::getLowPrice(pdata) ) /2.0;
				double kp8 = GetPrice2<typeOfPrice>::getCandlePrice(pdata) - GetPrice2<typeOfPrice>::getCandlePrice(pdata_4) + mp;

				          
				kp8Ser.push_back(kp8);
				
				if (kp8Ser.size() >= 3) {
					double factor1 = (kp8Ser[ kp8Ser.size() - 3 +0 ] + kp8Ser[ kp8Ser.size() - 3 + 1 ] + kp8Ser[ kp8Ser.size() - 3 + 2 ])/3.0; 
					factorSer.push_back( factor1 );
				}


				if (factorSer.size() >= pivotMaPer_m) {
					// can calculate moving average
					double mav = calcMavr(factorSer, factorSer.size()-pivotMaPer_m );
					smaSer.push_back(mav);
				}

				if (smaSer.size() > barsBack_m) {
					 result =  smaSer[ barsBack_m ] ;
					 break;
				}
				
			  
  
				// advance counter
				getPrevData();
				cnt++;
			}
			
			return result;

		}

private:
		int pivotMaPer_m;

		int barsBack_m;
};

// ----------------------------------------

template<class T>
class ParabolicMathEngine: public T {
	public:
		ParabolicMathEngine(double const& acelUp, double const& acelDown, double const& acelMax, bool const isLong):
				acelUp_m(acelUp),
				acelDown_m(acelDown),
				acelMax_m(acelMax),
				isLong_m(isLong)
		{
		}

		
		template<Math::CandlePrices const typeOfPrice>
		double performCalculation()
		{
			if (getCacheSize() <=2)
				return -1;
			
			double result = -1;

			double newHigh, newLow, prevHigh, prevLow;
			double af, ep, sar;


			// set to first
			setDataOffset(getCacheSize()-1);
			if(!isPtrValid())
				return -1;
			
			// initial zero data
			HlpStruct::PriceData const& pdata_0 = getCurrentData();
			
			newHigh = GetPrice2<typeOfPrice>::getHighPrice(pdata_0);
			newLow  = GetPrice2<typeOfPrice>::getLowPrice(pdata_0);

			// advance
			getNextData();
			if(!isPtrValid())
				return -1;
			
			HlpStruct::PriceData const& pdata_1 = getCurrentData();
			if( isLong_m == 1 )	{
				ep  = GetPrice2<typeOfPrice>::getHighPrice(pdata_1);
				sar = newLow;
			}
			else {
				ep  = GetPrice2<typeOfPrice>::getLowPrice(pdata_1);
				sar = newHigh;
			}

			// begin calculation
			newLow  = GetPrice2<typeOfPrice>::getLowPrice(pdata_1);
			newHigh = GetPrice2<typeOfPrice>::getHighPrice(pdata_1);


			long cnt = 0;
			while(isPtrValid()) {
				// curent data
				HlpStruct::PriceData const& pdata = getCurrentData();
				
				prevLow  = newLow;
				prevHigh = newHigh;
				newLow  = GetPrice2<typeOfPrice>::getLowPrice(pdata);
				newHigh = GetPrice2<typeOfPrice>::getHighPrice(pdata);   
				
				// 
				if( isLong_m  )
				{  
					/* Switch to short if the low penetrates the SAR value. */
					if( newLow <= sar )
					{
							/* Switch and Overide the SAR with the ep */
							isLong_m = false;
							sar = ep;

							/* Make sure the overide SAR is within
							* yesterday's and today's range.
							*/
							if( sar < prevHigh )
								sar = prevHigh;            
							if( sar < newHigh )
								sar = newHigh;

							/* Output the overide SAR  */
							//outReal[outIdx++] = sar;

							/* Adjust af and ep */
							af = acelUp_m;
							ep = newLow;
	 
							/* Calculate the new SAR */
							sar = sar + af * (ep - sar);

							/* Make sure the new SAR is within
							* yesterday's and today's range.
							*/
							if( sar < prevHigh )
								sar = prevHigh;            
							if( sar < newHigh )
								sar = newHigh;
					}
					else
					{
							/* No switch */

							/* Output the SAR (was calculated in the previous iteration) */
							//outReal[outIdx++] = sar;
	 
							/* Adjust af and ep. */
							if( newHigh > ep )
							{
								ep = newHigh;
								af += acelUp_m;
								if( af > acelMax_m )
										af = acelMax_m;
							}

							/* Calculate the new SAR */
							sar = sar + af * (ep - sar);

							/* Make sure the new SAR is within
							* yesterday's and today's range.
							*/
							if( sar > prevLow )
								sar = prevLow;            
							if( sar > newLow )
								sar = newLow;
					}
				}
				else
				{
					/* Switch to long if the high penetrates the SAR value. */
					if( newHigh >= sar )
					{
							/* Switch and Overide the SAR with the ep */
							isLong_m = true;
							sar = ep;

							/* Make sure the overide SAR is within
							* yesterday's and today's range.
							*/
							if( sar > prevLow )
								sar = prevLow;            
							if( sar > newLow )
								sar = newLow;

							/* Output the overide SAR  */
							//outReal[outIdx++] = sar;

							/* Adjust af and ep */
							af = acelDown_m;
							ep = newHigh;

							/* Calculate the new SAR */
							sar = sar + af * (ep - sar);

							/* Make sure the new SAR is within
							* yesterday's and today's range.
							*/
							if( sar > prevLow )
								sar = prevLow;            
							if( sar > newLow )
								sar = newLow;
					}
					else
					{
							/* No switch */

							/* Output the SAR (was calculated in the previous iteration) */
							//outReal[outIdx++] = sar;

							/* Adjust af and ep. */
							if( newLow < ep )
							{
								ep = newLow;  
								af += acelDown_m;
								if( af > acelMax_m )  
										af = acelMax_m;
							}
 
							/* Calculate the new SAR */ 
							sar = sar + af * (ep - sar);

							/* Make sure the new SAR is within
							* yesterday's and today's range.
							*/
							if( sar < prevHigh )
								sar = prevHigh;            
							if( sar < newHigh )
								sar = newHigh;
					}
				}


				// 
				getNextData();
				cnt++;
			}
			
			result = sar;

			return result;
		}

private:
		double acelUp_m;
		double acelDown_m;
		double acelMax_m;
		bool isLong_m;
};
	
// ----------------------------------------

template<class T>
class MetastockParabolicMathEngine: public T {
	public:
		MetastockParabolicMathEngine(double const& acelUp, double const& acelDown, double const& acelMax, bool const isLong):
				acelUp_m(acelUp),
				acelDown_m(acelDown),
				acelMax_m(acelMax),
				isLong_m(isLong)
		{
		}

		virtual int returnNecessaryCacheSize() const
		{
			return 4;
		}

		
		template<Math::CandlePrices const typeOfPrice>
		double performCalculation()
		{
			if (getCacheSize() <=3)
				return -1;
			
		
			double af = acelUp_m;
			bool is_long=true;
			double hp, lp;
			bool rev = false; 
			

			double sar = 0, prev_sar = 0;

			// set to third
			setDataOffset(getCacheSize()-3);

			HlpStruct::PriceData const& pdata = getCurrentData();
			HlpStruct::PriceData const& pdata_1 = getBackwardOffsetData(1);
			HlpStruct::PriceData const& pdata_2 = getBackwardOffsetData(2);

			hp = GetPrice2<typeOfPrice>::getHighPrice(pdata_2);  
			lp = GetPrice2<typeOfPrice>::getLowPrice(pdata_2);   

			

			while(isPtrValid()) {
				// cur data
				HlpStruct::PriceData const& pdata = getCurrentData();
				HlpStruct::PriceData const& pdata_1 = getBackwardOffsetData(1);
				HlpStruct::PriceData const& pdata_2 = getBackwardOffsetData(2);


				if (is_long) {
					sar = prev_sar + af * (hp-prev_sar);
				}
				else {
					sar = prev_sar + af * (lp-prev_sar);
				}

				rev = false;

				if (is_long) {
					if (GetPrice2<typeOfPrice>::getLowPrice(pdata) < sar) {
						is_long = false;
						rev = true;
						sar = hp;
						lp = GetPrice2<typeOfPrice>::getLowPrice(pdata);   
						af = acelUp_m;
					}
				}
				else {
					if (GetPrice2<typeOfPrice>::getHighPrice(pdata) > sar) {
						is_long = true;
						rev = true;
						sar = lp;
						hp= GetPrice2<typeOfPrice>::getHighPrice(pdata);   
						af = acelUp_m;
					}
				}

				//
				if (!rev) {
					if (is_long) {
						if (GetPrice2<typeOfPrice>::getHighPrice(pdata) > hp) {       
							hp = GetPrice2<typeOfPrice>::getHighPrice(pdata);
							af = af + acelUp_m;          
							af = min(af, acelMax_m);        
						}

						sar = min(sar, GetPrice2<typeOfPrice>::getLowPrice(pdata_1));
						sar = min(sar, GetPrice2<typeOfPrice>::getLowPrice(pdata_2));

					}
					else {
						if (GetPrice2<typeOfPrice>::getLowPrice(pdata) < lp) {       
							lp = GetPrice2<typeOfPrice>::getLowPrice(pdata);
							af = af + acelUp_m;          
							af = min(af, acelMax_m);        
						}

						sar = max(sar, GetPrice2<typeOfPrice>::getHighPrice(pdata_1));
						sar = max(sar, GetPrice2<typeOfPrice>::getHighPrice(pdata_2));

					}
				}


				// forward
				getNextData();
				prev_sar = sar;
			}

			return sar;
		}

private:
		double acelUp_m;
		double acelDown_m;
		double acelMax_m;
		bool isLong_m;
};



// ----------------------------------------

template<class T>
class KpVolEngine: public T {
public:
	KpVolEngine()
	{
	}
	virtual int returnNecessaryCacheSize() const
	{
			return 8+1;
	}

	template<Math::CandlePrices const typeOfPrice>
	double performCalculation()
	{
		if (getCacheSize() <=8)
			return -1;

		double result = -1.0;

		setDataOffset(0);


		int cnt = 0;
	
		
		CppUtils::DoubleList kp8Ser, volumeSer, priceSer;
		while(isPtrValid()) {

			HlpStruct::PriceData const& pdata = getCurrentData();
			kp8Ser.push_back( ( GetPrice2<typeOfPrice>::getHighPrice(pdata) + GetPrice2<typeOfPrice>::getLowPrice(pdata) ) / 2 );
			volumeSer.push_back(pdata.volume_m);
			priceSer.push_back(GetPrice2<typeOfPrice>::getCandlePrice(pdata));

			if (kp8Ser.size() >= 9) {
				double const& kp8ser_val8 = kp8Ser[ kp8Ser.size() - 9 + 8 ];
				double const& kp8ser_val3 = kp8Ser[ kp8Ser.size() - 9 + 3 ];

				float const& priceSer_0 = priceSer[priceSer.size() - 9 + 0];
				
				double kp0102  = priceSer[priceSer.size() - 9 + 0] - kp8Ser[ kp8Ser.size() - 9 + 0 ] + kp8ser_val8;

			
				double vr1 = (volumeSer[volumeSer.size() -9 + 0] + volumeSer[volumeSer.size() -9 + 3] + volumeSer[volumeSer.size() -9 + 8] / kp8ser_val8) / 10.0 +
					priceSer_0;
				double kpvol = kp0102/vr1 + (priceSer_0 - kp8ser_val3 ) + priceSer_0;

				// our result
				return kpvol;  
			}

			//
			getPrevData();
		}

		return result;
	}

private:

};

template<class T>
class STDEngine: public T {
public:
	STDEngine(int const stdPer):
		stdPer_m( stdPer )
	{
	}

	virtual int returnNecessaryCacheSize() const
	{
			return stdPer_m;
	}

	template<Math::CandlePrices const typeOfPrice>
	double performCalculation()
	{
	
		CppUtils::DoubleList prices;
	
		setDataOffset(0);

		//CppUtils::String datas;
		while(isPtrValid()) {

			HlpStruct::PriceData const& pdata = getCurrentData();

			// store out prices
			prices.push_back( GetPrice2<typeOfPrice>::getCandlePrice(pdata) );

			//datas += CppUtils::float2String(GetPrice2<typeOfPrice>::getCandlePrice(pdata), -1, 4) + ",";

			if (prices.size() >= stdPer_m)
				break;
			
			//
			getPrevData();
		}

		if (prices.size() != stdPer_m)
			return RESULT_INVALID;
	
		//LOG(MSG_INFO, "foo", "foo: " << datas );
		double r = calcStd(prices, 0);
		if (r < 0)
			return RESULT_INVALID;
		else
			return r;

	}

private:
	int stdPer_m;
};

// true range
// --------------------------------------

template<class T>
class TrueRangeEngine: public T {
public:
	
	TrueRangeEngine()
	{
		
	}

	virtual int returnNecessaryCacheSize() const
	{
			return 1;
	}

	template<Math::CandlePrices const typeOfPrice>
	double performCalculation()
	{
		
		setDataOffset(0);

		
		if(isPtrValid()) {

			HlpStruct::PriceData const& pdata_0 = getCurrentData();
			
			//
			getPrevData();

			if(isPtrValid()) {
				HlpStruct::PriceData const& pdata_1 = getCurrentData();

				

				//	The distance from today's high to today's low.
				double v1 = GetPrice2<typeOfPrice>::getHighPrice(pdata_0) - GetPrice2<typeOfPrice>::getLowPrice(pdata_0);

				//	The distance from yesterday's close to today's high.
				double v2 = abs(GetPrice2<typeOfPrice>::getHighPrice(pdata_0) - GetPrice2<typeOfPrice>::getClosePrice(pdata_1));
				
				//	The distance from yesterday's close to today's low
				double v3 = abs(GetPrice2<typeOfPrice>::getLowPrice(pdata_0) - GetPrice2<typeOfPrice>::getClosePrice(pdata_1));

				return max(max(v1,v2),v3);

			}
			
		}

		return RESULT_INVALID;

	
		

	}

private:
	
};


// this engine to check if last n candles have equal highs or lows
template<class T>
class EqualHighLowEngine: public T {
private:

	static int const LOWS_OUT = -9999;
	static int const HIGS_OUT = 9999;
	static int const LOWS_HIGS_OUT = 999;

public:
	
	

	EqualHighLowEngine(int const depth, double const priceRange):
		depth_m( depth ),
		priceRange_m( priceRange )
	{
	}

	virtual int returnNecessaryCacheSize() const
	{
			return depth_m;
	}

	template<Math::CandlePrices const typeOfPrice>
	double performCalculation()
	{
		// means not executed
		double result = 0.0;

		if (depth_m <= 1)
			return 0.0;

		if (getCacheSize() < depth_m)
			return 0.0;


		bool high_out = false;
		bool low_out = false;

		setDataOffset(depth_m);

		long idx = 0;
		double cur_low = 0, cur_high = 0;

		CppUtils::DoubleList highs, lows;

		while(isPtrValid()) {

			HlpStruct::PriceData const& pdata = getCurrentData();

			highs.push_back( GetPrice2<typeOfPrice>::getLowPrice(pdata) );
			lows.push_back( GetPrice2<typeOfPrice>::getHighPrice(pdata) );

		

			//
			getNextData();
		}

		// check highs
		for(int i = 0; i < highs.size(); i++) {
			
			for(int j = 0; j < highs.size(); j++) {
				if (i != j) {
				
					if (abs(highs[i]-highs[j]) > priceRange_m ) {
						high_out = true;
					}

					if (abs(lows[i]-lows[j]) > priceRange_m ) {
						low_out = true;
					}

				}
			}
		}

		if (high_out && low_out)
			result = (double)LOWS_HIGS_OUT;
		else if (high_out && !low_out)
			result = (double)HIGS_OUT;
		else if (!high_out && low_out)
			result = (double)LOWS_OUT;

		return result;
	}

	// special function to hadle the result
	static bool isHighsOut(double const &result)
	{
		if (result == (double)LOWS_HIGS_OUT || result == (double)HIGS_OUT)
			return true;

		return false;
	}

	static bool isLowsOut(double const& result) 
	{
		if (result == (double)LOWS_HIGS_OUT || result == (double)LOWS_OUT)
			return true;

		return false;
	}

private:


	int depth_m;

	double priceRange_m;

};

// convergent channel
template<class T>
class ConvergentEngine: public T {
public:
	static int const LOWS_CONVERGENT = -9999;
	static int const HIGHS_CONVERGENT = 9999;
	static int const HIGHS_LOWS_CONVERGENT = 999;
	static int const NO_CONVERGENT = 9;
	

public:
	
	
	ConvergentEngine(int const depth):
		depth_m( depth )
	{
	}

	template<Math::CandlePrices const typeOfPrice>
	double performCalculation()
	{
		// means not executed
		double result = 0;

		if (depth_m <= 1)
			return result;

		if (getCacheSize() < depth_m)
			return result;


		setDataOffset(depth_m);

		long idx = 0;
		double prev_low = -1.0, prev_high = -1.0;


		
		bool lows_convergent = true, highs_convergent = true;
		while(isPtrValid()) {

			HlpStruct::PriceData const& pdata = getCurrentData();

			if (prev_low < 0 && prev_high < 0) {

				prev_low  = GetPrice2<typeOfPrice>::getLowPrice(pdata);
				prev_high  = GetPrice2<typeOfPrice>::getHighPrice(pdata);

				getNextData();
				continue;
			}

			if (GetPrice2<typeOfPrice>::getLowPrice(pdata) < prev_low) {
				lows_convergent =  false;
			}

			if (GetPrice2<typeOfPrice>::getHighPrice(pdata) > prev_high) {
				highs_convergent =  false;
			}



			prev_low = GetPrice2<typeOfPrice>::getLowPrice(pdata); 
			prev_high  = GetPrice2<typeOfPrice>::getHighPrice(pdata);
			//
			getNextData();
		}


		if (lows_convergent && highs_convergent)
			result = HIGHS_LOWS_CONVERGENT;
		else if (lows_convergent && !highs_convergent)
			result = LOWS_CONVERGENT;
		else if (!lows_convergent && highs_convergent)
			result = HIGHS_CONVERGENT;
		else if (!lows_convergent && !highs_convergent)
			result = NO_CONVERGENT;

		
		return result;
	}

	

private:


	int depth_m;

	
};

template<class T>
class ConvergentCloseEngine: public T {
public:
	static int const LOWS_CONVERGENT = -9999;
	static int const HIGHS_CONVERGENT = 9999;
	static int const HIGHS_LOWS_CONVERGENT = 999;
	static int const NO_CONVERGENT = 9;
	

public:
	
	static char const* getConversionStringResult(int const cresult)
	{
		switch(cresult) {
			case LOWS_CONVERGENT:
				return "LOWS CONVERGENT";
			case HIGHS_CONVERGENT:
				return "LOWS CONVERGENT";
			case HIGHS_LOWS_CONVERGENT:
				return "BOTH CONVERGENT";
			case NO_CONVERGENT:
				return "NO CONVERGENT";
			default:
				return "N/A";

		}
	}
	

	ConvergentCloseEngine(int const depth):
		depth_m( depth )
	{
	}


	virtual int returnNecessaryCacheSize() const
	{
		return depth_m;
	}

	template<Math::CandlePrices const typeOfPrice>
	double performCalculation()
	{
		// means not executed
		double result = 0;

		if (depth_m <= 1)
			return result;

		if (getCacheSize() < depth_m)
			return result;


		setDataOffset(depth_m);

		long idx = 0;
		double prev_low = -1.0, prev_high = -1.0;


		
		bool lows_convergent = true, highs_convergent = true;
		while(isPtrValid()) {

			HlpStruct::PriceData const& pdata = getCurrentData();

			if (prev_low < 0 && prev_high < 0) {

				prev_low  = GetPrice2<typeOfPrice>::getClosePrice(pdata);
				prev_high  = GetPrice2<typeOfPrice>::getClosePrice(pdata);

				getNextData();
				continue;
			}

			if (GetPrice2<typeOfPrice>::getClosePrice(pdata) < prev_low) {
				lows_convergent =  false;
			}

			if (GetPrice2<typeOfPrice>::getClosePrice(pdata) > prev_high) {
				highs_convergent =  false;
			}



			prev_low = GetPrice2<typeOfPrice>::getClosePrice(pdata); 
			prev_high  = GetPrice2<typeOfPrice>::getClosePrice(pdata);
			//
			getNextData();
		}


		if (lows_convergent && highs_convergent)
			result = HIGHS_LOWS_CONVERGENT;
		else if (lows_convergent && !highs_convergent)
			result = LOWS_CONVERGENT;
		else if (!lows_convergent && highs_convergent)
			result = HIGHS_CONVERGENT;
		else if (!lows_convergent && !highs_convergent)
			result = NO_CONVERGENT;

		
		return result;
	}

	

private:


	int depth_m;

	
};





// this exports this to file

template<class T>
class ExporterEngine: public T {
public:
	ExporterEngine(CppUtils::String const& exportFile, bool const insert_unclosed, bool const debugmode = false):
		exportFile_m( exportFile ),
		debugMode_m( debugmode ),
		insertUnclosed_m(insert_unclosed)
	{
	}



	virtual int returnNecessaryCacheSize() const
	{
		return 1;
	}


	template<Math::CandlePrices const typeOfPrice>
	bool performCalculation()
	{
		ofstream f(exportFile_m.c_str(), ios::out | ios::trunc);

		f <<  "TIME,OPEN,HIGH,LOW,CLOSE,VOLUME" << endl;

		// empty!
		if (getCacheSize()<=0)
			return true;

		setDataOffset(getCacheSize()-1);
		if(!isPtrValid()) {
			f.close();
			return false;
		}


		while(isPtrValid()) { 
			HlpStruct::PriceData const& pdata = getCurrentData();
			


			// dump all here
			f << (pdata.time_m > 0 ? CppUtils::getGmtTime3(pdata.time_m):"N/A") << "," <<
				CppUtils::float2String(Math::GetPrice2<typeOfPrice>::getOpenPrice(pdata),-1,5) << "," <<
				CppUtils::float2String(Math::GetPrice2<typeOfPrice>::getHighPrice(pdata),-1,5) << "," <<
				CppUtils::float2String(Math::GetPrice2<typeOfPrice>::getLowPrice(pdata),-1,5) << "," <<
				CppUtils::float2String(Math::GetPrice2<typeOfPrice>::getClosePrice(pdata),-1,5) << "," <<
				CppUtils::float2String(pdata.volume_m,-1,5) <<	endl;

		

			//
			getNextData();
		}

		if (insertUnclosed_m) {
		
			HlpStruct::PriceData const& unclosed = getUncloseddata();

			f << (unclosed.time_m > 0 ? CppUtils::getGmtTime3(unclosed.time_m):"N/A") << "," <<
				CppUtils::float2String(Math::GetPrice2<typeOfPrice>::getOpenPrice(unclosed),-1,5) << "," <<
				CppUtils::float2String(Math::GetPrice2<typeOfPrice>::getHighPrice(unclosed),-1,5) << "," <<
				CppUtils::float2String(Math::GetPrice2<typeOfPrice>::getLowPrice(unclosed),-1,5) << "," <<
				CppUtils::float2String(Math::GetPrice2<typeOfPrice>::getClosePrice(unclosed),-1,5) << "," <<
				CppUtils::float2String(unclosed.volume_m,-1,5)  << endl;
		
		}

	
		f.close();


		return true;
	};

private:
	CppUtils::String exportFile_m;

	bool debugMode_m;

	bool insertUnclosed_m;

	
};


// helper fun

MATH_EXP int dummy();

MATH_EXP double calcMavr(CppUtils::DoubleList const& data, int bidx);

MATH_EXP double calcStd(CppUtils::DoubleList const& data, int bidx);

MATH_EXP double calcStd(CppUtils::DoubleList const& data, double const& mavr, int bidx);
		
MATH_EXP double calcMin(CppUtils::DoubleList const& data, int bidx);

MATH_EXP double calcMax(CppUtils::DoubleList const& data, int bidx);

// check prices for equality rounding to the number of digist
// using this function we can check if price is equal
MATH_EXP bool priceEquality(double const& p1, double const& p2, int const digits = 4);
	

template<int const D>
struct PowerTen {
	enum {
		value = 10 * PowerTen<D-1>::value
	};

	static inline double get()
	{
		return (double)value;
	}
};

template<>
struct PowerTen<0> {
	enum {
		value = 1
	};
	
	static inline double get()
	{
		return 1.0;
	}
};


// fast comparison
template<int const D>
class PriceEquality {
public:
	inline static bool isEqual(double const& p1, double const& p2)
	{	
		return ((long)(p1 * PowerTen<D>::get() + 0.5)) == ((long)(p2 * PowerTen<D>::get() + 0.5));
	}
};

// ---------------------------------------
template<int const D>
class DoubleLongHolder
{
public:
	DoubleLongHolder():
		internal_value_m(0),
		value_m(0)
	{
	}

	DoubleLongHolder(double const& value):
		value_m(value)
	{
		setupLong();
	}

	DoubleLongHolder& operator = (double const& value)
	{
		value_m = value;
		setupLong();
	}

	bool operator == (DoubleLongHolder const& rhs) const
	{
		return internal_value_m == rhs.internal_value_m;
	}

	bool operator > (DoubleLongHolder const& rhs) const
	{
		return internal_value_m > rhs.internal_value_m;
	}
																		  
	bool operator < (DoubleLongHolder const& rhs) const
	{
		return internal_value_m < rhs.internal_value_m;
	}

	inline double const& getDoubleValue() const
	{
		return value_m;
	}

	inline CppUtils::Int64 const& getLongInternalValue() const
	{
		return internal_value_m;
	}

	void setDoubleValue(double const& value)
	{
		value_m = value;
		setupLong();
	}

private:

	/**
	Setup long value assuming we know double value
	*/
	void setupLong()
	{
		internal_value_m = (CppUtils::Int64)(value_m * PowerTen<D>::get() + 0.5);
	}

	CppUtils::Int64 internal_value_m;
	
	double value_m;
};





}; // end of namespace

#endif