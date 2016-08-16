/*
=============================================================================
	File:	D3D10Utilities.h
	Desc:	Helper classes.
=============================================================================
*/

#ifndef __MX_D3D10_UTILITIES_H__
#define __MX_D3D10_UTILITIES_H__

namespace abc {

/*
======================================================================
	
		Miscellaneous helpers.

======================================================================
*/

//
//	TFixedList
//
template< typename T, UINT MaxNumItems >
class TFixedList
{
public:
			TFixedList( ENoInit );
			TFixedList( EInitZero );

	void	Add( const T& newItem );

	UINT	Num() const;
	
	T &			Get( UINT index );
	const T &	Get( UINT index ) const;

	T &			operator [] ( UINT index );
	const T &	operator [] ( UINT index ) const;

	T *			Ptr( void );
	const T *	Ptr( void ) const;

	void	Reset();

private:
	UINT	num;
	T		data[ MaxNumItems ];
};

template< typename T, UINT MaxNumItems >
FORCEINLINE TFixedList< T, MaxNumItems >::TFixedList( ENoInit )
	: num( 0 )
{}

template< typename T, UINT MaxNumItems >
FORCEINLINE TFixedList< T, MaxNumItems >::TFixedList( EInitZero )
	: num( 0 )
{
	MemZero( this->data, sizeof(this->data) );
}

template< typename T, UINT MaxNumItems >
FORCEINLINE void TFixedList< T, MaxNumItems >::Add( const T& newItem )
{
	Assert( this->num < MaxNumItems );

	// instead of checking for overflow, we just mask the index
	// so it wraps around
	// NOTE: for this trick to work, mask must be like 0xFF, 0xFFFF, etc.

	#define IS_MAGIC_NUMBER( x )	( (x) == 0x10			\
									|| (x) == 0x100			\
									|| (x) == 0x1000		\
									|| (x) == 0x10000		\
									|| (x) == 0x100000		\
									|| (x) == 0x1000000		\
									|| (x) == 0x10000000 )

	if ( IS_MAGIC_NUMBER( MaxNumItems ) )
	{
		UINT Mask = MaxNumItems - 1;
		UINT index = this->num & Mask;
		this->data[ index ] = newItem;
		this->num++;

	#undef IS_MAGIC_NUMBER

	}
	else
	{
		if ( this->num == MaxNumItems ) {
			this->num = MaxNumItems-1;
		}
		this->data[ this->num ] = newItem;
		this->num++;
	}
}

template< typename T, UINT MaxNumItems >
FORCEINLINE UINT TFixedList< T, MaxNumItems >::Num() const
{
	return this->num;
}

template< typename T, UINT MaxNumItems >
FORCEINLINE T& TFixedList< T, MaxNumItems >::Get( UINT index )
{
	Assert( index < this->num );
	return data[ index ];
}

template< typename T, UINT MaxNumItems >
FORCEINLINE const T& TFixedList< T, MaxNumItems >::Get( UINT index ) const
{
	Assert( index < this->num );
	return data[ index ];
}

template< typename T, UINT MaxNumItems >
FORCEINLINE T & TFixedList< T, MaxNumItems >::operator [] ( UINT index )
{
	return this->Get( index );
}

template< typename T, UINT MaxNumItems >
FORCEINLINE const T & TFixedList< T, MaxNumItems >::operator [] ( UINT index ) const
{
	return this->Get( index );
}
	
template< typename T, UINT MaxNumItems >
FORCEINLINE T * TFixedList< T, MaxNumItems >::Ptr( void )
{
	return this->data;
}

template< typename T, UINT MaxNumItems >
FORCEINLINE const T * TFixedList< T, MaxNumItems >::Ptr( void ) const
{
	return this->data;
}

template< typename T, UINT MaxNumItems >
FORCEINLINE void TFixedList< T, MaxNumItems >::Reset()
{
	this->num = 0;
}

/*
======================================================================
	
		Render targets.

======================================================================
*/

//
//	D3D10RenderTarget
//
class D3D10RenderTarget {
public:
	D3D10RenderTarget();
	~D3D10RenderTarget();

	void	Initialize( UINT width, UINT height );
	void	Shutdown();

	void	Bind();

public:
	DXPtr< ID3D10Texture2D >			pTexture;
	DXPtr< ID3D10ShaderResourceView >	pSRV;
	DXPtr< ID3D10RenderTargetView >		pRTV;
};

