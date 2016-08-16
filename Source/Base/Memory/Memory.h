/*
=============================================================================
	File:	Memory.h
	Desc:	Memory management.
=============================================================================
*/

#ifndef __MX_MEMORY_H__
#define __MX_MEMORY_H__

#include <Memory/Pool/Pool.h>
#include <Memory/FreeList/FreeList.h>
#include <Memory/BlockAlloc/BlockAllocator.h>

namespace abc {

// Forward declarations.
class mxOutput;

//------------------------------------------------------------------
//		Defines.
//------------------------------------------------------------------

// Use a separate memory pool for string data.
#define MX_USE_STRING_POOL	// This is needed for the String class.

//------------------------------------------------------------------
//		Declarations.
//------------------------------------------------------------------

//
//	EMemoryClass - a category under which the memory used by objects is placed.
//
//	Each object in the engine should be allocated in a dedicated heap.
//
enum EMemoryClass
{
	MX_MEMORY_CLASS_GENERIC,		// General purpose.

	MX_MEMORY_CLASS_GEOMETRY,		// Geometry held in main memory ( e.g. raw vertex buffers ).

	MX_MEMORY_CLASS_RENDER,			// Render system.
	MX_MEMORY_CLASS_PHYSICS,		// Physics system.
	MX_MEMORY_CLASS_SOUND,			// Sound system.

	MX_MEMORY_CLASS_SCENE_DATA,		// Scene graph structures, scene entitities, etc.

	MX_MEMORY_CLASS_MESSAGES,		// 'Message heap'.

	MX_MEMORY_CLASS_STRING,			// String data should be allocated from specialized string pools.

	MX_MEMORY_CLASS_SCRIPT,			// Scripting system (material scripts, UI, cinematics, etc).


	NUM_MEMORY_CLASSES
};

//
//	EAllocationSize - allocation policy (allocate memory by small pieces or by large chunks, etc).
//
enum EAllocationSize
{
	Alloc_Tiny,
	Alloc_Small,
	Alloc_Medium,
	Alloc_Huge
};

//
//	mxMemoryStats - memory usage stats.
//
class mxMemoryStats {
public:
	SizeT	totalBytesUsed;

	// Work in progress...
};

//
//	mxMemoryHeapSettings
//
class mxMemoryHeapSettings {
public:

	// true if we have a limited memory budget
	// which cannot be exceeded
	bool	isLimited;

	SizeT	memoryLimit;	// only valid if 'isLimited' is true

public:
	mxMemoryHeapSettings()
		: isLimited( false ), memoryLimit( 0 )
	{}
};

//===========================================================================

//
//	Used for debugging purposes.
//	Returns true if given a valid heap pointer.
//
FORCEINLINE bool G_IsValidPointer( const void* ptr )
{
#if MX_COMPILER_MSVC
	return ( ptr != (const void*) 0xCCCCCCCC )
		&& ( ptr != (const void*) 0xFeeeFeee )
	//	&& ( ptr != (const void*) 0xDeadBeef )
	//	&& ( ptr != (const void*) 0xDeadFace )
		;
#else
	return ( ptr != NULL );
#endif
}

//
//	Align< PtrType >
//
template< class T >
FORCEINLINE
T Align( const T pointer, size_t alignmentInBytes )
{
	return (T) ( ((SizeT)pointer + alignmentInBytes - 1)
				& ~(alignmentInBytes - 1) );
}

//===========================================================================

//
// GET_OUTER()
//
// A platform-independent way for a contained class to get a pointer to its
// owner. If you know a class is exclusively used in the context of some
// "outer" class, this is a much more space efficient way to get at the outer
// class than having the inner class store a pointer to it.
//
//	This comes from Source Engine, Valve.
//
//	class COuter
//	{
//		class CInner // Note: this does not need to be a nested class to work
//		{
//			void PrintAddressOfOuter()
//			{
//				printf( "Outer is at 0x%x\n", GET_OUTER( COuter, m_Inner ) );
//			}
//		};
//		
//		CInner m_Inner;
//		friend class CInner;
//	};

#define GET_OUTER( OuterType, OuterMember ) \
	( ( OuterType * ) ( (byte *)this - offsetof( OuterType, OuterMember ) ) )

//===========================================================================

//
//	mxMemoryHeap - is a memory allocation heap.
//
//	[Unfinished]
//
template
<
	enum EAllocationSize	allocSize = EAllocationSize::Alloc_Medium,
	SizeT					alignment = DEFAULT_MEMORY_ALIGNMENT,

	class mxMemoryHeapSettings = mxMemoryHeapSettings()
>
class mxMemoryHeap {
public:
	void * Allocate( size_t numBytes )
	{
		return ::_aligned_malloc( numBytes, alignment );
	}

	void * Reallocate( void* pOldMemory, size_t numBytes )
	{
		return ::_aligned_realloc( pOldMemory, numBytes, alignment );
	}

	void Free( void* pMemory )
	{
		return ::_aligned_free( pMemory );
	}

	// Testing & Debugging.

	void * Allocate( size_t numBytes, const char* file, int line )
	{
		return ::_aligned_malloc_dbg( numBytes, alignment, file, line );
	}

	void * Reallocate( void* pOldMemory, size_t numBytes, const char* file, int line )
	{
		assert( G_IsValidAlignment(numBytes) );
		return ::_aligned_realloc_dbg( pOldMemory, numBytes, alignment, file, line );
	}

	void Free( void* pMemory, const char* file, int line )
	{
		(void) file, (void) line;
		return ::_aligned_free_dbg( pMemory );
	}

