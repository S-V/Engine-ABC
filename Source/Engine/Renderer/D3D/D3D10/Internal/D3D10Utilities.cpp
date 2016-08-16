/*
=============================================================================
	File:	D3D10Utilities.cpp
	Desc:	Helper classes (mainly wrappers around DirectX functions).
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>
#include <Renderer/D3D/D3DRenderSystem.h>

namespace abc {

/*================================
		D3D10RenderTarget
================================*/

D3D10RenderTarget::D3D10RenderTarget()
{}

D3D10RenderTarget::~D3D10RenderTarget()
{}

void D3D10RenderTarget::Initialize( UINT width, UINT height )
{
	D3D10_TEXTURE2D_DESC texDesc;
	MemZero( &texDesc, sizeof(texDesc) );
	texDesc.BindFlags          = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	texDesc.Usage              = D3D10_USAGE_DEFAULT;
	texDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Width              = width;
	texDesc.Height             = height;
	texDesc.MipLevels          = 1;
	texDesc.ArraySize          = 1;
	texDesc.SampleDesc.Count   = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.CPUAccessFlags     = 0;
	texDesc.MiscFlags          = 0;
	check( d3d10::device->CreateTexture2D( &texDesc, null, & pTexture.Ptr ) );

	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	MemZero( &srvDesc, sizeof(srvDesc) );
	srvDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension		= D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	check( d3d10::device->CreateShaderResourceView( pTexture, &srvDesc, & pSRV.Ptr ) );

	D3D10_RENDER_TARGET_VIEW_DESC rtvDesc;
	MemZero( &rtvDesc, sizeof(rtvDesc) );
	rtvDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension		= D3D10_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice	= 0;
	check( d3d10::device->CreateRenderTargetView( pTexture, &rtvDesc, & pRTV.Ptr ) );
}

void D3D10RenderTarget::Shutdown()
{
	pTexture = null;
	pSRV = null;
	pRTV = null;
}

/*================================
		D3D10DepthMap
================================*/

D3D10DepthMap::D3D10DepthMap()
{}

D3D10DepthMap::~D3D10DepthMap()
{}

void D3D10DepthMap::Initialize( UINT maximumResolution )
{
	Assert( maximumResolution <= MAX_SHADOWMAP_RESOLUTION );
	this->maxResolution = maximumResolution;

	const UINT theWidth = maximumResolution;
	const UINT theHeight = maximumResolution;

	viewport.Width  = theWidth;
    viewport.Height = theHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

	D3D10_TEXTURE2D_DESC texDesc;
    texDesc.ArraySize          = 1;
    texDesc.BindFlags          = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags     = 0;
    texDesc.Format             = DXGI_FORMAT_R32_TYPELESS;
    texDesc.Width              = theWidth;
    texDesc.Height             = theHeight;
    texDesc.MipLevels          = 1;
    texDesc.MiscFlags          = 0;
    texDesc.SampleDesc.Count   = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage              = D3D10_USAGE_DEFAULT;
	check( d3d10::device->CreateTexture2D( &texDesc, null, & pTexture.Ptr ) );

	D3D10_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format				= DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension		= D3D10_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice	= 0;
	check( d3d10::device->CreateDepthStencilView( pTexture, &dsvDesc, & pDSV.Ptr ) );

	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format				= DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension		= D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels	= 1;
	check( d3d10::device->CreateShaderResourceView( pTexture, &srvDesc, & pSRV.Ptr ) );
}

void D3D10DepthMap::Shutdown()
{
	pTexture = null;
	pDSV = null;
	pSRV = null;
}

void D3D10DepthMap::Bind( UINT resolution )
{
	Assert( resolution <= this->maxResolution );

	ID3D10RenderTargetView * renderTargets[1] = { null };
	d3d10::device->OMSetRenderTargets( 1, renderTargets, pDSV );
	d3d10::device->ClearDepthStencilView( pDSV, D3D10_CLEAR_DEPTH, 1.0f, 0 );

	viewport.Height = viewport.Width = resolution;
	d3d10::device->RSSetViewports( 1, &viewport );
}

void D3D10DepthMap::Unbind()
{
	d3d10::renderer->RestoreMainRenderTarget();
}

