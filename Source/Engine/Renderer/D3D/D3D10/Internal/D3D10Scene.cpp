/*
=============================================================================
	File:	D3D10Scene.cpp
	Desc:	Actual D3D10 deferred shading scene renderer.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>
#include <Renderer/D3D/D3DRenderSystem.h>

namespace abc {

/*================================
	D3D10RenderTargetChain
================================*/

D3D10RenderTargetChain::D3D10RenderTargetChain()
{}

D3D10RenderTargetChain::~D3D10RenderTargetChain()
{}

void D3D10RenderTargetChain::Initialize( UINT screenWidth, UINT screenHeight )
{
	D3D10_TEXTURE2D_DESC texDesc;
	MemZero( &texDesc, sizeof(texDesc) );
	texDesc.Width				= screenWidth;
	texDesc.Height				= screenHeight;
	texDesc.MipLevels			= 1;
	texDesc.ArraySize			= 1;
	texDesc.SampleDesc.Count	= 1;	// no anti-aliasing
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage				= D3D10_USAGE_DEFAULT;
	texDesc.BindFlags			= D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags		= 0;
	texDesc.MiscFlags			= 0;

	D3D10_RENDER_TARGET_VIEW_DESC rtvDesc;
	MemZero( &rtvDesc, sizeof(rtvDesc) );
	rtvDesc.ViewDimension		= D3D10_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice	= 0;

	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	MemZero( &srvDesc, sizeof(srvDesc) );
	srvDesc.ViewDimension		= D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels	= 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	//
	// RT1 - Position
	//
	MX_OPTIMIZE( "don't create a position map, calculate X and Y from depth in pixel shader" )
	{
#if 1
		texDesc.Format = DXGI_FORMAT_R16G16B16A16_TYPELESS;
		rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
#elif 0
		texDesc.Format = DXGI_FORMAT_R10G10B10A2_TYPELESS;
		rtvDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
		srvDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
#else
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
#endif

		// Create a render target.
		//
		check( d3d10::device->CreateTexture2D(
			&texDesc,
			NULL,			// Initial Data
			& this->renderTargetTextures[ RT_POSITION ].Ptr )
		);

		// Create a render target view.
		//
		check( d3d10::device->CreateRenderTargetView(
			this->renderTargetTextures[ RT_POSITION ],
			&rtvDesc,
			& this->renderTargetViews[ RT_POSITION ].Ptr )
		);

		// Create a shader-resource view.
		//
		check( d3d10::device->CreateShaderResourceView(
			this->renderTargetTextures[ RT_POSITION ],
			&srvDesc,
			& this->renderTargetSRViews[ RT_POSITION ].Ptr )
		);
	}

	//
	// RT2 - Normals
	//
	{
		// ! we can use DXGI_FORMAT_R16G16_FLOAT and calc z = sqrt( sqr(x) + sqr(y) )
#if 1
		texDesc.Format = DXGI_FORMAT_R16G16B16A16_TYPELESS;
		rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
#elif 0
		texDesc.Format = DXGI_FORMAT_R10G10B10A2_TYPELESS;
		rtvDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
		srvDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
#else
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
#endif

		// Create a render target.
		//
		check( d3d10::device->CreateTexture2D(
			&texDesc,
			NULL,			// Initial Data
			& this->renderTargetTextures[ RT_NORMALS ].Ptr )
		);

		// Create a render target view.
		//
		check( d3d10::device->CreateRenderTargetView(
			this->renderTargetTextures[ RT_NORMALS ],
			&rtvDesc,
			& this->renderTargetViews[ RT_NORMALS ].Ptr )
		);

		// Create a shader-resource view.
		//
		check( d3d10::device->CreateShaderResourceView(
			this->renderTargetTextures[ RT_NORMALS ],
			&srvDesc,
			& this->renderTargetSRViews[ RT_NORMALS ].Ptr )
		);
	}

	//
	// RT3 - Diffuse color
	//
	{
#if 0
		const DXGI_FORMAT  theFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
#else
		texDesc.Format = DXGI_FORMAT_R16G16B16A16_TYPELESS;
		rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
#endif

		// Create a render target.
		//
		check( d3d10::device->CreateTexture2D(
			&texDesc,
			NULL,			// Initial Data
			& this->renderTargetTextures[ RT_DIFFUSE ].Ptr )
		);

		// Create a render target view.
		//
		check( d3d10::device->CreateRenderTargetView(
			this->renderTargetTextures[ RT_DIFFUSE ],
			&rtvDesc,
			& this->renderTargetViews[ RT_DIFFUSE ].Ptr )
		);

		// Create a shader-resource view.
		//
		check( d3d10::device->CreateShaderResourceView(
			this->renderTargetTextures[ RT_DIFFUSE ],
			&srvDesc,
			& this->renderTargetSRViews[ RT_DIFFUSE ].Ptr )
		);
	}

	//
	// RT4 - Auxiliary material parameters.
	//
	{
		//const DXGI_FORMAT  theFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		const DXGI_FORMAT  theFormat = DXGI_FORMAT_R16G16_FLOAT;

		// Create a render target.
		//
		texDesc.Format = theFormat;
		check( d3d10::device->CreateTexture2D(
			&texDesc,
			NULL,			// Initial Data
			& this->renderTargetTextures[ RT_MATERIAL ].Ptr )
		);

		// Create a render target view.
		//
		rtvDesc.Format = theFormat;
		check( d3d10::device->CreateRenderTargetView(
			this->renderTargetTextures[ RT_MATERIAL ],
			&rtvDesc,
			& this->renderTargetViews[ RT_MATERIAL ].Ptr )
		);

		// Create a shader-resource view.
		//
		srvDesc.Format = theFormat;
		check( d3d10::device->CreateShaderResourceView(
			this->renderTargetTextures[ RT_MATERIAL ],
			&srvDesc,
			& this->renderTargetSRViews[ RT_MATERIAL ].Ptr )
		);
	}
}

void D3D10RenderTargetChain::Shutdown()
{
	for ( IndexT nRT = 0; nRT < NUM_RENDER_TARGETS; nRT++ )
	{
		this->renderTargetTextures[ nRT ]	= null;
		this->renderTargetSRViews[ nRT ]	= null;
		this->renderTargetViews[ nRT ]		= null;
	}
}

void D3D10RenderTargetChain::Bind()
{
	d3d10::device->OMSetRenderTargets(
		NUM_RENDER_TARGETS,
		(ID3D10RenderTargetView *const *) this->renderTargetViews.Ptr(),
		d3d10::renderer->GetDepthStencilView()
	);
}

void D3D10RenderTargetChain::Unbind()
{
	d3d10::renderer->RestoreMainRenderTarget();
}

void D3D10RenderTargetChain::SetDiffuseRT()
{
	d3d10::device->OMSetRenderTargets(
		1,
		(ID3D10RenderTargetView *const *) & this->renderTargetViews[ RT_DIFFUSE ],
		d3d10::renderer->GetDepthStencilView()
	);
}

void D3D10RenderTargetChain::Clear()
{
	// unbind all render targets so that clearing them is faster (does it work on any hardware?)
	//d3d10::device->OMSetRenderTargets( 0, null, null );

	static const FLOAT defaultPosition[4] = { 0,0,1,0 };
	d3d10::device->ClearRenderTargetView( this->renderTargetViews[ D3D10RenderTargetChain::RT_POSITION ],	(const FLOAT*) defaultPosition );
	
	static const FLOAT defaultNormals[4] = { 0,0,0,0 };
	d3d10::device->ClearRenderTargetView( this->renderTargetViews[ D3D10RenderTargetChain::RT_NORMALS ],	(const FLOAT*) defaultNormals );

	static const FLOAT defaultDiffuse[4] = { 0,0,0,0 };
	d3d10::device->ClearRenderTargetView( this->renderTargetViews[ D3D10RenderTargetChain::RT_DIFFUSE ],	(const FLOAT*) defaultDiffuse );

	static const FLOAT defaultMaterial[4] = { 0,1,0,0 };
	d3d10::device->ClearRenderTargetView( this->renderTargetViews[ D3D10RenderTargetChain::RT_MATERIAL ],	(const FLOAT*) defaultMaterial );
}

/*================================
		D3D10FullScreenQuad
================================*/

D3D10FullScreenQuad::D3D10FullScreenQuad()
{}

D3D10FullScreenQuad::~D3D10FullScreenQuad()
{}

void D3D10FullScreenQuad::Initialize( D3D10Shader& screenShader )
{
	D3D10_PASS_DESC passDesc;
	screenShader.GetActivePass()->GetDesc( &passDesc );

	const void* pShaderBytecodeWithInputSignature = passDesc.pIAInputSignature;
	const SizeT bytecodeLength = passDesc.IAInputSignatureSize; 

	if ( this->vertexLayout.IsNull() )
	{
		D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ TEXT("POSITION"), 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ TEXT("TEXCOORD"), 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};

		check( d3d10::device->CreateInputLayout(
			vertexDesc, ARRAY_SIZE( vertexDesc ),
			pShaderBytecodeWithInputSignature, bytecodeLength,
			& this->vertexLayout.Ptr )
		);
	}

	//
	//	Create quad geometry
	//
	if ( this->vertexBuffer.IsNull() )
	{
		// Build a full-screen quad.
		ScreenQuadVertex quad[4] =
		{
			ScreenQuadVertex( Vec3D( -1.0f,  1.0f, 0.5f ),	Vec2D( 0.0f, 0.0f ) ),
			ScreenQuadVertex( Vec3D(  1.0f,  1.0f, 0.5f ),	Vec2D( 1.0f, 0.0f ) ),
			ScreenQuadVertex( Vec3D( -1.0f, -1.0f, 0.5f ),	Vec2D( 0.0f, 1.0f ) ),
			ScreenQuadVertex( Vec3D(  1.0f, -1.0f, 0.5f ),	Vec2D( 1.0f, 1.0f ) ),
		};

		D3D10_BUFFER_DESC vbdesc =
		{
			4 * sizeof( ScreenQuadVertex ),	// ByteWidth
			D3D10_USAGE_IMMUTABLE,		// Usage
			D3D10_BIND_VERTEX_BUFFER,	// BindFlags
			0,	// CPUAccessFlags
			0	// MiscFlags
		};

		D3D10_SUBRESOURCE_DATA vbInitData;
		vbInitData.pSysMem = quad;
		vbInitData.SysMemPitch = 0;
		vbInitData.SysMemSlicePitch = 0;

		check( d3d10::device->CreateBuffer(
			&vbdesc,
			&vbInitData,
			& this->vertexBuffer.Ptr )
		);
	}
}

void D3D10FullScreenQuad::Shutdown()
{
	this->vertexBuffer = null;
	this->vertexLayout = null;
}

void D3D10FullScreenQuad::Render( D3D10Shader& shader )
{
    UINT strides = sizeof( ScreenQuadVertex );
    UINT offsets = 0;
	ID3D10Buffer * pBuffers[1] = { this->vertexBuffer };

	d3d10::device->IASetInputLayout( this->vertexLayout );
	d3d10::device->IASetVertexBuffers( 0, 1, pBuffers, &strides, &offsets );
    d3d10::device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	shader.Apply();
	d3d10::device->Draw( 4, 0 );
}

