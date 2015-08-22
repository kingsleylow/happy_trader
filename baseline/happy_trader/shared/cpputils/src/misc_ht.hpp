#ifndef _CPPUTILS_MISC_INCLUDED
#define _CPPUTILS_MISC_INCLUDED



#include "cpputilsdefs.hpp"
#include <strstream>


namespace CppUtils {


//=============================================================
// Basic functions
//=============================================================

/** Convert string to lower case. */
CPPUTILS_EXP void tolower (
		String &
	);

/** Convert string to upper case. */
CPPUTILS_EXP void toupper (
		String &
	);

/** Convert string to lower case. */
CPPUTILS_EXP String toLowerCase (	String const& val	);

/** Convert string to upper case. */
CPPUTILS_EXP String toUpperCase (	String const & val	);


/** Parse the given command line; the output \a simpleOptions will contain
		all options found without argument, i.e. all items not followed by another
		item or followed by an item starting with a dash. \a complexOptions contains
		all options with an argument in a map, e.g.	"-foo bar" will result in a
		key/value pair foo/bar. \a unassociated finally holds all strings that
		were neither options, nor option values. */
CPPUTILS_EXP void parseArguments (
		const char **argv,
		int argc,
		StringList &simpleOptions,
		map<String, String> &complexOptions,
		StringList &unassociated
	);

/**	Parse the command line string into argc/argv. Allocates memory for \a argvPtr
		and fills \a argcPtr and \a argvPtr. */
CPPUTILS_EXP void setargv (
		const char *cmdLine,
		int *argcPtr,
		char ***argvPtr
	);

/**	Free the argv from setargv */
CPPUTILS_EXP void freeargv (
		char **argvPtr
	);

/** Helper to convert boolean */
CPPUTILS_EXP String bool2String (bool const val);

/** Helper to convert long value to string*/
CPPUTILS_EXP String long2String (long const val);

/** Helper to convert 64 long value to string*/
CPPUTILS_EXP String long64ToString(Int64 const val);

/** Helper to convert double value to string*/
CPPUTILS_EXP String double2String (double const val, int digits = 24);

/** Simple version of float2String */
CPPUTILS_EXP String simpleFloat2String (float const val);

/** Helper to convert float value to string using sprintf*/
CPPUTILS_EXP String float2String (float const val, int beforepoint, int afterpoint);

/** By default */
CPPUTILS_EXP String float2String (float const val);

/** Convert long to array of bytes*/
CPPUTILS_EXP void convertLongTo8Bytes(UnsignedLongLong const lVal, CppUtils::Byte* buffer);

/** Convert array of bytes to long*/
CPPUTILS_EXP UnsignedLongLong convert8BytesToLong(CppUtils::Byte* buffer);

/** Convert double to array of bytes*/
CPPUTILS_EXP void convertDoubleToIEEE8Bytes(double const lVal, CppUtils::Byte* buffer);

/** Custom impementation */
CPPUTILS_EXP float string2Float(CppUtils::String const &fVal);

CPPUTILS_EXP double string2DoubleRobust(CppUtils::String const &dVal);

CPPUTILS_EXP double stringToF64(const char* pString, char ** ppEnd);

CPPUTILS_EXP float stringToF32(const char* pString, char ** ppEnd);

CPPUTILS_EXP float stringToF32(const char* pString);

/** try to parse string and if fails returns default value */
CPPUTILS_EXP long string2LongDefVal(const char* pString, long defVal = -1);

/** Convert char representation to hex */
CPPUTILS_EXP void hexlify (
    int len,
    const char *dat,
    char *hex
  );

/** Convrt char representation to hex */
CPPUTILS_EXP void unhexlify (
    int len,
    const char *hex,
    char *dat
  );


CPPUTILS_EXP String hexlify (
   String data
  );


CPPUTILS_EXP String unhexlify (
   String hexdata
  );


/** Creates valid name that can be a part of file name susbtituting invalid charachters */
CPPUTILS_EXP String createValidFileName(CppUtils::String const& name);

/** Parses delimiter separeted list and pushes substrings to StringList */
CPPUTILS_EXP void parseCommaSeparated(char const delim, String const& src, StringList& data);

/** Substitute XML specific symbols so string would be contaned inside XML tags */
CPPUTILS_EXP String prepareValidXml(String const& src);

/** Wrap data to CTAGS */
CPPUTILS_EXP String wrapToCDataTags(String const& src);

CPPUTILS_EXP String wrapToCDataTags(char const* src);


/** Revert XML symbols substitutions to normal originals */
CPPUTILS_EXP String covertFromValidXml(String const& src);

/** Tokenizes input string
	str = the input string that will be tokenized
	result = the tokens for str
	delimiters = the delimiter characters
	delimiters preserve = same as above, but the delimiter characters
			will be put into the result as a token
	quote = characters to protect the enclosed characters
	esc = characters to protect a single character
*/
CPPUTILS_EXP void tokenize ( 
			const CppUtils::String& str, 
			CppUtils::StringList& result,
			const CppUtils::String& delimiters, 
			const CppUtils::String& delimiters_preserve = "",
			const CppUtils::String& quote = "\"", 
			const CppUtils::String& esc = "\\" 
);

/** substring replacment functions */
CPPUTILS_EXP void replaceOne(
	String &result, 
  String const& replaceWhat, 
  String const& replaceWithWhat);

CPPUTILS_EXP String replaceOne2(
	String const& data, 
  String const& replaceWhat, 
  String const& replaceWithWhat);

CPPUTILS_EXP void replaceAll(
	String &result, 
  String const& replaceWhat, 
  String const& replaceWithWhat);


CPPUTILS_EXP String replaceAll2(
	String const& data, 
  String const& replaceWhat, 
  String const& replaceWithWhat);


/* simple parsing of symbols residing in a file */
CPPUTILS_EXP bool parseSymbolListFile (
	CppUtils::StringSet& symbols,
	CppUtils::String const& fileName
);

CPPUTILS_EXP bool parseSymbolMapFile (
	CppUtils::StringMap& symbols,
	CppUtils::String const& fileName,
	bool isrevrese
);



CPPUTILS_EXP void trimLeft(String& str, const char* chars2remove = " ");

CPPUTILS_EXP void trimRight(String& str, const char* chars2remove = " ");

CPPUTILS_EXP void trimBoth(String& str, const char* chars2remove = " ");

CPPUTILS_EXP void replaceLeading(String& str, const char* chars2replace, char c);

CPPUTILS_EXP String extractFilePathFromName(CppUtils::String const& fullFileName);

CPPUTILS_EXP String constructTempName(char const* basePath, char const* prefix, char const* ext);
	
CPPUTILS_EXP bool nvl(char const* str);

/**
* Helper class to keep result of execution (to use instead of finallly)
*/

class CPPUTILS_EXP ExecutionResult
{
public:
	ExecutionResult(): 
		code_m(0)
	{
	}

	ExecutionResult(int const code): 
		code_m(code),
		result_m("Not Available")
	{
	}

	~ExecutionResult()
	{
	}

	inline CppUtils::String const& getResult() const
	{
		return result_m;
	}

	inline int getCode() const
	{
		return code_m;
	}

	inline bool isOk() const
	{
		return code_m == 0;
	}

	void setUp(CppUtils::String const& result, int const code)
	{
		result_m = result;
		code_m = code;
	}

	void setUp(int const code)
	{
		result_m.clear();
		code_m = code;
	}

	void setOk()
	{
		result_m.clear();
		code_m = 0;
	}

private:
	CppUtils::String result_m;

	int code_m;
};

#define SET_UP_EXECUTION_RESULT(RESULT, X, CODE)	\
{ \
	ostrstream sstr; sstr << X << ends; \
	RESULT.setUp(sstr.str(),CODE);	\
	sstr.freeze( false );	\
} 


}; // End of namespace




#endif // _CPPUTILS_MISC_INCLUDED
