/*
=============================================================================
	File:	D3D10Resources.cpp
	Desc:	Direct3D-specific implementation of graphics resource handling.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>
#include <Renderer/D3D/D3DRenderSystem.h>

namespace abc {

namespace d3d10
{
	D3D10Material * GetFallbackMaterial_D3D10()
	{
		//return checked_cast< D3D10Material* >(
		//	d3d10::pDebugDrawer->GetDebugMaterial( EDebugMaterial::Material_Fallback )
		//	);
		Unimplemented;
		return null;
	}

	LPSTR GetSemanticName( EVertexElementUsage usage )
	{
		switch ( usage )
		{
		case VEU_Unknown :
			return TEXT( "POSITION" );

		case VEU_Position :
			return TEXT( "POSITION" );
		case VEU_Normal :
			return TEXT( "NORMAL" );

		case VEU_TexCoords :
			return TEXT( "TEXCOORD" );

		case VEU_Diffuse :
			return TEXT( "COLOR" );
		case VEU_Specular :
			return TEXT( "COLOR" );

		case VEU_Tangent :
			return TEXT( "TANGENT" );
		case VEU_Binormal :
			return TEXT( "BINORMAL" );

		case VEU_BlendWeight :
			return TEXT( "BLENDWEIGHT" );
		case VEU_BlendIndices :
			return TEXT( "BLENDINDICES" );

		default:
			Unreachable;
		}
		return null;
	}

	DXGI_FORMAT GetVertexElementFormat( EVertexElementType  type )
	{
		switch ( type )
		{
		case VE_Float1 :
			return DXGI_FORMAT_R32_FLOAT;
		case VE_Float2 :
			return DXGI_FORMAT_R32G32_FLOAT;
		case VE_Float3 :
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case VE_Float4 :
			return DXGI_FORMAT_R8G8B8A8_UNORM;

		case VE_Short1 :
			return DXGI_FORMAT_R16_UNORM;
		case VE_Short2 :
			return DXGI_FORMAT_R16G16_UNORM;
		case VE_Short3 :
			Unimplemented;
		case VE_Short4 :
			Unimplemented;

		case VE_UByte4 :
			Unimplemented;

		default:
			Unreachable;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

}//End of namespace d3d10

/*================================
		D3D10VertexFormat
================================*/

D3D10VertexFormat::D3D10VertexFormat( ID3D10InputLayout* pVertexLayout )
	: VertexLayout( pVertexLayout )
{}

D3D10VertexFormat::~D3D10VertexFormat()
{}

mxUInt D3D10VertexFormat::GetNumElements() const
{
	Unimplemented;
	return 0;
}

const rxVertexElement & D3D10VertexFormat::GetElement( mxUInt index ) const
{
	Unimplemented;
	return *(rxVertexElement*)null;
}

mxUInt D3D10VertexFormat::GetSize() const
{
	Unimplemented;
	return 0;
}

/*================================
		D3D10VertexBuffer
================================*/

D3D10VertexBuffer::D3D10VertexBuffer( ID3D10Buffer* pVertexBuffer, SizeT theVertexSize )
	: pVB( pVertexBuffer ), VertexSize( theVertexSize )
{}

D3D10VertexBuffer::~D3D10VertexBuffer()
{}

//const rxVertexLayout * D3D10VertexBuffer::GetVertexLayout() const
//{
//	Unimplemented;
//	return 0;
//}

/*================================
		D3D10IndexBuffer
================================*/

D3D10IndexBuffer::D3D10IndexBuffer( ID3D10Buffer* pIndexBuffer, DXGI_FORMAT theFormat, mxUInt numIndices )
	: pIB( pIndexBuffer ), Format( theFormat ), NumIndices( numIndices )
{}

D3D10IndexBuffer::~D3D10IndexBuffer()
{}

/*================================
		D3D10Texture
================================*/

D3D10Texture::D3D10Texture(  ID3D10ShaderResourceView* theSRV )
	: SRV( theSRV )
{
#ifdef MX_DEBUG
	D3D10_SHADER_RESOURCE_VIEW_DESC desc;
	this->SRV->GetDesc( &desc );

	Assert( D3D10_SRV_DIMENSION_TEXTURE2D == desc.ViewDimension );
#endif // MX_DEBUG

	this->SRV->GetResource( (ID3D10Resource**) & this->pTexture.Ptr );
}

D3D10Texture::~D3D10Texture()
{}

