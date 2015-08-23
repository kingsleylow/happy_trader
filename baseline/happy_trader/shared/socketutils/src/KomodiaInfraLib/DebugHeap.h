#ifndef _DebugHeap_H_
#define _DebugHeap_H_

#ifdef _MEMORY_DEBUG 
	#pragma warning(disable : 4291)
	#include <CRTDBG.H>
	#define DEBUG_NEW				new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#define DEBUG_MALLOC(size)	_malloc_dbg(size, _NORMAL_BLOCK, THIS_FILE, __LINE__)   
    #undef THIS_FILE

	#define __new			 new(_NORMAL_BLOCK,  __FILE__, __LINE__)   
	#define __malloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__)   
	#define __free(o)		 _free_dbg(o, _NORMAL_BLOCK)
#endif

#endif