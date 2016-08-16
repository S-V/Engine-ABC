/*
=============================================================================
	File:	TArray.h
	Desc:	Dynamic templated array.
=============================================================================
*/

#ifndef __MX_CONTAINTERS_TARRAY_H__
#define __MX_CONTAINTERS_TARRAY_H__

namespace abc {

template< class type >
FORCEINLINE INT SortCompare( const type *a, const type *b ) {
	return *a - *b;
}

//
//	TArray< T > - dynamic array.
//
//	Does not allocate memory until the first item is added.
//	Uses 'new' and 'delete' to reallocate its contents so pointers
//	to TArray items can become invalid when TArrays are growing/shrinking
//	(in that case handles (integer offsets) can be used instead of pointers).
//
//	Swiped from Id Software ( idList ).
//

template< typename type >
class TArray {
public:

	typedef mxUInt	cmp_t( const type *, const type * );
	typedef type	new_t( void );

					TArray( mxUInt newgranularity = 16 );
					TArray( const TArray<type> &other );
					~TArray<type>( void );

	void			Clear( void );										// clear the list
	mxUInt			Num( void ) const;									// returns number of elements in list
	mxUInt			NumAllocated( void ) const;							// returns number of elements allocated for
	void			SetGranularity( mxUInt newgranularity );				// set new granularity
	mxUInt			GetGranularity( void ) const;						// get the current granularity

	size_t			Allocated( void ) const;							// returns total size of allocated memory
	size_t			Size( void ) const;									// returns total size of allocated memory including size of list type
	size_t			MemoryUsed( void ) const;							// returns size of the used elements in the list

	TArray<type> &	operator = ( const TArray<type> &other );
	const type &	operator [] ( mxUInt index ) const;
	type &			operator [] ( mxUInt index );

	void			Condense( void );									// resizes list to exactly the number of elements it contains
	void			Resize( mxUInt newsize );								// resizes list to the given number of elements
	void			Resize( mxUInt newsize, mxUInt newgranularity	 );			// resizes list and sets new granularity
	void			SetNum( mxUInt newnum, bool resize = true );			// set number of elements in list and resize to exactly this number if necessary
	void			AssureSize( mxUInt newSize);							// assure list has given number of elements, but leave them uninitialized
	void			AssureSize( mxUInt newSize, const type &initValue );	// assure list has given number of elements and initialize any new elements
	void			AssureSizeAlloc( mxUInt newSize, new_t *allocator );	// assure the pointer list has the given number of elements and allocate any new elements

	type *			Ptr( void );										// returns a pointer to the list
	const type *	Ptr( void ) const;									// returns a const pointer to the list

	type &			GetLast();
	const type &	GetLast() const;

