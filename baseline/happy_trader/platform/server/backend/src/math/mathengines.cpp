#include "mathengines.hpp"
#include "mathhelper.hpp"


namespace Math {

int dummy()
{
	return 42;
}

// ----------------------------------------
// HELPERS
// helper fun
double calcMavr(CppUtils::DoubleList const& data, int bidx)
{

	double result = -1.0;

	long idx = 0;
	for(long i = bidx; i < data.size(); i++) {
		if (idx++ == 0) {
				
			result = data[i];
			continue;
		}

		result = (result * (idx-1) + data[i] ) / idx;

	}

	return result;
	 
}

double calcStd(CppUtils::DoubleList const& data, int bidx)
{

	double result;
	double mavr = calcMavr(data, 0);

	long idx = 0;
	for(long i = bidx; i < data.size(); i++) {

		double a = data[i] - mavr;  
		double mafactor = a*a;
		if (idx++ == 0) {
				
			result = mafactor;
			continue;
		}

		result = (result * (idx-1) + mafactor ) / idx;

	}

	// sample standard deviation
	double bresult = (result * idx) / (idx - 1 );

	if (idx > 0) {
		return sqrt( bresult );
	}
	else {
		return -1.0;
	}
	
}

double calcStd(CppUtils::DoubleList const& data, double const& mavr, int bidx)
{
	double result;
	
	long idx = 0;
	for(long i = bidx; i < data.size(); i++) {

		double a = data[i] - mavr;  
		double mafactor = a*a;
		if (idx++ == 0) {
				
			result = mafactor;
			continue;
		}

		result = (result * (idx-1) + mafactor ) / idx;

	}

	// sample standard deviation
	double bresult = (result * idx) / (idx - 1 );

	if (idx > 0) {
		return sqrt( bresult );
	}
	else {
		return -1.0;
	}
}

double calcMin(CppUtils::DoubleList const& data, int bidx)
{
	double result = -1.0;

	
	for(long i = bidx; i < data.size(); i++) {
		if (i == bidx) {
				
			result = data[i];
			continue;
		}

		if (result > data[i])
			result = data[i];

	}

	return result;
}

double calcMax(CppUtils::DoubleList const& data, int bidx)
{
	double result = -1.0;

	
	for(long i = bidx; i < data.size(); i++) {
		if (i == bidx) {
				
			result = data[i];
			continue;
		}

		if (result < data[i])
			result = data[i];

	}

	return result;
}

			
bool priceEquality(double const& p1, double const& p2, int const digits)
{
	HTASSERT(digits >=0);
	bool result = false;

	
	switch (digits) {
		case 0:	 result = ((long)p1 == (long)p2); break;
		case 1:	 result = ((long)(p1 * 10 + 0.5) ==	(long) (p2 * 10 + 0.5)); break;
		case 2:	 result = ((long)(p1 * 100 + 0.5) ==	(long) (p2 * 100 + 0.5)); break;
		case 3:	 result = ((long)(p1 * 1000 + 0.5) ==	(long) (p2 * 1000 + 0.5)); break;
		case 4:	 result = ((long)(p1 * 10000 + 0.5) ==	(long) (p2 * 10000 + 0.5)); break;
		case 5:	 result = ((long)(p1 * 100000 + 0.5) ==	(long) (p2 * 100000 + 0.5)); break;
		case 6:	 result = ((long)(p1 * 1000000 + 0.5) ==	(long) (p2 * 1000000 + 0.5)); break;
		case 7:	 result = ((long)(p1 * 10000000 + 0.5) ==	(long) (p2 * 10000000 + 0.5)); break;
		case 8:	 result = ((long)(p1 * 100000000 + 0.5) ==	(long) (p2 * 100000000 + 0.5)); break;
		default: result = ((long)(p1 * pow ( 10.0, digits ) + 0.5) == (long)(p2 * pow ( 10.0, digits ) + 0.5)); break;
	};

	return result;
}

}; // end of ns
