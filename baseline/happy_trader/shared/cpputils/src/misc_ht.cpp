#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T 1
#endif

#include "misc_ht.hpp"

#include <algorithm>
#include <stdarg.h>
#include <float.h>
#include "except.hpp"
#include "template.hpp"
#include "assertdef.hpp"
#include <time.h>




namespace CppUtils {



//=============================================================
// Functions
//=============================================================
void tolower (	String &str	)
{
	for(int i = 0; i < str.length(); ++i)
		str[ i ] = (char) ::tolower( str[ i ] );
}

void toupper (	String &str )
{
	for(int i = 0; i < str.length(); ++i)
		str[ i ] = (char) ::toupper(str[ i ] );
}


//-------------------------------------------------------------

String toLowerCase (	String const & val	)
{
	CppUtils::String val_s(val);
	tolower(val_s);

	return val_s;
}

//-------------------------------------------------------------

String toUpperCase (	String const & val	)
{
	CppUtils::String val_s(val);
	toupper(val_s);

	return val_s;
}


//-------------------------------------------------------------
void parseArguments (
		const char **argv,
		int argc,
		StringList &simpleOptions,
		map<String, String> &complexOptions,
		StringList &unassociated
	)
{
	// Clear the results
	simpleOptions.clear();
	complexOptions.clear();
	unassociated.clear();

	// Go through args
	int i = 0;
	while( i<argc ) {
		// Is this an option?
		if( *argv[i] != '-' )
			// No; store as unassociated item
			unassociated.push_back( argv[ i ] );
		else {
			// Get the option string
			const char *c = argv[ i ];
			while( *c == '-' ) ++c;

			// Is the option a complex one (next item does not begin with a dash)?
			if( i+1 < argc && *argv[i+1] != '-' )
				// Yes; complex option
				complexOptions[ c ] = argv[ ++i ];
			else
				// No; simple option
				simpleOptions.push_back( c );
		}

		// Go on
		++i;
	}
}
//-------------------------------------------------------------
void setargv(
		const char *cmdLine,
		int *argcPtr,
		char ***argvPtr
	)
{
  const char *p;
	char *arg, *argSpace;
  char **argv;
  int argc, size, inquote, copy, slashes;

  /*
   * Precompute an overly pessimistic guess at the number of arguments
   * in the command line by counting non-space spans.
   */

  size = 2;
  for (p = cmdLine; *p != '\0'; p++) {
    if (isspace(*p)) {
      size++;
      while (isspace(*p)) {
        p++;
      }
      if (*p == '\0') {
        break;
      }
    }
  }
  argSpace = (char *) malloc((unsigned) (size * sizeof(char *)
        + strlen(cmdLine) + 1));
  argv = (char **) argSpace;
  argSpace += size * sizeof(char *);
  size--;

  p = cmdLine;
  for (argc = 0; argc < size; argc++) {
    argv[argc] = arg = argSpace;
    while (isspace(*p)) {
      p++;
    }
    if (*p == '\0') {
      break;
    }

    inquote = 0;
    slashes = 0;
    while (1) {
      copy = 1;
      while (*p == '\\') {
        slashes++;
        p++;
      }
      if (*p == '"') {
        if ((slashes & 1) == 0) {
          copy = 0;
          if ((inquote) && (p[1] == '"')) {
            p++;
            copy = 1;
          } else {
            inquote = !inquote;
          }
        }
        slashes >>= 1;
      }

      while (slashes) {
        *arg = '\\';
        arg++;
        slashes--;
      }

      if ((*p == '\0') || (!inquote && isspace(*p))) {
        break;
      }
      if (copy != 0) {
        *arg = *p;
        arg++;
      }
      p++;
    }
    *arg = '\0';
    argSpace = arg + 1;
  }
  argv[argc] = NULL;

  *argcPtr = argc;
  *argvPtr = argv;
}
//-------------------------------------------------------------
void freeargv (char **argvPtr)
{
	if(argvPtr != NULL)
	{
		free (argvPtr);
	}
	argvPtr = NULL;
}


String bool2String (bool const val)
{
	return (val == true ? "true" : "false");
}

String long2String (long const val)
{
	char buf[34];
	ltoa(val, buf, 10);

	return buf;
}

String long64ToString(Int64 const val)
{
	char buf[66];
	_i64toa(val, buf, 10);

	return buf;
}

String double2String (double const val, int digits)
{
	if (_finite (val)==0 ) {
		return "INF-NaN";
	}


	char buf[_CVTBUFSIZE];
	_gcvt(val, digits, buf);

	return buf;
}

String simpleFloat2String (float const val)
{
	return float2String (val, -1, 4);
}

String float2String (float const val, int beforepoint, int afterpoint)
{
	if (_finite ((double)val)==0 ) {
		return "INF-NaN";
	}

	char buf[_CVTBUFSIZE];
	char format[] = {'%','0','.','0','f', '\0'};
	

	if (beforepoint >=1 && beforepoint <=9)
		format[1] = '0' + char(beforepoint);
		
	
	if (afterpoint >=1 && afterpoint <=9)
		format[3] = '0' + char(afterpoint);

	sprintf(buf, format, val);
	
	return buf;
}

String float2String (float const val)
{
	return float2String(val, -1, 4);
}

void convertLongTo8Bytes(UnsignedLongLong const lVal, CppUtils::Byte* buffer)
{
				
				unsigned short bmask = 0x00FF;
				
				buffer[0] = (CppUtils::Byte) (lVal >> 56);
        buffer[1] = (CppUtils::Byte) ((lVal >> 48) & bmask);
        buffer[2] = (CppUtils::Byte) ((lVal >> 40) & bmask);
        buffer[3] = (CppUtils::Byte) ((lVal >> 32) & bmask);
        buffer[4] = (CppUtils::Byte) ((lVal >> 24) & bmask);
        buffer[5] = (CppUtils::Byte) ((lVal >> 16) & bmask);
        buffer[6] = (CppUtils::Byte) ((lVal >> 8) & bmask);
        buffer[7] = (CppUtils::Byte) (lVal & bmask);
}


UnsignedLongLong convert8BytesToLong(CppUtils::Byte* buffer)
{
	return (((UnsignedLongLong)buffer[0]) << 56) |
           (((UnsignedLongLong)buffer[1] & 0x0ffL) << 48) |
           (((UnsignedLongLong)buffer[2] & 0x0ffL) << 40) |
           (((UnsignedLongLong)buffer[3] & 0x0ffL) << 32) |
           (((UnsignedLongLong)buffer[4] & 0x0ffL) << 24) |
           (((UnsignedLongLong)buffer[5] & 0x0ffL) << 16) |
           (((UnsignedLongLong)buffer[6] & 0x0ffL) << 8) |
           ((UnsignedLongLong)buffer[7] & 0x0ff);
}

void convertDoubleToIEEE8Bytes(double const lVal, CppUtils::Byte* buffer)
{
	// not impl
	HTASSERT(false);
}


float string2Float(CppUtils::String const &fVal)
{
	const char* wcs = fVal.c_str();
	int hdr=0; 
	while (wcs[hdr]==' ') 
     hdr++; 
 
	int cur=hdr; 
 
	bool negative=false; 
	bool has_sign=false; 
 
	if (wcs[cur]=='+' || wcs[cur]=='-') 
	{ 
        if (wcs[cur]=='-') 
                negative=true; 
        has_sign=true; 
        cur++; 
	} 
	else 
     has_sign=false; 
 
	int quot_digs=0; 
	int frac_digs=0; 
 
	bool full=false; 
 
	char period=0; 
	int binexp=0; 
	int decexp=0; 
	unsigned long value=0; 
 
	while (wcs[cur]>='0' && wcs[cur]<='9') 
	{ 
        if (!full) 
        { 
                if (value>=0x19999999 && wcs[cur]-'0'>5 ||  value>0x19999999) 
                { 
                        full=true; 
                        decexp++; 
                } 
                else 
                     value=value*10+wcs[cur]-'0'; 
        } 
        else 
            decexp++; 
 
        quot_digs++; 
        cur++; 
	} 
 
	if (wcs[cur]=='.' || wcs[cur]==',') 
	{ 
        period=wcs[cur]; 
        cur++; 
 
        while (wcs[cur]>='0' && wcs[cur]<='9') 
        { 
                if (!full) 
                { 
                        if (value>=0x19999999 && wcs[cur]-'0'>5 || value>0x19999999) 
                                full=true; 
                        else 
                        { 
                                decexp--; 
                                value=value*10+wcs[cur]-'0'; 
                        } 
                } 
 
                frac_digs++; 
                cur++; 
        } 
	} 
 
	if (!quot_digs && !frac_digs) 
        return 0; 
 
	char exp_char=0; 
 
	int decexp2=0; // explicit exponent 
	bool exp_negative=false; 
	bool has_expsign=false; 
	int exp_digs=0; 
 
	// even if value is 0, we still need to eat exponent chars 
	if (wcs[cur]=='e' || wcs[cur]=='E') 
	{ 
        exp_char=wcs[cur]; 
        cur++; 
 
        if (wcs[cur]=='+' || wcs[cur]=='-') 
        { 
                has_expsign=true; 
                if (wcs[cur]=='-') 
                        exp_negative=true; 
                cur++; 
        } 
 
        while (wcs[cur]>='0' && wcs[cur]<='9') 
        { 
                if (decexp2>=0x19999999) 
                        return 0; 
                decexp2=10*decexp2+wcs[cur]-'0'; 
                exp_digs++; 
                cur++; 
        } 
 
        if (exp_negative) 
                decexp-=decexp2; 
        else 
                decexp+=decexp2; 
	} 
 
	// end of wcs scan, cur contains value's tail 
 
	if (value) 
	{ 
        while (value<=0x19999999) 
        { 
                decexp--; 
                value=value*10; 
        } 
  
        if (decexp) 
        { 
                // ensure 1bit space for mul by something lower than 2.0 
                if (value&0x80000000) 
                { 
                        value>>=1; 
                        binexp++; 
                } 
 
                if (decexp>308 || decexp<-307) 
                        return 0; 
 
                // convert exp from 10 to 2 (using FPU) 
                int E; 
                double v=pow(10.0,decexp); 
                double m=frexp(v,&E); 
                m=2.0*m; 
                E--; 
                value=(unsigned long)floor(value*m); 
 
                binexp+=E; 
        } 
 
        binexp+=23; // rebase exponent to 23bits of mantisa 
 
 
        // so the value is: +/- VALUE * pow(2,BINEXP); 
        // (normalize manthisa to 24bits, update exponent) 
        while (value&0xFE000000) 
        { 
                value>>=1; 
                binexp++; 
        } 
        if (value&0x01000000) 
        { 
                if (value&1) 
                        value++; 
                value>>=1; 
                binexp++; 
                if (value&0x01000000) 
                { 
                        value>>=1; 
                        binexp++; 
                } 
        } 
 
        while (!(value&0x00800000)) 
        { 
                value<<=1; 
                binexp--; 
        } 
 
        if (binexp<-127) 
        { 
                // underflow 
                value=0; 
                binexp=-127; 
        } 
        else 
        if (binexp>128) 
                return false; 
 
        //exclude "implicit 1" 
        value&=0x007FFFFF; 
 
        // encode exponent 
        unsigned long exponent=(binexp+127)<<23; 
        value |= exponent; 
	} 
 
	// encode sign 
	unsigned long sign=negative<<31; 
	value |= sign; 

	float result;
	*(unsigned long*)(&result)=value;

  return result; 
	 
 
}

double string2DoubleRobust(CppUtils::String const &dVal)
{
	char *stop = NULL;
	double result = strtod(dVal.c_str(),&stop); 

	if (result==HUGE_VAL) {
		char buf[128];
		CppUtils::hexlify(64, stop, buf);

		THROW(CppUtils::OperationFailed, "msg_InvalidDoubleConversion", "ctx_miscFun", "Scan stopped at: " + CppUtils::String(buf));
	}
	return result;
}

//******************************************************************************
// StringToF64
// This is a wrapper on 'strtod' because the VS2008 implementation calls
// 'strlen' on the input string which can be very slow if the parsed number is
// part of a large buffer. In order to speed up the potential 'strlen' we load
// the number portion of the string into an internal buffer first.
// 
//    strtod expects the input to point to a string of the following form:
//    [whitespace] [sign] [digits] [.digits] [{d | D | e | E}[sign]digits]
//******************************************************************************
double stringToF64(const char* pString, char ** ppEnd)
{
    //---
    // Find the start of the string
    const char * pNumberStart = pString;
 
    // skip whitespace
    while( isspace(*pNumberStart) )
        ++pNumberStart;
 
    //---
    // Find the end of the string
    const char * pNumberEnd = pNumberStart;
 
    // skip optional sign
    if( *pNumberEnd == '-' || *pNumberEnd == '+' )
        ++pNumberEnd;
 
    // skip optional digits
    while( isdigit(*pNumberEnd) )
        ++pNumberEnd;
 
    // skip optional decimal and digits
    if( *pNumberEnd == '.' )
    {
        ++pNumberEnd;
 
        while( isdigit(*pNumberEnd) )
            ++pNumberEnd;
    }
 
    // skip optional exponent
    if(    *pNumberEnd == 'd'
        || *pNumberEnd == 'D'
        || *pNumberEnd == 'e'
        || *pNumberEnd == 'E' )
    {
        ++pNumberEnd;
 
        if( *pNumberEnd == '-' || *pNumberEnd == '+' )
            ++pNumberEnd;
 
        while( isdigit(*pNumberEnd) )
            ++pNumberEnd;
    }
 
    //---
    // Process the string
    const unsigned numberLen = (pNumberEnd-pNumberStart);
    char buffer[32];
    if( numberLen+1 < sizeof(buffer)/sizeof(buffer[0]) )
    {
        // copy into buffer and terminate with NUL before calling the
        // standard function
        memcpy( buffer, pNumberStart, numberLen*sizeof(buffer[0]) );
        buffer[numberLen] = '\0';
        const double result = strtod( buffer, ppEnd );
 
        // translate end of string back from the buffer to the input string
        if( ppEnd )
        {
            *ppEnd = const_cast<char*>( pNumberStart + (*ppEnd-buffer) );
        }
 
        return result;
    }
    else
    {
        // buffer was too small so just call the standard function on the
        // source input to get a proper result
        return strtod( pString, ppEnd );
    }
}
 
//******************************************************************************
// StringToF32
// This is a helper function to wrap up StringToF64 for 32 bit values.
//******************************************************************************
float stringToF32(const char* pString, char ** ppEnd)
{
    return (float)stringToF64(pString,ppEnd);            
}

float stringToF32(const char* pString)
{
	char* pEnd;
	return stringToF32(pString, &pEnd);
}



// -------------------------------------

long string2LongDefVal(const char* pString, long defVal )
{
	if (pString == NULL)
		return defVal;

	try {
		return std::stol(CppUtils::String(pString));
	}
	catch(std::invalid_argument& e)
	{
	}
	catch(std::out_of_range& e2)
	{
	}

	return defVal;
}


