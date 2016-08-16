/*
=============================================================================
	File:	CSG.h
	Desc:	CSG-related code.
=============================================================================
*/

#ifndef __MESHOK_CSG_H__
#define __MESHOK_CSG_H__

namespace abc {

/*
=======================================================================
	
		Constructive Solid Geometry.

=======================================================================
*/

// Forward declarations.
class CSGModel;

//
//	ESetOp - enumerates all supported types of CSG operations.
//
enum ESetOp
{
	CSG_Difference,		// 'subtract'
	CSG_Union,			// 'add'
//	CSG_Intersection	// 'and', find the common part
};

const mxChar* GetSetOpName( ESetOp op );

//
//	CSGInput - contains settings for performing boolean set operations.
//
struct CSGInput
{
	ESetOp			type;		// type of CSG operation to perform
	CSGModel *		operand;	// the second operand

public:
	CSGInput()
		: type( ESetOp::CSG_Difference )
		, operand( null )
	{}

	bool IsOk() const
	{
		return 1
			&& operand != null
			;
	}
};

//
//	CSGOutput - result of a boolean set operation.
//
struct CSGOutput
{
	enum EFlags
	{
		MeshChanged	= BIT(0),	// true if the mesh has changed
	};
	UINT32	flags;

	rxDynMeshData	meshData;	// the resulting mesh

public:
	CSGOutput()
		: flags( 0 )
	{}
};

//
//	CSGModel - a solid model on which boolean set operations can be performed.
//
class CSGModel : public mxSpatialProxy
{
	DECLARE_ABSTRACT_CLASS( CSGModel );

public:

	// Sets the pointer to the local-to-world transformation matrix for this solid model.
	// NOTE: the matrix should only be composed from rotation, translation, and, possibly, uniform scaling.

	virtual void	SetTransform( const Matrix4* worldTransform ) = 0;

	// Performs the specified boolean set operation on this model and returns the result.

	virtual void	Apply( const CSGInput& csgInput, CSGOutput &Out ) = 0;

protected:
	virtual	~CSGModel() {}
};

//
//	CSGInfo - used to create new CSG models.
//
struct CSGInfo
{
	const mxMesh *	mesh;	// source geometry

	enum EFlags
	{
		// Flags controlling mesh checking.
		CheckMesh_DegeneratePolys	= BIT( 0 ),
		CheckMesh_Manifold			= BIT( 1 ),
	};
	UINT32	flags;

public:
	CSGInfo()
		: mesh( null )
		, flags( 0 )
	{
	}

