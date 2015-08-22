/**********************************************************************
 *
 * PURPOSE:
 * MD5 Message-Digest class derived from the RSA Data Security, Inc.
 * MD5 Message-Digest Algorithm as published in RFC 1321, April 1992.
 *
 * ASSUMES:
 * size_t as unsigned.
 *
 * NOTE:
 * Currently the MD5::update length is limited to unsigned int, which may
 * be less than uint32.
 *
 * COPYRIGHT:
 * Copyright (c) 1995, 1996, 1997 Tree Frog Software, All rights reserved.
 * This source code and the binaries that result may be freely distributed,
 * used and modified as long as the above copyright notice remains intact.
 *
 * WARRANTY:
 * The author of MD5.h (hereafter referred to as "the author") makes no
 * warranty of any kind, expressed or implied, including without limitation,
 * any warranties of merchantability and/or fitness for a particular purpose.
 * The author shall not be liable for any damages, whether direct, indirect,
 * special, or consequential arising from a failure of this program to
 * operate in the manner desired by the user.  The author shall not be liable
 * for any damage to data or property which may be caused directly or
 * indirectly by use of the program.
 *
 * In no event will the author be liable to the user for any damages,
 * including any lost profits, lost savings, or other incidental or
 * consequential damages arising out of the use or inability to use the
 * program, or for any claim by any other party.
 *
 * -------------------- MD5 RSA copyright follows: -------------------------
 *
 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 *
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 *
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 *
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 *
 * So there!
 **********************************************************************/



#ifndef _CPPUTILS_MD5_INCLUDED
#define _CPPUTILS_MD5_INCLUDED


#include "cpputilsdefs.hpp"


namespace CppUtils {


/** Class MD5 is a 3rd party implementation of the MD5
		hashing algorithm. It can consume any byte array as
		input data and produces hash digests of type Signature.
*/
class CPPUTILS_EXP MD5 {

public:

	/** Ctor, calling init(). */
	MD5 ( void );

	/** Dtor, calling clear(). */
	virtual ~MD5( void );

	/** Update the internal hash state with the input data
			of given length. */
	void update( const void *input, size_t length );

	/** Generate the hash digest. */
	void report( char * );

	/** Re-initialize the object. */
	void init(  void );

	/** Clear internal buffers. */
	void clear( void );

	/** Scramble the input string by computing its hash with
			a secret salt value and return the resulting hash digest
			as signature. */
	static Signature scramble (String const &str);


protected:

   unsigned int m_state[4];         // State (ABCD)
   unsigned int m_count[2];         // Number of bits, modulo 2^64 (lsb first).
   unsigned char m_buffer[64];       // Input buffer.
   void transform( const unsigned char block[64] );
   void encode( unsigned char *output, const unsigned int *input, int len );
   void encode( char *digest );
   void decode( unsigned int *output, const unsigned char *input, int len );
};


}; // End of namespace



#endif // _CPPUTILS_MD5_INCLUDED