rxTexture* D3D10Texture::MakeNormalMap( FLOAT amplitude )
{
	Unimplemented;
	
	UINT flags = 0;

	UINT channel = 0;
/*
	D3DX_CHANNEL_RED Indicates the red channel should be used. 
	D3DX_CHANNEL_BLUE Indicates the blue channel should be used. 
	D3DX_CHANNEL_GREEN Indicates the green channel should be used. 
	D3DX_CHANNEL_ALPHA Indicates the alpha channel should be used. 
*/

	ensure( D3DX10ComputeNormalMap(
		this->pTexture,
		flags,
		channel,
		amplitude,
		this->pTexture
	));

	return this;
}

/*================================
		D3D10Material
================================*/

D3D10Material::D3D10Material()
{}

D3D10Material::~D3D10Material()
{}

void D3D10Material::Setup( const rxMaterialDescription& desc )
{
	D3D10Texture * baseMap		= checked_cast< D3D10Texture* >( desc.layers[ TL_Diffuse ] );
	D3D10Texture * detailMap	= checked_cast< D3D10Texture* >( desc.layers[ TL_Detail ] );
	D3D10Texture * normalMap	= checked_cast< D3D10Texture* >( desc.layers[ TL_Normals ] );
	D3D10Texture * specularMap	= checked_cast< D3D10Texture* >( desc.layers[ TL_Specular ] );

	this->BaseMap		= baseMap ?		baseMap->SRV		: d3d10::scene->GetMiscData().srvWhiteTexture;
	this->DetailMap		= detailMap ?	detailMap->SRV		: d3d10::scene->GetMiscData().srvWhiteTexture;
	this->NormalMap		= normalMap ?	normalMap->SRV		: d3d10::scene->GetMiscData().srvWhiteTexture;
	this->SpecularMap	= specularMap ?	specularMap->SRV	: d3d10::scene->GetMiscData().srvWhiteTexture;

	this->Data.emissive				= desc.emissive;
	this->Data.specularIntensity	= desc.specularIntensity;
	this->Data.specularPower		= desc.shininess;
}

/*================================
		D3D10ResourceStats
================================*/

D3D10ResourceStats::D3D10ResourceStats()
{}

/*================================
		D3D10Resources
================================*/

D3D10Resources::D3D10Resources()
{}

D3D10Resources::~D3D10Resources()
{
	Shutdown();
}

//
//	D3D10Resources::Initialize
//
void D3D10Resources::Initialize()
{
}

//
//	D3D10Resources::Shutdown
//
void D3D10Resources::Shutdown()
{
	this->materials.	Delete();
	this->textures.		Delete();
	this->vertexBuffers.DeleteContents(true);
	this->indexBuffers.	DeleteContents(true);
}

//
//	D3D10Resources::CreateVertexFormat
//
rxVertexFormat* D3D10Resources::CreateVertexFormat( const rxVertexDeclaration& decl, rxMaterial* material )
{
	AssertPtr( decl.IsValid() );
	AssertPtr( material );

	MX_OPTIMIZE( "hash vertex formats to avoid duplication" )

	TArray< D3D10_INPUT_ELEMENT_DESC >	vertexElements10;

	enum { MAX_NUM_VERTEX_ELEMENTS = 8 };
	enum { MAX_VERTEX_ELEMENT_USAGES = 32 };
	UINT  semanticIndices[MAX_NUM_VERTEX_ELEMENTS][MAX_VERTEX_ELEMENT_USAGES] = { 0 };

	for ( IndexT iVertexElement = 0; iVertexElement < decl.GetNumElements(); iVertexElement++ )
	{
		const rxVertexElement & rElement = decl.GetElement( iVertexElement );

		D3D10_INPUT_ELEMENT_DESC & pNewVertexElement10 = vertexElements10.Alloc();

		pNewVertexElement10.SemanticName			= d3d10::GetSemanticName( rElement.GetUsage() );
		pNewVertexElement10.SemanticIndex			= semanticIndices[ iVertexElement ][ rElement.GetUsage() ]++;
		pNewVertexElement10.Format					= d3d10::GetVertexElementFormat( rElement.GetType() );
		pNewVertexElement10.InputSlot				= 0;
		pNewVertexElement10.AlignedByteOffset		= rElement.GetOffset();
		pNewVertexElement10.InputSlotClass			= D3D10_INPUT_PER_VERTEX_DATA;
		pNewVertexElement10.InstanceDataStepRate	= 0;
	}

	// Create a new vertex declaration.
//	ID3D10InputLayout *	pVertexLayout10 = null;
	Unimplemented;
	return 0;
#if 0
	D3D10Material * pMaterial10 = checked_cast< D3D10Material* >( material );

 // Create the input layout
    D3D10_PASS_DESC PassDesc;
    pMaterial10->ActiveTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    HRESULT hr = g_pd3dDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature,
                                          PassDesc.IAInputSignatureSize, &g_pVertexLayout );

	d3d10::device->CreateInputLayout(
		vertexElements10.Ptr(),
		vertexElements10.Num(),
		shaderCode->GetBufferPointer(),
	    shaderCode->GetBufferSize(),
		&pVertexLayout10
		);

	D3D10VertexFormat * pNewVertexFormat = MX_NEW D3D10VertexFormat( pVertexLayout10 );
	m_vertexFormats.Append( pNewVertexFormat );

	return ( pNewVertexFormat );