	bool IsOk() const
	{
		return 1
			&& mesh != null
			;
	}
};

//
// Creates a new CSG model from the given mesh.
//
extern CSGModel *	NewCSGModel( const CSGInfo& = CSGInfo() );

//=======================================================================














//------------------------------------------------------------------------
//	Defines
//------------------------------------------------------------------------

// for performance testing
#define PROFILE_CSG

//------------------------------------------------------------------------
//	Declarations
//------------------------------------------------------------------------

// Vertex type used for CSG operations.
typedef rxVertex	HVertex;

//
//	HPoly - a convex polygon.
//
ALIGN_16 struct HPoly
{
	enum { MAX_VERTICES = 16 };					// Maximal number of vertices a polygon may have.
	enum { SPLIT_THRESHOLD = MAX_VERTICES-2 };	// Threshold for splitting into two polys when the number of vertices is too big.

	typedef TFixedArray< HVertex, MAX_VERTICES >	VertexList;

public:
	HPoly()
		: numVertices( 0 )
	{}
	HPoly( ENoInit )
	{}

	FORCEINLINE void SetVertex( IndexT index, const HVertex& vertex )
	{
		Assert( index >=0 && index <= SPLIT_THRESHOLD );
		vertices[index] = vertex;
	}

	FORCEINLINE const Vec3D& GetPosition( IndexT vertexIndex ) const
	{
		return vertices[vertexIndex].XYZ;
	}

	FORCEINLINE mxUInt NumVertices() const {
		return numVertices;
	}

	FORCEINLINE void AddVertex( const HVertex& v )
	{
		Assert( numVertices < SPLIT_THRESHOLD );
		vertices[ numVertices++ ] = v;
	}

	FORCEINLINE void SetNext( HPoly* poly )
	{
		next = poly;
	}
	FORCEINLINE const HPoly* GetNext() const
	{
		return next;
	}

	void CalculateBounds( mxBounds &OutBounds ) const
	{
		OutBounds.Clear();
		for( mxUInt iVertex = 0; iVertex < numVertices; iVertex++ )
		{
			OutBounds.AddPoint( vertices[iVertex].XYZ );
		}
	}
	void ExpandBounds( mxBounds &OutBounds ) const
	{
		for( mxUInt iVertex = 0; iVertex < numVertices; iVertex++ )
		{
			OutBounds.AddPoint( vertices[iVertex].XYZ );
		}
	}

	bool PointInside( const Vec3D& point, const FLOAT epsilon = 0.001f ) const;

public:
	mxUInt			numVertices;
	TPtr<HPoly>		next;			// Used for organizing linked lists of polygons.
	VertexList		vertices;
};

typedef Triple< rxIndex >	IndexTriple;

//
//	DynamicMesh
//
struct DynamicMesh
{
	TArray< HVertex >		vertices;
	TArray< IndexTriple >	triangles;

public:
	DynamicMesh()
	{}

	UINT NumTriangles() const { return triangles.Num(); }
	UINT NumVertices() const { return vertices.Num(); }

	void Copy( const mxMesh* mesh );
	
	void Copy( const DynamicMesh& other )
	{
		this->vertices = other.vertices;
		this->triangles = other.triangles;
	}

	void Transform( const Matrix4& mat );

	void Reset()
	{
		vertices.SetNum( 0, false );
		triangles.SetNum( 0, false );
	}

	void Clear()
	{
		vertices.Clear();
		triangles.Clear();
	}

	void ToRenderMesh( rxDynMeshData &OutMeshData )
	{
		OutMeshData.numVertices	= this->vertices.Num();
		OutMeshData.vertices	= this->vertices.Ptr();
		OutMeshData.numIndices	= this->triangles.Num() * 3;
		OutMeshData.indices		= (rxIndex*) this->triangles.Ptr();
	}

private:
	NO_COPY_CONSTRUCTOR( DynamicMesh );
	NO_ASSIGNMENT( DynamicMesh );
};

//
//	BSPNode - represents a single node of a node-storing solid BSP tree.
//
ALIGN_16 struct BSPNode
{
	// This enum describes the allowed BSP node types.
	// NOTE: the order is important (optimized for range checks).
	enum ENodeType
	{
		Internal,	// An internal partitioning node.
		OutCell,	// An outcell leaf node ( representing empty space ).
		InCell,		// An incell leaf node ( representing solid space ).
	};

	TEnum< ENodeType >	type;	// (4) Specifies the node's type.
	mxPlane				plane;	// (16) Splitting plane of the node.

	// Two pointers to children nodes.
	BSPNode *			front;	// (4)
	BSPNode *			back;	// (4)

	// Extent of the node including all its children.
	// Used for frustum culling, coarse side tests, etc.
	mxBounds			bounds;	// (24)

	// List of polygons lying on this node's plane.
	HPoly *				polys;	// (4)

	// Miscellaneous bit flags.
	UINT32				flags;	// (4)

	// 4 + 16 + 4 + 4 + 24 + 4 + 4 = 60 bytes.

public:
			BSPNode( ENoInit )
			{}

			BSPNode( ENodeType theType )
				: type( theType )
				, front( null ), back( null )
				, bounds( Vec3D(0,0,0) )
				, polys( null )
				, flags( null )
			{}

	FORCEINLINE bool IsLeaf() const
	{
		return ( type != ENodeType::Internal );
	}
	FORCEINLINE bool IsInternal() const
	{
		return ( type == ENodeType::Internal );
	}
	FORCEINLINE bool IsSolid() const
	{
		return ( type == ENodeType::InCell );
	}
	FORCEINLINE bool IsSpace() const
	{
		return ( type == ENodeType::OutCell );
	}

	BSPNode * GetContainingNode( const Vec3D& point, const FLOAT epsilon = ON_EPSILON );

	// index can be 0 or 1
	FORCEINLINE BSPNode * GetChild( IndexT index )
	{
		Assert( index >= 0 && index < 2 );
		return &( front[index] );
	}
	FORCEINLINE const BSPNode *	GetChild( IndexT index ) const
	{
		Assert( index >= 0 && index < 2 );
		return &( front[index] );
	}

	// Compute bounds of all polys lying in this node.[slow]
	void CalculateBounds( mxBounds &OutBounds ) const;
};

//------------------------------------------------------------------------
//	Testing & debugging
//------------------------------------------------------------------------

#ifdef PROFILE_CSG

//
//	BSPStats
//
class BSPStats {
public:
	mxUInt	numOrigPolygons;
	mxUInt	numPolygons;	// number of resulting polygons
	mxUInt	numSplits;		// number of cuts caused by BSP