ID3D10InputLayout * D3D10FullScreenQuad::GetScreenVertexLayout()
{
	return this->vertexLayout;
}

/*================================
		D3D10Model
================================*/

D3D10Model::D3D10Model()
	: worldTransform( _InitIdentity )
{}

D3D10Model::~D3D10Model()
{}

void D3D10Model::Setup( const rxModelDescription& desc )
{
	Assert( desc.IsOk() );

	Assert2( desc.meshDesc.PrimitiveType == EPrimitiveType::PT_TriangleList, "only triangle lists are supported" );


	this->worldTransform.SetIdentity();

	this->flags = desc.flags;

	const bool bCsgModel = ( desc.flags & EModelFlags::MF_DynamicGeometry );

	
	if( bCsgModel )
	{
	}
	else
	{
	}




	const D3D10_USAGE  bufferUsage = bCsgModel ? D3D10_USAGE_DYNAMIC : D3D10_USAGE_IMMUTABLE;

	const UINT INITIAL_SIZE = MAX_UINT16;	// 64K

	// Create vertex buffer.
	{
		const SizeT vertexCount = desc.meshDesc.Vertices.VertexCount;
		const SizeT vertexSize = desc.meshDesc.Vertices.VertexSize;

		D3D10_BUFFER_DESC vbd;
		vbd.Usage			= bufferUsage;
		vbd.ByteWidth		= bCsgModel ? (Max( vertexCount, INITIAL_SIZE ) * vertexSize) : vertexCount*vertexSize;
		vbd.BindFlags		= D3D10_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags	= bCsgModel ? D3D10_CPU_ACCESS_WRITE : 0;
		vbd.MiscFlags		= 0;

		D3D10_SUBRESOURCE_DATA vinitData;	// initial data
		vinitData.pSysMem = desc.meshDesc.Vertices.Data;

		ensure( d3d10::device->CreateBuffer(
			&vbd,
			bCsgModel ? null : &vinitData,
			&this->pVB.Ptr
		));

		AssertPtr( this->pVB );

		if( bCsgModel )
		{
			void * pData;
			check( pVB->Map( D3D10_MAP::D3D10_MAP_WRITE_DISCARD, 0, &pData ) );
			MemCopy( pData, desc.meshDesc.Vertices.Data, vertexCount * vertexSize );
			pVB->Unmap();
		}

		this->batch.StartVertex = 0;
		this->batch.VertexCount = vertexCount;
	}

	// Create index buffer.
	{
		const SizeT indexCount = desc.meshDesc.Indices.IndexCount;
		const SizeT indexSize = desc.meshDesc.Indices.IndexSize;

		D3D10_BUFFER_DESC ibd;
		ibd.Usage			= bufferUsage;
		ibd.ByteWidth		= bCsgModel ? (Max( indexCount, INITIAL_SIZE ) * indexSize) : indexCount*indexSize;
		ibd.BindFlags		= D3D10_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags	= bCsgModel ? D3D10_CPU_ACCESS_WRITE : 0;
		ibd.MiscFlags		= 0;

		D3D10_SUBRESOURCE_DATA iinitData;	// initial data
		iinitData.pSysMem = desc.meshDesc.Indices.Data;

		ensure( d3d10::device->CreateBuffer(
			&ibd,
			bCsgModel ? null : &iinitData,
			&this->pIB.Ptr
		));

		AssertPtr( this->pIB );

		if( bCsgModel )
		{
			void * pData;
			check( pIB->Map( D3D10_MAP::D3D10_MAP_WRITE_DISCARD, 0, &pData ) );
			MemCopy( pData, desc.meshDesc.Indices.Data, indexCount * indexSize );
			pIB->Unmap();
		}

		this->batch.StartIndex = 0;
		this->batch.IndexCount = indexCount;
	}



	this->depth = 0.f;
}

void D3D10Model::Destroy()
{
	this->pVB = null;
	this->pIB = null;

	this->material = null;
}

void D3D10Model::Render( const rxView& view, rxQueue& queue )
{
	ToD3D10Queue( queue ).models.Add( this );

	Vec4D  worldPosition( this->GetOrigin(), 1.0f );
	Vec4D  worldPositionH( worldPosition * ToD3D10View( view ).ViewProjMatrix );
	FLOAT  invW = mxMath::Reciprocal( worldPositionH.w );

	this->depth = worldPositionH.z * invW;
}

void D3D10Model::Remove()
{
	d3d10::scene->RemoveModel( this );
}

void D3D10Model::SetTransform( const Matrix4& newWorldTransform )
{
	this->worldTransform = newWorldTransform;
}

void D3D10Model::SetMaterial( rxMaterial* newMaterial )
{
	AssertPtr( newMaterial );
	this->material = checked_cast< D3D10Material*, rxMaterial* >( newMaterial );
}

void D3D10Model::SetGeometry( const rxDynMeshData* newMesh )
{
	if( ! (this->flags & EModelFlags::MF_DynamicGeometry) ) {
		DEBUG_CODE( sys::Warning("attempted to modify geometry of a static mesh") );
		return;
	}
	
	D3D10_BUFFER_DESC vbDesc;
	this->pVB->GetDesc( &vbDesc );

	D3D10_BUFFER_DESC ibDesc;
	this->pIB->GetDesc( &ibDesc );

	if( vbDesc.ByteWidth > newMesh->numVertices * sizeof(rxVertex)
		&& ibDesc.ByteWidth > newMesh->numIndices * sizeof(rxIndex) )
	{
		{
			void * pData;
			check( pVB->Map( D3D10_MAP::D3D10_MAP_WRITE_DISCARD, 0, &pData ) );
			MemCopy( pData, newMesh->vertices, newMesh->numVertices * sizeof(rxVertex) );
			pVB->Unmap();
		}

		{
			void * pData;
			check( pIB->Map( D3D10_MAP::D3D10_MAP_WRITE_DISCARD, 0, &pData ) );
			MemCopy( pData, newMesh->indices, newMesh->numIndices * sizeof(rxIndex) );
			pIB->Unmap();
		}

		this->batch.StartVertex = 0;
		this->batch.VertexCount = newMesh->numVertices;
		this->batch.StartIndex = 0;
		this->batch.IndexCount = newMesh->numIndices;
	}
	else
	{
		Unimplemented;
	}
}

void D3D10Model::RenderGeometry()
{
	d3d10::device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	const UINT vertexSize = sizeof(rxVertex);
	const UINT offset = 0;

	d3d10::device->IASetVertexBuffers(
		0,				// StartSlot
		1,				// NumBuffers
		& this->pVB.Ptr,
		& vertexSize,
		& offset
	);

	if( this->pIB != null )
	{
		d3d10::device->IASetIndexBuffer(
			this->pIB,
			DXGI_FORMAT_R32_UINT,
			0	// Offset
		);

		d3d10::device->DrawIndexed(
			this->batch.IndexCount,
			0,	// StartIndexLocation
			0	// BaseVertexLocation
		);
	}
	else
	{
		d3d10::device->Draw(
			this->batch.VertexCount,
			0	// StartVertexLocation
		);
	}
}

void D3D10Model::RenderShadowMap()
{
	RenderGeometry();
}

bool D3D10Model::IsOk() const
{
	return 1
		&& (this->pVB != null)
		&& (this->pIB != null)
		&& this->material.IsValid()
		;
}

const Vec3D & D3D10Model::GetOrigin() const {
	return this->worldTransform.GetTranslation();
}

/*================================
		D3D10RenderQueue
================================*/

D3D10RenderQueue::D3D10RenderQueue()
	: models		( _NoInit )
	, translucent	( _NoInit )
	, globalLights	( _NoInit )
	, localLights	( _NoInit )
	, miniLights	( _NoInit )
	, portals		( _NoInit )
	, skies			( _NoInit )
{}

D3D10RenderQueue::~D3D10RenderQueue()
{}

/*================================
		D3D10RenderStage
================================*/

void D3D10RenderStage::PrepareRender()
{
	d3d10::scene->SetCurrentRenderStage( this );
}

/*================================
		D3D10DebugStage
================================*/

D3D10DebugStage::D3D10DebugStage()
	: mode( EDebugRenderMode::Render_Normal )
{}

D3D10DebugStage::~D3D10DebugStage()
{}

void D3D10DebugStage::Initialize()
{
	this->fxDiffuseOnly		.Load( PATH_TO_SHADERS_"Debug_DiffuseOnly.fx" );
	this->fxRenderDepth		.Load( PATH_TO_SHADERS_"Debug_RenderDepth.fx" );
	this->fxShowNormals		.Load( PATH_TO_SHADERS_"Debug_ShowNormals.fx" );
}

void D3D10DebugStage::Shutdown()
{
	this->fxDiffuseOnly		.Unload();
	this->fxRenderDepth		.Unload();
	this->fxShowNormals		.Unload();
}

bool D3D10DebugStage::Render( D3D10RenderTargetChain& renderTargets, D3D10FullScreenQuad& screenQuad )
{
	D3D10RenderStage::PrepareRender();

	switch ( this->mode )
	{
		case EDebugRenderMode::Render_Normal :
			return false;

		case EDebugRenderMode::Render_DiffuseOnly :
			{
				this->fxDiffuseOnly.Bind();
				screenQuad.Render( this->fxDiffuseOnly );
				this->fxDiffuseOnly.Unbind();
			}
			return true;

		case EDebugRenderMode::Render_DepthOnly :
			{
				this->fxRenderDepth.Bind();
				screenQuad.Render( this->fxRenderDepth );
				this->fxRenderDepth.Unbind();
			}
			return true;

		case EDebugRenderMode::Render_SurfaceNormals :
			{
				this->fxShowNormals.Bind();
				screenQuad.Render( this->fxShowNormals );
				this->fxShowNormals.Unbind();
			}
			return true;
	}

	Unreachable;
	return false;
}

/*================================
		D3D10GeometryStage
================================*/

D3D10GeometryStage::D3D10GeometryStage()
{}

D3D10GeometryStage::~D3D10GeometryStage()
{}

void D3D10GeometryStage::Initialize()
{
	// Load shaders.
	{
		this->fxFillGBuffer			.Load( PATH_TO_SHADERS_"FillGBuffer.fx" );
	}

	// Create vertex declarations.
	{
		D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ TEXT("POSITION"),   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ TEXT("NORMAL"),     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ TEXT("TANGENT"),    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ TEXT("TEXCOORD"),   0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};

		D3D10_PASS_DESC passDesc;
		this->fxFillGBuffer.GetActivePass()->GetDesc( &passDesc );

		check( d3d10::device->CreateInputLayout(
			vertexDesc, ARRAY_SIZE( vertexDesc ),
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,
			& this->geometryVertexLayout.Ptr )
		);
	}
}

void D3D10GeometryStage::Shutdown()
{
	this->fxFillGBuffer		.Unload();

	this->geometryVertexLayout = null;
}