#endif
}

//
//	D3D10Resources::AllocateVertexBuffer
//
rxVertices* D3D10Resources::AllocateVertexBuffer( const rxVertexBufferDescription& desc )
{
	Assert( desc.IsValid() );

	D3D10_BUFFER_DESC vbd;
	vbd.Usage			= d3d10::GetBufferUsageType( desc.UsageType );
	vbd.ByteWidth		= desc.VertexCount * desc.VertexSize;
	vbd.BindFlags		= D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags	= 0;
	vbd.MiscFlags		= 0;

	D3D10_SUBRESOURCE_DATA vinitData;	// initial data
	vinitData.pSysMem = desc.Data;

	ID3D10Buffer * pVertexBuffer = null;

	ensure( d3d10::device->CreateBuffer(
		&vbd,
		&vinitData,
		&pVertexBuffer
		));

	AssertPtr( pVertexBuffer );

	D3D10VertexBuffer * pNewVertexBuffer = MX_NEW D3D10VertexBuffer( pVertexBuffer, desc.VertexSize );
	pNewVertexBuffer->VertexCount = desc.VertexCount;

	this->vertexBuffers.Append( pNewVertexBuffer );
	return pNewVertexBuffer;
}

//
//	D3D10Resources::AllocateIndexBuffer
//
rxIndices* D3D10Resources::AllocateIndexBuffer( const rxIndexBufferDescription& desc )
{
	Assert( desc.IsValid() );

	D3D10_BUFFER_DESC ibd;
	ibd.Usage			= d3d10::GetBufferUsageType( desc.UsageType );
	ibd.ByteWidth		= desc.IndexSize * desc.IndexCount;
	ibd.BindFlags		= D3D10_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags	= 0;
	ibd.MiscFlags		= 0;

	D3D10_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = desc.Data;

	ID3D10Buffer * pIndexBuffer = null;

	check( d3d10::device->CreateBuffer(
		&ibd,
		&iinitData,
		&pIndexBuffer
		));

	AssertPtr( pIndexBuffer );

	const DXGI_FORMAT ibFormat = (GetIndexType(desc.IndexSize) == IBT_16_BIT) ?
								DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	D3D10IndexBuffer * pNewIndexBuffer = MX_NEW D3D10IndexBuffer( pIndexBuffer, ibFormat, desc.IndexCount );

	this->indexBuffers.Append( pNewIndexBuffer );
	return pNewIndexBuffer;
}

//
//	D3D10Resources::LoadTexture
//
rxTexture* D3D10Resources::LoadTexture( const mxFilePath& filename )
{
	if ( ! filename.Exists() ) {
		sys::Error( "Failed to load texture %s", filename.GetName() );
		return null;
	}

	if ( rxTexture * pTexture = this->textures.Find( filename.GetName() ) )
	{
		return pTexture;
	}

	ID3D10ShaderResourceView * pSRV = null;
/*
	D3DX10_IMAGE_LOAD_INFO loadInfo;
	loadInfo.Width  		= D3DX10_DEFAULT;//D3DX10_FROM_FILE;
	loadInfo.Height 		= D3DX10_DEFAULT;//D3DX10_FROM_FILE;
	loadInfo.Depth  		= D3DX10_DEFAULT;//D3DX10_FROM_FILE;
	loadInfo.FirstMipLevel	= 0;
	loadInfo.MipLevels		= D3DX10_DEFAULT;//D3DX10_FROM_FILE;
	loadInfo.Usage			= D3D10_USAGE_DEFAULT;
	loadInfo.BindFlags		= D3D10_BIND_SHADER_RESOURCE;
	loadInfo.CpuAccessFlags	= 0;
	loadInfo.MiscFlags		= 0;
	loadInfo.Format 		= DXGI_FORMAT_R8G8B8A8_UNORM;
	loadInfo.Filter 		= D3DX10_FILTER_NONE;
	loadInfo.MipFilter		= D3DX10_FILTER_NONE;
	loadInfo.pSrcInfo		= 0;
*/
	HRESULT hr = D3DX10CreateShaderResourceViewFromFile(
		d3d10::device,
		filename.GetName(),
		null,	// D3DX10_IMAGE_LOAD_INFO *
		null,	// ID3DX10ThreadPump *
		&pSRV,
		null	// HRESULT *
	);

	if ( FAILED(hr) ) {
		sys::Error( "Failed to load texture %s", filename.GetName() );
		return null;
	}

	AssertPtr( pSRV );


	D3D10Texture * pNewTexture = MX_NEW D3D10Texture( pSRV );
	this->textures.Insert( filename.GetName(), pNewTexture );
	return pNewTexture;
}