//
//	D3D10DepthMap - used as a depth render target for shadow mapping.
//
class D3D10DepthMap {
public:
	enum { MAX_SHADOWMAP_RESOLUTION = 2048 };

public:
			D3D10DepthMap();
			~D3D10DepthMap();

	void	Initialize( UINT maximumResolution );
	void	Shutdown();

	void	Bind( UINT resolution );
	void	Unbind();

	void	Clear( FLOAT fDepth );

public:
	DXPtr< ID3D10Texture2D >			pTexture;
	DXPtr< ID3D10DepthStencilView >		pDSV;
	DXPtr< ID3D10ShaderResourceView >	pSRV;
	D3D10_VIEWPORT						viewport;	// holds current resolution
	UINT								maxResolution;
};

/*
======================================================================
	
		Geometry helpers.

======================================================================
*/

namespace d3d10
{

	ID3D10Buffer * NewStaticBuffer();
	ID3D10Buffer * NewDynamicBuffer();

}//end of namespace d3d10

//-----------------------------------------------------------
//		Geometry for rendering light shapes.
//-----------------------------------------------------------

//
//	LVertex - used for drawing light shapes.
//
struct LVertex
{
	Vec3D	position;

public:
	LVertex()
	{}
	LVertex( const Vec3D& pos )
		: position( pos )
	{}
	LVertex( FLOAT x, FLOAT y, FLOAT z )
		: position( x,y,z )
	{}
};

typedef TArray< LVertex >	LVertexList;
typedef TArray< UINT16 >	LIndexList;

//
//	D3D10LightShape - should use triangle lists and 16-bit indices for rendering.
//
class D3D10LightShape {
public:
	D3D10LightShape()
		: numVertices( 0 )
		, numTriangles( 0 )
	{}
	virtual ~D3D10LightShape()
	{
		Clear();
	}

	// NOTE: this function doesn't set the input layout by itself.
	void Render( class D3D10Shader& shader );

	void Clear()
	{
		this->pVB = null;
		this->pIB = null;

		this->numVertices = 0;
		this->numTriangles = 0;
	}

protected:
	DXPtr< ID3D10Buffer >	pVB;
	DXPtr< ID3D10Buffer >	pIB;
	
	UINT 	numVertices;
	UINT 	numTriangles;
};

//
//	D3D10LightShape_Sphere
//
class D3D10LightShape_Sphere : public D3D10LightShape {
public:
	D3D10LightShape_Sphere();
	~D3D10LightShape_Sphere();

	void Build( FLOAT radius, UINT numSlices, UINT numStacks );

private:
	void BuildStacks( LVertexList & vertices, LIndexList & indices );

private:
	FLOAT	radius;
	UINT	numSlices;
	UINT	numStacks;
};

//
//	D3D10LightShape_Cylinder
//
class D3D10LightShape_Cylinder : public D3D10LightShape {
public:
	D3D10LightShape_Cylinder();
	~D3D10LightShape_Cylinder();

	void Build( FLOAT topRadius, FLOAT bottomRadius, FLOAT height,
				UINT numSlices, UINT numStacks );

private:
	void BuildStacks( LVertexList & vertices, LIndexList & indices );
	void BuildBottomCap( LVertexList & vertices, LIndexList & indices );
	void BuildTopCap( LVertexList & vertices, LIndexList & indices );

private:
	FLOAT	topRadius;
	FLOAT	bottomRadius;
	FLOAT	height;

	UINT	numSlices;
	UINT	numStacks;
};

/*
======================================================================
	
		Shader helpers.

======================================================================
*/

namespace d3d10
{

D3D10_USAGE GetBufferUsageType( EHardwareBufferUsage eUsageType );

const TCHAR* GetErrorCodeString( HRESULT hErrorCode );

ID3D10Effect * LoadEffect( const TCHAR* filename, ID3D10EffectPool* pPool = null );

}//End of namespace d3d10

//-----------------------------------------------------------
//	Mish helper functions.
//-----------------------------------------------------------

/*
// This is used to mitigate precision problems.
//
FORCEINLINE
Vec3D FixUpVector( const Vec3D& lookAt )
{
	const Vec3D worldY( 0,1,0 );
	const Vec3D worldZ( 0,1,0 );
	return (mxMath::Fabs( lookAt.Dot(worldY) ) > 0.99f) ?
												worldZ : worldY;
}
*/

}//End of namespace abc

#endif // !__MX_D3D10_UTILITIES_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