void D3D10GeometryStage::Render( const D3D10ViewConstants& view, D3D10RenderQueue& renderQueue )
{
	D3D10RenderStage::PrepareRender();

	d3d10::device->IASetInputLayout( this->geometryVertexLayout );

	// Render solid objects.
	for ( IndexT iModel = 0; iModel < renderQueue.models.Num(); iModel++ )
	{
		D3D10Model * model = renderQueue.models[ iModel ];

		this->fxFillGBuffer.SetMatrices( model->worldTransform, view.ViewProjMatrix );
		this->fxFillGBuffer.SetMaterial( model->material );
		this->fxFillGBuffer.Apply();
		
		model->RenderGeometry();
	}

	this->fxFillGBuffer.Unbind();
}

/*================================
			D3D10Light
================================*/

D3D10Light::~D3D10Light()
{}

/*================================
	D3D10DirectionalLight
================================*/

D3D10DirectionalLight::D3D10DirectionalLight( const rxLightDescription& desc, const Vec3D& direction )
{
	rxLight::Init( desc );

	this->data.color		= desc.color;
	this->data.direction	= direction;

	Assert( this->data.IsOk() );
}

D3D10DirectionalLight::~D3D10DirectionalLight()
{}

const Vec3D & D3D10DirectionalLight::GetDirection() const
{
	return this->data.direction;
}

void D3D10DirectionalLight::SetDirection( const Vec3D& newDir )
{
	Assert( newDir.IsNormalized() );
	this->data.direction = newDir;
}

void D3D10DirectionalLight::SetColor( const ColorRGB& newDiffuseColor )
{
	this->data.color = newDiffuseColor;
}

void D3D10DirectionalLight::Render( const rxView& view, rxQueue& queue )
{
	if( this->IsEnabled() ) {
		ToD3D10Queue( queue ).globalLights.Add( this );
	}
}

void D3D10DirectionalLight::Remove()
{
	Unimplemented;
}

void D3D10DirectionalLight::RenderGlobalLight()
{
	D3D10Shader_ParallelLight & shader = d3d10::scene->GetLightStage().GetData().fxDirectionalLight;

	shader.Bind( this->data );
	d3d10::scene->GetFullscreenQuad().Render( shader );
	shader.Unbind();
}

/*================================
		D3D10PointLight
================================*/

D3D10PointLight::D3D10PointLight( const rxLightDescription& desc, const Vec3D& origin, FLOAT range )
{
	rxLight::Init( desc );

	this->data.color		= desc.color;
	this->data.range		= range;
	this->data.position		= origin;
	this->data.attenuation	= desc.attenuation;

	Assert( this->data.IsOk() );
}

D3D10PointLight::~D3D10PointLight()
{}

const Vec3D & D3D10PointLight::GetOrigin() const
{
	return this->data.position;
}

void D3D10PointLight::SetOrigin( const Vec3D& newOrigin )
{
	this->data.position = newOrigin;
}

FLOAT D3D10PointLight::GetRange() const
{
	return this->data.range;
}

void D3D10PointLight::SetRange( FLOAT newRange )
{
	this->data.range = newRange;
}

void D3D10PointLight::SetColor( const ColorRGB& newDiffuseColor )
{
	this->data.color = newDiffuseColor;
}

void D3D10PointLight::Render( const rxView& view, rxQueue& queue )
{
	if( this->IsEnabled() ) {
		ToD3D10Queue( queue ).localLights.Add( this );
	}
}

void D3D10PointLight::Remove()
{
	Unimplemented;
}

Matrix4 D3D10PointLight::GetWorldTransform() const
{
	FLOAT scale = this->data.range;
	
#ifdef RX_D3D10_SCALE_BIAS
	// add a small bias to hide light shape geometry artifacts
	scale *= 1.1f;
#endif

	Vec3D origin = this->data.position;
	return Matrix4(
		scale,		0,			0,			0,
		0,			scale,		0,			0,
		0,			0,			scale,		0,
		origin.x,	origin.y,	origin.z,	1
	);
}

bool D3D10PointLight::ContainsPoint( const Vec3D& point ) const
{
	FLOAT distanceSqr = (this->data.position - point).LengthSqr();
	return distanceSqr < Square( this->GetRange() );
}

void D3D10PointLight::RenderLocalLight( const D3D10ViewConstants& view )
{
	Matrix4  wvp( GetWorldTransform() * view.ViewProjMatrix );

	D3D10Shader_PointLight & shader = d3d10::scene->GetLightStage().GetData().fxPointLight;
	shader.Bind( wvp, this->data );

	// reverse backface cull mode when the eye is inside the light volume
	if ( this->ContainsPoint( view.EyePosition ) ) {
		d3d10::device->OMSetDepthStencilState( d3d10::scene->GetLightStage().GetData().dsState_CullFrontFaces, 0 );
		d3d10::device->RSSetState( d3d10::scene->GetLightStage().GetData().rsState_CullFrontFaces );
	} else {
		d3d10::device->OMSetDepthStencilState( d3d10::scene->GetLightStage().GetData().dsState_CullBackFaces, 0 );
		d3d10::device->RSSetState( d3d10::scene->GetLightStage().GetData().rsState_CullBackFaces );
	}

	D3D10LightShape_Sphere & lightShape = d3d10::scene->GetLightStage().GetData().pointLightShape;
	lightShape.Render( shader );

	shader.Unbind();
}

/*================================
		D3D10SpotLight
================================*/

D3D10SpotLight::D3D10SpotLight(
	   const rxLightDescription& desc,
	   const Vec3D& origin, const Vec3D& lightDirection,
	   FLOAT range, FLOAT innerAngle, FLOAT outerAngle
	)
	: view( _NoInit )
{
	rxLight::Init( desc );

	this->data.color		= desc.color;
	this->data.range		= range;
	this->data.position		= origin;
	this->data.cosTheta		= mxMath::Cos( innerAngle * 0.5f );
	this->data.direction	= lightDirection;
	this->data.cosPhi		= mxMath::Cos( outerAngle * 0.5f );
	this->data.attenuation	= desc.attenuation;

	Assert( this->data.IsOk() );
	
	this->coneAngle = outerAngle;

	this->RecalcViewAndTransform();

	this->projector = d3d10::scene->GetMiscData().srvBlackTexture;
}

D3D10SpotLight::~D3D10SpotLight()
{}

void D3D10SpotLight::RecalcViewAndTransform() const
{
	Assert( this->data.IsOk() );

	// Rebuild our world transform.
	{
		FLOAT halfAngle = 0.5f * this->coneAngle;

		FLOAT bottomRadius = GetRange() * mxMath::Tan( halfAngle );

#ifdef RX_D3D10_SCALE_BIAS
		// add a small bias to hide light shape geometry artifacts
		bottomRadius *= 1.1f;
#endif

		// untransformed spot light cone points along -Y
		this->worldTransform =
			Matrix4( 
			bottomRadius,		0,					0,					0,
			0,					GetRange(),			0,					0,
			0,					0,					bottomRadius,		0,
			0,					-0.5f * GetRange(),	0,					1
			)
				* Matrix3::CreateRotation( -Vec3D::UNIT_Y, GetDirection() ).ToMat4()
						* Matrix4::CreateTranslation( GetOrigin() );
	}

	// Rebuild our view.
	{
		this->view.Position = this->GetOrigin();
		this->view.LookAt = this->GetDirection();

		Vec3D  from( 0, 0, 1 );
		Vec3D  to( this->GetDirection() );
		Matrix3 rot( Matrix3::CreateRotation( from, to ) );
		this->view.ViewMatrix = Matrix4( rot, this->GetOrigin() ).Inverse();

		FLOAT fovY = this->coneAngle;
		FLOAT aspect = 1.0f;
		FLOAT nearZ = 0.1f;
		FLOAT farZ = this->GetRange();
		this->view.ProjMatrix.BuildProjection( fovY, aspect, nearZ, farZ );

		this->view.RecalculateViewFrustum();
	}
}

bool D3D10SpotLight::ContainsPoint( const Vec3D& point ) const
{
	MX_OPTIMIZE( "make sure these are NOT virtual calls")

	Vec3D  origin = this->GetOrigin();	// cone origin

	Vec3D  dir = this->GetDirection();	// direction of cone axis
	Assert( dir.IsNormalized() );

	FLOAT range = this->GetRange();

	FLOAT t = dir * ( point - origin );

	if ( t < 0 || t > range ) {
		return false;
	}

	FLOAT halfAngle = 0.5f * this->coneAngle;
	FLOAT bottomRadius = this->GetRange() * mxMath::Tan( halfAngle );

	FLOAT r = t * ( bottomRadius / range );	// cone radius at closest point

	// squared distance from the point to the cone axis
	FLOAT distSq = (( origin + dir * t ) - point).LengthSqr();

	return ( distSq < Square(r) );
}

void D3D10SpotLight::SetInnerConeAngle( FLOAT theta )
{
	Assert2( 0.0f < theta && theta < mxMath::HALF_PI, "spotlight inner angle must be in [0..PI/2]" );
	
	this->data.cosTheta = mxMath::Cos( theta * 0.5f );
	Assert( this->data.IsOk() );
}

void D3D10SpotLight::SetOuterConeAngle( FLOAT phi )
{
	Assert2( 0.0f < phi && phi < mxMath::HALF_PI, "spotlight outer angle must be in [0..PI/2]" );
	
	this->data.cosPhi = mxMath::Cos( phi * 0.5f );
	Assert( this->data.IsOk() );

	this->coneAngle = phi;
	RecalcViewAndTransform();
}

const Vec3D & D3D10SpotLight::GetDirection() const
{
	return this->data.direction;
}

void D3D10SpotLight::SetDirection( const Vec3D& newDir )
{
	Assert( newDir.IsNormalized() );
	this->data.direction = newDir;
	RecalcViewAndTransform();
}

const Vec3D & D3D10SpotLight::GetOrigin() const
{
	return this->data.position;
}

void D3D10SpotLight::SetOrigin( const Vec3D& newOrigin )
{
	this->data.position = newOrigin;
	RecalcViewAndTransform();
}

FLOAT D3D10SpotLight::GetRange() const
{
	return this->data.range;
}

void D3D10SpotLight::SetRange( FLOAT newRange )
{
	this->data.range = newRange;
	RecalcViewAndTransform();
}

void D3D10SpotLight::SetColor( const ColorRGB& newDiffuseColor )
{
	this->data.color = newDiffuseColor;
}

void D3D10SpotLight::Remove()
{
	Unimplemented;
}

void D3D10SpotLight::SetProjector( rxTexture* texture )
{	AssertPtr( texture );
	if( texture != null ) {
		this->projector = (checked_cast< D3D10Texture*, rxTexture* >( texture ))->SRV;
	} else {
		this->projector = d3d10::scene->GetMiscData().srvBlackTexture;
	}
}

void D3D10SpotLight::Render( const rxView& view, rxQueue& queue )
{
	if( this->IsEnabled() ) {
		ToD3D10Queue( queue ).localLights.Add( this );
	}
}