	type &			Alloc( void );										// returns reference to a new data element at the end of the list
	mxUInt			Append( const type & obj );							// append element
	mxUInt			Append( const TArray<type> &other );				// append list
	mxUInt			AddUnique( const type & obj );						// add unique element
	mxUInt			Insert( const type & obj, mxUInt index = 0 );			// insert the element at the given index
	bool			FindIndex( const type & obj, mxUInt &index ) const;	// find the index for the given element
	type *			Find( type const & obj ) const;						// find pointer to the given element
	mxUInt			FindNull( void ) const;								// find the index for the first NULL pointer in the list
	mxUInt			IndexOf( const type *obj ) const;					// returns the index for the pointer to an element in the list
	bool			RemoveIndex( mxUInt index );							// remove the element at the given index
	bool			Remove( const type & obj );							// remove the element
	void			Sort( cmp_t *compare = ( cmp_t * )&SortCompare<type> );
	void			SortSubSection( mxUInt startIndex, mxUInt endIndex, cmp_t *compare = ( cmp_t * )&SortCompare<type> );
	void			Swap( TArray<type> &other );						// swap the contents of the lists
	void			DeleteContents( bool clear );						// delete the contents of the list

private:
	mxUInt		num;			// number of elements in list
	mxUInt		size;			// number of allocated elements
	mxUInt		granularity;	// specifies by how many items the list will grow
	type *		list;			// pointer to allocated memory
};

/*
================
TArray<type>::TArray( mxUInt )
================
*/
template< class type >
FORCEINLINE TArray<type>::TArray( mxUInt newgranularity /* = 16 */ ) {
	Assert( newgranularity > 0 );

	list		= NULL;
	granularity	= newgranularity;
	Clear();
}

/*
================
TArray<type>::TArray( const TArray<type> &other )
================
*/
template< class type >
FORCEINLINE TArray<type>::TArray( const TArray<type> &other ) {
	list = NULL;
	*this = other;
}

/*
================
TArray<type>::~TArray<type>
================
*/
template< class type >
FORCEINLINE TArray<type>::~TArray( void ) {
	Clear();
}

/*
================
TArray<type>::Clear

Frees up the memory allocated by the list.  Assumes that type automatically handles freeing up memory.
================
*/
template< class type >
FORCEINLINE void TArray<type>::Clear( void ) {
	if ( list ) {
		delete[] list;
	}

	list	= NULL;
	num		= 0;
	size	= 0;
}

/*
================
TArray<type>::DeleteContents

Calls the destructor of all elements in the list.  Conditionally frees up memory used by the list.
Note that this only works on lists containing pointers to objects and will cause a compiler error
if called with non-pointers.  Since the list was not responsible for allocating the object, it has
no information on whether the object still exists or not, so care must be taken to ensure that
the pointers are still valid when this function is called.  Function will set all pointers in the
list to NULL.
================
*/
template< class type >
FORCEINLINE void TArray<type>::DeleteContents( bool clear ) {
	mxUInt i;

	for( i = 0; i < num; i++ ) {
		delete list[ i ];
		list[ i ] = NULL;
	}

	if ( clear ) {
		Clear();
	} else {
		MemSet( list, 0, size * sizeof( type ) );
	}
}

/*
================
TArray<type>::Allocated

return total memory allocated for the list in bytes, but doesn't take into account additional memory allocated by type
================
*/
template< class type >
FORCEINLINE size_t TArray<type>::Allocated( void ) const {
	return size * sizeof( type );
}

/*
================
TArray<type>::Size

return total size of list in bytes, but doesn't take into account additional memory allocated by type
================
*/
template< class type >
FORCEINLINE size_t TArray<type>::Size( void ) const {
	return sizeof( TArray<type> ) + Allocated();
}

/*
================
TArray<type>::MemoryUsed
================
*/
template< class type >
FORCEINLINE size_t TArray<type>::MemoryUsed( void ) const {
	return num * sizeof( *list );
}

/*
================
TArray<type>::Num

Returns the number of elements currently contained in the list.
Note that this is NOT an indication of the memory allocated.
================
*/
template< class type >
FORCEINLINE mxUInt TArray<type>::Num( void ) const {
	return num;
}

/*
================
TArray<type>::NumAllocated

Returns the number of elements currently allocated for.
================
*/
template< class type >
FORCEINLINE mxUInt TArray<type>::NumAllocated( void ) const {
	return size;
}

/*
================
TArray<type>::SetNum

Resize to the exact size specified irregardless of granularity
================
*/
template< class type >
FORCEINLINE void TArray<type>::SetNum( mxUInt newnum, bool resize ) {
	Assert( newnum >= 0 );
	if ( resize || newnum > size ) {
		Resize( newnum );
	}
	num = newnum;
}

/*
================
TArray<type>::SetGranularity

Sets the base size of the array and resizes the array to match.
================
*/
template< class type >
FORCEINLINE void TArray<type>::SetGranularity( mxUInt newgranularity ) {
	mxUInt newsize;

	Assert( newgranularity > 0 );
	granularity = newgranularity;

	if ( list ) {
		// resize it to the closest level of granularity
		newsize = num + granularity - 1;
		newsize -= newsize % granularity;
		if ( newsize != size ) {
			Resize( newsize );
		}
	}
}

/*
================
TArray<type>::GetGranularity

Get the current granularity.
================
*/
template< class type >
FORCEINLINE mxUInt TArray<type>::GetGranularity( void ) const {
	return granularity;
}

/*
================
TArray<type>::Condense

Resizes the array to exactly the number of elements it contains or frees up memory if empty.
================
*/
template< class type >
FORCEINLINE void TArray<type>::Condense( void ) {
	if ( list ) {
		if ( num ) {
			Resize( num );
		} else {
			Clear();
		}
	}
}

/*
================
TArray<type>::Resize

Allocates memory for the amount of elements requested while keeping the contents intact.
Contents are copied using their = operator so that data is correctly instantiated.
================
*/
template< class type >
FORCEINLINE void TArray<type>::Resize( mxUInt newsize ) {
	type	*temp;
	mxUInt		i;

	Assert( newsize >= 0 );

	// free up the list if no data is being reserved
	if ( newsize <= 0 ) {
		Clear();
		return;
	}

	if ( newsize == size ) {
		// not changing the size, so just exit
		return;
	}

	temp	= list;
	size	= newsize;

	//MX_BUG( "added "|| num == 0"" );
	if ( size < num ) {
		num = size;
	}

	// copy the old list into our new one
	list = new type[ size ];
	for( i = 0; i < num; i++ ) {
		list[ i ] = temp[ i ];
	}

	// delete the old list if it exists
	if ( temp ) {
		delete[] temp;
	}
}

/*
================
TArray<type>::Resize

Allocates memory for the amount of elements requested while keeping the contents intact.
Contents are copied using their = operator so that data is correnctly instantiated.
================
*/
template< class type >
FORCEINLINE void TArray<type>::Resize( mxUInt newsize, mxUInt newgranularity ) {
	type	*temp;
	mxUInt		i;

	Assert( newsize >= 0 );

	Assert( newgranularity > 0 );
	granularity = newgranularity;

	// free up the list if no data is being reserved
	if ( newsize <= 0 ) {
		Clear();
		return;
	}

	temp	= list;
	size	= newsize;
	if ( size < num ) {
		num = size;
	}

	// copy the old list into our new one
	list = new type[ size ];
	for( i = 0; i < num; i++ ) {
		list[ i ] = temp[ i ];
	}

	// delete the old list if it exists
	if ( temp ) {
		delete[] temp;
	}
}

/*
================
TArray<type>::AssureSize

Makes sure the list has at least the given number of elements.
================
*/
template< class type >
FORCEINLINE void TArray<type>::AssureSize( mxUInt newSize ) {
	mxUInt newNum = newSize;

	if ( newSize > size ) {

		if ( granularity == 0 ) {	// this is a hack to fix our MemSet classes
			granularity = 16;
		}

		newSize += granularity - 1;
		newSize -= newSize % granularity;
		Resize( newSize );
	}

	num = newNum;
}

/*
================
TArray<type>::AssureSize

Makes sure the list has at least the given number of elements and initialize any elements not yet initialized.
================
*/
template< class type >
FORCEINLINE void TArray<type>::AssureSize( mxUInt newSize, const type &initValue ) {
	mxUInt newNum = newSize;

	if ( newSize > size ) {

		if ( granularity == 0 ) {	// this is a hack to fix our MemSet classes
			granularity = 16;
		}

		newSize += granularity - 1;
		newSize -= newSize % granularity;
		num = size;
		Resize( newSize );

		for ( mxUInt i = num; i < newSize; i++ ) {
			list[i] = initValue;
		}
	}

	num = newNum;
}

/*
================
TArray<type>::AssureSizeAlloc

Makes sure the list has at least the given number of elements and allocates any elements using the allocator.

NOTE: This function can only be called on lists containing pointers. Calling it
on non-pointer lists will cause a compiler error.
================
*/
template< class type >
FORCEINLINE void TArray<type>::AssureSizeAlloc( mxUInt newSize, new_t *allocator ) {
	mxUInt newNum = newSize;

	if ( newSize > size ) {

		if ( granularity == 0 ) {	// this is a hack to fix our MemSet classes
			granularity = 16;
		}

		newSize += granularity - 1;
		newSize -= newSize % granularity;
		num = size;
		Resize( newSize );

		for ( mxUInt i = num; i < newSize; i++ ) {
			list[i] = (*allocator)();
		}
	}

	num = newNum;
}

/*
================
TArray<type>::operator=

Copies the contents and size attributes of another list.
================
*/
template< class type >
FORCEINLINE TArray<type> &TArray<type>::operator=( const TArray<type> &other ) {
	mxUInt	i;

	Clear();

	num			= other.num;
	size		= other.size;
	granularity	= other.granularity;

	if ( size ) {
		list = new type[ size ];
		for( i = 0; i < num; i++ ) {
			list[ i ] = other.list[ i ];
		}
	}

	return *this;
}

/*
================
TArray<type>::operator[] const

Access operator.  Index must be within range or an Assert will be issued in debug builds.
Release builds do no range checking.
================
*/
template< class type >
FORCEINLINE const type &TArray<type>::operator[]( mxUInt index ) const {
	Assert( index >= 0 );
	Assert( index < num );

	return list[ index ];
}

/*
================
TArray<type>::operator[]

Access operator.  Index must be within range or an Assert will be issued in debug builds.
Release builds do no range checking.
================
*/
template< class type >
FORCEINLINE type &TArray<type>::operator[]( mxUInt index ) {
	Assert( index >= 0 );
	Assert( index < num );

	return list[ index ];
}

/*
================
TArray<type>::Ptr

Returns a pointer to the begining of the array.  Useful for iterating through the list in loops.

Note: may return NULL if the list is empty.

FIXME: Create an iterator template for this kind of thing.
================
*/
template< class type >
FORCEINLINE type *TArray<type>::Ptr( void ) {
	return list;
}

/*
================
TArray<type>::Ptr

Returns a pointer to the begining of the array.  Useful for iterating through the list in loops.

Note: may return NULL if the list is empty.

FIXME: Create an iterator template for this kind of thing.
================
*/
template< class type >
const FORCEINLINE type * TArray<type>::Ptr( void ) const {
	return list;
}


template< class type >
FORCEINLINE type & TArray<type>::GetLast() {
	return list[ num-1 ];
}

template< class type >
FORCEINLINE const type & TArray<type>::GetLast() const {
	return list[ num-1 ];
}


/*
================
TArray<type>::Alloc

Returns a reference to a new data element at the end of the list.
================
*/
template< class type >
FORCEINLINE type &TArray<type>::Alloc( void ) {
	if ( !list ) {
		Resize( granularity );
	}

	if ( num == size ) {
		Resize( size + granularity );
	}

	return list[ num++ ];
}

/*
================
TArray<type>::Append

Increases the size of the list by one element and copies the supplied data into it.

Returns the index of the new element.
================
*/
template< class type >
FORCEINLINE mxUInt TArray<type>::Append( type const & obj ) {
	if ( !list ) {
		Resize( granularity );
	}

	if ( num == size ) {
		mxUInt newsize;

		if ( granularity == 0 ) {	// this is a hack to fix our MemSet classes
			granularity = 16;
		}
		newsize = size + granularity;
		Resize( newsize - newsize % granularity );
	}

	list[ num ] = obj;
	num++;

	return num - 1;
}


/*
================
TArray<type>::Insert

Increases the size of the list by at leat one element if necessary 
and inserts the supplied data into it.

Returns the index of the new element.
================
*/
template< class type >
FORCEINLINE mxUInt TArray<type>::Insert( type const & obj, mxUInt index ) {
	if ( !list ) {
		Resize( granularity );
	}

	if ( num == size ) {
		mxUInt newsize;

		if ( granularity == 0 ) {	// this is a hack to fix our MemSet classes
			granularity = 16;
		}
		newsize = size + granularity;
		Resize( newsize - newsize % granularity );
	}

	if ( index < 0 ) {
		index = 0;
	}
	else if ( index > num ) {
		index = num;
	}
	for ( mxUInt i = num; i > index; --i ) {
		list[i] = list[i-1];
	}
	num++;
	list[index] = obj;
	return index;
}

/*
================
TArray<type>::Append

adds the other list to this one

Returns the size of the new combined list
================
*/
template< class type >
FORCEINLINE mxUInt TArray<type>::Append( const TArray<type> &other ) {
	if ( !list ) {
		if ( granularity == 0 ) {	// this is a hack to fix our MemSet classes
			granularity = 16;
		}
		Resize( granularity );
	}

	mxUInt n = other.Num();
	for (mxUInt i = 0; i < n; i++) {
		Append(other[i]);
	}

	return Num();
}

/*
================
TArray<type>::AddUnique

Adds the data to the list if it doesn't already exist.  Returns the index of the data in the list.
================
*/
template< class type >
FORCEINLINE mxUInt TArray<type>::AddUnique( type const & obj ) {
	mxUInt index;

	index = FindIndex( obj );
	if ( index < 0 ) {
		index = Append( obj );
	}

	return index;
}

/*
================
TArray<type>::FindIndex

Searches for the specified data in the list and returns it's index.  Returns INDEX_NONE if the data is not found.
================
*/
template< class type >
FORCEINLINE bool TArray<type>::FindIndex( type const & obj, mxUInt &index ) const {
	mxUInt i;

	for( i = 0; i < num; i++ ) {
		if ( list[ i ] == obj ) {
			index = i;
			return true;
		}
	}

	// Not found
	return false;
}

/*
================
TArray<type>::Find

Searches for the specified data in the list and returns it's address. Returns NULL if the data is not found.
================
*/
template< class type >
FORCEINLINE type *TArray<type>::Find( type const & obj ) const {
	mxUInt i;

	if ( FindIndex( obj, i ) ) {
		return &list[ i ];
	}

	return NULL;
}

/*
================
TArray<type>::FindNull

Searches for a NULL pointer in the list.  Returns -1 if NULL is not found.

NOTE: This function can only be called on lists containing pointers. Calling it
on non-pointer lists will cause a compiler error.
================
*/
template< class type >
FORCEINLINE mxUInt TArray<type>::FindNull( void ) const {
	mxUInt i;

	for( i = 0; i < num; i++ ) {
		if ( list[ i ] == NULL ) {
			return i;
		}
	}

	// Not found
	return -1;
}

/*
================
TArray<type>::IndexOf

Takes a pointer to an element in the list and returns the index of the element.
This is NOT a guarantee that the object is really in the list. 
Function will Assert in debug builds if pointer is outside the bounds of the list,
but remains silent in release builds.
================
*/
template< class type >
FORCEINLINE mxUInt TArray<type>::IndexOf( type const *objptr ) const {
	mxUInt index;

	index = objptr - list;

	Assert( index >= 0 );
	Assert( index < num );

	return index;
}


/*
================
TArray<type>::RemoveIndex

Removes the element at the specified index and moves all data following the element down to fill in the gap.
The number of elements in the list is reduced by one.  Returns false if the index is outside the bounds of the list.
Note that the element is not destroyed, so any memory used by it may not be freed until the destruction of the list.
================
*/
template< class type >
FORCEINLINE bool TArray<type>::RemoveIndex( mxUInt index ) {
	mxUInt i;

	Assert( list != NULL );
	Assert( index >= 0 );
	Assert( index < num );

	if ( ( index < 0 ) || ( index >= num ) ) {
		return false;
	}

	num--;
	for( i = index; i < num; i++ ) {
		list[ i ] = list[ i + 1 ];
	}

	return true;
}

/*
================
TArray<type>::Remove

Removes the element if it is found within the list and moves all data following the element down to fill in the gap.
The number of elements in the list is reduced by one.  Returns false if the data is not found in the list.  Note that
the element is not destroyed, so any memory used by it may not be freed until the destruction of the list.
================
*/
template< class type >
FORCEINLINE bool TArray<type>::Remove( type const & obj ) {
	mxUInt index;

	if ( FindIndex( obj, index ) ) {
		return RemoveIndex( index );
	}
	
	return false;
}

/*
================
TArray<type>::Sort

Performs a qsort on the list using the supplied comparison function.  Note that the data is merely moved around the
list, so any pointers to data within the list may no longer be valid.
================
*/
template< class type >
FORCEINLINE void TArray<type>::Sort( cmp_t *compare ) {
	if ( !list ) {
		return;
	}
	typedef mxUInt cmp_c(const void *, const void *);

	cmp_c *vCompare = (cmp_c *)compare;
	::qsort( ( void * )list, ( size_t )num, sizeof( type ), vCompare );
}

/*
================
TArray<type>::SortSubSection

Sorts a subsection of the list.
================
*/
template< class type >
FORCEINLINE void TArray<type>::SortSubSection( mxUInt startIndex, mxUInt endIndex, cmp_t *compare ) {
	if ( !list ) {
		return;
	}
	if ( startIndex < 0 ) {
		startIndex = 0;
	}
	if ( endIndex >= num ) {
		endIndex = num - 1;
	}
	if ( startIndex >= endIndex ) {
		return;
	}
	typedef mxUInt cmp_c(const void *, const void *);

	cmp_c *vCompare = (cmp_c *)compare;
	::qsort( ( void * )( &list[startIndex] ), ( size_t )( endIndex - startIndex + 1 ), sizeof( type ), vCompare );
}

/*
================
TArray<type>::Swap

Swaps the contents of two lists
================
*/
template< class type >
FORCEINLINE void TArray<type>::Swap( TArray<type> &other ) {
	Swap( num, other.num );
	Swap( size, other.size );
	Swap( granularity, other.granularity );
	Swap( list, other.list );
}

}//end of namespace abc

//---------------------------------------------------------
//			Global scope.
//---------------------------------------------------------

//
// Overloaded operators for placement new.
// NOTE: must be global.

template< typename T >
FORCEINLINE void * operator new( size_t size, ::abc::TArray< T > & rArray )
{
	return & rArray.Alloc();
}

// For preventing compiler warnings.
template< typename T >
FORCEINLINE void operator delete( void* p, ::abc::TArray< T > & rArray )
{
	// nothing
}

#endif // ! __MX_CONTAINTERS_TARRAY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
