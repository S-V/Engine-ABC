/*
=============================================================================
	File:	Memory.cpp
	Desc:
	Note:	Undone.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include "Memory.h"

namespace abc {

MX_FIXME( "memory allocation functions may return NULL sometimes" )

#if 0

//
//	Global variables.
//
namespace _hidden {

	static mxMemoryHeap< EAllocationSize::Alloc_Medium, 16 >	heaps[ NUM_MEMORY_CLASSES ];

}//end of namespace _hidden

void *	Allocate( size_t numBytes, EMemoryClass memClass )
{
	void * pMem = _hidden::heaps[ memClass ].Allocate( numBytes );
	Assert( pMem );
	return pMem;
}

void	Free( void* pMemory, EMemoryClass memClass )
{
	return _hidden::heaps[ memClass ].Free( pMemory );
}

void *	Allocate( size_t numBytes, const char* file, int line, EMemoryClass memClass )
{
	void * pMem = _hidden::heaps[ memClass ].Allocate( numBytes, file, line );
	Assert( pMem );
	return pMem;
}

void	Free( void* pMemory, const char* file, int line, EMemoryClass memClass )
{
	return _hidden::heaps[ memClass ].Free( pMemory, file, line );
}

#elif 1


void *	Allocate( size_t numBytes, EMemoryClass memClass )
{
	(void) memClass;
	void * pMem = malloc( numBytes );
	Assert( pMem );
	return pMem;
}

void	Free( void* pMemory, EMemoryClass memClass )
{
	(void) memClass;
	return free( pMemory );
}

void *	Allocate( size_t numBytes, const char* file, int line, EMemoryClass memClass )
{
	(void) memClass;
	void * pMem = _malloc_dbg( numBytes, _NORMAL_BLOCK, file, line );
	Assert( pMem );
	return pMem;
}

void	Free( void* pMemory, const char* file, int line, EMemoryClass memClass )
{
	(void) memClass, (void) file, (void) line;
	return _free_dbg( pMemory, _NORMAL_BLOCK );
}


#else

void *	Allocate( size_t numBytes, EMemoryClass memClass )
{
	(void) memClass;
	void * pMem = malloc( numBytes );
	Assert( pMem );
	return pMem;
}

void	Free( void* pMemory, EMemoryClass memClass )
{
	(void) memClass;
	return free( pMemory );
}

void *	Allocate( size_t numBytes, const char* file, int line, EMemoryClass memClass )
{
	(void) memClass, (void) file, (void) line;
	void * pMem = malloc( numBytes );
	Assert( pMem );
	return pMem;
}

void	Free( void* pMemory, const char* file, int line, EMemoryClass memClass )
{
	(void) memClass, (void) file, (void) line;
	return free( pMemory );
}

#endif


/*
const mxMemoryStats & mxMemoryManager::GetStats() const
{
	// collect stats from all memory pools
	for ( mxUInt iMemGroup = 0;
		iMemGroup < EMemoryClass::NUM_MEMORY_CLASSES;
		iMemGroup++ )
	{
		m_stats.totalBytes += m_memory[ iMemGroup ].GetStats().totalBytes;
	}
	return m_stats;
}
*/

}//end of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
