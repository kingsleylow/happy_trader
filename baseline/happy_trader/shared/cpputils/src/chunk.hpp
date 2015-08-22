

#ifndef _CPPUTILS_CHUNK_INCLUDED
#define _CPPUTILS_CHUNK_INCLUDED


#include "cpputilsdefs.hpp"


namespace CppUtils {


//=============================================================
// Class MemoryChunk
//=============================================================
/** Class MemoryChunk is used to encapsulated a contingous piece
		of memory along with its length in an object. It provides
		basic access functionality, including resize.

	
*/
class CPPUTILS_EXP MemoryChunk {

public:
						MemoryChunk ( );
            MemoryChunk (  MemoryChunk const &src );
            explicit MemoryChunk (  string const &arg );
            explicit MemoryChunk (  int length  );
            ~MemoryChunk(    );
            int length ( ) const;
            void length ( int newlen );
            Byte *data ( );
            Byte const *data ( ) const;
            bool operator == ( MemoryChunk const &rhs ) const;
            bool operator < ( MemoryChunk const &rhs ) const;
            MemoryChunk & operator = ( MemoryChunk const &rhs  );
            void set(Byte const* datap, int const lengthd);
            string toString(int maxbytes = -1) const;
            string toHexString(int maxbytes = -1) const;
            void fromString(string const& src);
            void fromString(char const* src);
            MemoryChunk& operator << (MemoryChunk const &rhs);
            MemoryChunk& operator << (string const& str);
            MemoryChunk& operator << (char const* str); 
	
private:

	 int length_m;
   Byte *chunk_m; 
};



/** Short-hand. */
typedef vector<MemoryChunk> MemoryChunkList;

/** Short-hand. */
typedef set<MemoryChunk> MemoryChunkSet;



// -------------------------------------------------------------
/** Structure holding two binary chunks that define a small and
    a large icon. */
struct Icon {
  /** 16x16 icon. */
  CppUtils::MemoryChunk smallIcon;

  /** 32x32 icon. */
  CppUtils::MemoryChunk largeIcon;

  /** Ctor. */
  Icon () :
    smallIcon(),
    largeIcon()
  {
  }
};



// End of namespace
}


#endif // _CPPUTILS_CHUNK_INCLUDED