void D3D10DepthMap::Clear( FLOAT fDepth )
{
	d3d10::device->ClearDepthStencilView( pDSV, D3D10_CLEAR_DEPTH, fDepth, 0 );
}

/*================================
		D3D10LightShape
================================*/

void D3D10LightShape::Render( D3D10Shader& shader )
{
	UINT stride = sizeof( LVertex );
	UINT offset = 0;
	d3d10::device->IASetVertexBuffers( 0, 1, & this->pVB.Ptr, &stride, &offset );
	d3d10::device->IASetIndexBuffer( this->pIB, DXGI_FORMAT_R16_UINT, 0 );
	d3d10::device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST  );

	shader.Apply();
	d3d10::device->DrawIndexed( this->numTriangles * 3, 0, 0 );
}

/*================================
	D3D10LightShape_Sphere
================================*/

D3D10LightShape_Sphere::D3D10LightShape_Sphere()
	: radius( 0.0f )
	, numSlices( 0 )
	, numStacks( 0 )
{}

D3D10LightShape_Sphere::~D3D10LightShape_Sphere()
{}

void D3D10LightShape_Sphere::Build( FLOAT radius, UINT numSlices, UINT numStacks )
{
	Assert( radius > 0 && numSlices < 64 && numStacks < 64 );
	Clear();

	this->radius    = radius;
	this->numSlices = numSlices;
	this->numStacks = numStacks;

	LVertexList   vertices;
	LIndexList    indices;

	BuildStacks( vertices, indices );

	this->numVertices	= (UINT) vertices.Num();
	this->numTriangles  = (UINT) indices.Num() / 3;

	D3D10_BUFFER_DESC vbd;
    vbd.Usage		= D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth	= sizeof(LVertex) * this->numVertices;
    vbd.BindFlags	= D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags	= 0;
	D3D10_SUBRESOURCE_DATA vInitData;
    vInitData.pSysMem = &vertices[0];
    check( d3d10::device->CreateBuffer( &vbd, &vInitData, & this->pVB.Ptr ) );

	D3D10_BUFFER_DESC ibd;
    ibd.Usage		= D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth	= sizeof(UINT16) * this->numTriangles * 3;
    ibd.BindFlags	= D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags	= 0;
    D3D10_SUBRESOURCE_DATA iInitData;
    iInitData.pSysMem = &indices[0];
    check( d3d10::device->CreateBuffer( &ibd, &iInitData, & this->pIB.Ptr ) );
}

void D3D10LightShape_Sphere::BuildStacks( LVertexList & vertices, LIndexList & indices )
{
	FLOAT phiStep = mxMath::PI / (FLOAT) this->numStacks;

	// do not count the poles as rings
	UINT numRings = this->numStacks-1;

	// Compute vertices for each stack ring.
	for ( UINT i = 1; i <= numRings; ++i )
	{
		FLOAT phi = i * phiStep;

		// vertices of ring
		FLOAT thetaStep = 2.0f * mxMath::PI / this->numSlices;
		for ( UINT j = 0; j <= this->numSlices; ++j )
		{
			FLOAT theta = j * thetaStep;

			LVertex v;

			FLOAT sinPhi, cosPhi, sinTheta, cosTheta;
			mxMath::SinCos( phi, sinPhi, cosPhi );
			mxMath::SinCos( theta, sinTheta, cosTheta );

			// spherical to cartesian
			v.position.x = this->radius * sinPhi* cosTheta;
			v.position.y = this->radius * cosPhi;
			v.position.z = this->radius * sinPhi * sinTheta;
#if 0
			// partial derivative of P with respect to theta
			v.tangent.x = -this->radius*mxMath::Sin(phi)*mxMath::Sin(theta );
			v.tangent.y = 0.0f;
			v.tangent.z = this->radius*mxMath::Sin(phi)*mxMath::Cos(theta );

			D3DXVec3Normalize(&v.normal, &v.pos );

			v.texC.x = theta / (2.0f*PI );
			v.texC.y = phi / PI;
#endif
			vertices.Append( v  );
		}
	}

#if 0
	// poles: note that there will be texture coordinate distortion
	vertices.Append( LVertex(0.0f, -this->radius, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f) );
	vertices.Append( LVertex(0.0f, this->radius, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f) );
#else
	vertices.Append( LVertex( 0.0f, -this->radius, 0.0f ) );
	vertices.Append( LVertex( 0.0f, this->radius, 0.0f ) );
#endif

	UINT northPoleIndex = (UINT) vertices.Num() - 1;
	UINT southPoleIndex = (UINT) vertices.Num() - 2;

	UINT numRingVertices = this->numSlices + 1;

	// Compute indices for inner stacks (not connected to poles).
	for ( UINT i = 0; i < this->numStacks-2; ++i )
	{
		for( UINT j = 0; j < this->numSlices; ++j )
		{
			indices.Append( i*numRingVertices + j );
			indices.Append( i*numRingVertices + j+1 );
			indices.Append((i+1)*numRingVertices + j );

			indices.Append(( i+1)*numRingVertices + j );
			indices.Append( i*numRingVertices + j+1 );
			indices.Append( (i+1)*numRingVertices + j+1 );
		}
	}

	// Compute indices for top stack.  The top stack was written 
	// first to the vertex buffer.
	for(UINT i = 0; i < this->numSlices; ++i)
	{
		indices.Append(northPoleIndex );
		indices.Append(i+1 );
		indices.Append(i );
	}

	// Compute indices for bottom stack.  The bottom stack was written
	// last to the vertex buffer, so we need to offset to the index
	// of first vertex in the last ring.
	UINT baseIndex = (numRings-1)*numRingVertices;
	for(UINT i = 0; i < this->numSlices; ++i)
	{
		indices.Append(southPoleIndex );
		indices.Append(baseIndex+i );
		indices.Append(baseIndex+i+1 );
	}
}