void D3D10SpotLight::RenderLocalLight( const D3D10ViewConstants& view )
{
	// transform for drawing light shape
	Matrix4  wvp( GetWorldTransform() * view.ViewProjMatrix );

	D3D10LightShape_Cylinder & lightShape = d3d10::scene->GetLightStage().GetData().spotLightShape;

	if ( this->flags & LF_SHADOWS )
	{
		Matrix4 lightVP = this->view.ViewMatrix * this->view.ProjMatrix;

		UINT smResolution = DEFAULT_SHADOWMAP_RESOLUTION;
		D3D10DepthMap & shadowMap = d3d10::scene->GetLightStage().BuildShadowMap( this->view, smResolution );

		// reverse backface cull mode when the eye is inside the light volume
		if ( this->ContainsPoint( view.EyePosition ) ) {
			d3d10::device->OMSetDepthStencilState( d3d10::scene->GetLightStage().GetData().dsState_CullFrontFaces, 0 );
			d3d10::device->RSSetState( d3d10::scene->GetLightStage().GetData().rsState_CullFrontFaces );
		} else {
			d3d10::device->OMSetDepthStencilState( d3d10::scene->GetLightStage().GetData().dsState_CullBackFaces, 0 );
			d3d10::device->RSSetState( d3d10::scene->GetLightStage().GetData().rsState_CullBackFaces );
		}

		D3D10Shader_SpotLight_Shadowed & shader = d3d10::scene->GetLightStage().GetData().fxSpotLight_Shadowed;

		shader.Bind( wvp, lightVP, this->data, this->projector, shadowMap.pSRV );
		lightShape.Render( shader );
		shader.Unbind();
	}
	else
	{
		// reverse backface cull mode when the eye is inside the light volume
		if ( this->ContainsPoint( view.EyePosition ) ) {
			d3d10::device->OMSetDepthStencilState( d3d10::scene->GetLightStage().GetData().dsState_CullFrontFaces, 0 );
			d3d10::device->RSSetState( d3d10::scene->GetLightStage().GetData().rsState_CullFrontFaces );
		} else {
			d3d10::device->OMSetDepthStencilState( d3d10::scene->GetLightStage().GetData().dsState_CullBackFaces, 0 );
			d3d10::device->RSSetState( d3d10::scene->GetLightStage().GetData().rsState_CullBackFaces );
		}

		D3D10Shader_SpotLight & shader = d3d10::scene->GetLightStage().GetData().fxSpotLight;
		shader.Bind( wvp, this->data );
		lightShape.Render( shader );
		shader.Unbind();
	}
}

/*================================
		D3D10LightStageData
================================*/

D3D10LightStageData::D3D10LightStageData()
{}

D3D10LightStageData::~D3D10LightStageData()
{}

void D3D10LightStageData::Initialize()
{
	// Initialize render target for shadow mapping first
	// so that they are placed in the best (fastest) portion of VRAM.
	this->shadowMap.Initialize( DEFAULT_SHADOWMAP_RESOLUTION );

	// Load shaders.
	{
		this->fxAmbientLight		.Load( PATH_TO_SHADERS_"AmbientLight.fx" );
		this->fxDirectionalLight	.Load( PATH_TO_SHADERS_"Light_Directional.fx" );
		this->fxPointLight			.Load( PATH_TO_SHADERS_"Light_Point.fx" );
		this->fxSpotLight			.Load( PATH_TO_SHADERS_"Light_Spot.fx" );

		this->fxBuildShadowMap		.Load( PATH_TO_SHADERS_"BuildShadowMap.fx" );

		this->fxSpotLight_Shadowed	.Load( PATH_TO_SHADERS_"Light_Spot_Shadowed.fx" );
	}

	// Create vertex declarations for drawing light shapes.
	{
		D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ TEXT("POSITION"),   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};

		D3D10_PASS_DESC passDesc;
		this->fxPointLight.GetActivePass()->GetDesc( &passDesc );

		check( d3d10::device->CreateInputLayout(
			vertexDesc, ARRAY_SIZE( vertexDesc ),
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,
			& this->lightShapeVertexLayout.Ptr )
		);
	}

	// Build light shapes.
	{
		this->pointLightShape.Build( 1.0f, 8, 8 );
		this->spotLightShape.Build( 0.001f, 1.0f, 1.0f, 16, 16 );
	}

	// Create render states.
	{
		D3D10_RASTERIZER_DESC rasterizerState;
		rasterizerState.FillMode				= D3D10_FILL_MODE::D3D10_FILL_SOLID;
		rasterizerState.CullMode				= D3D10_CULL_MODE::D3D10_CULL_BACK;
		rasterizerState.FrontCounterClockwise	= false;
		rasterizerState.DepthBias				= false;
		rasterizerState.DepthBiasClamp			= 0;
		rasterizerState.SlopeScaledDepthBias	= 0;
		rasterizerState.DepthClipEnable			= true;
		rasterizerState.ScissorEnable			= false;
		rasterizerState.MultisampleEnable		= false;
		rasterizerState.AntialiasedLineEnable	= false;


		D3D10_DEPTH_STENCIL_DESC depthStencilState;
		depthStencilState.DepthEnable			= true;
		depthStencilState.DepthWriteMask		= D3D10_DEPTH_WRITE_MASK::D3D10_DEPTH_WRITE_MASK_ZERO;
		depthStencilState.DepthFunc				= D3D10_COMPARISON_FUNC::D3D10_COMPARISON_LESS_EQUAL;
		depthStencilState.StencilEnable			= true;
		depthStencilState.StencilReadMask		= 0;
		depthStencilState.StencilWriteMask		= 0;
		// Stencil operations if pixel is front-facing
		depthStencilState.FrontFace.StencilFailOp		= D3D10_STENCIL_OP_KEEP;
		depthStencilState.FrontFace.StencilDepthFailOp	= D3D10_STENCIL_OP_KEEP;
		depthStencilState.FrontFace.StencilPassOp		= D3D10_STENCIL_OP_KEEP;
		depthStencilState.FrontFace.StencilFunc			= D3D10_COMPARISON_NEVER;
		// Stencil operations if pixel is back-facing
		depthStencilState.BackFace.StencilFailOp		= D3D10_STENCIL_OP_KEEP;
		depthStencilState.BackFace.StencilDepthFailOp	= D3D10_STENCIL_OP_KEEP;
		depthStencilState.BackFace.StencilPassOp		= D3D10_STENCIL_OP_KEEP;
		depthStencilState.BackFace.StencilFunc			= D3D10_COMPARISON_NEVER;



		rasterizerState.CullMode				= D3D10_CULL_MODE::D3D10_CULL_FRONT;
		check( d3d10::device->CreateRasterizerState( &rasterizerState, & this->rsState_CullFrontFaces.Ptr ) );
		depthStencilState.DepthFunc			= D3D10_COMPARISON_FUNC::D3D10_COMPARISON_GREATER;	// to reject 'floating-in-the-air' portions of light
		check( d3d10::device->CreateDepthStencilState( &depthStencilState, & this->dsState_CullFrontFaces.Ptr ) );


		rasterizerState.CullMode				= D3D10_CULL_MODE::D3D10_CULL_BACK;
		check( d3d10::device->CreateRasterizerState( &rasterizerState, & this->rsState_CullBackFaces.Ptr ) );
		depthStencilState.DepthFunc			= D3D10_COMPARISON_FUNC::D3D10_COMPARISON_LESS;	// to reject 'buried undeground' light regions
		check( d3d10::device->CreateDepthStencilState( &depthStencilState, & this->dsState_CullBackFaces.Ptr ) );
	}

	this->tempVisibleSet.Empty();
	this->tempRenderQueue.Clear();
}

void D3D10LightStageData::Shutdown()
{
	this->shadowMap.Shutdown();

	this->fxAmbientLight		.Unload();
	this->fxDirectionalLight	.Unload();
	this->fxPointLight			.Unload();
	this->fxSpotLight			.Unload();

	this->fxBuildShadowMap		.Unload();

	this->fxSpotLight_Shadowed	.Unload();


	this->lightShapeVertexLayout = null;
	this->pointLightShape.Clear();
	this->spotLightShape.Clear();


	this->rsState_CullFrontFaces = null;
	this->rsState_CullBackFaces = null;


	this->tempVisibleSet.Empty();
	this->tempRenderQueue.Clear();
}

/*================================
		D3D10LightStage
================================*/

static const FColor	DEFAULT_AMBIENT_COLOR( 0.1f, 0.1f, 0.1f, 1.0f );

D3D10LightStage::D3D10LightStage()
	: ambientColor( DEFAULT_AMBIENT_COLOR )
{}

D3D10LightStage::~D3D10LightStage()
{}

void D3D10LightStage::Initialize()
{
	this->data.Initialize();

	this->ambientColor = DEFAULT_AMBIENT_COLOR;
}

void D3D10LightStage::Shutdown()
{
	this->data.Shutdown();

	this->ambientColor = DEFAULT_AMBIENT_COLOR;

	this->spawnedLights.DeleteContents( true );
}

UINT D3D10LightStage::NumCreatedLights() const
{
	return this->spawnedLights.Num();
}

void D3D10LightStage::Render( const D3D10ViewConstants& view, D3D10RenderQueue& renderQueue )
{
	D3D10RenderStage::PrepareRender();

	// First render those lights that affect the scene globally ( ambient, directional, etc ).

	// ambient lighting pass
	{
		this->data.fxAmbientLight.Bind( ambientColor );
		d3d10::scene->GetFullscreenQuad().Render( this->data.fxAmbientLight );
		this->data.fxAmbientLight.Unbind();
	}

	// Render global lights.
	{
		for ( IndexT iLight = 0; iLight < renderQueue.globalLights.Num(); iLight++ )
		{
			D3D10GlobalLight * pLight = renderQueue.globalLights[ iLight ];
			
			pLight->RenderGlobalLight();
		}
	}

	// Render local lights.
	{
		d3d10::device->IASetInputLayout( this->data.lightShapeVertexLayout );
		for ( IndexT iLight = 0; iLight < renderQueue.localLights.Num(); iLight++ )
		{
			D3D10LocalLight * pLight = renderQueue.localLights[ iLight ];

			pLight->RenderLocalLight( view );
		}
	}
}

D3D10DepthMap & D3D10LightStage::BuildShadowMap( const mxSceneView& lightView, UINT resolution )
{
	D3D10_STAT_COUNTER( d3d10::stats.shadowMapRenderTime );

	D3D10View  fullLightView;
	d3d10::scene->BuildRenderQueue( lightView,
						fullLightView, this->data.tempVisibleSet, this->data.tempRenderQueue );

	this->data.tempVisibleSet.Empty();

	this->data.shadowMap.Bind( resolution );
/**/
	// Render shadow-casting geometry.

	DXPtr< ID3D10InputLayout > pOldIL;
	d3d10::device->IAGetInputLayout( & pOldIL.Ptr );

	MX_OPTIMIZE("use separate vertex stream (positions only) to render depth shadows, don't use full vertex format")
	d3d10::device->IASetInputLayout( d3d10::scene->GetGeometryStage().geometryVertexLayout );

	for( IndexT iModel = 0; iModel < this->data.tempRenderQueue.models.Num(); iModel++ )
	{
		D3D10Model * model = this->data.tempRenderQueue.models[ iModel ];

		Matrix4 lightWVP( model->worldTransform * fullLightView.constants.ViewProjMatrix );
		this->data.fxBuildShadowMap.Bind( lightWVP );
		this->data.fxBuildShadowMap.Apply();
		
		model->RenderShadowMap();
	}
	this->data.tempRenderQueue.Clear();

	d3d10::device->IASetInputLayout( pOldIL );

	this->data.shadowMap.Unbind();
	return this->data.shadowMap;
}

