/*
=============================================================================
	File:	Geometry.h
	Desc:	Drawing geometry types, vertex formats, mesh buffers, etc.
=============================================================================
*/

#ifndef __RX_RENDER_GEOMETRY_H__
#define __RX_RENDER_GEOMETRY_H__

namespace abc {

/*
=====================================================================
	
	Vertex format.

=====================================================================
*/

//
//	EVertexElementUsage - describes the meaning (semantics) of vertex buffer contents.
//
enum EVertexElementUsage
{
	VEU_Unknown = 0,

	VEU_Position,		// Position, 3 floats per vertex.
	VEU_Normal,			// Normal, 3 floats per vertex.

	VEU_TexCoords,		// Texture coordinates.

	VEU_Diffuse,		// Diffuse colors.
	VEU_Specular,		// Specular colors.

	VEU_Tangent,	// X axis if normal is Z
	VEU_Binormal,	// Y axis if normal is Z

	// These are used for skeletal animation.
	VEU_BlendWeight,
	VEU_BlendIndices,

	VEU_FORCE_DWORD = MAX_INT32
};

//
//	EVertexElementType - is a type of a vertex element.
//
enum EVertexElementType
{
	VE_Float1,	// One-component float expanded to (float, 0, 0, 1).
	VE_Float2,	// Two-component float expanded to (float, float, 0, 1).
	VE_Float3,	// Three-component float expanded to (float, float, float, 1).
	VE_Float4,	// Four-component float expanded to (float, float, float, float).
	
	VE_Short1,
	VE_Short2,
	VE_Short3,
	VE_Short4,
	
	VE_UByte4,
};

//
//	rxVertexElement - defines a base type that vertices consist of.
//
struct rxVertexElement
{
	EVertexElementType	Type;	// The type of this element.
	EVertexElementUsage	Usage;	// The meaning of this element.
	mxUInt				Offset;	// The offset in the buffer that this element starts at.

public:
		rxVertexElement(); // empty ctor, noinit

		rxVertexElement( EVertexElementType  type,
						EVertexElementUsage  usage, mxUInt16 offset );

	EVertexElementType	GetType() const;
	EVertexElementUsage	GetUsage() const;
	mxUInt				GetOffset() const;

	// Returns the size of this element, in bytes.
	mxUInt	GetSize() const;

public:
	// Utility function for helping to calculate offsets.
	static mxUInt		GetVertexElementSize( EVertexElementType  elementType );
};

//
//	rxVertexLayout - describes vertex buffers contents.
//
class rxVertexLayout {
public:
	// Returns the number of elements in this vertex type.
	//
	virtual mxUInt	GetNumElements() const = 0;

	// Returns the vertex component with the given index.
	//
	virtual const rxVertexElement &	GetElement( mxUInt index ) const = 0;

	// Returns the size of this vertex type, in bytes.
	//
	virtual mxUInt	GetSize() const = 0;

public:
	bool IsValid() const;

protected:
			rxVertexLayout() {}
	virtual	~rxVertexLayout() {}
};

//
//	rxVertexDeclaration - used for creating new types of vertices.
//
//	NOTE: to ensure maximum portability,
//	vertex elements should be added to vertex declarations
//	in the following order:
//	position, blend weights, normals, diffuse colors, specular colors
//	and texture coordinates.
//	Moreover, there must be no unused gaps in a vertex structure.
//
class rxVertexDeclaration : public rxVertexLayout {
public:
	enum { MAX_NUM_VERTEX_ELEMENTS = 15/*old value: 256*/ };

	//
	//	Override ( rxVertexLayout ) :
	//
	mxUInt	GetNumElements() const;

	const rxVertexElement &	GetElement( mxUInt index ) const;

	mxUInt	GetSize() const;

			// Appends a new vertex element to the end.
			// Offset is calculated automatically.
	void	Put( EVertexElementUsage usage, EVertexElementType type );

	bool	IsValid() const;