/*================================
	D3D10LightShape_Cylinder
================================*/

D3D10LightShape_Cylinder::D3D10LightShape_Cylinder()
	: topRadius		( 0.0f )
	, bottomRadius	( 0.0f )
	, height		( 0.0f )

	, numSlices( 0 )
	, numStacks( 0 )
{}

D3D10LightShape_Cylinder::~D3D10LightShape_Cylinder()
{}

void D3D10LightShape_Cylinder::Build(
	FLOAT topRadius, FLOAT bottomRadius, FLOAT height,
	UINT numSlices, UINT numStacks )
{
	this->topRadius    = topRadius;
	this->bottomRadius = bottomRadius;
	this->height       = height;
	this->numSlices    = numSlices;
	this->numStacks    = numStacks;

	LVertexList   vertices;
	LIndexList    indices;

	BuildStacks( vertices, indices );
	BuildTopCap( vertices, indices );
	BuildBottomCap( vertices, indices );
	
	this->numVertices = (UINT)vertices.Num();
	this->numTriangles = (UINT)indices.Num()/3;

	D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(LVertex) * this->numVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    check( d3d10::device->CreateBuffer( &vbd, &vinitData, &this->pVB.Ptr ) );

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT16) * this->numTriangles * 3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    check( d3d10::device->CreateBuffer( &ibd, &iinitData, &this->pIB.Ptr ) );
}

