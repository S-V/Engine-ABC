/*
=============================================================================
	File:	TStaticList.
	Desc:	Swiped from Id Software ( idStaticList ).
			Copyright (C) 2004 Id Software, Inc.
=============================================================================
*/

#ifndef __T_STATIC_LIST_H__
#define __T_STATIC_LIST_H__

namespace abc {

/*
===============================================================================

	Static list template.
	A non-growing, memset-able list using no memory allocation.

===============================================================================
*/

template< typename type, int size >
class TStaticList {
public:

						TStaticList();
						TStaticList( const TStaticList<type,size> &other );
						~TStaticList<type,size>( void );

	void				Clear( void );										// marks the list as empty.  does not deallocate or intialize data.
	int					Num( void ) const;									// returns number of elements in list
	int					Max( void ) const;									// returns the maximum number of elements in the list
	void				SetNum( int newnum );								// set number of elements in list

	size_t				Allocated( void ) const;							// returns total size of allocated memory
	size_t				Size( void ) const;									// returns total size of allocated memory including size of list type
	size_t				MemoryUsed( void ) const;							// returns size of the used elements in the list

	const type &		operator[]( int index ) const;
	type &				operator[]( int index );

	type *				Ptr( void );										// returns a pointer to the list
	const type *		Ptr( void ) const;									// returns a pointer to the list
	type *				Alloc( void );										// returns reference to a new data element at the end of the list.  returns NULL when full.
	int					Append( const type & obj );							// append element
	int					Append( const TStaticList<type,size> &other );		// append list
	int					AddUnique( const type & obj );						// add unique element
	int					Insert( const type & obj, int index );				// insert the element at the given index
	int					FindIndex( const type & obj ) const;				// find the index for the given element
	type *				Find( type const & obj ) const;						// find pointer to the given element
	int					FindNull( void ) const;								// find the index for the first NULL pointer in the list
	int					IndexOf( const type *obj ) const;					// returns the index for the pointer to an element in the list
	bool				RemoveIndex( int index );							// remove the element at the given index
	bool				Remove( const type & obj );							// remove the element
	void				Swap( TStaticList<type,size> &other );				// swap the contents of the lists
	void				DeleteContents( bool clear );						// delete the contents of the list

private:
	int					num;
	type 				list[ size ];
};

/*
================
TStaticList<type,size>::TStaticList()
================
*/
template< typename type, int size >
FORCEINLINE TStaticList<type,size>::TStaticList() {
	num = 0;
}

/*
================
TStaticList<type,size>::TStaticList( const TStaticList<type,size> &other )
================
*/
template< typename type, int size >
FORCEINLINE TStaticList<type,size>::TStaticList( const TStaticList<type,size> &other ) {
	*this = other;
}

/*
================
TStaticList<type,size>::~TStaticList<type,size>
================
*/
template< typename type, int size >
FORCEINLINE TStaticList<type,size>::~TStaticList( void ) {
}

/*
================
TStaticList<type,size>::Clear

Sets the number of elements in the list to 0.  Assumes that type automatically handles freeing up memory.
================
*/
template< typename type, int size >
FORCEINLINE void TStaticList<type,size>::Clear( void ) {
	num	= 0;
}

/*
================
TStaticList<type,size>::DeleteContents

Calls the destructor of all elements in the list.  Conditionally frees up memory used by the list.
Note that this only works on lists containing pointers to objects and will cause a compiler error
if called with non-pointers.  Since the list was not responsible for allocating the object, it has
no information on whether the object still exists or not, so care must be taken to ensure that
the pointers are still valid when this function is called.  Function will set all pointers in the
list to NULL.
================
*/
template< typename type, int size >
FORCEINLINE void TStaticList<type,size>::DeleteContents( bool clear ) {
	int i;

	for( i = 0; i < size; i++ ) {
		delete list[ i ];
		list[ i ] = NULL;
	}

	if ( clear ) {
		Clear();
	} else {
		memset( list, 0, sizeof( list ) );
	}
}

/*
================
TStaticList<type,size>::Num

Returns the number of elements currently contained in the list.
================
*/
template< typename type, int size >
FORCEINLINE int TStaticList<type,size>::Num( void ) const {
	return num;
}

/*
================
TStaticList<type,size>::Num

Returns the maximum number of elements in the list.
================
*/
template< typename type, int size >
FORCEINLINE int TStaticList<type,size>::Max( void ) const {
	return size;
}

/*
================
TStaticList<type>::Allocated
================
*/
template< typename type, int size >
FORCEINLINE size_t TStaticList<type,size>::Allocated( void ) const {
	return size * sizeof( type );
}

/*
================
TStaticList<type>::Size
================
*/
template< typename type, int size >
FORCEINLINE size_t TStaticList<type,size>::Size( void ) const {
	return sizeof( TStaticList<type,size> ) + Allocated();
}

/*
================
TStaticList<type,size>::Num
================
*/
template< typename type, int size >
FORCEINLINE size_t TStaticList<type,size>::MemoryUsed( void ) const {
	return num * sizeof( list[ 0 ] );
}

/*
================
TStaticList<type,size>::SetNum

Set number of elements in list.
================
*/
template< typename type, int size >
FORCEINLINE void TStaticList<type,size>::SetNum( int newnum ) {
	Assert( newnum >= 0 );
	Assert( newnum <= size );
	num = newnum;
}

/*
================
TStaticList<type,size>::operator[] const

Access operator.  Index must be within range or an Assert will be issued in debug builds.
Release builds do no range checking.
================
*/
template< typename type, int size >
FORCEINLINE const type &TStaticList<type,size>::operator[]( int index ) const {
	Assert( index >= 0 );
	Assert( index < num );

	return list[ index ];
}

/*
================
TStaticList<type,size>::operator[]

Access operator.  Index must be within range or an Assert will be issued in debug builds.
Release builds do no range checking.
================
*/
template< typename type, int size >
FORCEINLINE type &TStaticList<type,size>::operator[]( int index ) {
	Assert( index >= 0 );
	Assert( index < num );

	return list[ index ];
}

/*
================
TStaticList<type,size>::Ptr

Returns a pointer to the begining of the array.  Useful for iterating through the list in loops.

Note: may return NULL if the list is empty.

FIXME: Create an iterator template for this kind of thing.
================
*/
template< typename type, int size >
FORCEINLINE type *TStaticList<type,size>::Ptr( void ) {
	return &list[ 0 ];
}

/*
================
TStaticList<type,size>::Ptr

Returns a pointer to the begining of the array.  Useful for iterating through the list in loops.

Note: may return NULL if the list is empty.

FIXME: Create an iterator template for this kind of thing.
================
*/
template< typename type, int size >
FORCEINLINE const type *TStaticList<type,size>::Ptr( void ) const {
	return &list[ 0 ];
}

/*
================
TStaticList<type,size>::Alloc

Returns a pointer to a new data element at the end of the list.
================
*/
template< typename type, int size >
FORCEINLINE type *TStaticList<type,size>::Alloc( void ) {
	if ( num >= size ) {
		return NULL;
	}

	return &list[ num++ ];
}

/*
================
TStaticList<type,size>::Append

Increases the size of the list by one element and copies the supplied data into it.

Returns the index of the new element, or -1 when list is full.
================
*/
template< typename type, int size >
FORCEINLINE int TStaticList<type,size>::Append( type const & obj ) {
	Assert( num < size );
	if ( num < size ) {
		list[ num ] = obj;
		num++;
		return num - 1;
	}

	return -1;
}


/*
================
TStaticList<type,size>::Insert

Increases the size of the list by at leat one element if necessary 
and inserts the supplied data into it.

Returns the index of the new element, or -1 when list is full.
================
*/
template< typename type, int size >
FORCEINLINE int TStaticList<type,size>::Insert( type const & obj, int index ) {
	int i;

	Assert( num < size );
	if ( num >= size ) {
		return -1;
	}

	Assert( index >= 0 );
	if ( index < 0 ) {
		index = 0;
	} else if ( index > num ) {
		index = num;
	}

	for( i = num; i > index; --i ) {
		list[i] = list[i-1];
	}

	num++;
	list[index] = obj;
	return index;
}

/*
================
TStaticList<type,size>::Append

adds the other list to this one

Returns the size of the new combined list
================
*/
template< typename type, int size >
FORCEINLINE int TStaticList<type,size>::Append( const TStaticList<type,size> &other ) {
	int i;
	int n = other.Num();

	if ( num + n > size ) {
		n = size - num;
	}
	for( i = 0; i < n; i++ ) {
		list[i + num] = other.list[i];
	}
	num += n;
	return Num();
}

/*
================
TStaticList<type,size>::AddUnique

Adds the data to the list if it doesn't already exist.  Returns the index of the data in the list.
================
*/
template< typename type, int size >
FORCEINLINE int TStaticList<type,size>::AddUnique( type const & obj ) {
	int index;

	index = FindIndex( obj );
	if ( index < 0 ) {
		index = Append( obj );
	}

	return index;
}

/*
================
TStaticList<type,size>::FindIndex

Searches for the specified data in the list and returns it's index.  Returns -1 if the data is not found.
================
*/
template< typename type, int size >
FORCEINLINE int TStaticList<type,size>::FindIndex( type const & obj ) const {
	int i;

	for( i = 0; i < num; i++ ) {
		if ( list[ i ] == obj ) {
			return i;
		}
	}

	// Not found
	return -1;
}

/*
================
TStaticList<type,size>::Find

Searches for the specified data in the list and returns it's address. Returns NULL if the data is not found.
================
*/
template< typename type, int size >
FORCEINLINE type *TStaticList<type,size>::Find( type const & obj ) const {
	int i;

	i = FindIndex( obj );
	if ( i >= 0 ) {
		return &list[ i ];
	}

	return NULL;
}

/*
================
TStaticList<type,size>::FindNull

Searches for a NULL pointer in the list.  Returns -1 if NULL is not found.

NOTE: This function can only be called on lists containing pointers. Calling it
on non-pointer lists will cause a compiler error.
================
*/
template< typename type, int size >
FORCEINLINE int TStaticList<type,size>::FindNull( void ) const {
	int i;

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
TStaticList<type,size>::IndexOf

Takes a pointer to an element in the list and returns the index of the element.
This is NOT a guarantee that the object is really in the list. 
Function will Assert in debug builds if pointer is outside the bounds of the list,
but remains silent in release builds.
================
*/
template< typename type, int size >
FORCEINLINE int TStaticList<type,size>::IndexOf( type const *objptr ) const {
	int index;

	index = objptr - list;

	Assert( index >= 0 );
	Assert( index < num );

	return index;
}

/*
================
TStaticList<type,size>::RemoveIndex

Removes the element at the specified index and moves all data following the element down to fill in the gap.
The number of elements in the list is reduced by one.  Returns false if the index is outside the bounds of the list.
Note that the element is not destroyed, so any memory used by it may not be freed until the destruction of the list.
================
*/
template< typename type, int size >
FORCEINLINE bool TStaticList<type,size>::RemoveIndex( int index ) {
	int i;

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
TStaticList<type,size>::Remove

Removes the element if it is found within the list and moves all data following the element down to fill in the gap.
The number of elements in the list is reduced by one.  Returns false if the data is not found in the list.  Note that
the element is not destroyed, so any memory used by it may not be freed until the destruction of the list.
================
*/
template< typename type, int size >
FORCEINLINE bool TStaticList<type,size>::Remove( type const & obj ) {
	int index;

	index = FindIndex( obj );
	if ( index >= 0 ) {
		return RemoveIndex( index );
	}
	
	return false;
}

/*
================
TStaticList<type,size>::Swap

Swaps the contents of two lists
================
*/
template< typename type, int size >
FORCEINLINE void TStaticList<type,size>::Swap( TStaticList<type,size> &other ) {
	TStaticList<type,size> temp = *this;
	*this = other;
	other = temp;
}

}//end of namespace abc

#endif /* !__T_STATIC_LIST_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
