#include "symbolstruct.hpp"

namespace AlgLib {

	double calcAvr(CppUtils::DoubleList const& data)
	{
		double result = -1.0;
	
	
		for(long i = 0; i < data.size(); i++) {
			if (i == 0) {
					
				result = data[i];
				continue;
			}

			result = (result * i + data[i] ) / (i+1);
		

		}
  
		return result;
	}

	double calcStd(CppUtils::DoubleList const& data)
	{
		double result = -1;
		double mavr = calcAvr(data);
	
		
		for(long i = 0; i < data.size(); i++) {

			double a = data[i] - mavr;
			double mafactor = a*a;
			if (i == 0) {
					
				result = mafactor;
				continue;
			}

			result = (result * i + mafactor ) / (i+1);

		}

		if (result >= 0) {
			return sqrt( result );
		}
		else {
			return -1.0;
		}
	}

};