 // -------------------------------------

void hexlify (
    int len,
    const char *dat,
    char *hex
  )
{
  for( ;len > 0; --len, ++dat) {
    *hex = (*dat>>4 & 0x0f);
    *hex += (*hex < 0x0a ? '0' : 'A'-10);
    ++hex;

    *hex = (*dat & 0x0f);
    *hex += (*hex < 0x0a ? '0' : 'A'-10);
    ++hex;
  }

	*hex = 0;
}


// -------------------------------------------------------
void unhexlify (
    int len,
    const char *hex,
    char *dat
  )
{
  char buf;

  for( ;*hex && len > 0; len-=2, ++dat) {
    buf = *hex - (*hex > '9' ? (*hex >= 'a' ? 'a'-10 : 'A'-10) : '0');
    *dat = (buf & 0x0f) << 4;
    ++hex;

    buf = *hex - (*hex > '9' ? (*hex >= 'a' ? 'a'-10 : 'A'-10) : '0');
    *dat |= (buf & 0x0f);
    ++hex;
  }
}

CPPUTILS_EXP String hexlify (
   String data
  )
{
	String hex_param;
	if (data.length() > 0) {
		hex_param.resize(data.length()*2);

		char * ptr = (char*)hex_param.c_str();
		hexlify(	data.length(), data.c_str(),	ptr );
	}

	return hex_param;
}


CPPUTILS_EXP String unhexlify (
   String hexdata
  )
{
	String unhex_param;
	if (hexdata.length() > 0) {
		unhex_param.resize(hexdata.size()/2);

		char * ptr = (char*)unhex_param.c_str();
		CppUtils::unhexlify(hexdata.size(), hexdata.c_str(), ptr);
	}

	return unhex_param;
}

// --------------------------------------------------------



// --------------------------------------------------------

String createValidFileName(CppUtils::String const& name)
{
	String result;
	

	for(int i = 0; i < name.size(); i++) {
		char c = name.at(i);
		if ((c >= 'A' && c <= 'Z') ||
			 (c >= 'a' && c <= 'z')	||
			 (c >= '0' && c <= '9'))
			 result += c;
		else {
			
			char c1 = (c >> 4 & 0x0f);
			c1 += (c1 < 0x0a ? '0' : 'A'-10);

			char c2 = (c & 0x0f);
			c2 += (c2 < 0x0a ? '0' : 'A'-10);
			
			result += '_';
			result += c1;
			result += c2;
			result += '_';
		}
	}

	return result;
}

void parseCommaSeparated(char const delim, CppUtils::String const& src, StringList& data)
{
	data.clear();
	String srcs = src;
	if (src.at(src.size()-1) != delim )
		srcs = srcs + delim;

	int token = 0;  
  int prev = 0;
	
	int n1 = srcs.size(); 
	

	for (int i = 0; i < n1; i++) {  
		if (srcs.at(i)==delim ) {
			data.push_back( srcs.substr(prev, i-prev) );

			prev = i+1;
      token++;
		}
	}
}

static const char* to_search_g[]	= {"&", "<", ">", "\"", "'", 0};
static const char* to_subst_g[]		= {"&amp;", "&lt;", "&gt;", "&quot;", "&apos;", 0};

String prepareValidXml(String const& src)
{
	String result;
	String srcs = src;

	
	for(int i = 0; to_search_g[i] != 0; i++ ) {
		
		int fidx = 0;
		result.clear();
		while (true) {
			int idx = srcs.find(to_search_g[i], fidx );
			if (idx >= 0) {
				// before idx
				result.append( srcs.substr(fidx, idx - fidx) );
				result.append(to_subst_g[i]);
		
				// skip symbol
				fidx = idx + 1;
			}
			else {
				result.append(srcs.substr(fidx) );
				break;
			}
		}

		// store result;
		srcs = result;


	}

	return result;
	

};

String wrapToCDataTags(String const& src)
{
	CppUtils::String src_s(src);
	CppUtils::findAndReplace<String>(src_s, "]]>", "]]]]><![CDATA[>");

	return CppUtils::String("<![CDATA[") + src_s + "]]>";
}

String wrapToCDataTags(char const* src)
{
	CppUtils::String src_s(src);
	CppUtils::findAndReplace<String>(src_s, "]]>", "]]]]><![CDATA[>");
	CppUtils::String result =  CppUtils::String("<![CDATA[") + src_s + "]]>";

	return result;
}

String covertFromValidXml(String const& src)
{
	String result;
	String srcs = src;
	
	for(int i = 0; to_search_g[i] != 0; i++ ) {
		
		int fidx = 0;
		result.clear();
		while (true) {
			int idx = srcs.find(to_subst_g[i], fidx );
			if (idx >= 0) {
				// before idx
				result.append( srcs.substr(fidx, idx - fidx) );
				result.append(to_search_g[i]);
		
				// skip symbol
				fidx = idx + strlen(to_subst_g[i]);
			}
			else {
				result.append(srcs.substr(fidx) );
				break;
			}
		}

		// store result;
		srcs = result;


	}

	return result;
	
}


// -------------------------------------

void tokenize ( 
	const CppUtils::String& str, 
	CppUtils::StringList& result,
	const CppUtils::String& delimiters, 
	const CppUtils::String& delimiters_preserve,
	const CppUtils::String& quote, 
	const CppUtils::String& esc 
)
{
	// clear the vector
	if ( false == result.empty() )
	{
		result.clear();
	}

	CppUtils::String::size_type pos = 0; // the current position (char) in the CppUtils::String
	char ch = 0; // buffer for the current character
	char delimiter = 0;	// the buffer for the delimiter char which
							// will be added to the tokens if the delimiter
							// is preserved
	char current_quote = 0; // the char of the current open quote
	bool quoted = false; // indicator if there is an open quote
	CppUtils::String token;  // CppUtils::String buffer for the token
	bool token_complete = false; // indicates if the current token is
								 // read to be added to the result vector
	CppUtils::String::size_type len = str.length();  // length of the input-CppUtils::String

	// for every char in the input-CppUtils::String
	while ( len > pos )
	{
		// get the character of the CppUtils::String and reset the delimiter buffer
		ch = str.at(pos);
		delimiter = 0;

		// assume ch isn't a delimiter
		bool add_char = true;

		// check ...

		// ... if the delimiter is an escaped character
		bool escaped = false; // indicates if the next char is protected
		if ( false == esc.empty() ) // check if esc-chars are  provided
		{
			if ( CppUtils::String::npos != esc.find_first_of(ch) )
			{
				// get the escaped char
				++pos;
				if ( pos < len ) // if there are more chars left
				{
					// get the next one
					ch = str.at(pos);

					// add the escaped character to the token
					add_char = true;
				}
				else // cannot get any more characters
				{
					// don't add the esc-char
					add_char = false;
				}

				// ignore the remaining delimiter checks
				escaped = true;
			}
		}

		// ... if the delimiter is a quote
		if ( false == quote.empty() && false == escaped )
		{
			// if quote chars are provided and the char isn't protected
			if ( CppUtils::String::npos != quote.find_first_of(ch) )
			{
				// if not quoted, set state to open quote and set
				// the quote character
				if ( false == quoted )
				{
					quoted = true;
					current_quote = ch;

					// don't add the quote-char to the token
					add_char = false;
				}
				else // if quote is open already
				{
					// check if it is the matching character to close it
					if ( current_quote == ch )
					{
						// close quote and reset the quote character
						quoted = false;
						current_quote = 0;

						// don't add the quote-char to the token
						add_char = false;
					}
				} // else
			}
		}

		// ... if the delimiter isn't preserved
		if ( false == delimiters.empty() && false == escaped &&
			 false == quoted )
		{
			// if a delimiter is provided and the char isn't protected by
			// quote or escape char
			if ( CppUtils::String::npos != delimiters.find_first_of(ch) )
			{
				// if ch is a delimiter and the token CppUtils::String isn't empty
				// the token is complete
				if ( false == token.empty() ) // BUGFIX: 2006-03-04
				{
					token_complete = true;
				}

				// don't add the delimiter to the token
				add_char = false;
			}
		}

		// ... if the delimiter is preserved - add it as a token
		bool add_delimiter = false;
		if ( false == delimiters_preserve.empty() && false == escaped &&
			 false == quoted )
		{
			// if a delimiter which will be preserved is provided and the
			// char isn't protected by quote or escape char
			if ( CppUtils::String::npos != delimiters_preserve.find_first_of(ch) )
			{
				// if ch is a delimiter and the token CppUtils::String isn't empty
				// the token is complete
				if ( false == token.empty() ) // BUGFIX: 2006-03-04
				{
					token_complete = true;
				}

				// don't add the delimiter to the token
				add_char = false;

				// add the delimiter
				delimiter = ch;
				add_delimiter = true;
			}
		}


		// add the character to the token
		if ( true == add_char )
		{
			// add the current char
			token.push_back( ch );
		}

		// add the token if it is complete
		if ( true == token_complete && false == token.empty() )
		{
			// add the token CppUtils::String
			result.push_back( token );

			// clear the contents
			token.clear();

			// build the next token
			token_complete = false;
		}

		// add the delimiter
		if ( true == add_delimiter )
		{
			// the next token is the delimiter
			CppUtils::String delim_token;
			delim_token.push_back( delimiter );
			result.push_back( delim_token );

			// REMOVED: 2006-03-04, Bugfix
		}

		// repeat for the next character
		++pos;
	} // while

	// add the final token
	if ( false == token.empty() )
	{
		result.push_back( token );
	}
}

void replaceOne(
	String &result, 
  String const& replaceWhat, 
  String const& replaceWithWhat)
{

	const int pos = result.find(replaceWhat);
  if (pos==-1) 
		return;
  result.replace(pos,replaceWhat.size(),replaceWithWhat);

}

String replaceOne2(
	String const& data, 
  String const& replaceWhat, 
  String const& replaceWithWhat)
{
	String result = data;
	replaceOne(result, replaceWhat, replaceWithWhat);

	return result;
}

void replaceAll(
	String &result, 
  String const& replaceWhat, 
  String const& replaceWithWhat)
{
	while(1)  {
    const int pos = result.find(replaceWhat);
    if (pos==-1) 
			break;
    result.replace(pos,replaceWhat.size(),replaceWithWhat);
  }
 
}

String replaceAll2(
	String const& data, 
  String const& replaceWhat, 
  String const& replaceWithWhat)
{
	
	String result = data;
	replaceAll(result, replaceWhat, replaceWithWhat);
	return result;

}