rxParallelLight * D3D10LightStage::CreateParallelLight( const Vec3D& direction,
								const rxLightDescription& desc )
{
	Assert( direction.IsNormalized() && desc.IsOk() );

	D3D10DirectionalLight * pNewLight = MX_NEW D3D10DirectionalLight( desc, direction );

	this->spawnedLights.Append( pNewLight );
	return pNewLight;
}

rxPointLight * D3D10LightStage::CreatePointLight( const Vec3D& center, FLOAT radius,
							const rxLightDescription& desc )
{
	Assert( (radius > 0.0f) && desc.IsOk() );

	D3D10PointLight * pNewLight = MX_NEW D3D10PointLight( desc, center, radius );

	this->spawnedLights.Append( pNewLight );
	return pNewLight;
}

rxSpotLight * D3D10LightStage::CreateSpotLight( const Vec3D& center, const Vec3D& direction, FLOAT range,
							FLOAT innerAngle, FLOAT outerAngle,
							const rxLightDescription& desc )
{
	D3D10SpotLight * pNewLight = MX_NEW D3D10SpotLight( desc, center, direction, range, innerAngle, outerAngle );

	this->spawnedLights.Append( pNewLight );
	return pNewLight;
}

/*================================
		D3D10AlphaStage
================================*/

D3D10AlphaStage::D3D10AlphaStage()
{}

D3D10AlphaStage::~D3D10AlphaStage()
{}

void D3D10AlphaStage::Initialize()
{
}

void D3D10AlphaStage::Shutdown()
{
}

void D3D10AlphaStage::Render( const D3D10ViewConstants& view, D3D10RenderQueue& renderQueue )
{
	D3D10RenderStage::PrepareRender();

	// Render billboards.
	{
		Matrix4  invView( view.ViewMatrix.Inverse() );

		for( IndexT iObj = 0; iObj < renderQueue.translucent.Num(); iObj++ )
		{
			D3D10Billboard * pObj = renderQueue.translucent[ iObj ];
			
			// Construct billboard matrix.
			Matrix4  wvp;

			MX_OPTIMIZE(:)

			wvp.SetIdentity();
			wvp *= Matrix4::CreateScale( Vec3D( pObj->size, 1.0f ) );
			wvp *= invView;
			wvp.SetTranslation( pObj->origin );
			wvp *= view.ViewProjMatrix;

			D3D10ParticleSystemData & data = d3d10::scene->GetMiscData().particleSysData;
			D3D10Shader_Billboard & shader = data.fxBillboard;
			
			shader.Bind( wvp, pObj->color, pObj->texture->SRV );

			d3d10::device->IASetInputLayout( data.pIL );
			d3d10::device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

			UINT strides = sizeof( D3D10ParticleSystemData::BillboardVertex );
			UINT offsets = 0;
			ID3D10Buffer * pBuffers[1] = { data.pVB };
			d3d10::device->IASetVertexBuffers( 0, 1, pBuffers, &strides, &offsets );

			shader.Apply();
			d3d10::device->Draw( 4, 0 );

			shader.Unbind();

			d3d10::stats.numBatches++;
			d3d10::stats.numTriangles += 2;
		}
	}
}

/*================================
		D3D10Billboard
================================*/

D3D10Billboard::D3D10Billboard(
		rxTexture* texture, const Vec3D& origin,
		const Vec2D& size, const ColorRGB& color
	)
{
	this->texture = checked_cast< D3D10Texture*, rxTexture* >( texture );
	this->origin = origin;
	this->size = size;
	this->color = color;
}

D3D10Billboard::~D3D10Billboard()
{}

void D3D10Billboard::SetColor( const ColorRGB& newColor )
{
	this->color = newColor;
}

void D3D10Billboard::SetTexture( rxTexture* newTexture )
{
	this->texture = checked_cast< D3D10Texture*, rxTexture* >( newTexture );
}

void D3D10Billboard::SetOrigin( const Vec3D& newOrigin )
{
	this->origin = newOrigin;
}

void D3D10Billboard::SetSize( const Vec2D& newSize )
{
	this->size = newSize;
}

void D3D10Billboard::Render( const rxView& view, rxQueue& queue )
{
	ToD3D10Queue( queue ).translucent.Add( this );
}

void D3D10Billboard::Remove()
{
	Unimplemented;
}

/*================================
			D3D10Skybox
================================*/

void D3D10Sky::Render( const rxView& view, rxQueue& queue )
{
	ToD3D10Queue( queue ).skies.Add( this );
}

void D3D10Sky::Remove()
{
	d3d10::scene->RemoveSky( this );
}

/*================================
			D3D10Skybox
================================*/

D3D10Skybox::D3D10Skybox( rxTexture* sides[AABB::NUM_SIDES] )
	: orientation( _InitIdentity )
{
	for( UINT iSide = 0; iSide < AABB::NUM_SIDES; iSide++ )
	{
		this->textures[iSide] = checked_cast< D3D10Texture*, rxTexture* >( sides[iSide] );
	}
}

D3D10Skybox::~D3D10Skybox()
{}

void D3D10Skybox::SetOrientation( const Quat& newOrientation )
{
	this->orientation = newOrientation;
}

void D3D10Skybox::SetTextures( rxTexture* sides[AABB::NUM_SIDES] )
{
	for( UINT iSide = 0; iSide < AABB::NUM_SIDES; iSide++ )
	{
		this->textures[iSide] = checked_cast< D3D10Texture*, rxTexture* >( sides[iSide] );
	}
}

void D3D10Skybox::RenderSky( const D3D10ViewConstants& view )
{
	D3D10SkyData & data = d3d10::scene->GetMiscData().skyData;
	D3D10Shader_Skybox & shader = data.fxSkybox;

	// Set correct world transform.
	{
		Matrix4  skyTransform;
		skyTransform = this->orientation.ToMat4() * view.ViewMatrix;
		skyTransform.SetTranslation(Vec3D(0));
		skyTransform = skyTransform * view.ProjectionMatrix;

		shader.SetWorldViewProj( skyTransform );
	}

	// Draw each side of the box.

	UINT strides = sizeof( D3D10SkyData::SkyBoxVertex );
	UINT offsets = 0;
	ID3D10Buffer * pBuffers[1] = { data.pVB };

	d3d10::device->IASetInputLayout( data.pIL );
	d3d10::device->IASetVertexBuffers( 0, 1, pBuffers, &strides, &offsets );
	d3d10::device->IASetIndexBuffer( data.pIB, DXGI_FORMAT_R16_UINT, 0 );
	d3d10::device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST  );

	for( UINT iSide = 0; iSide < AABB::NUM_SIDES; iSide++ )
	{
		shader.SetTexture( this->textures[ iSide ]->SRV );
		shader.Apply();
		d3d10::device->DrawIndexed( 6, iSide * 6, 0 );
	}

	shader.Unbind();
}

/*================================
		D3D10Portal
================================*/

D3D10Portal::D3D10Portal( FLOAT width, FLOAT height )
{
	Assert( IsInRange( width, 0.0f, MAX_PORTAL_SIZE ) && IsInRange( height, 0.0f, MAX_PORTAL_SIZE ) );
	
	this->size.x = Clamp( width, 1.0f, MAX_PORTAL_SIZE );
	this->size.y = Clamp( height, 1.0f, MAX_PORTAL_SIZE );
}

D3D10Portal::~D3D10Portal()
{}

void D3D10Portal::SetLookAt( const Vec3D& lookAt )
{
	Assert( lookAt.IsNormalized() );
	this->view.LookAt = lookAt;
}

void D3D10Portal::SetWorldTransform( const Matrix4& localToWorld )
{
	this->worldTransform = localToWorld;
}

void D3D10Portal::SetSize( FLOAT width, FLOAT height )
{
	Assert( IsInRange( width, 0.0f, MAX_PORTAL_SIZE ) && IsInRange( height, 0.0f, MAX_PORTAL_SIZE ) );
	this->size.x = Clamp( width, 1.0f, MAX_PORTAL_SIZE );
	this->size.y = Clamp( height, 1.0f, MAX_PORTAL_SIZE );
}

void D3D10Portal::SetBuddy( rxPortal* other )
{
	Unimplemented;
//	this->twin = other;
}

rxPortal * D3D10Portal::GetBuddy()
{
	return this->twin;
}

void D3D10Portal::Render( const rxView& view, rxQueue& queue )
{
	ToD3D10Queue( queue ).portals.Add( this );
}

void D3D10Portal::Remove()
{
	Unimplemented
}

void D3D10Portal::DrawPortalQuadOnly()
{
	Unimplemented
}

/*================================
		D3D10PostStage
================================*/

D3D10PostStage::D3D10PostStage()
	: bEnabled( True )
{}

D3D10PostStage::~D3D10PostStage()
{}

void D3D10PostStage::Initialize( UINT screenWidth, UINT screenHeight )
{
	this->bEnabled = True;
	
	this->accumRT.Initialize( screenWidth, screenHeight );

	this->postEffects.DeleteContents( true );

	this->fxCombine				.Load(PATH_TO_SHADERS_"PostFx_Combine.fx" );

//	this->fxColoring			.Load(PATH_TO_SHADERS_"PostFx_Coloring.fx" );
	this->fxGrayscale			.Load(PATH_TO_SHADERS_"PostFx_Grayscale.fx");
	this->fxBlackAndWhite		.Load(PATH_TO_SHADERS_"PostFx_BlackAndWhite.fx");
	this->fxInvertColor			.Load(PATH_TO_SHADERS_"PostFx_InvertColor.fx");
	this->fxEmboss				.Load(PATH_TO_SHADERS_"PostFx_Emboss.fx");
	this->fxSharpen				.Load(PATH_TO_SHADERS_"PostFx_Sharpen.fx");
	this->fxSepia				.Load(PATH_TO_SHADERS_"PostFx_Sepia.fx");
//	this->fxCellShading			.Load(PATH_TO_SHADERS_"PostFx_CellShading.fx");
	this->fxModulatingWave		.Load(PATH_TO_SHADERS_"PostFx_ModulatingWave.fx");
	this->fxBlur				.Load(PATH_TO_SHADERS_"PostFx_Blur.fx");
	this->fxGBlur				.Load(PATH_TO_SHADERS_"PostFx_GBlur.fx");
	this->fxBloom				.Load(PATH_TO_SHADERS_"PostFx_Bloom.fx");
//	this->fxDoF					.Load(PATH_TO_SHADERS_"PostFx_DoF.fx");
//	this->fxMotionBlur			.Load(PATH_TO_SHADERS_"PostFx_MotionBlur.fx");
//	this->fxRadialBlur			.Load(PATH_TO_SHADERS_"PostFx_RadialBlur.fx");
//	this->fxHDR					.Load(PATH_TO_SHADERS_"PostFx_HDR.fx");
//	this->fxSSAO				.Load(PATH_TO_SHADERS_"PostFx_SSAO.fx");
}

