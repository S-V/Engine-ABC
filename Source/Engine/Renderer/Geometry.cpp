/*
=============================================================================
	File:	Geometry.cpp
	Desc:
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>

namespace abc {

/*================================
		rxVertexLayout
================================*/

bool rxVertexLayout::IsValid() const
{
	//TODO: checks
	// return GetNumElements() < MAX_VERTEX_ELEMENTS
	return true;
}

/*================================
		rxVertexElement
================================*/

rxVertexElement::rxVertexElement()
{}

rxVertexElement::rxVertexElement(
		EVertexElementType  type, EVertexElementUsage  usage,
		mxUInt16 offset )

	: Type( type ), Usage( usage ), Offset( offset )
{}

EVertexElementType rxVertexElement::GetType() const
{
	return this->Type;
}

EVertexElementUsage rxVertexElement::GetUsage() const
{
	return this->Usage;
}

mxUInt rxVertexElement::GetOffset() const
{
	return this->Offset;
}

mxUInt rxVertexElement::GetSize() const
{
	return GetVertexElementSize( this->Type );
}

mxUInt rxVertexElement::GetVertexElementSize( EVertexElementType  elementType )
{
	switch ( elementType )
	{

	case EVertexElementType::VE_Float1 :
		return ( sizeof(float) * 1 );

	case EVertexElementType::VE_Float2 :
		return ( sizeof(float) * 2 );

	case EVertexElementType::VE_Float3 :
		return ( sizeof(float) * 3 );

	case EVertexElementType::VE_Float4 :
		return ( sizeof(float) * 4 );


	case EVertexElementType::VE_Short1 :
		return ( sizeof(short) * 1 );

	case EVertexElementType::VE_Short2 :
		return ( sizeof(short) * 2 );

	case EVertexElementType::VE_Short3 :
		return ( sizeof(short) * 3 );

	case EVertexElementType::VE_Short4 :
		return ( sizeof(short) * 4 );


	case EVertexElementType::VE_UByte4 :
		return ( sizeof(unsigned char) * 4 );

	default:
		Unreachable;
	}
	return 0;
}

/*================================
		rxVertexDeclaration
================================*/

rxVertexDeclaration::rxVertexDeclaration()
	: m_vertexSize( 0 )
{}

void rxVertexDeclaration::Put( EVertexElementUsage usage, EVertexElementType type )
{
	Assert( GetNumElements() < MAX_NUM_VERTEX_ELEMENTS );
	rxVertexElement & rNewElement = m_elements.Alloc();

	rNewElement.Type	= type;
	rNewElement.Usage	= usage;
	rNewElement.Offset	= m_vertexSize;

	m_vertexSize += rxVertexElement::GetVertexElementSize( type );
}

//void rxVertexDeclaration::AddElement( mxUInt16 offset, EVertexElementType type, EVertexElementUsage usage )
//{
//	rxVertexElement * pNewElement = m_elements.Alloc();
//	Assert( pNewElement );
//
//	rNewElement.Type	= type;
//	rNewElement.Usage	= usage;
//	rNewElement.Offset	= offset;
//}

mxUInt rxVertexDeclaration::GetNumElements() const
{
	return m_elements.Num();
}

const rxVertexElement & rxVertexDeclaration::GetElement( mxUInt index ) const
{
	return m_elements[ index ];
}

mxUInt rxVertexDeclaration::GetSize() const
{
	return m_vertexSize;
}

bool rxVertexDeclaration::IsValid() const
{
	return ( m_elements.Num() <= MAX_NUM_VERTEX_ELEMENTS );
}

/*================================
			rxVertex
================================*/

void rxVertex::Lerp( const rxVertex &a, const rxVertex &b, const FLOAT c )
{
	this->XYZ		= a.XYZ		+ c * ( b.XYZ - a.XYZ );
	this->UV		= a.UV		+ c * ( b.UV - a.UV );
	this->Normal	= a.Normal	+ c * ( b.Normal - a.Normal );
	this->Tangent	= a.Tangent	+ c * ( b.Tangent - a.Tangent );
}

/*================================
	rxVertexBufferDescription
================================*/

