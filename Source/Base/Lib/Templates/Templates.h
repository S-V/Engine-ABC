/*
=============================================================================
	File:	Templates.h
	Desc:	Handy container classes.
=============================================================================
*/

#ifndef __MX_TEMPLATES_H__
#define __MX_TEMPLATES_H__

namespace abc {

// Value indicating invalid index.
enum { INDEX_NONE = -1 };

// ENoInit - Used to mark uninitialized variables.
enum ENoInit { _NoInit };

// EInitZero - Used to mark data initialized with zeros.
enum EInitZero { _InitZero };

// EInitIdentity - Used to mark data initialized with default Identity/One value.
enum EInitIdentity { _InitIdentity };

// Handle type.
// (Such names as 'HANDLE', 'Handle' are already taken.)
typedef int	qhandle;

//
// Helper templates.
//

template< typename type >
void Swap( type & a, type & b )
{
	const type  temp = a;
	a = b;
	b = temp;
}

// For preventing compiler warnings "unreferenced formal parameters".
template< typename T >
void UnusedParameter( const T& param )
{ (void)param; }

//=============================================================================

//
//	Triple
// 
template< typename T >
class Triple {
public:
	union {
		struct { T   iA, iB, iC; };
		struct { T   Points[ 3 ]; };
	};

	Triple() {}

	Triple( const T & A, const T & B, const T & C )
		: iA( A ), iB( B ), iC( C )
	{}

	T & operator [] ( mxUInt index )
	{
		Assert( index >= 0 && index <= 3 );
		return Points[ index ];
	}

	const T & operator [] ( mxUInt index ) const
	{
		Assert( index >= 0 && index <= 3 );
		return Points[ index ];
	}
};

}//end of namespace abc

#endif /* ! __MX_TEMPLATES_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