	mxUInt	numInternalNodes;
	//mxUInt	numSolidLeaves, numEmptyLeaves;
	//mxUInt	depth;

public:
	BSPStats()
	{
		Reset();
	}
	void Reset()
	{
		numOrigPolygons = 0;
		numPolygons = 0;
		numSplits = 0;

		numInternalNodes = 0;
		//numSolidLeaves = 0;
		//numEmptyLeaves = 0;
		//depth = 0;

		startTime = sys::GetMilliseconds();
		elapsedTime = 0;
	}
	void Stop()
	{
		elapsedTime = sys::GetMilliseconds() - startTime;
		startTime = 0;
	}

	mxUInt GetElapsedTime() const { return elapsedTime; }

	void Dump();

private:
	mxUInt	startTime;	// for measuring time with timer
	mxUInt	elapsedTime;	// time required to finish task
};

//
//	CSGStats
//
class CSGStats {
public:
	mxUInt	numRecursiveCalls;
	mxUInt	mergeTime;

public:
	CSGStats()
	{
		Reset();
	}
	void Reset()
	{
		numRecursiveCalls = 0;
		mergeTime = 0;

		startTime = sys::GetMilliseconds();
		elapsedTime = 0;
	}
	void Stop()
	{
		elapsedTime = sys::GetMilliseconds() - startTime;
		startTime = 0;
	}

	void Dump();

	mxUInt GetElapsedTime() const { return elapsedTime; }

private:
	mxUInt	startTime;		// for measuring time with timer
	mxUInt	elapsedTime;	// time required to finish task
};

#	define BSP_STATS(x)	x
#	define CSG_STATS(x)	x

#else // if !PROFILE_CSG

#	define BSP_STATS(x)
#	define CSG_STATS(x)

#endif // PROFILE_CSG

//
//	SolidBSP - node-storing solid leaf-labelled BSP tree.
//
class SolidBSP {
public:
				SolidBSP();
				~SolidBSP();

	void		Build( const mxMesh* mesh );
	void		EmitMesh( DynamicMesh &OutMesh );

	// Returns the bounding volume of the entire tree in local space.
	const mxBounds & GetBoundsLocal() const;

	// intersection point is start + dir * scale
	bool RayIntersection( const Vec3D &start, const Vec3D &dir, FLOAT &OutScale ) const;

	void DoCSG( ESetOp setOp, const DynamicMesh& mesh );

	BSPNode *	GetRoot();

