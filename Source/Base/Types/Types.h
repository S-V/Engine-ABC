/*
=============================================================================
	File:	Types.h
	Desc:	Typedefs for common types for scalability, portability, etc.
	Note:	This file uses other base types defined
			in a platform-specific header.
=============================================================================
*/

#ifndef __MX_TYPES_H__
#define __MX_TYPES_H__

namespace abc {

//
//	Base integer types.
//

typedef mxInt8		INT8;
typedef mxUInt8		UINT8;

typedef mxInt16		INT16;
typedef mxUInt16	UINT16;

typedef mxInt32		INT32;
typedef mxUInt32	UINT32;

typedef mxInt64		INT64;
typedef mxUInt64	UINT64;

typedef mxInt		INT;
typedef mxUInt		UINT;
typedef mxLong		LONG;
typedef mxULong		ULONG;

//
//	Array index type.
//
typedef mxUInt	IndexT;

//
//	Boolean types.
//

//
//	ETroolean
//
//	NOTE: don't modify the values of these constants!
//
enum ETroolean
{
	False	= 0,
	True	= 1,
	DontCare		// Whatever/don't care value
};

//
//	TBool - a boolean type with explicit storage type.
//
template< typename STORAGE = UINT32 >
class TBool {
public:
	TBool()
	{}

	FORCEINLINE	explicit TBool( bool value )
		: m_value( value )
	{}
	
	// Casting to 'bool'.

	FORCEINLINE operator bool () const
	{
		return ( m_value != 0 );
	}

	// Assignment.

	FORCEINLINE TBool & operator = ( bool value )
	{
		m_value = value;
		return *this;
	}
	FORCEINLINE TBool & operator = ( const TBool other )
	{
		m_value = other.m_value;
		return *this;
	}

	// Comparison.

	FORCEINLINE TBool operator == ( bool value ) const
	{
		return (m_value == static_cast< STORAGE >( value ));
	}
	FORCEINLINE TBool operator != ( bool value ) const
	{
		return (m_value != static_cast< STORAGE >( value ));
	}

	FORCEINLINE TBool operator == ( const TBool other ) const
	{
		return m_value == other.m_value;
	}
	FORCEINLINE TBool operator != ( const TBool other ) const
	{
		return m_value != other.m_value;
	}

	// TODO: overload 'forbidden' operators like '+', '-', '/', '<<', etc
	// to catch programming errors early?
	//TBool operator + (const TBool& other) const
	//{ StaticAssert( false && "Invalid operation" ); }

private:
	STORAGE		m_value;
};

//
//	mxBool - default boolean type.
//
//typedef TBool< mxUInt32 >	mxBool;
typedef FASTBOOL	mxBool;

//
//	mxBool32
//
// False is zero, true is any non-zero value.
// Thus comparisons like bool32 == true will not work as expected.
typedef INT32		mxBool32;

//--------------------------------------------------------

//
//	'Bitfield' type.
//
typedef mxULong	BITFIELD;

//--------------------------------------------------------

//
//	Floating-point types.
//
//	Notes: avoid mixing integers and floats in the same expression!
//
typedef mxFloat32		FLOAT;	// Single-precision floating-point type (used by default in many cases).
typedef mxFloat64		DOUBLE;	// Double-precision floating-point type.

//
//	mxTime - is used for delta time values throughout the engine.
//
//	NOTE: this structure should be as lightweight as possible
//	because of the overhead of allocating these structures on the stack.
//
struct mxTime
{
	mxFloat32	fTime;	// in seconds
	mxUInt32	iTime;	// in milliseconds

public:
	mxTime()
	{}

	FORCEINLINE operator mxFloat32 () const
	{
		return fTime;
	}
	FORCEINLINE operator mxUInt32 () const
	{
		return iTime;
	}
};

//
//	TEnum< enumName, storage > - a wrapper to store an enum with explicit size.
//
template< typename ENUM, typename STORAGE = mxUInt32 >
class TEnum {
public:
	TEnum()
	{}

	FORCEINLINE TEnum( ENUM e )
	{
		m_value = static_cast< STORAGE >( e );
	}

	FORCEINLINE operator ENUM () const
	{
		return static_cast< ENUM >( m_value );
	}

	FORCEINLINE void operator = ( ENUM e )
	{
		m_value = static_cast< STORAGE >( e );
	}

	FORCEINLINE bool operator == ( ENUM e ) const
	{
		return (m_value == static_cast< STORAGE >( e ));
	}
	FORCEINLINE bool operator != ( ENUM e ) const
	{
		return (m_value != static_cast< STORAGE >( e ));
	}

	// TODO: overload 'forbidden' operators like '+', '-', '/', '<<', etc
	// to catch programming errors early?

private:
	STORAGE		m_value;
};

//
//	EResult - an enum of success/failure codes.
//
enum EResult
{
	E_SUCCESS = 0,
	E_FAILURE = 1,

	ERESULT_FORCE_DWORD = MAX_SDWORD
};

}//End of namespace abc

#endif // !__MX_TYPES_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
