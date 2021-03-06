/*
=============================================================================
	File:	TFreeList.h
	Desc:
=============================================================================
*/

#ifndef __FREE_LIST_H__
#define __FREE_LIST_H__

namespace abc {

//
// TFreeList< T > - an array of fixed-size memory blocks available for allocation.
// No destructor / constructor is called for the type !
//
template< typename T >
class TFreeList {
public:
	// Construct a TFreeList with the specified number
	// of objects available for allocation.
	TFreeList( mxUInt iNumObjects )
	{
		Assert2( (iNumObjects > 0), "Invalid TFreeList size specified." );

		m_pObjectData = MemNew T[iNumObjects];
		m_ppFreeObjects = MemNew T*[iNumObjects];

		Assert( m_pObjectData && "Not enough memory to allocate object data!" );
		Assert( m_ppFreeObjects && "Not enough memory to allocate pointer stack!" );

		m_iNumObjects = iNumObjects;
		m_bFreeOnDestroy = true;

		FreeAll();
	}
/*
	// Constructs a TFreeList with the specified number
	// of objects available for allocation from pre-allocated
	// memory referenced by "pObjectData" and "ppFreeObjects".
	// Note that pObjectData and ppFreeObjects must have
	// "iNumObjects" elements each.
	TFreeList( T *pObjectData, T **ppFreeObjects, INT iNumObjects )
	{
		Assert( (iNumObjects > 0) && "Invalid TFreeList size specified." );

		m_pObjectData = pObjectData;
		m_ppFreeObjects = ppFreeObjects;

		Assert( m_pObjectData && "A NULL pointer was passed for the object data!" );
		Assert( m_ppFreeObjects && "A NULL pointer was passed for the pointer stack!" );

		m_iNumObjects = iNumObjects;
		m_bFreeOnDestroy = false;

		FreeAll();
	}
*/
	~TFreeList()
	{
		// If we have allocated memory,
		// then we must free it.
		if ( m_bFreeOnDestroy )
		{
			MemDelete [] m_ppFreeObjects;
			MemDelete [] m_pObjectData;
		}
	}

	// Returns a pointer to a free instance of T.
	T * Alloc()
	{
		Assert( m_iTop && "Tried to get a new instance but there"
						  "were no free instances available for "
						  "allocation. Consider using GetFree()!" );
			
		return m_ppFreeObjects[--m_iTop];
	}

	// Reclaims the instance referenced by pInstance.
	void Delete( T *pInstance )
	{
		Assert( (pInstance >= &(m_pObjectData[0])) &&
				(pInstance <= &(m_pObjectData[m_iNumObjects-1])) &&
				"Tried to free an object that was"
				"not from this TFreeList" );

		// You might consider putting a debug-only check here to make sure
		// the instance that is being freed isn't already free.

		Assert( (m_iTop < m_iNumObjects) && "You have freed at least one"
											"instance more than once." );

		m_ppFreeObjects[ m_iTop++ ] = pInstance;
	}

	// Makes all instances available for allocation.
	void FreeAll()
	{
		mxUInt iIndex = m_iNumObjects - 1;

		for ( m_iTop = 0; m_iTop < m_iNumObjects; m_iTop++ )
		{
			m_ppFreeObjects[m_iTop] = &(m_pObjectData[iIndex--]);
		}		
	}

	// Returns the total number of objects
	// managed by this TFreeList.
	mxUInt GetCapacity() const
	{
		return m_iNumObjects;
	}

	mxUInt Num() const
	{
		return m_iNumObjects - m_iTop;
	}

	// Returns the number of instances available
	// for allocation.
	mxUInt GetFree() const
	{
		return m_iTop;
	}

private:

	// Points to the array of objects.
	T *		m_pObjectData;

	// The number of objects in m_pObjectData.
	mxUInt		m_iNumObjects;


	// Points to an array of pointers to free
	// objects within m_pObjectData.  Used as
	// a fixed sized stack.
	T **	m_ppFreeObjects;

	// Keeps track of the first available object in
	// m_ppFreeObjects (the top of the stack).
	mxUInt m_iTop;


	// Remembers weather or not to free memory on
	// destruction.
	bool m_bFreeOnDestroy;
};

//
// TStaticFreeList< T, MaxCount >
//
template< typename T, mxUInt MaxCount = 1024 >
class TStaticFreeList {
public:
	TStaticFreeList()
	{
		FreeAll();
	}

	~TStaticFreeList()
	{}

	T * Alloc()
	{
		Assert( m_top && "Tried to get a new instance but there"
						  "were no free instances available for "
						  "allocation. Consider using GetFree()!" );
			
		return m_freeObjects[ --m_top ];
	}

	// Reclaims the instance referenced by pInstance.
	void Delete( T * pInstance )
	{
		Assert( ( pInstance >= &(m_data[0]) ) &&
				( pInstance <= &(m_data[MaxCount-1]) ) &&
				"Tried to free an object that was"
				"not from this TFreeList" );

		// You might consider putting a debug-only check here to make sure
		// the instance that is being freed isn't already free.

		Assert( (m_top < MaxCount) && "You have freed at least one"
											"instance more than once." );

		m_freeObjects[ m_top++ ] = pInstance;
	}

	// Makes all instances available for allocation.
	void FreeAll()
	{
		INT iIndex = (MaxCount - 1);

		for ( m_top = 0; m_top < MaxCount; m_top++ )
		{
			m_freeObjects[ m_top ] = &( m_data[ iIndex-- ] );
		}
	}

	// Returns the total number of objects
	// managed by this TFreeList.
	INT GetCapacity() const
	{
		return MaxCount;
	}

	mxUInt Num() const
	{
		return MaxCount - m_top;
	}

	// Returns the number of instances available
	// for allocation.
	mxUInt GetFree() const
	{
		return m_top;
	}

	// Deletes all elements.
	void Clear()
	{
		FreeAll();
	}

private:
	T		m_data[ MaxCount ];	// actual object data
	T *		m_freeObjects[ MaxCount ];

	// Keeps track of the first available object in
	// m_freeObjects ( the top of the stack ).
	mxUInt	m_top;
};

}//end of namespace abc

#endif /* !__FREE_LIST_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