void rxVertexBufferDescription::SetDefaults()
{
	UsageType	= EHardwareBufferUsage::HBU_STATIC;
	VertexCount	= 0;
	VertexSize	= 0;
	Data		= null;
}

bool rxVertexBufferDescription::IsValid() const
{
	return (VertexCount > 0)
		&& (VertexSize > 0)
		&& (Data != null);
}

/*================================
	rxIndexBufferDescription
================================*/

void rxIndexBufferDescription::SetDefaults()
{
	UsageType	= EHardwareBufferUsage::HBU_STATIC;
	IndexSize	= sizeof( rxIndex );
	IndexCount	= 0;
	Data		= null;
}

bool rxIndexBufferDescription::IsValid() const
{
	return (IndexCount > 0) && (Data != null)
		&& ( (IndexSize == 2) || (IndexSize == 4) );
}

/*================================
		rxMeshDescription
================================*/

rxMeshDescription::rxMeshDescription()
{
	this->PrimitiveType = EPrimitiveType::PT_Unknown;
}

bool rxMeshDescription::IsOk() const
{
	return this->Vertices.IsValid()
		&& this->Indices.IsValid()
		&& ( this->PrimitiveType != EPrimitiveType::PT_Unknown );
}

/*================================
			rxMesh
================================*/

rxMesh::rxMesh()
	: PrimitiveType( EPrimitiveType::PT_Unknown )
{}

bool rxMesh::IsValid() const
{
	return ( this->Vertices != null )
		&& ( this->PrimitiveType != EPrimitiveType::PT_Unknown );
}
#if 0
/*================================
			mxRawMesh
================================*/

bool mxRawMesh::IsValid() const
{
	return ( pVertexLayout != null )
		&& ( pVertices != null )
		&& ( numVertices > 0 )
		
		// indices can be null
		
		&& ( primType != EPrimitiveType::PT_Unknown )

		// more checks are needed

		&& 1;
}
#endif

/*================================
			mxMesh
================================*/

mxMesh::mxMesh()
	: numVertices	( 0 )
	, vertices		( null )
	, numIndices	( 0 )
	, indices		( null )
//	, primType		( EPrimitiveType::PT_Unknown )
{
	bounds.Clear();
}

mxMesh::~mxMesh()
{
	Clear();
}

bool mxMesh::IsValid() const
{
	return ( this->numVertices > 0 )
		&& ( this->vertices != null )
		&& ( this->numIndices > 0 )
		&& ( this->indices != null )
	//	&& ( this->primType != EPrimitiveType::PT_Unknown )
		;
}

void mxMesh::RecalculateBounds()
{
	this->bounds.Clear();
	for( UINT iVertex = 0; iVertex < this->numVertices; iVertex++ )
	{
		this->bounds.AddPoint( this->vertices[iVertex].XYZ );
	}
}

void mxMesh::FlipNormals()
{
//	Assert( EPrimitiveType::PT_TriangleList == this->primType );
	// for each triangle in the mesh change its winding order...
	for( UINT i = 0; i < this->numIndices; i+=3 )
	{
		Swap( indices[i+1], indices[i+2] );
	}

	for( UINT iVertex = 0; iVertex < this->numVertices; iVertex++ )
	{
		this->vertices[iVertex].Normal *= -1.0f;
	//	this->vertices[iVertex].Tangent *= -1.0f;
	}
}

void mxMesh::Transform( const Matrix4& mat )
{
	for( UINT iVertex = 0; iVertex < this->numVertices; iVertex++ )
	{
		rxVertex & vertex = this->vertices[iVertex];
		mat.TransformVector( vertex.XYZ );
		mat.TransformNormal( vertex.Normal );
		mat.TransformNormal( vertex.Tangent );
	}

	this->bounds.TrasformSelf( mat );
}

void mxMesh::Copy( const mxMesh* other )
{
	Assert( other->IsValid() );
	Clear();
	
	this->numVertices = other->numVertices;
	this->vertices = MX_NEW rxVertex [this->numVertices];
	MemCopy( this->vertices, other->vertices, other->numVertices * sizeof(rxVertex) );
	
	this->numIndices = other->numIndices;
	this->indices = MX_NEW rxIndex [this->numIndices];
	MemCopy( this->indices, other->indices, other->numIndices * sizeof(rxIndex) );
	
//	this->primType = other->primType;
	this->bounds = other->bounds;
}

