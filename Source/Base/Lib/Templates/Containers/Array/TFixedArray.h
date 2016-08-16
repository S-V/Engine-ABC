/*
=============================================================================
	File:	TFixedArray.h
	Desc:
=============================================================================
*/

#ifndef __MX_CONTAINTERS_STATIC_ARRAY_H__
#define __MX_CONTAINTERS_STATIC_ARRAY_H__

namespace abc {

//
//	TFixedArray - fixed-size array.
//
//	It's recommended to use this class instead of bare C-style arrays.
//
template< typename T, SizeT MaxNumItems >
class TFixedArray {
public:
	mxUInt  GetCapacity() const { return MaxNumItems; }

	// empty constructor
//	TFixedArray( ENoInit )
//	{}
	TFixedArray( void )
	{}

	TFixedArray( EInitZero )
	{
		Zero();
	}

	T *			Ptr()		{ return this->items; }
	const T *	Ptr() const	{ return this->items; }

	T & operator [] ( mxUInt index )
	{
		BREAK_IF( !(index >= 0 && index < MaxNumItems) );
		return this->items[ index ];
	}

	const T & operator [] ( mxUInt index ) const
	{
		BREAK_IF( !(index >= 0 && index < MaxNumItems) );
		return this->items[ index ];
	}

	// Zero out the elements of the array.
	void Zero()
	{
/*		if( MaxNumItems <= 8 && sizeof(T) == sizeof(T*) )
		{
			for ( IndexT i = 0; i < MaxNumItems; ++i )
			{
				this->items[ i ] = 0;
			}
		}
		else
*/
		{
			MemSet( this->items, 0, sizeof( T ) * MaxNumItems );
		}
	}

	// Set all items of this array to the given value.
	void SetValue( const T& theValue )
	{
		for ( IndexT i = 0; i < MaxNumItems; ++i )
		{
			this->items[ i ] = theValue;
		}
	}

	// Assumes that elements are pointers. Calles 'delete' on all array elements.
	void DeleteItems()
	{
		for ( IndexT i = 0; i < MaxNumItems; ++i )
		{
			MX_FREE( this->items[ i ] );
			this->items[ i ] = 0;
		}
	}

protected:
	T		items[ MaxNumItems ];
};

}//End of namespace abc

#endif // ! __MX_CONTAINTERS_STATIC_ARRAY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
