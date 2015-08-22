#ifndef _CPPUTILS_TEMPLATE_INCLUDED
#define _CPPUTILS_TEMPLATE_INCLUDED



#include "cpputilsdefs.hpp"



namespace CppUtils {





//=============================================================
// Template functions
//=============================================================

/** Read keys from map to vector */
template<class K, class T> void getKeys  (
		vector<K> &keys, map<K, T> const& values		
	)
{
	keys.clear();
	for ( map<K,T>::const_iterator it = values.begin(); it != values.end(); it++ )
		keys.push_back(it->first);
}

/** Template function to remove duplicates from an STL vector. */
template<class T> 
void reduce (vector<T> &v)
{
	set<T> s;
	int i=0, j=0;
	
	while( i < v.size() ) {
		if( s.insert( v[ i ] ).second ) {
			if( j != i )
				v[ j++ ] = v[ i ];
			else
				++j;
		}

		++i;
	}

	v.resize( j );
}


/** Template function to assign the contents of a vector to a set. */
template<class T, size_t chunk_size> void assign (
		block_allocated_set<T, chunk_size> &s,
		vector<T> const &v
	)
{
	s.clear();
	for(int i=0; i<v.size(); ++i)
		s.insert( v[ i ] );
}

/** Template function to assign the contents of a set to a vector. */
template<class T,size_t chunk_size> void assign (
		vector<T> &v,
		block_allocated_set<T, chunk_size> const &s
	)
{
	v.resize( s.size() );

	int i=0;
	for(block_allocated_set<T, chunk_size>::const_iterator it = s.begin(); it != s.end(); ++i, ++it)
		v[ i ] = *it;
}

/** Template function to assign the contents of a set to a vector. */
template<class T> void assign (
		vector<T> &v,
		std::set<T> const &s
	)
{
	v.resize( s.size() );

	int i=0;
	for(std::set<T>::const_iterator it = s.begin(); it != s.end(); ++i, ++it)
		v[ i ] = *it;
}



/** Template function to merge the contents of a vector into a set. */
template<class T, size_t chunk_size> void merge (
		block_allocated_set<T, chunk_size> &s,
		vector<T> const &v
	)
{
	for(int i=0; i<v.size(); ++i)
		s.insert( v[ i ] );
}

/** Template function to merge the contents of a vector into a set. */
template<class T> void merge (
		std::set<T> &s,
		vector<T> const &v
	)
{
	for(int i=0; i<v.size(); ++i)
		s.insert( v[ i ] );
}

/** Template function to check if a vector contains a given value. */
template<class T> bool contains (
		vector<T> const &v,
		T const &val
	)
{
	for(int i=0; i<v.size(); ++i)
		if( v[ i ] == val )
			return true;

	return false;
}

/** Template function to check if a set contains at least one
		element of a given vector. */
template<class T, size_t chunk_size> bool contains (
		block_allocated_set<T, chunk_size> const &s,
		vector<T> const &v
	)
{
	for(int i=0; i<v.size(); ++i)
		if( s.find( v[ i ] ) != s.end() )
			return true;

	return false;
}

/** Template function to transpose a dictionary of lists. */
template<class T> void transpose (
		map< T, vector<T> > &m
	)
{
	map< T, vector<T> > tmp;
	for(map< T, vector<T> >::iterator i = m.begin(); i != m.end(); ++i) {
		for(int j = 0; j < i->second.size(); ++j)
			tmp[ i->second[ j ] ].push_back( i->first );
	}

	m = tmp;
}

/** Copy 2 vectors of different type */
template<class SRC, class DST> void copyVectors (
		vector<SRC> const &src,
		vector<DST> &dst)
{
	dst.resize(src.size());
	for(int i = 0; i < src.size(); i++) {
		dst[i] = (DST)src[i];
	}
}

/** Compute the difference of two vectors. */
template<class T> void difference (
		vector<T> const &a,
		vector<T> const &b,
		vector<T> &diff
	)
{
	diff.clear();

	set<T> sa;
	for(int i=0; i<a.size(); ++i)
		sa.insert( a[ i ] );

	for(int j=0; j<b.size(); ++j)
		if( sa.find( b[ j ] ) == sa.end() )
			diff.push_back( b[ j ] );
		else
			sa.erase( b[ j ] );

	for(set<T>::const_iterator it = sa.begin(); it != sa.end(); ++it)
		diff.push_back( *it );
}


/** Compute the intersection of two vectors. */
template<class T> void intersection (
		typename vector<T> const &a,
		typename vector<T> const &b,
		typename vector<T> &intersect
	)
{
	intersect.clear();

	set<T> sa;
	for(int i=0; i<a.size(); ++i)
		sa.insert( a[ i ] );

	for(int j=0; j<b.size(); ++j)
		if( sa.find( b[ j ] ) != sa.end() )
			intersect.push_back( b[ j ] );
}

// advance container
template<class It> 
It advanceContainer(It it, size_t n) 
{    
	std::advance(it, n);    
	return it; 
} 


/** Helper macro to remove elements from a list. */
#define REMOVE_IF( L, P ) \
	{ \
		int j = 0; \
		for(int i = 0; i < L.size(); ++i) \
			if( !P ) \
				L[ j++ ] = L[ i ]; \
		L.resize( j ); \
	}




/** Least recently used element queue. 

		\attention Not thread-safe.
*/
template<class T, size_t chunk_size>
class lruqueue {