void mxMesh::Clear()
{
	numVertices = 0;
	MX_FREE( vertices );
	vertices = null;

	numIndices = 0;
	MX_FREE( indices );
	indices = null;

//	this->primType = EPrimitiveType::PT_Unknown;
	this->bounds.Clear();
}

/*
================================
	MakeMesh_Quad
================================
*/
mxMesh* MakeMesh_Quad( mxReal side, const Vec2D& texCoordScale )
{
	mxMesh* pNewMesh = MX_NEW mxMesh();

	Assert( side > 0.0f );

	enum { NUM_VERTICES = 4 };
	enum { NUM_INDICES = 6 };

	// Set vertices.

	pNewMesh->numVertices = NUM_VERTICES;

	/*
	12
	03
	*/
	const mxReal a = side * 0.5f;

	pNewMesh->vertices = MX_NEW rxVertex[ NUM_VERTICES ];

	pNewMesh->vertices[0].XYZ.Set( -a, 0, -a );
	pNewMesh->vertices[1].XYZ.Set( -a, 0, a );
	pNewMesh->vertices[2].XYZ.Set( a, 0, a );
	pNewMesh->vertices[3].XYZ.Set( a, 0, -a );

	pNewMesh->vertices[0].Normal = Vec3D::UNIT_Y;
	pNewMesh->vertices[1].Normal = Vec3D::UNIT_Y;
	pNewMesh->vertices[2].Normal = Vec3D::UNIT_Y;
	pNewMesh->vertices[3].Normal = Vec3D::UNIT_Y;

	pNewMesh->vertices[0].Tangent = Vec3D::UNIT_X;
	pNewMesh->vertices[1].Tangent = Vec3D::UNIT_X;
	pNewMesh->vertices[2].Tangent = Vec3D::UNIT_X;
	pNewMesh->vertices[3].Tangent = Vec3D::UNIT_X;

	const Matrix2 mat(
		texCoordScale.x,	0.0f,
		0.0f,				texCoordScale.y
	);
	pNewMesh->vertices[0].UV = ( Vec2D( 0, 1 ) * mat );
	pNewMesh->vertices[1].UV = ( Vec2D( 0, 0 ) * mat );
	pNewMesh->vertices[2].UV = ( Vec2D( 1, 0 ) * mat );
	pNewMesh->vertices[3].UV = ( Vec2D( 1, 1 ) * mat );

	// Indices.

	pNewMesh->numIndices = NUM_INDICES;

	pNewMesh->indices = MX_NEW rxIndex[ NUM_INDICES ];
	pNewMesh->indices[0] = 0;
	pNewMesh->indices[1] = 1;
	pNewMesh->indices[2] = 2;
	pNewMesh->indices[3] = 0;
	pNewMesh->indices[4] = 2;
	pNewMesh->indices[5] = 3;

//	pNewMesh->primType = EPrimitiveType::PT_TriangleList;

	pNewMesh->RecalculateBounds();

	return pNewMesh;
}