//
//	D3D10Resources::CreateTexture
//
rxTexture* D3D10Resources::CreateTexture( const void* pData, mxUInt numBytes, const rxTextureDescription& desc )
{
Unimplemented;
	return null;
}

//
//	D3D10Resources::GetTexture
//
rxTexture * D3D10Resources::GetTexture( const mxChar* name )
{
Unimplemented;
	return null;
}

//
//	D3D10Resources::LoadMaterial
//
rxMaterial* D3D10Resources::LoadMaterial( const mxFilePath& filename )
{
	Unimplemented;
	return 0;
#if 0
	if ( ! filename.Exists() ) {
		sys::Warning( "File '%s' doesn't exist, switching to fallback material", filename.GetName() );
		return d3d10::GetFallbackMaterial_D3D10();
	}

	// Files are hashed by their base names.
	//
	if ( rxMaterial * pMaterial = this->materials.Find( filename.GetBaseName() ) )
	{
		return pMaterial;
	}

	//
	// Create the effect.
	//
	ID3D10Effect * pEffect = null;

	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
	
	DEBUG_CODE
	(
		dwShaderFlags |= D3D10_SHADER_DEBUG;
	)

	ID3D10Blob * pErrorBuffer = null;

	const HRESULT  hResult = D3DX10CreateEffectFromFile(
		filename.GetName(),
		null,				// defines
		null,				// include
		"fx_4_0",			// shader profile
		dwShaderFlags,		// HLSL compile options
		0,					// effect compile options
		d3d10::device,
		null,				// effect pool for sharing variables between effects
		null,				// thread pump
		& pEffect,
		& pErrorBuffer,
		null
		);

	if( FAILED( hResult ) )
	{
		if ( pErrorBuffer )
		{
			sys::Error( "Failed to create effect, %s", (const Char* ) pErrorBuffer->GetBufferPointer() );
			pErrorBuffer->Release();
		}
		return d3d10::GetFallbackMaterial_D3D10();
	}

	AssertPtr( pEffect );
	Assert( pEffect->IsValid() );

	D3D10Material * pNewMaterial = MX_NEW D3D10Material( pEffect );
	this->materials.Insert( filename.GetBaseName(), pNewMaterial );

	return pNewMaterial;
#endif
}

//
//	D3D10Resources::CreateMaterial
//
rxMaterial * D3D10Resources::CreateMaterial( const rxMaterialDescription& desc )
{
	Assert( desc.IsOk() );

	if ( D3D10Material * pMaterial = this->materials.Find( desc.name.ToChar() ) )
	{
		sys::Warning( "material with name '%s' already exists", desc.name.ToChar() );
		return pMaterial;
	}

	D3D10Material * pNewMaterial = MX_NEW D3D10Material();
	pNewMaterial->Setup( desc );

	this->materials.Insert( desc.name, pNewMaterial );

	return pNewMaterial;
}

//
//	D3D10Resources::GetMaterial
//
rxMaterial * D3D10Resources::GetMaterial( const mxChar* name )
{
	if ( rxMaterial * pMaterial = this->materials.Find( name ) )
	{
		return pMaterial;
	}
	sys::Warning( "Failed to find material '%s', switching to fallback", name );
	return null;
}

//
//	D3D10Resources::GetStats
//
const rxResourceStats & D3D10Resources::GetStats() const
{
	return this->stats;
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