	void	Clear();

public:
	enum ESettings {
		RESERVE_NODES = 256,	// how much space to reserve for nodes
		RESERVE_POLYS = 256,	// how much space to reserve for polys
	};

private:
	// Internal functions

	// Converts the given mesh into a linked list of polygons and returns the head of the list.
	HPoly *	BuildPolygonList( const mxMesh* mesh, mxUInt &OutNumPolys );
	HPoly *	BuildPolygonList( const DynamicMesh& mesh, mxUInt &OutNumPolys );

	// Builds a tree from the given linked list of polygons and returns the root of the tree.
	BSPNode * BuildTree_R( HPoly* pFaces );

	// Selects the best splitter polygon from the given linked list of polygons and removes the splitter from the list.
	HPoly *	SelectSplitter( HPoly *& polygons );

	// Partitions the given polygon by the specified plane and returns the resulting polygons.
	void SplitFace( HPoly* inFace, const mxPlane& splitPlane,
					HPoly *& pOutFrontPolys, HPoly *& pOutBackPolys, HPoly *& pOutCoplanars,
					const FLOAT epsilon );

				// Creates a new (internal) BSP node and assigns the given poly.
	BSPNode *	AllocNode( HPoly* splitter );

	void		Discard( BSPNode* node );

	void DiscardPoly( HPoly* poly );
	void DiscardPolyList( HPoly* head );

	// old, slow and buggy version
	bool RayIntersection_R( const BSPNode* node, const Vec3D &start, const Vec3D &end, Vec3D &OutHit ) const;

	bool RayIntersection_R( const BSPNode* node, const Vec3D &start, const Vec3D &dir, FLOAT min, FLOAT max, FLOAT &OutHit ) const;

	void MergeSubtract_R( BSPNode*& pMyNode, BSPNode* pOtherNode );
	void MergeUnion_R	( BSPNode*& pMyNode, BSPNode* pOtherNode );

	void PartitionNodeWithPlane( const mxPlane& splitPlane, BSPNode* node,
								BSPNode*& OutFront, BSPNode*& OutBack );

	void RemoveFacesOutsideNode( HPoly *& inFaces, const BSPNode* pNode );
	void RemoveFacesOutsideNode_R( HPoly* inFaces, const BSPNode* pNode, HPoly *& OutFaces );

	void EmitNode_R( BSPNode* pNode, DynamicMesh &OutMesh );

private:
	TPtr< BSPNode >		root;	// Root node of the entire tree.

	mxMemoryPool< BSPNode >	nodes;	// all nodes are stored here
	mxMemoryPool< HPoly >	polys;	// all polygons are stored here

	// For testing & debugging.
	BSP_STATS( BSPStats  bspStats; )
	CSG_STATS( CSGStats  csgStats; )
};

FORCEINLINE BSPNode * SolidBSP::GetRoot() {
	return this->root;
}

//
//	mxSolid
//
class mxSolid : public CSGModel
{
	DECLARE_CLASS( mxSolid );

public:
			mxSolid();
			~mxSolid();

	// mxSolid interface.

	void	Setup( const CSGInfo& buildInfo );
	void	Shutdown();

	//
	//	Override ( CSGModel ) :
	//
	void	SetTransform( const Matrix4* worldTransform );
	void	Apply( const CSGInput& csgInfo, CSGOutput &Out );

	//
	//	Override ( mxSpatialProxy ) :
	//
	void GetBoundsLocal( mxBounds & OutBounds ) const;
	void GetBoundsWorld( mxBounds & OutBounds ) const;
	bool CastRay( const Vec3D& origin, const Vec3D& direction, FLOAT &fraction ) const;

private:
	const Matrix4 *		mLocalToWorld;
	SolidBSP			bsp;
	DynamicMesh			srcMesh;	// source geometry, modified only when this model is CSG'ed
	DynamicMesh			dynMesh;	// generated mesh (for rendering)
};

}//End of namespace abc

#endif // ! __MESHOK_CSG_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
