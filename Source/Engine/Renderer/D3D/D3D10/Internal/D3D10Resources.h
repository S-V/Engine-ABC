/*
=============================================================================
	File:	D3D10Resources.h
	Desc:	Direct3D-specific implementation of graphics resource handling.
=============================================================================
*/

#ifndef __MX_D3D10_RESOURCES_H__
#define __MX_D3D10_RESOURCES_H__

namespace abc {

//
//	D3D10Resource
//
struct D3D10Resource
{
	D3D10Resource()
	{}
	~D3D10Resource()
	{}

	//TODO: add resource states, Load() and Unload(), CancelLoading(), etc.
};

//
//	D3D10VertexFormat
//
class D3D10VertexFormat : public rxVertexFormat {
public:
	//
	// Override ( rxVertexFormat ) :
	//
	mxUInt	GetNumElements() const;

	const rxVertexElement &	GetElement( mxUInt index ) const;

	mxUInt	GetSize() const;

public:
	D3D10VertexFormat( ID3D10InputLayout* pVertexLayout );
	~D3D10VertexFormat();

public:
	DXPtr< ID3D10InputLayout >	VertexLayout;
};

//
//	D3D10VertexBuffer
//
class D3D10VertexBuffer : public rxVertices {
public:
	//
	// Overrides ( rxVertices ) :
	//
	//const rxVertexLayout * GetVertexLayout() const;

public:
	D3D10VertexBuffer( ID3D10Buffer* pVertexBuffer, SizeT theVertexSize );
	~D3D10VertexBuffer();

public:
	DXPtr< ID3D10Buffer >	pVB;

	const UINT		VertexSize;	// used by D3D10 drawing
};

//
//	D3D10IndexBuffer
//
class D3D10IndexBuffer : public rxIndices {
public:
	//
	// Override ( rxIndices ) :
	//

public:
	D3D10IndexBuffer( ID3D10Buffer* pIndexBuffer, DXGI_FORMAT theFormat, mxUInt numIndices );
	~D3D10IndexBuffer();

public:
	DXPtr< ID3D10Buffer >	pIB;
	const DXGI_FORMAT		Format;	// Only DXGI_FORMAT_R16_UINT or DXGI_FORMAT_R32_UINT are valid!
	const mxUInt			NumIndices;
};

//
//	D3D10Texture
//
class D3D10Texture : public rxTexture {
public:
		D3D10Texture( ID3D10ShaderResourceView* theSRV );
		~D3D10Texture();

	//
	// Override ( rxTexture ) :
	//
	rxTexture* MakeNormalMap( FLOAT amplitude = 1.0f );

public:
	DXPtr< ID3D10ShaderResourceView >	SRV;
	DXPtr< ID3D10Texture2D >			pTexture;
};

//
//	D3D10Material
//
class D3D10Material : public rxMaterial {
public:
	D3D10Material();
	~D3D10Material();

	void Setup( const rxMaterialDescription& desc );

	//
	// Override ( rxMaterial ) :
	//

public:
	DXPtr< ID3D10ShaderResourceView >	BaseMap;	// diffuse (texture) color
	DXPtr< ID3D10ShaderResourceView >	DetailMap;
	DXPtr< ID3D10ShaderResourceView >	NormalMap;
	DXPtr< ID3D10ShaderResourceView >	SpecularMap;
	//DXPtr< ID3D10ShaderResourceView >		Reflection;
	//DXPtr< ID3D10ShaderResourceView >		Extra[ 4 ];

	D3D10MaterialData					Data;
};

//
//	D3D10Mesh - is a simple helper mesh.
//
struct D3D10Mesh
{
	struct Vertex
	{
		Vec3D	XYZ;
		Vec2D	UV;
	};
	typedef UINT16	Index;

	DXPtr< ID3D10InputLayout >	pIL;
	DXPtr< ID3D10Buffer >		pVB;
	DXPtr< ID3D10Buffer >		pIB;

	UINT 	numVertices;
	UINT 	numIndices;

public:
	D3D10Mesh()
		: numVertices( 0 )
		, numIndices( 0 )
	{}

	~D3D10Mesh()
	{}

	void Shutdown()
	{
		this->pIL = null;
		this->pVB = null;
		this->pIB = null;

		this->numVertices = 0;
		this->numIndices = 0;
	}

	void Render( D3D10Shader& shader )
	{
		UINT strides = sizeof( Vertex );
		UINT offsets = 0;
		ID3D10Buffer * pBuffers[1] = { this->pVB };

		d3d10::device->IASetInputLayout( this->pIL );
		d3d10::device->IASetVertexBuffers( 0, 1, pBuffers, &strides, &offsets );
		d3d10::device->IASetIndexBuffer( this->pIB, DXGI_FORMAT_R16_UINT, 0 );
		d3d10::device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST  );

		shader.Apply();
		d3d10::device->DrawIndexed( this->numIndices, 0, 0 );
	}
};

//
//	D3D10ResourceStats
//
class D3D10ResourceStats : public rxResourceStats {
public:

public:
	D3D10ResourceStats();
};

//
//	D3D10Resources
//
class D3D10Resources : public rxResources {
public:
	//
	//	Override ( rxResources ) :
	//
	
	//--------------------------------------------------------
	//	Renderable geometry.
	//--------------------------------------------------------

	rxVertexFormat *	CreateVertexFormat( const rxVertexDeclaration& decl, rxMaterial* material );

	rxVertices *	AllocateVertexBuffer( const rxVertexBufferDescription& desc );

	rxIndices *		AllocateIndexBuffer( const rxIndexBufferDescription& desc );

	//--------------------------------------------------------
	// Texture resources.
	//--------------------------------------------------------

	rxTexture *		LoadTexture( const mxFilePath& filename );
	rxTexture *		CreateTexture( const void* pData, mxUInt numBytes, const rxTextureDescription& desc );
	rxTexture *		GetTexture( const mxChar* name );

	//--------------------------------------------------------
	//	Render materials.
	//--------------------------------------------------------

	rxMaterial *	LoadMaterial( const mxFilePath& filename );
	rxMaterial *	CreateMaterial( const rxMaterialDescription& desc );
	rxMaterial *	GetMaterial( const mxChar* name );

	//--------------------------------------------------------
	// Testing & Debugging.
	//--------------------------------------------------------


	//--------------------------------------------------------
	//	Stats.
	//--------------------------------------------------------

	const rxResourceStats &	GetStats() const;

public:
	//	Public attributes.

	
private:
	// Internal functions.

	friend class D3D10Renderer;

			D3D10Resources();
			~D3D10Resources();

	void	Initialize();	// Create fallback materials, textures, etc.
	void	Shutdown();		// Free all allocated resources, release textures, buffers, etc.

private:
	TResourceSet< D3D10Material >	materials;
	TResourceSet< D3D10Texture >	textures;
	TArray< D3D10VertexBuffer* >	vertexBuffers;
	TArray< D3D10IndexBuffer* >		indexBuffers;

	D3D10ResourceStats		stats;
};

}//End of namespace abc

#endif // !__MX_D3D10_RESOURCES_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
