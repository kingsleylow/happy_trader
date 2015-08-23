#ifndef _CPPUTILS_FIFOQUEUE_INCLUDED
#define _CPPUTILS_FIFOQUEUE_INCLUDED

#include "cpputilsdefs.hpp"
#include "log.hpp"

namespace CppUtils {
/**
 This is special class to substitute std::deque as Microsoft VS 2003 seems stuck again!
 std::deque cannot deallocate moemory after high usage
 it allocates new chunk as necessary and after usage deallocates them
 not thread safe so one need to take care of it
*/

// T - this is the bas type of object to be contained
// chunk_size - this is the consequent size of block to be allocated in a consequent chunk
	

template<class T, size_t chunk_size>
class FifoQueue{

private:
	struct Chunk {
		Chunk():
			data(0),
			next(0)//,
			//prev(0)
		{
			//count(true);

			data = (Byte*)malloc(sizeof(T) * chunk_size);
			clearChunk();
		
					
		};

		~Chunk()
		{
		
			free(data);
		}

		inline void clearChunk()
		{
			// empty all chunk
			memset(data, 0, sizeof(T) * chunk_size );
		}

		void cloneObject(int const i,T const& obj)
		{
			T* ptr = getObject(i);
			new(static_cast<void*>(ptr)) T(obj);
			
		}

		void clearObject(int const i)
		{
			T* ptr = getObject(i);
			ptr->~T();

			// and clean up this piece
			memset(ptr, 0, sizeof(T));
			
		}

		T* getObject(int const i) 
		{
				if (i >=0 && i < chunk_size) {
					// returns the pointer where the object reside

					T* ptr = reinterpret_cast<T*>(data + (size_t)(i*sizeof(T)));	
					return ptr;

				}

				return 0;
		}

							
		static void count(bool increment) {
			static CppUtils::Mutex mtx;
			static int counter = 0;
			LOCK_FOR_SCOPE(mtx) {
				if (increment) {
					counter++;
					LOG( MSG_DEBUG, "Chunk::count(true)",  "Incremented to: " << counter );
				}
				else {
					counter--;
					LOG( MSG_DEBUG, "Chunk::count(false)",  "Decremented to: " << counter );
				}
			}
		}

		Chunk* next; // pointer to next chunk
		//Chunk* prev; // pointer to next chunk
		Byte* data; // array of data
	};

public:

	FifoQueue():
		firstChunk_m( new Chunk() ),
		lastChunk_m(0),
		frontId_m(0),
		backId_m(0),
		size_m(0)
	{

		lastChunk_m = firstChunk_m;
	}

	~FifoQueue()
	{
		clear();

		// and remove the first chunk
		delete firstChunk_m;
	}

	// clear the queue
	void clear()
	{
		while(delete_front()) {};
		
	}

	// pushes object to the rear back of the queue
	void push_back(T const& obj)
	{
		if (backId_m>=0 && backId_m < chunk_size) {
			// there is a space
			lastChunk_m->cloneObject(backId_m, obj);
			
			// increment 
			backId_m++;

			size_m++;

			if (backId_m==chunk_size) {
				allocate_last_chunk();
				backId_m = 0;
			}
		}
		
	}

	// get the front reference
	T& get_front()
	{
		T* frontPtr = 0;
		
		if (frontId_m>=0 && frontId_m < chunk_size)
			frontPtr = firstChunk_m->getObject(frontId_m);

		return *frontPtr;
	}

	// remove front reference
	// returns true if removed
	bool delete_front()
	{

		if (firstChunk_m==lastChunk_m) {
			
			// no more data
			if (frontId_m>=backId_m)
				return false;

			// frontId_m <= backId_m
			if (frontId_m>=0 && frontId_m < chunk_size && frontId_m < backId_m) {
					firstChunk_m->clearObject(frontId_m);
					frontId_m++;
					if (frontId_m==chunk_size) {
						// clear the chunk as it is not necessary any more
						clear_first_chunk();
						frontId_m = 0;
						
					}

					size_m--;
					return true;
			}
			
		}
		else {
			if (frontId_m>=0 && frontId_m < chunk_size) {
				firstChunk_m->clearObject(frontId_m);
				frontId_m++;

				if (frontId_m==chunk_size) {
					deallocate_first_chunk();
					frontId_m=0;
				}
			}
		}

		size_m--;
		return true;
	
	};

	// is empty
	bool isEmpty() const
	{
		// onluy if the same chunks and front id points to the future place of back id
		if (firstChunk_m==lastChunk_m) {
			if (frontId_m>=backId_m) {
				return true;
			}
		}

		return false;
	};

	size_t size()	const
	{
		return size_m;
	}
private:

	
	void allocate_last_chunk()
	{
	
		Chunk* last = new Chunk();
		
		// the last become previous the last 
		Chunk* prev_last_chunk = lastChunk_m;
		last->next = 0;
		//last->prev = prev_last_chunk;
		
		prev_last_chunk->next = last;

		lastChunk_m = last;
		//lastChunk_m->prev = prev_last_chunk;

	}

	

	// removes the first chunk
	// returns false if the last==the first
	bool deallocate_first_chunk()
	{
		if (lastChunk_m==firstChunk_m)
			return false;

		Chunk* new_first_chunk = firstChunk_m->next;
		//new_first_chunk->prev = 0;

		delete firstChunk_m;
		firstChunk_m = new_first_chunk;

		return true;

	}

	void clear_first_chunk()
	{
		firstChunk_m->clearChunk();
		firstChunk_m->next = 0;
		//firstChunk_m->prev = 0;
	}

	
	// first chunk in a sequence
	Chunk* firstChunk_m;

	// last chunk in the sequence
	Chunk* lastChunk_m;

	// the id of what to be the front reference
	// in the first chunk
	size_t frontId_m;

	// the id of what to be the last reference
	// in the last chunk
	size_t backId_m;

	// size of the list
	size_t size_m;

	
};

}; // end of namespace

#endif