void D3D10PostStage::Shutdown()
{
	this->bEnabled = False;
	
	this->accumRT.Shutdown();
	
	this->postEffects.DeleteContents( true );
	
	
	this->fxCombine				.Unload();

//	this->fxColoring			.Unload();
	this->fxGrayscale			.Unload();
	this->fxBlackAndWhite		.Unload();
	this->fxInvertColor			.Unload();
	this->fxEmboss				.Unload();
	this->fxSharpen				.Unload();
	this->fxSepia				.Unload();
//	this->fxCellShading			.Unload();
	this->fxModulatingWave		.Unload();
	this->fxBlur				.Unload();
	this->fxGBlur				.Unload();
	this->fxBloom				.Unload();
//	this->fxDoF					.Unload();
//	this->fxMotionBlur			.Unload();
//	this->fxRadialBlur			.Unload();
//	this->fxHDR					.Unload();
//	this->fxSSAO				.Unload();
}

void D3D10PostStage::Render()
{
	if( !this->bEnabled || !NumPostEffects() ) {
		return;
	}

	D3D10RenderStage::PrepareRender();

	// Render target ping-ponging...

	ID3D10ShaderResourceView *	pMainSRV = d3d10::renderer->GetBackbufferSRV();
	ID3D10RenderTargetView *	pMainRTV = d3d10::renderer->GetBackbufferRTV();

	ID3D10ShaderResourceView *	pSrcTexture = pMainSRV;
	ID3D10RenderTargetView *	pSrcRTV = pMainRTV;

	ID3D10ShaderResourceView *	pDestTexture = this->accumRT.pSRV;
	ID3D10RenderTargetView *	pDestRTV = this->accumRT.pRTV;
	
	MX_OPTIMIZE(:)
	Swap( pSrcRTV, pDestRTV );
	Swap( pSrcTexture, pDestTexture );

	for( IndexT iPostFx = 0; iPostFx < this->postEffects.Num(); iPostFx++ )
	{
		D3D10PostEffect * pFX = this->postEffects[ iPostFx ];

		if( ! pFX->IsEnabled() ) {
			continue;
		}

		// Swap src & dest render targets.
		Swap( pSrcRTV, pDestRTV );
		Swap( pSrcTexture, pDestTexture );

		// Set destination render target.

		d3d10::device->OMSetRenderTargets(
			1,
			& pDestRTV,
			null
		);

		pFX->Render( this, pSrcTexture );
	}

	if( pDestTexture != pMainSRV )
	{
		d3d10::renderer->RestoreMainRenderTarget();

		fxCombine.Bind( pDestTexture );
		d3d10::scene->GetFullscreenQuad().Render( fxCombine );
		fxCombine.Unbind();
	}
}

UINT D3D10PostStage::NumPostEffects() const
{
	return this->postEffects.Num();
}

void D3D10PostStage::ToggleAllEffects( bool bEnable )
{
	this->bEnabled = bEnable;
}

rxPostEffect * D3D10PostStage::AddColoring()
{
	Unimplemented;
	return null;
}
rxPostEffect * D3D10PostStage::AddGrayScale()
{
	D3D10PostEffect_Grayscale * newPostFx = MX_NEW D3D10PostEffect_Grayscale();
	this->postEffects.Append( newPostFx );
	return newPostFx;
}

rxPostEffect * D3D10PostStage::AddBlackAndWhite()
{
	D3D10PostEffect_BlackAndWhite * newPostFx = MX_NEW D3D10PostEffect_BlackAndWhite();
	this->postEffects.Append( newPostFx );
	return newPostFx;
}

rxPostEffect * D3D10PostStage::AddInvertColor()
{
	D3D10PostEffect_InvertColor * newPostFx = MX_NEW D3D10PostEffect_InvertColor();
	this->postEffects.Append( newPostFx );
	return newPostFx;
}

rxPostEffect * D3D10PostStage::AddEmboss()
{
	D3D10PostEffect_Emboss * newPostFx = MX_NEW D3D10PostEffect_Emboss();
	this->postEffects.Append( newPostFx );
	return newPostFx;
}

rxPostEffect * D3D10PostStage::AddSharpen()
{
	D3D10PostEffect_Sharpen * newPostFx = MX_NEW D3D10PostEffect_Sharpen();
	this->postEffects.Append( newPostFx );
	return newPostFx;
}

rxPostEffect * D3D10PostStage::AddSepia()
{
	D3D10PostEffect_Sepia * newPostFx = MX_NEW D3D10PostEffect_Sepia();
	this->postEffects.Append( newPostFx );
	return newPostFx;
}

rxPostEffect * D3D10PostStage::AddCellShading()
{
	Unimplemented;
	return null;
}

rxPostEffect * D3D10PostStage::AddModulatingWave()
{
	D3D10PostEffect_ModulatingWave * newPostFx = MX_NEW D3D10PostEffect_ModulatingWave();
	this->postEffects.Append( newPostFx );
	return newPostFx;
}

rxPostEffect * D3D10PostStage::AddBlur()
{
	D3D10PostEffect_Blur * newPostFx = MX_NEW D3D10PostEffect_Blur();
	this->postEffects.Append( newPostFx );
	return newPostFx;
}

rxPostEffect * D3D10PostStage::AddGBlur()
{
	D3D10PostEffect_GBlur * newPostFx = MX_NEW D3D10PostEffect_GBlur();
	this->postEffects.Append( newPostFx );
	return newPostFx;
}

rxPostEffect* D3D10PostStage::AddBloom()
{
	D3D10PostEffect_Bloom * newPostFx = MX_NEW D3D10PostEffect_Bloom();
	this->postEffects.Append( newPostFx );
	return newPostFx;
}

rxPostEffect * D3D10PostStage::AddDepthOfField()
{
	Unimplemented;
	return null;
}

rxPostEffect * D3D10PostStage::AddMotionBlur()
{
	Unimplemented;
	return null;
}
rxPostEffect * D3D10PostStage::AddRadialBlur()
{
	Unimplemented;
	return null;
}

rxPostEffect * D3D10PostStage::AddHDR()
{
	Unimplemented;
	return null;
}
rxPostEffect * D3D10PostStage::AddSSAO()
{
	Unimplemented;
	return null;
}

/*================================
	D3D10PostEffect
================================*/

void D3D10PostEffect::Remove()
{
	if( d3d10::scene->GetPostStage().postEffects.Remove( this ) )
	{
		delete this;
	}
}

/*================================
	D3D10PostEffect_Grayscale
================================*/

D3D10PostEffect_Grayscale::D3D10PostEffect_Grayscale()
	: D3D10PostEffect( EPostFx::PostFx_GrayScale )
{}

D3D10PostEffect_Grayscale::~D3D10PostEffect_Grayscale()
{}

void D3D10PostEffect_Grayscale::Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	postStage->fxGrayscale.Bind( pSourceTexture );
	d3d10::scene->GetFullscreenQuad().Render( postStage->fxGrayscale );
	postStage->fxGrayscale.Unbind();
}

/*================================
	D3D10PostEffect_BlackAndWhite
================================*/

D3D10PostEffect_BlackAndWhite::D3D10PostEffect_BlackAndWhite()
	: D3D10PostEffect( EPostFx::PostFx_BlackAndWhite )
{}

D3D10PostEffect_BlackAndWhite::~D3D10PostEffect_BlackAndWhite()
{}

void D3D10PostEffect_BlackAndWhite::Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	postStage->fxBlackAndWhite.Bind( pSourceTexture );
	d3d10::scene->GetFullscreenQuad().Render( postStage->fxBlackAndWhite );
	postStage->fxBlackAndWhite.Unbind();
}

/*================================
	D3D10PostEffect_InvertColor
================================*/

D3D10PostEffect_InvertColor::D3D10PostEffect_InvertColor()
	: D3D10PostEffect( EPostFx::PostFx_InvertColor )
{}

D3D10PostEffect_InvertColor::~D3D10PostEffect_InvertColor()
{}

void D3D10PostEffect_InvertColor::Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	postStage->fxInvertColor.Bind( pSourceTexture );
	d3d10::scene->GetFullscreenQuad().Render( postStage->fxInvertColor );
	postStage->fxInvertColor.Unbind();
}

/*================================
	D3D10PostEffect_Emboss
================================*/

D3D10PostEffect_Emboss::D3D10PostEffect_Emboss()
	: D3D10PostEffect( EPostFx::PostFx_Emboss )
{}

D3D10PostEffect_Emboss::~D3D10PostEffect_Emboss()
{}

void D3D10PostEffect_Emboss::Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	postStage->fxEmboss.Bind( pSourceTexture );
	d3d10::scene->GetFullscreenQuad().Render( postStage->fxEmboss );
	postStage->fxEmboss.Unbind();
}

/*================================
	D3D10PostEffect_Sharpen
================================*/

D3D10PostEffect_Sharpen::D3D10PostEffect_Sharpen()
	: D3D10PostEffect( EPostFx::PostFx_Sharpen )
{}

D3D10PostEffect_Sharpen::~D3D10PostEffect_Sharpen()
{}

void D3D10PostEffect_Sharpen::Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	postStage->fxSharpen.Bind( pSourceTexture );
	d3d10::scene->GetFullscreenQuad().Render( postStage->fxSharpen );
	postStage->fxSharpen.Unbind();
}

/*================================
	D3D10PostEffect_Sepia
================================*/

D3D10PostEffect_Sepia::D3D10PostEffect_Sepia()
	: D3D10PostEffect( EPostFx::PostFx_Sepia )
{}

D3D10PostEffect_Sepia::~D3D10PostEffect_Sepia()
{}

void D3D10PostEffect_Sepia::Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	postStage->fxSepia.Bind( pSourceTexture );
	d3d10::scene->GetFullscreenQuad().Render( postStage->fxSepia );
	postStage->fxSepia.Unbind();
}

/*================================
	D3D10PostEffect_ModulatingWave
================================*/

D3D10PostEffect_ModulatingWave::D3D10PostEffect_ModulatingWave()
	: D3D10PostEffect( EPostFx::PostFx_ModulatingWave )
{}

D3D10PostEffect_ModulatingWave::~D3D10PostEffect_ModulatingWave()
{}

void D3D10PostEffect_ModulatingWave::Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	postStage->fxModulatingWave.Bind( pSourceTexture );
	d3d10::scene->GetFullscreenQuad().Render( postStage->fxModulatingWave );
	postStage->fxModulatingWave.Unbind();
}

/*================================
	D3D10PostEffect_Blur
================================*/

D3D10PostEffect_Blur::D3D10PostEffect_Blur()
	: D3D10PostEffect( EPostFx::PostFx_Blur )
{}

D3D10PostEffect_Blur::~D3D10PostEffect_Blur()
{}