	// Statistics.

	const mxMemoryStats & GetStats() const;

	void	Dump( mxOutput& output );

private:
	void StaticAssertions()
	{
		StaticAssert( G_IsValidAlignment(alignment) );
	}
};

MX_OPTIMIZE("Use an optimized memory pool allocator for extremely small-sized allocations.")

//===========================================================================
//
//	Memory management functions.
//

void *	Allocate( size_t numBytes, EMemoryClass memClass = MX_MEMORY_CLASS_GENERIC );
void	Free( void* pMemory, EMemoryClass memClass = MX_MEMORY_CLASS_GENERIC );

// Testing & Debugging.
void *	Allocate( size_t numBytes, const char* file, int line, EMemoryClass memClass = MX_MEMORY_CLASS_GENERIC );
void	Free( void* pMemory, const char* file, int line, EMemoryClass memClass = MX_MEMORY_CLASS_GENERIC );

//===========================================================================

//
//	IMemoryAllocator - is an abstract memory allocator.
//
class IMemoryAllocator {
public:
	virtual		~IMemoryAllocator() {}

	virtual void *	Allocate( size_t numBytes ) = 0;
	virtual void *	Reallocate( void* oldMemory, size_t numBytes ) = 0;
	virtual void	Free( void* pMemory ) = 0;

	// Returns the size of a memory block allocated by this allocator.
    virtual SizeT	SizeOf( void* ptr ) const = 0;
 
	virtual void	GetStats( mxMemoryStats &OutStats ) = 0;

    // Log statistics.
    virtual void	DumpStats() const = 0;
};

//
//	Mem_GetAllocator
//	Returns the heap allocator corresponding to the specified memory class.
//
IMemoryAllocator *	Mem_GetAllocator( EMemoryClass memClass = MX_MEMORY_CLASS_GENERIC );

/*
MX_TODO(:)

	Make memory management functions templated, resolve memory class type at compile-time for efficiency.

	For example:

	void *	Allocate( size_t numBytes, EMemoryClass memClass = MX_MEMORY_CLASS_GENERIC )
	{
		return TMemHeap< memClass >::Allocate( numBytes );
	}

	where

	template< EMemoryClass memClass >
	class TMemHeap {
	public:
		TMemHeap()
		{
			if ( memClass = STRING_POOL )
			{
				// Settings aimed to improve many small, temporary memory allocations
				// ...
			}
			else
			//...
		}

	// ...
*/

//===========================================================================

// Undef 'new' and 'delete'.
#undef new
#undef delete

//---------------------------------------------------------------------------
//	MX_DECLARE_MEMORY_CLASS( MemoryClassName )
//---------------------------------------------------------------------------
//
#define MX_DECLARE_MEMORY_CLASS( MemoryClassName )								\
	void *	operator new( size_t numBytes )										\
	{																			\
		return Allocate( numBytes, MemoryClassName );									\
	}																			\
	void *	operator new( size_t numBytes, char* filename, int lineNum )		\
	{																			\
		return Allocate( numBytes, filename, lineNum, MemoryClassName );				\
	}																			\
	void	operator delete( void* pointer )									\
	{																			\
		Free( pointer, MemoryClassName );										\
	}																			\
	void	operator delete( void* pointer, char* filename, int lineNum )		\
	{																			\
		Free( pointer, filename, lineNum, MemoryClassName );					\
	}

//===========================================================================

//
//	TAllocator<T>
//
template< typename T >
class TAllocator {
};

}//end of namespace abc

//---------------------------------------------------------------------------
//			Global 'new' and 'delete'.
//---------------------------------------------------------------------------

#ifdef MX_OVERRIDE_NEWDELETE

FORCEINLINE
void *	operator new ( size_t  numBytes )
{
	return ::abc::Allocate( numBytes );
}

FORCEINLINE
void *	operator new [] ( size_t  numBytes )
{
	return ::abc::Allocate( numBytes );
}

FORCEINLINE
void	operator delete ( void* pointer )
{
	return ::abc::Free( pointer );
}

FORCEINLINE
void	operator delete [] ( void* pointer )
{
	return ::abc::Free( pointer );
}

//
// Overriden 'new' and 'delete' for detecting memory leaks.
//

FORCEINLINE
void *	operator new ( size_t  numBytes, const char* fileName, int lineNumber )
{
	return ::abc::Allocate( numBytes, fileName, lineNumber );
}

FORCEINLINE
void *	operator new [] ( size_t  numBytes, const char* fileName, int lineNumber )
{
	return ::abc::Allocate( numBytes, fileName, lineNumber );
}

FORCEINLINE
void	operator delete ( void* pointer, const char* fileName, int lineNumber )
{
	::abc::Free( pointer, fileName, lineNumber );
}

FORCEINLINE
void	operator delete [] ( void* pointer, const char* fileName, int lineNumber )
{
	::abc::Free( pointer, fileName, lineNumber );
}

#endif // MX_OVERRIDE_NEWDELETE

//---------------------------------------------------------------------------
//			Debugging
//---------------------------------------------------------------------------

#if defined(MX_DEBUG_MEMORY) && defined(MX_OVERRIDE_NEWDELETE)

	//
	//	For engine development you should use these macros instead of 'new' and 'delete'
	//	to track each memory allocation/deletion and catch memory leaks.
	//
	#define MX_NEW		new(__FILE__,__LINE__)
	#define MX_FREE		delete

#else

	#define MX_NEW		new
	#define MX_FREE		delete

#endif // ! MX_DEBUG_MEMORY

#endif /* ! __MX_MEMORY_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