 bool parseSymbolListFile (
	CppUtils::StringSet& symbols,
	CppUtils::String const& fileName
)
{
	CppUtils::String content;
	if (!CppUtils::readContentFromFile2(fileName, content))
		return false;

	CppUtils::StringList slist;
	CppUtils::tokenize(content, slist, "\n");

	symbols.clear();
	for(int i = 0; i < slist.size(); i++) {
		symbols.insert(slist[i]);
	}

	return true;
}
		

 bool parseSymbolMapFile (
	CppUtils::StringMap& symbols,
	CppUtils::String const& fileName,
	bool isrevrese
)
{
	

	CppUtils::String content;
	if (!CppUtils::readContentFromFile2(fileName, content))
		return false;

	CppUtils::StringList slist;
	CppUtils::tokenize(content, slist, ",\n");

	symbols.clear();
	int i = 0;
	while(true ) {
		if ((i+1) < slist.size()) {
			if (!isrevrese)
				symbols[ slist[i] ] = slist[i + 1];
			else
				symbols[ slist[i+1] ] = slist[i];
			i += 2;
		}
		else
			break;
	}

	return true;
}



void trimLeft(String& str, const char* chars2remove)
{
	if (!str.empty())
	{
		String::size_type pos = str.find_first_not_of(chars2remove);

		if (pos != String::npos)
			str.erase(0,pos);
		else
			str.erase( str.begin() , str.end() ); // make empty
	}
}

void trimRight(String& str, const char* chars2remove)
{
	if (!str.empty())
	{
		String::size_type pos = str.find_last_not_of(chars2remove);

		if (pos != String::npos)
			str.erase(pos+1);
		else
			str.erase( str.begin() , str.end() ); // make empty
	}
}

void trimBoth(String& str, const char* chars2remove )
{
	trimRight(str, chars2remove);
	trimLeft(str, chars2remove);
}


void replaceLeading(String& str, const char* chars2replace, char c)
{
	if (!str.empty())
	{
		String::size_type pos = str.find_first_not_of(chars2replace);

		if (pos != String::npos)
			str.replace(0,pos,pos,c);
		else
		{
			int n = str.size();
			str.replace(str.begin(),str.end()-1,n-1,c);
		}
	}
}


CppUtils::String extractFilePathFromName(CppUtils::String const& fullFileName)
{
	return fullFileName.substr( 0, fullFileName.rfind(PATH_SEPARATOR_CHAR)+1 );
}

CppUtils::String	constructTempName(char const* basePath, char const* prefix, char const* ext)
{
	if (basePath == NULL || prefix == NULL || ext==NULL) 
		return "";

	CppUtils::String logFileName;
	do {
		struct tm *now; long lt;
		time( &lt );
		now = localtime( &lt );
		static char buf[1024];
		sprintf( buf, "%s-%4d%02d%02d-%02d%02d%02d_", prefix, now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec );
			
		CppUtils::String basePathP = CppUtils::String(basePath);
		if (basePathP[basePathP.length()-1] != PATH_SEPARATOR_CHAR)
			 basePathP += PATH_SEPARATOR_CHAR;

		logFileName = CppUtils::String(basePath) + buf + CppUtils::String(ext);
				

		// Check if the file really does not exist
		FILE *f = fopen( logFileName.c_str(), "r" );
		if( f )
			// File exists! Close handle and repeat loop
			fclose( f );
		else
			// File does not exist; can break
			break;
	} while( true );

	

	return logFileName;
}


bool nvl(char const* str)
{
	return str == NULL || strlen(str) == 0;
}

// End of namespace
}