	rxVertexDeclaration();

private:
	TArray< rxVertexElement >	m_elements;
	mxUInt	m_vertexSize;	// accumulated offset, in bytes
};

//
//	rxVertexFormat - is a handle to a vertex declaration, used to identify vertex buffer contents during rendering.
//
class rxVertexFormat : public rxVertexLayout {
public:
	// More to follow ...

protected:
	virtual	~rxVertexFormat() {}
};

/*
=====================================================================
	
	Standard (built-in) vertex types.

=====================================================================
*/

//
//	rxVertex - a standard vertex.
//
class rxVertex {
public:
	Vec3D	XYZ;		// 12
	Vec3D	Normal;		// 12
	Vec3D	Tangent;	// 12
	Vec2D	UV;			// 8

	// (12 * 3) + 8 = 44 bytes.

public:
	rxVertex()
	{}

	rxVertex( const Vec3D& position, const Vec3D& normal, const Vec3D& tangent, const Vec2D& texCoords )
		: XYZ( position )
		, Normal( normal )
		, Tangent( tangent )
		, UV( texCoords )
	{}

	void	Lerp( const rxVertex &a, const rxVertex &b, const FLOAT c );

	FORCEINLINE FLOAT operator [] ( IndexT index ) const {
		return ((const FLOAT*) this)[ index ];
	}
	FORCEINLINE FLOAT& operator [] ( IndexT index ) {
		return ((FLOAT*) this)[ index ];
	}
};

/*
=====================================================================
	
	Hardware buffers (vertex, index, etc).

=====================================================================
*/

//
//	EHardwareBufferUsage
//
enum EHardwareBufferUsage
{
	// The contents of the buffer will most likely be placed in the GPU memory.
	// Modifying the contents of this buffer will involve a huge performance hit.
	HBU_STATIC = 0,
	
	// Set to indicate that the buffer requires dynamic memory use.
	// This is useful for drivers because it enables them to decide where to place the buffer.
	// In general, static vertex buffers are placed in video memory and dynamic vertex buffers are placed in AGP memory.
	HBU_DYNAMIC,

	// Informs the system that the application writes only to the buffer.
	// Using this flag enables the driver to choose the best memory location for efficient write operations and rendering.
	// Attempts to read from a buffer that is created with this capability will fail.
	HBU_WRITEONLY,
};

//
// EHardwareBufferLockOptions
//
enum EHardwareBufferLockOptions
{
	// Normal mode, ie allows read/write and contents are preserved.
	HBL_NORMAL = 0,

	// Discards the entire buffer while locking; this allows optimisation to be performed because synchronisation issues are relaxed.
	// Only allowed on buffers created with the HBU_DYNAMIC flag. 
	HBL_DISCARD,

	// Lock the buffer for reading only. Not allowed in buffers which are created with HBU_WRITE_ONLY.
	// Mandatory on static buffers, i.e. those created without the HBU_DYNAMIC flag.
	HBL_READ_ONLY,

	// As HBL_NORMAL, except the application guarantees not to overwrite any region of the buffer which has already been used in this frame,
	// can allow some optimisation on some APIs.
	HBL_NO_OVERWRITE
};

//
//	rxVertexBufferDescription - used for creating vertex buffers.
//
class rxVertexBufferDescription {
public:
	EHardwareBufferUsage	UsageType;
	SizeT					VertexCount;	// Number of vertices in this buffer.
	SizeT					VertexSize;
	const void *			Data;			// Pointer to actual vertex data ( continuous memory block ).

public:
	rxVertexBufferDescription()
	{
		SetDefaults();
	}

	void	SetDefaults();
	bool	IsValid() const;
};

//
//	EIndexBufferType - describes all allowed index types.
//
enum EIndexBufferType
{
	IBT_16_BIT	= sizeof( UINT16 ),
	IBT_32_BIT	= sizeof( UINT32 ),
};

// These functions are provided for convenience .

FORCEINLINE
mxUInt GetIndexSize( EIndexBufferType  type )
{
	return (mxUInt) type;
}

FORCEINLINE
EIndexBufferType GetIndexType( mxUInt indexSize )
{
	Assert( indexSize == 2 || indexSize == 4 );
	return (indexSize == 2) ? IBT_16_BIT : IBT_32_BIT;
}

//
// Default index type. This can 16 or 32 bits wide.
//
#if 1

	typedef UINT32	rxIndex;

	enum { MAX_INDEX = MAX_UINT32 };

#else

	typedef UINT16	rxIndex;

