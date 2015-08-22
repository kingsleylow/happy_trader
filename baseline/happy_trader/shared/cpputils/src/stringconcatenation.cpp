#include "stringconcatenation.hpp"
#include "misc_ht.hpp"

namespace CppUtils {
	StringConcat::StringConcat():
			stream_m(false)
		{
		}

		StringConcat& StringConcat::operator << (bool const val)
		{
				get() << CppUtils::bool2String(val);
				return *this;
		}

		StringConcat& StringConcat::operator << (char const* val)
		{
				get() << (val ? val : "");
				return *this;
		}

		StringConcat& StringConcat::operator << (double const& val)
		{
				get() <<  CppUtils::float2String(val, -1, 4);
				return *this;
		}

		StringConcat& StringConcat::operator << (int const& val)
		{
				get() <<  CppUtils::long2String(val);
				return *this;
		}

		StringConcat& StringConcat::operator << (unsigned int const& val)
		{
			get() <<  CppUtils::long2String(val);
			return *this;
		}

		StringConcat& StringConcat::operator << (long const& val)
		{
				get() <<  CppUtils::long2String(val);
				return *this;
		}

		StringConcat& StringConcat::operator << (String const& val)
		{
				get() << val;
				return *this;
		}

		StringConcat& StringConcat::operator << (StringContactDateWrapper const& val)
		{
			get() << CppUtils::getGmtTime(val.value_m);
			return *this;
		}

		StringConcat& StringConcat::operator << (Uid const& val)
		{
			get() << val.toString();
			return *this;
		}

		CppUtils::String StringConcat::getString()
		{
			get() << ends;
			return get().str();
		}

} // end of namespace