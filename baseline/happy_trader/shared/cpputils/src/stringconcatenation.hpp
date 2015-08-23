#ifndef _CPPUTILSS_STRINGCONCATENATION_INCLUDED
#define _CPPUTILSS_STRINGCONCATENATION_INCLUDED


#include "cpputilsdefs.hpp"
#include "uid.hpp"
#include <strstream>
#include "template.hpp"

namespace CppUtils {

	class CPPUTILS_EXP StringContactDateWrapper
	{
	public:
		inline StringContactDateWrapper(double const& val):
				value_m(val)
		{
		}

		double const& value_m;
	};

	class CPPUTILS_EXP StringConcat
	{
	public:
		StringConcat();

		StringConcat& operator << (bool const val);

		StringConcat& operator << (char const* val);
		
		StringConcat& operator << (double const& val);
		
		StringConcat& operator << (int const& val);

		StringConcat& operator << (unsigned int const& val);
		
		StringConcat& operator << (long const& val);
		
		StringConcat& operator << (String const& val);
		
		StringConcat& operator << (StringContactDateWrapper const& val);
		
		StringConcat& operator << (Uid const& val);

		
		CppUtils::String getString();
		
		inline ostrstream& get()
		{
			return (ostrstream&)stream_m;
		}
	private:
		CppUtils::CleanedStreamBuffer<ostrstream> stream_m;
	};

template< class T>
class VectorStringConcat
{
	public:

		static void append(StringConcat& stringConcat, vector<T> const& values)
		{
			stringConcat << "[ ";
			for(int i = 0; i < values.size(); i++) {
				stringConcat << values[ i ].toString();
			}
			stringConcat << "] ";
		}
};

#define	STRINGCONB	\
		"\n["		

#define	STRINGCONE	\
		"]\n";			
	

#define STRINGCONTANYVALUE(V)	\
		V << ";\n"

#define STRINGCONTPAIR(V)	\
	#V << "=" << V << ";\n"

#define STRINGCONTPAIR_DATE(V)	\
	#V << "=" << CppUtils::StringContactDateWrapper(V) << ";\n"

#define STRINGCONTPAIR_FUN(FUN, V)	\
	#V << "=" << FUN(V) << ";\n"

#define STRINGCONTPAIR_MEMBERFUN(FUN, V)	\
	#V << "=" << V.FUN() << ";\n"
		

};

#endif