/*
================================
	MakeMesh_Box
================================
*/
mxMesh* MakeMesh_Box( mxReal length, mxReal height, mxReal depth )
{
	mxMesh* pNewMesh = MX_NEW mxMesh();

	enum { NUM_VERTICES = 24 };
	enum { NUM_INDICES = 36 };

	// Create vertex buffer.

	pNewMesh->numVertices = NUM_VERTICES;
	pNewMesh->vertices = MX_NEW rxVertex[ NUM_VERTICES ];

	// Fill in the front face vertex data.
	pNewMesh->vertices[0] = rxVertex( Vec3D( -0.5f, -0.5f, -0.5f ),		Vec3D( 0.0f, 0.0f, -1.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec2D( 0.0f, 1.0f )	);
	pNewMesh->vertices[1] = rxVertex( Vec3D( -0.5f,  0.5f, -0.5f ),		Vec3D( 0.0f, 0.0f, -1.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec2D( 0.0f, 0.0f )	);
	pNewMesh->vertices[2] = rxVertex( Vec3D(  0.5f,  0.5f, -0.5f ),		Vec3D( 0.0f, 0.0f, -1.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec2D( 1.0f, 0.0f )	);
	pNewMesh->vertices[3] = rxVertex( Vec3D(  0.5f, -0.5f, -0.5f ),		Vec3D( 0.0f, 0.0f, -1.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec2D( 1.0f, 1.0f )	);

	// Fill in the back face vertex data.																						
	pNewMesh->vertices[4] = rxVertex( Vec3D( -0.5f, -0.5f, 0.5f ), 	 	Vec3D( 0.0f, 0.0f, 1.0f ), 		Vec3D( -1.0f, 0.0f, 0.0f ),	Vec2D( 1.0f, 1.0f )	);
	pNewMesh->vertices[5] = rxVertex( Vec3D(  0.5f, -0.5f, 0.5f ), 	 	Vec3D( 0.0f, 0.0f, 1.0f ), 		Vec3D( -1.0f, 0.0f, 0.0f ),	Vec2D( 0.0f, 1.0f )	);
	pNewMesh->vertices[6] = rxVertex( Vec3D(  0.5f,  0.5f, 0.5f ), 	 	Vec3D( 0.0f, 0.0f, 1.0f ), 		Vec3D( -1.0f, 0.0f, 0.0f ),	Vec2D( 0.0f, 0.0f )	);
	pNewMesh->vertices[7] = rxVertex( Vec3D( -0.5f,  0.5f, 0.5f ), 	 	Vec3D( 0.0f, 0.0f, 1.0f ), 		Vec3D( -1.0f, 0.0f, 0.0f ),	Vec2D( 1.0f, 0.0f )	);

	// Fill in the top face vertex data.																						
	pNewMesh->vertices[8]  = rxVertex( Vec3D( -0.5f, 0.5f, -0.5f ),		Vec3D( 0.0f, 1.0f, 0.0f ), 	 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec2D( 0.0f, 1.0f )	);
	pNewMesh->vertices[9]  = rxVertex( Vec3D( -0.5f, 0.5f,  0.5f ),		Vec3D( 0.0f, 1.0f, 0.0f ), 	 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec2D( 0.0f, 0.0f )	);
	pNewMesh->vertices[10] = rxVertex( Vec3D(  0.5f, 0.5f,  0.5f ),		Vec3D( 0.0f, 1.0f, 0.0f ), 	 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec2D( 1.0f, 0.0f )	);
	pNewMesh->vertices[11] = rxVertex( Vec3D(  0.5f, 0.5f, -0.5f ),		Vec3D( 0.0f, 1.0f, 0.0f ), 	 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec2D( 1.0f, 1.0f )	);

	// Fill in the bottom face vertex data.													
	pNewMesh->vertices[12] = rxVertex( Vec3D( -0.5f, -0.5f, -0.5f ),	Vec3D( 0.0f, -1.0f, 0.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f ),	Vec2D( 1.0f, 1.0f )	);
	pNewMesh->vertices[13] = rxVertex( Vec3D(  0.5f, -0.5f, -0.5f ),	Vec3D( 0.0f, -1.0f, 0.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f ),	Vec2D( 0.0f, 1.0f )	);
	pNewMesh->vertices[14] = rxVertex( Vec3D(  0.5f, -0.5f,  0.5f ),	Vec3D( 0.0f, -1.0f, 0.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f ),	Vec2D( 0.0f, 0.0f )	);
	pNewMesh->vertices[15] = rxVertex( Vec3D( -0.5f, -0.5f,  0.5f ),	Vec3D( 0.0f, -1.0f, 0.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f ),	Vec2D( 1.0f, 0.0f )	);

	// Fill in the left face vertex data.																					
	pNewMesh->vertices[16] = rxVertex( Vec3D( -0.5f, -0.5f,  0.5f ),	Vec3D( -1.0f, 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, -1.0f ),	Vec2D( 0.0f, 1.0f )	);
	pNewMesh->vertices[17] = rxVertex( Vec3D( -0.5f,  0.5f,  0.5f ),	Vec3D( -1.0f, 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, -1.0f ),	Vec2D( 0.0f, 0.0f )	);
	pNewMesh->vertices[18] = rxVertex( Vec3D( -0.5f,  0.5f, -0.5f ),	Vec3D( -1.0f, 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, -1.0f ),	Vec2D( 1.0f, 0.0f )	);
	pNewMesh->vertices[19] = rxVertex( Vec3D( -0.5f, -0.5f, -0.5f ),	Vec3D( -1.0f, 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, -1.0f ),	Vec2D( 1.0f, 1.0f )	);

	// Fill in the right face vertex data.																					
	pNewMesh->vertices[20] = rxVertex( Vec3D(  0.5f, -0.5f, -0.5f ),	Vec3D( 1.0f, 0.0f, 0.0f ), 	 	Vec3D( 0.0f, 0.0f, 1.0f ), 	Vec2D( 0.0f, 1.0f )	);
	pNewMesh->vertices[21] = rxVertex( Vec3D(  0.5f,  0.5f, -0.5f ),	Vec3D( 1.0f, 0.0f, 0.0f ), 	 	Vec3D( 0.0f, 0.0f, 1.0f ), 	Vec2D( 0.0f, 0.0f )	);
	pNewMesh->vertices[22] = rxVertex( Vec3D(  0.5f,  0.5f,  0.5f ),	Vec3D( 1.0f, 0.0f, 0.0f ), 	 	Vec3D( 0.0f, 0.0f, 1.0f ), 	Vec2D( 1.0f, 0.0f )	);
	pNewMesh->vertices[23] = rxVertex( Vec3D(  0.5f, -0.5f,  0.5f ),	Vec3D( 1.0f, 0.0f, 0.0f ), 	 	Vec3D( 0.0f, 0.0f, 1.0f ), 	Vec2D( 1.0f, 1.0f )	);

	// Scale the box.
	{
		const Vec3D  vScale( length, height, depth );
		Matrix4  scaleMat( Matrix4::CreateScale( vScale ) );
		Matrix4  invTranspose( scaleMat.Inverse().Transpose() );

		for(UINT i = 0; i < NUM_VERTICES; ++i)
		{
			scaleMat.TransformVector( pNewMesh->vertices[i].XYZ );
			invTranspose.TransformNormal( pNewMesh->vertices[i].Normal );
			invTranspose.TransformNormal( pNewMesh->vertices[i].Tangent );
		}
	}

	// Create the index buffer.

	pNewMesh->numIndices = NUM_INDICES;
	pNewMesh->indices = MX_NEW rxIndex[ NUM_INDICES ];

	// Fill in the front face index data
	pNewMesh->indices[0] = 0; pNewMesh->indices[1] = 1; pNewMesh->indices[2] = 2;
	pNewMesh->indices[3] = 0; pNewMesh->indices[4] = 2; pNewMesh->indices[5] = 3;

	// Fill in the back face index data
	pNewMesh->indices[6] = 4; pNewMesh->indices[7]  = 5; pNewMesh->indices[8]  = 6;
	pNewMesh->indices[9] = 4; pNewMesh->indices[10] = 6; pNewMesh->indices[11] = 7;

	// Fill in the top face index data
	pNewMesh->indices[12] = 8; pNewMesh->indices[13] =  9; pNewMesh->indices[14] = 10;
	pNewMesh->indices[15] = 8; pNewMesh->indices[16] = 10; pNewMesh->indices[17] = 11;

	// Fill in the bottom face index data
	pNewMesh->indices[18] = 12; pNewMesh->indices[19] = 13; pNewMesh->indices[20] = 14;
	pNewMesh->indices[21] = 12; pNewMesh->indices[22] = 14; pNewMesh->indices[23] = 15;

	// Fill in the left face index data
	pNewMesh->indices[24] = 16; pNewMesh->indices[25] = 17; pNewMesh->indices[26] = 18;
	pNewMesh->indices[27] = 16; pNewMesh->indices[28] = 18; pNewMesh->indices[29] = 19;

	// Fill in the right face index data
	pNewMesh->indices[30] = 20; pNewMesh->indices[31] = 21; pNewMesh->indices[32] = 22;
	pNewMesh->indices[33] = 20; pNewMesh->indices[34] = 22; pNewMesh->indices[35] = 23;

//	pNewMesh->primType = EPrimitiveType::PT_TriangleList;

	pNewMesh->RecalculateBounds();

	return pNewMesh;
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