void D3D10PostEffect_Blur::Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	postStage->fxBlur.Bind( pSourceTexture );
	d3d10::scene->GetFullscreenQuad().Render( postStage->fxBlur );
	postStage->fxBlur.Unbind();
}

/*================================
	D3D10PostEffect_GBlur
================================*/

D3D10PostEffect_GBlur::D3D10PostEffect_GBlur()
	: D3D10PostEffect( EPostFx::PostFx_GBlur )
{}

D3D10PostEffect_GBlur::~D3D10PostEffect_GBlur()
{}

void D3D10PostEffect_GBlur::Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	postStage->fxGBlur.Bind( pSourceTexture );
	d3d10::scene->GetFullscreenQuad().Render( postStage->fxGBlur );
	postStage->fxGBlur.Unbind();
}

/*================================
		D3D10PostEffect_Bloom
================================*/

D3D10PostEffect_Bloom::D3D10PostEffect_Bloom()
	: D3D10PostEffect( EPostFx::PostFx_Bloom )
{}

D3D10PostEffect_Bloom::~D3D10PostEffect_Bloom()
{}

void D3D10PostEffect_Bloom::Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	postStage->fxBloom.Bind( pSourceTexture );
	d3d10::scene->GetFullscreenQuad().Render( postStage->fxBloom );
	postStage->fxBloom.Unbind();
}

/*================================
		D3D10SkyData
================================*/

D3D10SkyData::D3D10SkyData()
{}

D3D10SkyData::~D3D10SkyData()
{}

void D3D10SkyData::Initialize()
{
	this->fxSkybox.Load( PATH_TO_SHADERS_"Skybox.fx" );

	// Create input layout.
	{
		D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ TEXT("POSITION"), 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ TEXT("TEXCOORD"), 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};

		D3D10_PASS_DESC passDesc;
		this->fxSkybox.GetActivePass()->GetDesc( &passDesc );

		const void* pShaderBytecodeWithInputSignature = passDesc.pIAInputSignature;
		const SizeT bytecodeLength = passDesc.IAInputSignatureSize; 

		check( d3d10::device->CreateInputLayout(
			vertexDesc, ARRAY_SIZE( vertexDesc ),
			pShaderBytecodeWithInputSignature, bytecodeLength,
			& this->pIL.Ptr )
		);
	}

	enum { NUM_VERTICES = 24 };
	enum { NUM_INDICES = 36 };

	// Create vertex buffer.
    SkyBoxVertex vertices[ NUM_VERTICES ];

	// Fill in the front face vertex data.
	vertices[0] = SkyBoxVertex( Vec3D( -0.5f, -0.5f, -0.5f ),	Vec2D( 0.0f, 1.0f )	);
	vertices[1] = SkyBoxVertex( Vec3D( -0.5f,  0.5f, -0.5f ),	Vec2D( 0.0f, 0.0f )	);
	vertices[2] = SkyBoxVertex( Vec3D(  0.5f,  0.5f, -0.5f ),	Vec2D( 1.0f, 0.0f )	);
	vertices[3] = SkyBoxVertex( Vec3D(  0.5f, -0.5f, -0.5f ),	Vec2D( 1.0f, 1.0f )	);
																
	// Fill in the back face vertex data.						
	vertices[4] = SkyBoxVertex( Vec3D( -0.5f, -0.5f, 0.5f ), 	Vec2D( 1.0f, 1.0f )	);
	vertices[5] = SkyBoxVertex( Vec3D(  0.5f, -0.5f, 0.5f ), 	Vec2D( 0.0f, 1.0f )	);
	vertices[6] = SkyBoxVertex( Vec3D(  0.5f,  0.5f, 0.5f ), 	Vec2D( 0.0f, 0.0f )	);
	vertices[7] = SkyBoxVertex( Vec3D( -0.5f,  0.5f, 0.5f ), 	Vec2D( 1.0f, 0.0f )	);
																
	// Fill in the top face vertex data.						
	vertices[8]  = SkyBoxVertex( Vec3D( -0.5f, 0.5f, -0.5f ),	Vec2D( 0.0f, 1.0f )	);
	vertices[9]  = SkyBoxVertex( Vec3D( -0.5f, 0.5f,  0.5f ),	Vec2D( 0.0f, 0.0f )	);
	vertices[10] = SkyBoxVertex( Vec3D(  0.5f, 0.5f,  0.5f ),	Vec2D( 1.0f, 0.0f )	);
	vertices[11] = SkyBoxVertex( Vec3D(  0.5f, 0.5f, -0.5f ),	Vec2D( 1.0f, 1.0f )	);
																							
	// Fill in the bottom face vertex data.													
	vertices[12] = SkyBoxVertex( Vec3D( -0.5f, -0.5f, -0.5f ),	Vec2D( 1.0f, 1.0f )	);
	vertices[13] = SkyBoxVertex( Vec3D(  0.5f, -0.5f, -0.5f ),	Vec2D( 0.0f, 1.0f )	);
	vertices[14] = SkyBoxVertex( Vec3D(  0.5f, -0.5f,  0.5f ),	Vec2D( 0.0f, 0.0f )	);
	vertices[15] = SkyBoxVertex( Vec3D( -0.5f, -0.5f,  0.5f ),	Vec2D( 1.0f, 0.0f )	);
																
	// Fill in the left face vertex data.						
	vertices[16] = SkyBoxVertex( Vec3D( -0.5f, -0.5f,  0.5f ),	Vec2D( 0.0f, 1.0f )	);
	vertices[17] = SkyBoxVertex( Vec3D( -0.5f,  0.5f,  0.5f ),	Vec2D( 0.0f, 0.0f )	);
	vertices[18] = SkyBoxVertex( Vec3D( -0.5f,  0.5f, -0.5f ),	Vec2D( 1.0f, 0.0f )	);
	vertices[19] = SkyBoxVertex( Vec3D( -0.5f, -0.5f, -0.5f ),	Vec2D( 1.0f, 1.0f )	);
																
	// Fill in the right face vertex data.						
	vertices[20] = SkyBoxVertex( Vec3D(  0.5f, -0.5f, -0.5f ),	Vec2D( 0.0f, 1.0f )	);
	vertices[21] = SkyBoxVertex( Vec3D(  0.5f,  0.5f, -0.5f ),	Vec2D( 0.0f, 0.0f )	);
	vertices[22] = SkyBoxVertex( Vec3D(  0.5f,  0.5f,  0.5f ),	Vec2D( 1.0f, 0.0f )	);
	vertices[23] = SkyBoxVertex( Vec3D(  0.5f, -0.5f,  0.5f ),	Vec2D( 1.0f, 1.0f )	);


	// Create the index buffer
	SkyBoxIndexType indices[ NUM_INDICES ];

	// Fill in the front face index data
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;

	// Fill in the back face index data
	indices[6] = 4; indices[7]  = 5; indices[8]  = 6;
	indices[9] = 4; indices[10] = 6; indices[11] = 7;

	// Fill in the top face index data
	indices[12] = 8; indices[13] =  9; indices[14] = 10;
	indices[15] = 8; indices[16] = 10; indices[17] = 11;

	// Fill in the bottom face index data
	indices[18] = 12; indices[19] = 13; indices[20] = 14;
	indices[21] = 12; indices[22] = 14; indices[23] = 15;

	// Fill in the left face index data
	indices[24] = 16; indices[25] = 17; indices[26] = 18;
	indices[27] = 16; indices[28] = 18; indices[29] = 19;

	// Fill in the right face index data
	indices[30] = 20; indices[31] = 21; indices[32] = 22;
	indices[33] = 20; indices[34] = 22; indices[35] = 23;


	D3D10_BUFFER_DESC vbd;
    vbd.Usage		= D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth	= sizeof(SkyBoxVertex) * NUM_VERTICES;
    vbd.BindFlags	= D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags	= 0;
	D3D10_SUBRESOURCE_DATA vInitData;
    vInitData.pSysMem = vertices;
    check( d3d10::device->CreateBuffer( &vbd, &vInitData, & this->pVB.Ptr ) );

	D3D10_BUFFER_DESC ibd;
    ibd.Usage		= D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth	= sizeof(SkyBoxIndexType) * NUM_INDICES;
    ibd.BindFlags	= D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags	= 0;
    D3D10_SUBRESOURCE_DATA iInitData;
    iInitData.pSysMem = indices;
    check( d3d10::device->CreateBuffer( &ibd, &iInitData, & this->pIB.Ptr ) );

	this->numVertices = NUM_VERTICES;
	this->numIndices = NUM_INDICES;
}

void D3D10SkyData::Shutdown()
{
	this->fxSkybox.Unload();

	this->pIL = null;
	this->pVB = null;
	this->pIB = null;

	this->numVertices = 0;
	this->numIndices = 0;
}

/*================================
	D3D10ParticleSystemData
================================*/

D3D10ParticleSystemData::D3D10ParticleSystemData()
{}

D3D10ParticleSystemData::~D3D10ParticleSystemData()
{}

void D3D10ParticleSystemData::Initialize()
{
	this->fxBillboard.Load( PATH_TO_SHADERS_"Billboard.fx" );
	// reuse skybox vertex layout
	this->pIL = d3d10::scene->GetMiscData().skyData.pIL;
	AssertPtr( this->pIL );

	// Validity check.
	{
		D3D10_PASS_DESC passDesc;
		this->fxBillboard.GetActivePass()->GetDesc( &passDesc );

		const void* pShaderBytecodeWithInputSignature = passDesc.pIAInputSignature;
		const SizeT bytecodeLength = passDesc.IAInputSignatureSize; 

		D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ TEXT("POSITION"), 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ TEXT("TEXCOORD"), 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};

		if( S_FALSE != d3d10::device->CreateInputLayout(
			vertexDesc, ARRAY_SIZE( vertexDesc ),
			pShaderBytecodeWithInputSignature, bytecodeLength,
			null ))
		{
			sys::Error(TEXT("Input-layout declaration does not match the data type in a shader-input signature"));
		}
	}

	//	Create billboard geometry (unit quad).
	{
		// Build a full-screen quad.
		BillboardVertex quad[4] =
		{
			BillboardVertex( Vec3D( -1.0f,  1.0f, 0.0f ),	Vec2D( 0.0f, 0.0f ) ),
			BillboardVertex( Vec3D(  1.0f,  1.0f, 0.0f ),	Vec2D( 1.0f, 0.0f ) ),
			BillboardVertex( Vec3D( -1.0f, -1.0f, 0.0f ),	Vec2D( 0.0f, 1.0f ) ),
			BillboardVertex( Vec3D(  1.0f, -1.0f, 0.0f ),	Vec2D( 1.0f, 1.0f ) ),
		};

		D3D10_BUFFER_DESC vbdesc =
		{
			4 * sizeof( BillboardVertex ),	// ByteWidth
			D3D10_USAGE_IMMUTABLE,		// Usage
			D3D10_BIND_VERTEX_BUFFER,	// BindFlags
			0,	// CPUAccessFlags
			0	// MiscFlags
		};

		D3D10_SUBRESOURCE_DATA vbInitData;
		vbInitData.pSysMem = quad;
		vbInitData.SysMemPitch = 0;
		vbInitData.SysMemSlicePitch = 0;

		check( d3d10::device->CreateBuffer(
			&vbdesc,
			&vbInitData,
			& this->pVB.Ptr )
		);
	}
}