void D3D10LightShape_Cylinder::BuildStacks( LVertexList & vertices, LIndexList & indices )
{
	float stackHeight = this->height / this->numStacks;

	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (this->topRadius - this->bottomRadius) / this->numStacks;

	UINT numRings = this->numStacks+1;

	// Compute vertices for each stack ring.
	for(UINT i = 0; i < numRings; ++i)
	{
		float y = -0.5f*this->height + i*stackHeight;
		float r = this->bottomRadius + i*radiusStep;

		// Height and radius of next ring up.
		float y_next = -0.5f*this->height + (i+1)*stackHeight;
		float r_next = this->bottomRadius + (i+1)*radiusStep;
	 
		// vertices of ring
		float dTheta = 2.0f * mxMath::PI / this->numSlices;
		for(UINT j = 0; j <= this->numSlices; ++j)
		{
			float c = mxMath::Cos(j*dTheta);
			float s = mxMath::Sin(j*dTheta);
#if 0
			float u = (float)j/this->numSlices;
			float v = 1.0f - (float)i/this->numStacks;

			// Partial derivative in theta direction to get tangent vector (this is a unit vector).
			D3DXVECTOR3 T(-s, 0.0f, c);

			// Compute tangent vector down the slope of the cone (if the top/bottom 
			// radii differ then we get a cone and not a true cylinder).
			D3DXVECTOR3 P(r*c, y, r*s);
			D3DXVECTOR3 P_next(r_next*c, y_next, r_next*s);
			D3DXVECTOR3 B = P - P_next;
			D3DXVec3Normalize(&B, &B);

			D3DXVECTOR3 N;
			D3DXVec3Cross(&N, &T, &B);
			D3DXVec3Normalize(&N, &N);

			vertices.Append( LVertex(P.x, P.y, P.z, T.x, T.y, T.z, N.x, N.y, N.z, u, v) );
#else
			(void) y_next, (void) r_next;
			D3DXVECTOR3 P(r*c, y, r*s);
			vertices.Append( LVertex(P.x, P.y, P.z));
#endif
		}
	}

	UINT numRingVertices = this->numSlices+1;

	// Compute indices for each stack.
	for(UINT i = 0; i < this->numStacks; ++i)
	{
		for(UINT j = 0; j < this->numSlices; ++j)
		{
			indices.Append(i*numRingVertices + j);
			indices.Append((i+1)*numRingVertices + j);
			indices.Append((i+1)*numRingVertices + j+1);

			indices.Append(i*numRingVertices + j);
			indices.Append((i+1)*numRingVertices + j+1);
			indices.Append(i*numRingVertices + j+1);
		}
	}
}
 
void D3D10LightShape_Cylinder::BuildBottomCap( LVertexList & vertices, LIndexList & indices )
{
	UINT baseIndex = (UINT)vertices.Num();

	// Duplicate cap vertices because the texture coordinates and normals differ.
	float y = -0.5f*this->height;

	// vertices of ring
	float dTheta = 2.0f * mxMath::PI / this->numSlices;
	for(UINT i = 0; i <= this->numSlices; ++i)
	{
		float x = this->bottomRadius*mxMath::Cos(i*dTheta);
		float z = this->bottomRadius*mxMath::Sin(i*dTheta);
#if 0
		// Map [-1,1]-->[0,1] for planar texture coordinates.
		float u = +0.5f * x/this->bottomRadius + 0.5f;
		float v = -0.5f * z/this->bottomRadius + 0.5f;

	vertices.Append( LVertex(x, y, z, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, u, v) );
#else
	vertices.Append( LVertex(x, y, z));
#endif
	}

	// cap center vertex
#if 0
	vertices.Append( LVertex(0.0f, y, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.5f) );
#else
	vertices.Append( LVertex(0.0f, y, 0.0f));
#endif

	// index of center vertex
	UINT centerIndex = (UINT)vertices.Num()-1;

	for(UINT i = 0; i < this->numSlices; ++i)
	{
		indices.Append(centerIndex);
		indices.Append(baseIndex + i);
		indices.Append(baseIndex + i+1);
	}
}

//
//	D3D10LightShape_Cylinder::BuildTopCap
//
void D3D10LightShape_Cylinder::BuildTopCap( LVertexList & vertices, LIndexList & indices )
{
	UINT baseIndex = (UINT)vertices.Num();

	// Duplicate cap vertices because the texture coordinates and normals differ.
	float y = 0.5f * this->height;

	// vertices of ring
	float dTheta = 2.0f * mxMath::PI / this->numSlices;
	for(UINT i = 0; i <= this->numSlices; ++i)
	{
		float x = this->topRadius * mxMath::Cos( i * dTheta );
		float z = this->topRadius * mxMath::Sin( i * dTheta );
#if 0
		// Map [-1,1]-->[0,1] for planar texture coordinates.
		float u = +0.5f*x/this->topRadius + 0.5f;
		float v = -0.5f*z/this->topRadius + 0.5f;

		vertices.Append( LVertex(x, y, z, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, u, v) );
#else
		vertices.Append( LVertex(x, y, z));
#endif
	}

	// cap center vertex
#if 0
	vertices.Append( LVertex(0.0f, y, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f) );
#else
	vertices.Append( LVertex(0.0f, y, 0.0f));
#endif

	// index of center vertex
	UINT centerIndex = (UINT)vertices.Num()-1;

	for(UINT i = 0; i < this->numSlices; ++i)
	{
		indices.Append(centerIndex);
		indices.Append(baseIndex + i+1);
		indices.Append(baseIndex + i);
	}
}

