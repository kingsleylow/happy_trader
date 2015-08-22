
#include "chunk.hpp"
#include "misc_ht.hpp"


namespace CppUtils {



	// ----------------------------

	MemoryChunk::MemoryChunk() :
		length_m(0),
		chunk_m(NULL)
	{
	}
	//-------------------------------------------------------------

	MemoryChunk::MemoryChunk(MemoryChunk const &src) :
		length_m(src.length_m),
		chunk_m(NULL) 
	{
		// Copy chunk
		if (length_m > 0) {
			chunk_m = (Byte *) malloc(length_m);
			memcpy(chunk_m, src.chunk_m, length_m);
		}
	}
	//-------------------------------------------------------------

	MemoryChunk::MemoryChunk(string const &arg) :
		length_m(arg.length()),
		chunk_m(NULL) 
	{
		if (arg.length() > 0) {
			chunk_m = (Byte *) malloc(length_m);
			memcpy(chunk_m, arg.c_str(), length_m);
		}
	}
	//-------------------------------------------------------------

	MemoryChunk::MemoryChunk(int length) :
		length_m(length),
		chunk_m(NULL) 
	{
		if (length > 0) {
			chunk_m = (Byte *) malloc(length);
			memset(chunk_m, 0, length);
		}
	}
	//-------------------------------------------------------------

	MemoryChunk::~MemoryChunk() {
		if (chunk_m)
			free(chunk_m);
	}

	int MemoryChunk::length() const {
		return length_m;
	}
	//-------------------------------------------------------------

	void MemoryChunk::length(int len) {
		if (length_m != len) {
			length_m = len;
			if (len > 0) {
				chunk_m = (Byte *) realloc(chunk_m, len);
			} else {
				free(chunk_m);
				chunk_m = NULL;
			}
		}
	}
	//-------------------------------------------------------------

	Byte * MemoryChunk::data() 
	{
		return chunk_m;
	}
	//-------------------------------------------------------------

	Byte const * MemoryChunk::data() const 
	{
		return chunk_m;
	}

	bool MemoryChunk::operator==(MemoryChunk const &rhs) const {
		return length_m == rhs.length_m && !memcmp(chunk_m, rhs.chunk_m, length_m);
	}
	//-------------------------------------------------------------

	bool MemoryChunk::operator<(MemoryChunk const &rhs) const {
		return (length_m < rhs.length_m) ||
			(length_m == rhs.length_m && memcmp(chunk_m, rhs.chunk_m, length_m) < 0);
	}
	//-------------------------------------------------------------

	MemoryChunk & MemoryChunk::operator=(MemoryChunk const &rhs) {
		if (length_m != rhs.length_m)
			// Resize
				chunk_m = (Byte *) realloc(chunk_m, length_m = rhs.length_m);

		memcpy(chunk_m, rhs.chunk_m, length_m);

		return *this;
	}

	MemoryChunk& MemoryChunk::operator<<(MemoryChunk const &rhs) {
		// do nothing
		if (rhs.length() <= 0)
			return *this;

		// append data
		size_t old_size = length();
		size_t new_size = old_size + rhs.length();


		// resize
		length(new_size);
		Byte* newPtr = data() + old_size;

		// copy
		memcpy(newPtr, rhs.data(), rhs.length());
		return *this;

	}

	MemoryChunk& MemoryChunk::operator<<(string const& str) {
		if (str.size() <= 0)
			return *this;

		size_t old_size = length();
		size_t new_size = old_size + str.size();


		length(new_size);
		Byte* newPtr = data() + old_size;

		// copy
		memcpy(newPtr, str.c_str(), str.size());

		return *this;
	}

	MemoryChunk& MemoryChunk::operator<<(char const* str) {
		if (str == NULL)
			return *this;

		size_t strsize = strlen(str);
		if (strsize <= 0)
			return *this;

		size_t old_size = length();

		// minus 1 cause null terminated
		size_t new_size = old_size + strsize;

		length(new_size);
		Byte* newPtr = data() + old_size;

		memcpy(newPtr, str, strsize);

		return *this;
	}

	void MemoryChunk::set(Byte const* datap, int const lengthd)
	{
		if (lengthd > 0 && datap) {
			length(lengthd);
			memcpy(this->data(), datap, lengthd);
		}
	}

	string MemoryChunk::toString(int maxbytes) const {
		string result;

		if (length() == 0)
			return result;

		int process_length = ( maxbytes <= 0 ? length() : min(maxbytes, length()) );

		char buf[2];
		for (int i = 0; i < process_length; i++) {
			Byte chP = *(data() + i);

			if (chP != '\0') {

				// can print
				if (isprint(chP)) {
					result.push_back(chP);
				} else {

					buf[0] = (chP >> 4 & 0x0f);
					buf[0] += (buf[0] < 0x0a ? '0' : 'A' - 10);

					buf[1] = (chP & 0x0f);
					buf[1] += (buf[1] < 0x0a ? '0' : 'A' - 10);

					result.append("\\0x");
					result.push_back(buf[0]);
					result.push_back(buf[1]);
					result.append(";");
				}
			} else
				result.append("\\0x00;");

		}

		return result;
	}

	string MemoryChunk::toHexString(int maxbytes) const {
		string result;

		if (length() == 0)
			return result;

		int process_length = ( maxbytes <= 0 ? length() : min(maxbytes, length()) );

		char buf[2];
		for (int i = 0; i < process_length; i++) {
			Byte chP = *(data() + i);


			buf[0] = (chP >> 4 & 0x0f);
			buf[0] += (buf[0] < 0x0a ? '0' : 'A' - 10);

			buf[1] = (chP & 0x0f);
			buf[1] += (buf[1] < 0x0a ? '0' : 'A' - 10);

			buf[2] = '\0';

			result.append("\\0x");
			result.push_back(buf[0]);
			result.push_back(buf[1]);
			result.append(";");

		}

		return result;
	}

	void MemoryChunk::fromString(string const& str) {
		// do not include terminating NULL
		// reset
		length(0);

		if (str.size() <= 0)
			return;
		size_t new_size = str.size();
		if (new_size > 0) {
			length(new_size);
			Byte* newPtr = data();
			// copy
			memcpy(newPtr, str.c_str(), str.size());
		}

		//

	}

	void MemoryChunk::fromString(char const* src)
	{
		length(0);
		if (src) {
			size_t new_size = strlen(src);
			if (new_size > 0) {
				length(new_size);
				Byte* newPtr = data();
				// copy
				memcpy(newPtr, src, new_size);
			}
		}
	} 

	// End of namespace
}