void D3D10ParticleSystemData::Shutdown()
{
	this->fxBillboard.Unload();
	this->pIL = null;
	this->pVB = 0;
}

/*================================
		D3D10MiscData
================================*/

D3D10MiscData::D3D10MiscData()
{}

D3D10MiscData::~D3D10MiscData()
{}

void D3D10MiscData::Initialize()
{
	this->fxShowRT			.Load( PATH_TO_SHADERS_"Debug_RenderTarget.fx" );

	this->skyData			.Initialize();
	this->particleSysData	.Initialize();

	{
		DXPtr< ID3D10Texture2D >	pWhiteTexture;

		const UINT theWidth = 2, theHeight = 2;

		D3D10_TEXTURE2D_DESC texDesc;
		texDesc.ArraySize          = 1;
		texDesc.BindFlags          = D3D10_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags     = 0;
		texDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.Width              = theWidth;
		texDesc.Height             = theHeight;
		texDesc.MipLevels          = 1;
		texDesc.MiscFlags          = 0;
		texDesc.SampleDesc.Count   = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage              = D3D10_USAGE_IMMUTABLE;

		const UINT8 rgba8_White[4] = { 0xFF, 0xFF, 0xFF, 0xFF };

		D3D10_SUBRESOURCE_DATA initialData;
		initialData.pSysMem = rgba8_White;
		initialData.SysMemPitch = theWidth * 4 * sizeof(UINT8); // width * texelSize
		initialData.SysMemSlicePitch = 0;
		check( d3d10::device->CreateTexture2D( &texDesc, &initialData, & pWhiteTexture.Ptr ) );

		D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension		= D3D10_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels	= 1;
		check( d3d10::device->CreateShaderResourceView( pWhiteTexture, &srvDesc, & srvWhiteTexture.Ptr ) );
	}
	{
		DXPtr< ID3D10Texture2D >	pBlackTexture;

		const UINT theWidth = 2, theHeight = 2;

		D3D10_TEXTURE2D_DESC texDesc;
		texDesc.ArraySize          = 1;
		texDesc.BindFlags          = D3D10_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags     = 0;
		texDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.Width              = theWidth;
		texDesc.Height             = theHeight;
		texDesc.MipLevels          = 1;
		texDesc.MiscFlags          = 0;
		texDesc.SampleDesc.Count   = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage              = D3D10_USAGE_IMMUTABLE;

		const UINT8 rgba8_Black[4] = { 0, 0, 0, 0xFF };

		D3D10_SUBRESOURCE_DATA initialData;
		initialData.pSysMem = rgba8_Black;
		initialData.SysMemPitch = theWidth * 4 * sizeof(UINT8); // width * texelSize
		initialData.SysMemSlicePitch = 0;
		check( d3d10::device->CreateTexture2D( &texDesc, &initialData, & pBlackTexture.Ptr ) );

		D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension		= D3D10_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels	= 1;
		check( d3d10::device->CreateShaderResourceView( pBlackTexture, &srvDesc, & srvBlackTexture.Ptr ) );
	}
}

void D3D10MiscData::Shutdown()
{
	this->fxShowRT			.Unload();

	this->skyData			.Shutdown();
	this->particleSysData	.Shutdown();

	this->srvWhiteTexture = null;
	this->srvBlackTexture = null;
}

/*================================
		D3D10Scene
================================*/

D3D10Scene::D3D10Scene()
{
	ENSURE_ONE_CALL;
}

D3D10Scene::~D3D10Scene()
{}

//
//	D3D10Scene::Initialize
//
void D3D10Scene::Initialize( UINT screenWidth, UINT screenHeight )
{
	// Create render targets first so that they're placed in the fastest portion of VRAM.
	this->renderTargetChain.Initialize( screenWidth, screenHeight );

	// Load shaders.

	this->shaderVars.Initialize( PATH_TO_SHADERS_"Common.fxh" );

	// Init stages.

	this->debugStage.Initialize();
	this->fullScreenQuad.Initialize( this->debugStage.fxDiffuseOnly );

	this->geometryStage.Initialize();
	this->lightingStage.Initialize();
	this->alphaStage.Initialize();
	this->postStage.Initialize( screenWidth, screenHeight );

	this->currentStage = null;

	this->miscData.Initialize();

	Assert( allModels.Num() == 0
		&& allSkies.Num() == 0
		&& allPortals.Num() == 0
		&& renderQueue.Num() == 0
		&& visibleSet.GetNum() == 0 );

	//-- optional --
	this->shaderVars.OptimizeLoadedShaders();
}

//
//	D3D10Scene::Shutdown
//
void D3D10Scene::Shutdown()
{
	this->renderTargetChain.Shutdown();
	this->fullScreenQuad.Shutdown();

	this->scene = null;
	this->spatialHash = null;

	this->cachedView.Refresh( mxSceneView() );

	this->shaderVars.Shutdown();

	this->geometryStage	.Shutdown();
	this->debugStage	.Shutdown();
	this->lightingStage	.Shutdown();
	this->alphaStage	.Shutdown();
	this->postStage		.Shutdown();

	this->currentStage = null;

	this->renderQueue.Clear();
	this->visibleSet.Empty();

	this->miscData.Shutdown();

	this->allModels.DeleteContents( true );
	this->allBillboards.DeleteContents( true );
	this->allSkies.DeleteContents( true );
	this->allPortals.DeleteContents( true );
}

void D3D10Scene::BuildRenderQueue( const mxSceneView& view,
		D3D10View &OutFullView, mxVisibleSet &OutVisibleSet, D3D10RenderQueue &OutQueue )
{
	OutFullView.Refresh( view );

	// Find visible objects.
	{
		MX_PROFILE( "Get visible set" );
		D3D10_STAT_COUNTER( d3d10::stats.Cpu_cull_time_milliseconds );
		this->spatialHash->GetVisibleSet( view, OutVisibleSet );
	}

	// Render the visible set. Generate a render queue from the visible set.
	{
		MX_PROFILE( "Gen render queue" );
		OutQueue.Clear();
		for( SizeT iEnt = 0; iEnt < OutVisibleSet.GetNum(); iEnt++ )
		{
			mxEntity * pEnt = OutVisibleSet.Get( iEnt );
			pEnt->GetGraphics()->Render( OutFullView, OutQueue );
		}
	}
}

//
//	D3D10Scene::DrawScene
//
void D3D10Scene::DrawScene( const mxSceneView& view, mxScene* scene )
{
	this->scene = scene;
	this->spatialHash = & scene->GetSpatialDatabase();

	this->BuildRenderQueue( view, this->cachedView, this->visibleSet, this->renderQueue );

	this->shaderVars.Update( & this->cachedView.constants );

	d3d10::stats.numEntities = this->renderQueue.Num();

	// Prepare render targets for filling G-buffer.
	this->renderTargetChain.Clear();
	this->renderTargetChain.Bind();

	// Fill the fat buffer.
	{
		MX_PROFILE( "G-buffer filling" );
		this->geometryStage.Render( this->cachedView, this->renderQueue );

		this->renderTargetChain.Unbind();
	}

	// Now apply render stages as post-processing effects.
	{
		// Optional debug stage.

	//	this->debugStage.SetMode( D3D10DebugStage::Render_DiffuseOnly );
	//	this->debugStage.SetMode( D3D10DebugStage::Render_SurfaceNormals );
	//	this->debugStage.SetMode( D3D10DebugStage::Render_DepthOnly );

	//	if ( ! this->debugStage.Render( this->renderTargetChain, this->fullScreenQuad ) )
		{
			MX_PROFILE( "Render stages" );

			this->lightingStage.Render( this->cachedView, this->renderQueue );
			RenderUnlitPrimitives( this->cachedView, this->renderQueue );

			this->alphaStage.Render( this->cachedView, this->renderQueue );

			this->postStage.Render();
		}
	}//End render stages

	this->currentStage = null;

	this->renderQueue.Clear();

	this->scene = null;
	this->spatialHash = null;
}

void D3D10Scene::RenderUnlitPrimitives( const D3D10ViewConstants& view, D3D10RenderQueue& renderQueue )
{
	// Render skies.
	{
		for ( IndexT iSky = 0; iSky < renderQueue.skies.Num(); iSky++ )
		{
			D3D10Sky * sky = renderQueue.skies[ iSky ];
			sky->RenderSky( view );
		}
	}
}

rxModel * D3D10Scene::CreateModel( const rxModelDescription& desc )
{
	D3D10Model * newRenderModel = MX_NEW D3D10Model();
	newRenderModel->Setup( desc );
	this->allModels.Append( newRenderModel );
	return newRenderModel;
}

rxParallelLight * D3D10Scene::CreateParallelLight( const Vec3D& direction,
											   const rxLightDescription& desc )
{
	return this->lightingStage.CreateParallelLight( direction, desc );
}

rxPointLight * D3D10Scene::CreatePointLight( const Vec3D& center, FLOAT radius,
										 const rxLightDescription& desc )
{
	return this->lightingStage.CreatePointLight( center, radius, desc );
}

rxSpotLight * D3D10Scene::CreateSpotLight( const Vec3D& center, const Vec3D& direction, FLOAT range,
										FLOAT innerAngle, FLOAT outerAngle,
										const rxLightDescription& desc )
{
	return this->lightingStage.CreateSpotLight( center, direction, range, innerAngle, outerAngle, desc );
}

void D3D10Scene::SetAmbientLightColor( const FColor& ambientColor )
{
	this->lightingStage.SetAmbientLightColor( ambientColor );
}

const FColor & D3D10Scene::GetAmbientLightColor() const
{
	return this->lightingStage.GetAmbientLightColor();
}

rxBillboard * D3D10Scene::CreateBillboard( rxTexture* texture, const Vec3D& origin,
				const Vec2D& size, const ColorRGB& color )
{
	D3D10Billboard * pNewBillboard = MX_NEW D3D10Billboard( texture, origin, size, color );
	this->allBillboards.Append( pNewBillboard );
	return pNewBillboard;
}

rxSky * D3D10Scene::CreateSkybox( rxTexture* sides[AABB::NUM_SIDES] )
{
	D3D10Skybox * skyBox = MX_NEW D3D10Skybox( sides );
	this->allSkies.Append( skyBox );
	return skyBox;
}

rxPortal * D3D10Scene::CreatePortal( FLOAT width, FLOAT height )
{
	D3D10Portal * portal = MX_NEW D3D10Portal( width, height );
	this->allPortals.Append( portal );
	return portal;
}

rxPostEffectManager* D3D10Scene::GetPostEffectManager()
{
	return &this->postStage;
}

void D3D10Scene::RemoveModel( D3D10Model* theModel )
{
	AssertPtr( theModel );
	this->allModels.Remove( theModel );
	MX_FREE( theModel );
}

void D3D10Scene::RemoveSky( D3D10Sky* theSky )
{
	AssertPtr( theSky );
	this->allSkies.Remove( theSky );
	MX_FREE( theSky );
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