//======================================================================

namespace d3d10 {

/*
================================
		GetBufferUsageType
================================
*/
D3D10_USAGE GetBufferUsageType( EHardwareBufferUsage eUsageType )
{
	switch ( eUsageType )
	{
		case HBU_STATIC :
			return D3D10_USAGE_IMMUTABLE;

		case HBU_DYNAMIC :
			return D3D10_USAGE_DYNAMIC;

		case HBU_WRITEONLY :
			return D3D10_USAGE_DYNAMIC;

		default:
			return D3D10_USAGE_DEFAULT;
	}
}

/*
================================
		GetErrorCodeString
================================
*/
const TCHAR* GetErrorCodeString( HRESULT hErrorCode )
{
	switch ( hErrorCode )
	{
		case D3D10_ERROR_FILE_NOT_FOUND :
			return TEXT("The file was not found");

		case D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS :
			return TEXT("There are too many unique instances of a particular type of state object");

		case D3DERR_INVALIDCALL :
			return TEXT("The method call is invalid");

		case D3DERR_WASSTILLDRAWING :
			return TEXT("The previous blit operation that is transferring information to or from this surface is incomplete");

		case E_FAIL :
			return TEXT("Attempted to create a device with the debug layer enabled and the layer is not installed");

		case E_INVALIDARG :
			return TEXT("An invalid parameter was passed to the returning function");

		case E_OUTOFMEMORY :
			return TEXT("Direct3D could not allocate sufficient memory to complete the call");

		case S_FALSE :
			return TEXT("Alternate success value, indicating a successful but nonstandard completion (the precise meaning depends on context");

		case S_OK :
			return TEXT("No error occurred"); 

		default:
			Unreachable;
	}
	return null;
}

/*
================================
		LoadEffect
================================
*/
ID3D10Effect * LoadEffect( const TCHAR* filename, ID3D10EffectPool* pPool )
{
	AssertPtr( filename );
	Assert( sys::FileExists(filename) );

	ID3D10Effect * pEffect = null;

	UINT dwShaderFlags;
/*
	D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
	//dwShaderFlags |= D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
	DEBUG_CODE
	(
		dwShaderFlags |= ( D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION );
	)
*/
	dwShaderFlags = 0;

	ID3D10Blob * pErrorBuffer = null;

	const HRESULT  hResult = D3DX10CreateEffectFromFile(
		filename,
		null,				// defines
		null,				// include
		"fx_4_0",			// shader profile
		dwShaderFlags,		// HLSL compile options
		pPool ? D3D10_EFFECT_COMPILE_CHILD_EFFECT : 0,	// effect compile options
		d3d10::device,
		pPool,				// effect pool for sharing variables between effects
		null,				// thread pump
		& pEffect,
		& pErrorBuffer,
		null
	);
	if ( pErrorBuffer )
	{
		sys::Error( TEXT("Error: %s"), (const mxChar*) pErrorBuffer->GetBufferPointer() );
		pErrorBuffer->Release();
	}
	if( FAILED( hResult ) )
	{
		sys::Error( TEXT("Failed to load effect file '%s'"), filename );
		return null;
	}

	Assert( pEffect && pEffect->IsValid() );

	D3D10_EFFECT_DESC fxDesc;
	ensure( pEffect->GetDesc( &fxDesc ) );
	if ( fxDesc.Techniques != 1 ) {
		sys::Error( TEXT("Effect file '%s' must contain a single technique"), filename );
	}

	for( UINT iTech = 0; iTech < fxDesc.Techniques; iTech++ )
	{
		ID3D10EffectTechnique * pTechnique = pEffect->GetTechniqueByIndex( iTech );
		if ( ! pTechnique->IsValid() ) {
			D3D10_TECHNIQUE_DESC techDesc;
			pTechnique->GetDesc( &techDesc );
			sys::Error( TEXT("Technique %s is invalid"), techDesc.Name );
		}
	}

	return pEffect;
}

}//End of namespace d3d10

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
