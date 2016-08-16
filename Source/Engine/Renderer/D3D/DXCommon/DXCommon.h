/*
=============================================================================
	File:	DXCommon.h
	Desc:	Common DirectX stuff.
	Note:	This file must be included before
			any other DirectX-related stuff.
=============================================================================
*/

#ifndef __DX_COMMON_H__
#define __DX_COMMON_H__

//-----------------------------------------------------------
//			Defines
//-----------------------------------------------------------

#define SAFE_DELETE( p )         { if(null != p) { delete (p);     (p) = null; } }
#define SAFE_DELETE_ARRAY( p )   { if(null != p) { delete[] (p);   (p) = null; } }
#define SAFE_RELEASE( p )        { if(null != p) { (p)->Release(); (p) = null; } }

#define RELEASE( dxPtr )		(dxPtr)->Release()

//----------------------------------------------------------

//
//	Utilities for debugging.
//

#ifdef MX_DEBUG

	//
	//	DX_CHECK_HRESULT
	//
	#define DX_CHECK_HRESULT( expr, error_message )	\
	{												\
		const HRESULT hr = (expr);					\
		const int line = __LINE__;					\
		if ( FAILED( hr ) ) {						\
			const TCHAR * pErrorStr = DXGetErrorDescription( hr );								\
			sys::FatalError( "'%s', why: '%s', where: '%s' in %s, in '%s', line %d",			\
							error_message, pErrorStr, #expr, __FUNCTION__, __FILE__, line );	\
		}	\
	}

	//
	//	DX_TRACE
	//
	#define DX_TRACE( expr )	\
	{																				\
		const HRESULT hr = (expr);													\
		const int line = __LINE__;													\
		if ( FAILED( hr ) ) {														\
			const TCHAR * pErrorStr = DXGetErrorDescription( hr );					\
			sys::FatalError( "'%s',\n'%s'\n in %s, in '%s', line %d",				\
							#expr, pErrorStr, __FUNCTION__, __FILE__, line );		\
		}																			\
	}

/*	Old  DX_TRACE  code:

	#define DX_TRACE( expr )	\
	{											\
		const HRESULT hr = (expr);				\
		if ( FAILED( hr ) ) {					\
			DXTRACE_ERR_MSGBOX( #expr, hr );	\
		}										\
	}
*/

#else

	#define DX_CHECK_HRESULT( expr, error_message )		((void) expr)
	#define DX_TRACE( expr )							((void) expr)

#endif // MX_DEBUG

//=============================================================================

namespace abc {

//
//	d3d
//
namespace d3d
{
	//
	//	Functions.
	//

	D3DXCOLOR	ConvertColor( const SColor& color );
	D3DXCOLOR	ConvertColor( const FColor& color );
	
	//
	// Variables.
	//
	extern const D3DXCOLOR	WHITE;
	extern const D3DXCOLOR	BLACK;
	extern const D3DXCOLOR	RED;
	extern const D3DXCOLOR	GREEN;
	extern const D3DXCOLOR	BLUE;
	extern const D3DXCOLOR	YELLOW;
	extern const D3DXCOLOR	CYAN;
	extern const D3DXCOLOR	MAGENTA;

	extern const D3DXCOLOR	BEACH_SAND;
	extern const D3DXCOLOR	LIGHT_YELLOW_GREEN;
	extern const D3DXCOLOR	DARK_YELLOW_GREEN;
	extern const D3DXCOLOR	DARKBROWN;
};

//=============================================================================

FORCEINLINE
void VectorAssign( D3DXVECTOR3& dest, const Vec3D& src ) {
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
}

FORCEINLINE
void VectorAssign( Vec3D& dest, const D3DXVECTOR3& src ) {
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
}

//=============================================================================

//
//	DXPtr< T > - for safe automatic handling of DirectX resources.
//

#ifdef MX_DEBUG
namespace d3d
{
	extern mxLong	TotalRefCount;	// For debugging.
}
#endif //MX_DEBUG

template< class T >
class DXPtr
{
public:
	DXPtr()
		: Ptr( NULL )
	{}

	DXPtr( T * pointer )
		: Ptr( pointer )
	{
		if ( Ptr ) {
			AcquirePointer();
		}
	}

	DXPtr( const DXPtr<T> & other )
	{
		Ptr = other.Ptr;
		if ( Ptr ) {
			AcquirePointer();
		}
	}

	~DXPtr()
	{
		if ( Ptr ) {
			ReleasePointer();
		}
	}

	void Release()
	{
		if ( Ptr ) {
			ReleasePointer();
			Ptr = null;
		}
	}

	void operator = ( T * pointer )
	{
		if ( Ptr ) {
			ReleasePointer();
		}
		
		Ptr = pointer;
		
		if ( Ptr ) {
			AcquirePointer();
		}
	}

	void operator = ( const DXPtr<T> & other )
	{
		if ( Ptr ) {
			ReleasePointer();
		}

		Ptr = other.Ptr;

		if ( Ptr ) {
			AcquirePointer();
		}
	}

	operator T* () const
	{
		return Ptr;
	}

	T * operator -> () const
	{
		AssertPtr( Ptr );
		return Ptr;
	}

	bool IsNull() const
	{
		return ( null == Ptr );
	}
	bool IsValid() const
	{
		return ( null != Ptr );
	}

private:
	// assumes that the pointer is not null
	void AcquirePointer()
	{
		AssertPtr( Ptr );
		Ptr->AddRef();

		DEBUG_CODE( d3d::TotalRefCount++ );
	}

	// assumes that the pointer is not null
	void ReleasePointer()
	{
		AssertPtr( Ptr );
		Ptr->Release();

		DEBUG_CODE(
			MX_HACK("ZZZ D3D10 debug layer does not report about memory leaks, although ref. count becomes negative")
			//Assert( d3d::TotalRefCount > 0 );
			d3d::TotalRefCount--
		);
	}

public:
	T *		Ptr;
};

}//End of namespace abc

#endif // !__DX_COMMON_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