	enum { MAX_INDEX = MAX_UINT16 };

#endif

//
//	rxIndexBufferDescription - used for creating index buffers.
//
class rxIndexBufferDescription {
public:
	EHardwareBufferUsage	UsageType;
	SizeT					IndexSize;	// This can be 2 or 4 (bytes).
	SizeT					IndexCount;	// Number of indices in this buffer.
	const void *			Data;		// Pointer to actual index data ( continuous, contiguous memory block ).

public:
	rxIndexBufferDescription()
	{
		SetDefaults();
	}

	void	SetDefaults();
	bool	IsValid() const;
};

//
//	rxVertices - represents a vertex buffer (used for rendering).
//
class rxVertices {
public:
	SizeT		VertexCount;	// [read-only]
//	SizeT		VertexSize;		// [read-only]

	// Used to identify the contents of this vertex buffer.
	//virtual const rxVertexLayout * GetVertexLayout() const = 0;

	// More to follow ...

protected:
	rxVertices()
		: VertexCount( 0 )
	//	, VertexSize( 0 )
	{}

	virtual	~rxVertices()
	{}
};

//
//	rxIndices - represents an index buffer (used for rendering).
//
class rxIndices {
public:
	//SizeT		IndexCount;	// [read-only]

	// More to follow ...

protected:
	rxIndices()
	//	: IndexCount( 0 )
	{}

	virtual ~rxIndices() {}
};

/*
=====================================================================
	
	Elementary graphics primitives used for rasterizing.

=====================================================================
*/

//
//	EPrimitiveType - enumerates all allowed types of elementary graphics primitives.
//
//	NOTE: these are defined to have the same value as the equivalent Direct3D constants.
//
enum EPrimitiveType
{
	PT_Unknown		 = 0,	// Error.
	PT_PointList	 = 1,	// A collection of isolated points.
	PT_LineList		 = 2,	// A list of points, one vertex per point.
	PT_LineStrip	 = 3,	// A strip of connected lines, 1 vertex per line plus one 1 start vertex.
	PT_TriangleList  = 4,	// A list of triangles, 3 vertices per triangle.
	PT_TriangleStrip = 5,	// A string of triangles, 3 vertices for the first triangle, and 1 per triange after that.
	PT_TriangleFan	 = 6,	// A string of triangles, 3 vertices for the first triangle, and 1 per triange after that.
	PT_Quads		 = 7,
};

/*
=====================================================================
	
	Render meshes.

=====================================================================
*/

//
//	rxMeshDescription - this structure is used for creating render meshes.
//
struct rxMeshDescription
{
	rxVertexBufferDescription	Vertices;		// initial vertex data
	rxIndexBufferDescription	Indices;		// initial index data
	EPrimitiveType				PrimitiveType;

public:
	rxMeshDescription();

	bool IsOk() const;
};

//
//	rxMesh - represents a mesh used for rendering.
//
class rxMesh {
public:

	// Vertex buffer data (not directly readable by the CPU).
	TPtr< rxVertices >	Vertices;

	// Index buffer data (not directly readable by the CPU).
	TPtr< rxIndices >	Indices;	// If null, then nonindexed drawing is used.

	// Type of primitives this mesh consists of.
	TEnum< EPrimitiveType, UINT32 >	PrimitiveType;

public:
	rxMesh();

	bool HasVertices() const	{ return ( this->Vertices != null ); }
	bool HasIndices() const		{ return ( this->Indices != null ); }

	bool IsValid() const;
};

/*
=====================================================================
	
	Non-renderable geometry held in main memory.

=====================================================================
*/

// Forward declarations.
class mxRawMesh;

#if 0
//
//	EMeshType - for saving general information about meshes to speed up some checks.
//
enum EMeshType
{
	Mesh_Unknown = 0,

	PrimMesh_Box,
	PrimMesh_Sphere,
	PrimMesh_Cylinder,
	PrimMesh_Cone,
	PrimMesh_Tetrahedron,

	PrimMesh_Triangle,	// single triangle
	PrimMesh_Quad,		// single quad

	UserMesh	= 256,

	// ...