	private:

		/** LRU order of entries; LRU element is at tail. */
		block_allocated_list<T, chunk_size> order_m;
		
		/** Access order list entries via the entry. */
		typename block_allocated_map<T, typename block_allocated_list<T, chunk_size>::iterator, chunk_size> index_m;

	public:

		lruqueue (
			) :
				order_m(),
				index_m()
		{
		}

		T const &head (
			)
		{
			return order_m.back();
		}

		void touch (
				T const &val
			)
		{
			order_m.push_front( val );

			block_allocated_map<T, block_allocated_list<T, chunk_size>::iterator, chunk_size>::iterator i_index = index_m.find( val );
			if( i_index != index_m.end() ) {
				order_m.erase( i_index->second );
				i_index->second = order_m.begin();
			}
			else
				index_m[ val ] = order_m.begin();
		}

		void remove (
				T const &val
			)
		{
			block_allocated_map<T, block_allocated_list<T, chunk_size>::iterator, chunk_size>::iterator i_index = index_m.find( val );

			if( i_index != index_m.end() ) {
				order_m.erase( i_index->second );
				index_m.erase( i_index );
			}
		}

		void clear (
			)
		{
			order_m.clear();
			index_m.clear();
		}

		bool isKnown (
				T const &val
			) const
		{
			return ( index_m.find( val ) != index_m.end() );
		}

		size_t size (
			) const
		{
			return index_m.size();
		}
};

/**
	Automatically call freeze(...) of stream buffer upon exit off sco[pe
*/
template<class T>
class CleanedStreamBuffer {
public:
	CleanedStreamBuffer(bool const how):
			how_m(how)
	{
	}

	~CleanedStreamBuffer()
	{
		stream_m.freeze(how_m);
	};

	inline T& operator ()()
	{
		return stream_m;
	};


private:
	T stream_m;

	bool how_m;
};


/**
 Replace substring
*/
template<class T> 
int findAndReplace(T& source, const T& find, const T& replace) 
{ 
    int num=0; 
    size_t fLen = find.size(); 
    size_t rLen = replace.size(); 
    for (size_t pos=0; (pos=source.find(find, pos))!=T::npos; pos+=rLen) 
    { 
        num++; 
        source.replace(pos, fLen, replace); 
    } 
    return num; 
} 




/**
 Helper class for using enumerations with names
*/


class EnumerationHelper {	
private:	
    const char *name_m;	
		int ID_m;	
public:	


   EnumerationHelper(const char *name_, const int id_):	
			ID_m(id_),	
			name_m(name_)	
		{	 
    }; 

		EnumerationHelper():	
			ID_m(0),	
			name_m(NULL)	
		{	
    };	

		EnumerationHelper(EnumerationHelper const& src) :	
		ID_m(src.ID_m),	
		name_m(src.name_m)	
		{	
		};	

		EnumerationHelper& operator=(EnumerationHelper const& src)	
		{	
			ID_m = src.ID_m; 
			name_m = src.name_m;	
			return *this;	
		};	

		inline bool operator == (EnumerationHelper const& rhs) const
		{
			return (ID_m == rhs.ID_m);
		}

		inline bool operator != (EnumerationHelper const& rhs) const
		{
			return (ID_m != rhs.ID_m);
		}

    inline const char *getName() const	
		{	
        return (const char *)name_m;	
    };	

		inline CppUtils::String getName2() const
		{
			return (name_m != NULL ? name_m : "");
		}

	  inline operator int ()	
		{	
			return ID_m;	
		};	

		void setup(char const* name, int const id)
		{
			ID_m = id;
			name_m = name;

		}

		inline operator const int () const	
		{	
			return (const int)ID_m;	
		};	
};

#define DEFINE_ENUM_ENTRY(ENTRY, ID)	\
	const CppUtils::EnumerationHelper ENTRY(#ENTRY, ID);	\
	const int ENTRY##_ID_ = ID;

#define GET_ENUM_NAME(ENTRY)	\
	ENTRY.getName()

#define GET_ENUM_ENTRY(ENTRY)	\
	ENTRY##_ID_


// ---------------------------------------
// this is the base class for all enumeration helpers
// -----------------------------------------
	class EnumerationHelperBase {
	public:
		EnumerationHelperBase()
		{
		}

		
		EnumerationHelperBase(CppUtils::EnumerationHelper const& enumMember):
			enumMember_m(enumMember)
		{
		}

		
		~EnumerationHelperBase()
		{
		}

		

		// must be implemented
		virtual void clear() = 0;
		
		inline CppUtils::EnumerationHelper& getMember()
		{
			return enumMember_m;
		}

		inline  CppUtils::EnumerationHelper const& getMember() const
		{
			return enumMember_m;
		}

		virtual CppUtils::String toString() const
		{
			return enumMember_m.getName();
		}

	private:
			CppUtils::EnumerationHelper enumMember_m;
	};

};  // End of namespace


#endif // _CPPUTILS_TEMPLATE_INCLUDED