	MaxMeshTypes = MAX_SDWORD
};

//
//	mxMeshProperties - miscellaneous properties that renderable geometry may have.
//
struct mxMeshProperties
{
	// 'true' if the mesh is a consistent, perfect closed hull, doesn't have holes, T-junctions, i.e. there aren't any dangling edges, etc.
	// (this will be 'false' for triangle soups.)
	//
	bool	bIsClosed2Manifold;

	// There can be many, many of them...

public:
	mxMeshProperties()
		: bIsClosed2Manifold( false )
	{}

	// Set every field to 'unknown' values.
	void Clear()
	{
		bIsClosed2Manifold = false;
	}
};

//
//	mxRawMesh - raw geometry held in system RAM (cannot be used for rendering).
//
//	Used mainly for mesh operations during initialization.
//
class mxRawMesh {
public:
	const rxVertexLayout * pVertexLayout;	// used to identify the contents of the vertex buffer

	void *			pVertices;	// vertex data (readable by CPU)
	mxUInt			numVertices;

	void *			pIndices;	// index data (readable by CPU)
	mxUInt			numIndices;
	mxUInt			indexSize;	// 16 or 32 bits

	EPrimitiveType	primType;	// type of primitives in this buffer (triangle lists, fans, etc)

	EMeshType		meshType;	// may be unknown, but may be CUBE, SPHERE, CYLINDER, etc.

	// mesh bounds in local space
	mxBounds		bounds;
	Sphere			sphere;

	//Array<Vec3D>	extremePoints;	// for building spatial hierarchies efficiently

	mxMeshProperties	properties;

public:
	mxRawMesh()
		: pVertexLayout( null )
		, pVertices( null )
		, numVertices( 0 )
		, pIndices( null )
		, numIndices( 0 )
		, indexSize( 0 )
		, primType( EPrimitiveType::PT_Unknown )
		, meshType( EMeshType::Mesh_Unknown )
	{
		bounds.Clear();
		sphere.Clear();
	}

	~mxRawMesh()
	{
		FreeData();
	}

	// Compute the bounding volume using vertex buffer.
	void CalculateBounds( mxBounds &OutBounds );

	void FreeData()
	{
		pVertexLayout = null;

		if ( pVertices ) {
			MX_FREE( pVertices );
			pVertices = null;
		}
		numVertices = null;

		if ( pIndices ) {
			MX_FREE( pIndices );
			pIndices = null;
		}
		numIndices = null;

		indexSize = null;

		primType = EPrimitiveType::PT_Unknown;
		meshType = EMeshType::Mesh_Unknown;

		bounds.Clear();
		sphere.Clear();

		properties.Clear();
	}

	bool IsValid() const;
};
#endif

//
//	mxMesh- raw geometry held in system RAM, accessable by CPU and used primarily for various mesh operations.
//
struct mxMesh : public ReferenceCounted
{
	//MX_DECLARE_MEMORY_CLASS( MX_MEMORY_CLASS_GEOMETRY );

	mxMesh();
	~mxMesh();

	bool IsValid() const;

public:
	UINT			numVertices;
	rxVertex *		vertices;	// vertex data (readable by CPU)
	UINT			numIndices;
	rxIndex *		indices;	// index data (readable by CPU)

	//UINT			primType;	// EPrimitiveType - type of primitives in this buffer

	mxBounds		bounds;	// mesh bounds in local space

public:
	void RecalculateBounds();

	void FlipNormals();	// reverses the order of indices

	// assumes that matrix is composed from rotation and translation and uniform scaling
	void Transform( const Matrix4& mat );

	void Copy( const mxMesh* other );

	void Clear();

private:
	void zzChecks()
	{
		StaticAssert( sizeof(numVertices) == sizeof(rxIndex) );
	}
private:
	NO_COPY_CONSTRUCTOR( mxMesh );
	NO_ASSIGNMENT( mxMesh );
};

typedef RefPtr< mxMesh >	mxMeshPtr;

/*
=====================================================================
	
	Mesh utilities.

=====================================================================
*/

mxMesh* MakeMesh_Quad( mxReal side, const Vec2D& texCoordScale );
mxMesh* MakeMesh_Box( mxReal length, mxReal height, mxReal depth );

}//End of namespace abc

#endif // !__RX_RENDER_GEOMETRY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
