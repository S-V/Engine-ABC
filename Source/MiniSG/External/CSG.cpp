/*
=============================================================================
	File:	CSG.cpp
	Desc:	CSG-related code.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <MiniSG.h>

namespace abc {

/*================================
			CSGModel
================================*/

DEFINE_ABSTRACT_CLASS( CSGModel, 'CSGM', mxSpatialProxy );

/*
================================
		GetSetOpName
================================
*/
const mxChar* GetSetOpName( ESetOp op )
{
	switch( op )
	{
	case ESetOp::CSG_Difference :
		return TEXT("Difference");

	case ESetOp::CSG_Union :
		return TEXT("Union");

	default:
		;
	}
	Unreachable;
	return null;
}

//------------------------------------------------------------------------
//	Settings
//------------------------------------------------------------------------

// planeside epsilon used for polygon clipping during tree building
const FLOAT SPLIT_THRESHOLD	= 0.0001f;

// planeside epsilon used for polygon clipping during CSG operations
static const FLOAT SPLIT_POLY		= 
										//0.00001f
										0.0001f
										//0.001f
										;

// Use a more precise, albeit a slower polygon clipping routine.
static const bool bUsePrecisePolyClip = false;

// if true then merge trees without testing bounds, etc.;
// this usually introduces less errors into the polygon clipping algorithm.
static const bool bSplitBothHack = false;

// if true then don't clip polys when performing union set operation
static const bool bCSGUnionNoSplitHack = true;

// if true then during ray queries hit polygons with normals that are facing away from the ray origin.
static const bool bHitBackfacingPolys = false;

/*================================
			HPoly
================================*/

bool HPoly::PointInside( const Vec3D& point, const FLOAT epsilon ) const
{
	// Get the first normal to test.
	Vec3D   toPt = point - GetPosition( 0 );
	Vec3D   edge = GetPosition( 1 ) - GetPosition( 0 );
	Vec3D   testCross = edge.Cross( toPt );
//	testCross.Normalize();

	for( mxUInt iVertex = 0; iVertex < NumVertices(); iVertex++ )
	{
		toPt = point - GetPosition( iVertex );
		edge = GetPosition( (iVertex+1) % NumVertices() ) - GetPosition( iVertex );
		Vec3D   cross = edge.Cross( toPt );
	//	cross.Normalize();

		if ( cross.Dot( testCross ) < epsilon ) {
			return false;
		}
	}
	return true;
}

/*================================
		DynamicMesh
================================*/

void DynamicMesh::Copy( const mxMesh* mesh )
{
	Assert( mesh->IsValid() );
	
	this->vertices.SetNum( mesh->numVertices );
	this->triangles.SetNum( mesh->numIndices / 3 );
	
	MemCopy( this->vertices.Ptr(), mesh->vertices, mesh->numVertices * sizeof(rxVertex) );
	MemCopy( this->triangles.Ptr(), mesh->indices, mesh->numIndices * sizeof(rxIndex) );
}

void DynamicMesh::Transform( const Matrix4& mat )
{
	for( UINT iVertex = 0; iVertex < this->vertices.Num(); iVertex++ )
	{
		rxVertex & vertex = this->vertices[ iVertex ];
		mat.TransformVector( vertex.XYZ );
		mat.TransformNormal( vertex.Normal );
		mat.TransformNormal( vertex.Tangent );
	}
}

/*================================
			BSPStats
================================*/

//
//	BSPNode::GetContainingNode
//
BSPNode * BSPNode::GetContainingNode( const Vec3D& point, const FLOAT epsilon )
{
	if ( ENodeType::Internal == this->type )
	{
		const FLOAT dot = plane.Distance( point );
		if ( dot > +epsilon ) {
			return front->GetContainingNode( point );
		}
		if ( dot < -epsilon ) {
			return back->GetContainingNode( point );
		}
	}
    return this;
}

void BSPNode::CalculateBounds( mxBounds &OutBounds ) const
{
	OutBounds.Clear();
	HPoly * face = this->polys;
	while( face )
	{
		face->ExpandBounds( OutBounds );
		face = face->next;
	}
}

/*================================
			BSPStats
================================*/

void BSPStats::Dump()
{
	sys::Print(TEXT( "\n=== BSP statistics ========\n")				);
	sys::Print(TEXT( "Num. Polys(Begin): %u\n"), numOrigPolygons	);
	sys::Print(TEXT( "Num. Polys(End):   %u\n"), numPolygons		);
	sys::Print(TEXT( "Num. Splits:	     %u\n"), numSplits			);
	sys::Print(TEXT( "Num. Inner Nodes:  %u\n"), numInternalNodes	);
	//sys::Print(TEXT( "Num. Solid Leaves: %d\n"), numSolidLeaves		);
	//sys::Print(TEXT( "Num. Empty Leaves: %d\n"), numEmptyLeaves		);
	//sys::Print(TEXT( "Tree Depth:        %d\n"), depth			);
	sys::Print(TEXT( "Time elapsed(msec): %u\n"), elapsedTime		);
	sys::Print(TEXT( "==== End ====================\n")				);
}

/*================================
			CSGStats
================================*/

void CSGStats::Dump()
{
	sys::Print(TEXT( "\n=== CSG statistics ========\n")					);
	sys::Print(TEXT( "Calls:	      %u\n"),		numRecursiveCalls	);
	sys::Print(TEXT( "Merge:	      %u msec\n"),	mergeTime			);
	sys::Print(TEXT( "Time elapsed(msec): %d\n"),	elapsedTime			);
	sys::Print(TEXT( "==== End ====================\n")					);
}

/*================================
			SolidBSP
================================*/

namespace {

static BSPNode  emptyLeaf( BSPNode::ENodeType::OutCell );
static BSPNode  solidLeaf( BSPNode::ENodeType::InCell );

}//End of anonymous namespace

//
//	SolidBSP::SolidBSP
//
SolidBSP::SolidBSP()
{}

//
//	SolidBSP::~SolidBSP
//
SolidBSP::~SolidBSP()
{
	Clear();
}

//
//	SolidBSP::Build
//
void SolidBSP::Build( const mxMesh* mesh )
{
	Clear();

	BSP_STATS( bspStats.Reset() );

	mxUInt  numPolys;
	HPoly * polys = BuildPolygonList( mesh, numPolys );

	BSP_STATS( bspStats.numPolygons = bspStats.numOrigPolygons = numPolys );

//	this->nodes.Resize( mesh->GetNumFaces() );
	this->root = BuildTree_R( polys );

	BSP_STATS( bspStats.Stop(); bspStats.Dump() );
}

//
//	SolidBSP::BuildPolygonList
//
HPoly* SolidBSP::BuildPolygonList( const mxMesh* mesh, mxUInt &OutNumPolys )
{
	OutNumPolys = 0;

	HPoly * prevFace = null;

	// build a poly from each triangle in the mesh...

	for( mxUInt i = 0; i < mesh->numIndices; i += 3 )
	{
		HPoly * pNewPoly = new (this->polys) HPoly(_NoInit);

		++OutNumPolys;

		const rxIndex triangle[3] =
		{
			mesh->indices[ i + 0 ],
			mesh->indices[ i + 1 ],
			mesh->indices[ i + 2 ]
		};

		pNewPoly->numVertices = 3;
		pNewPoly->SetVertex( 0, mesh->vertices[ triangle[0] ] );
		pNewPoly->SetVertex( 1, mesh->vertices[ triangle[1] ] );
		pNewPoly->SetVertex( 2, mesh->vertices[ triangle[2] ] );

		pNewPoly->next = prevFace;
		prevFace = pNewPoly;
	}

	return prevFace;
}

HPoly * SolidBSP::BuildPolygonList( const DynamicMesh& mesh, mxUInt &OutNumPolys )
{
	OutNumPolys = 0;

	HPoly * prevFace = null;

	// build a poly from each triangle in the mesh...

	for( mxUInt iTri = 0; iTri < mesh.triangles.Num(); iTri++ )
	{
		HPoly * pNewPoly = new (this->polys) HPoly(_NoInit);

		++OutNumPolys;

		const IndexTriple currTri = mesh.triangles[ iTri ];

		pNewPoly->numVertices = 3;
		pNewPoly->SetVertex( 0, mesh.vertices[ currTri[0] ] );
		pNewPoly->SetVertex( 1, mesh.vertices[ currTri[1] ] );
		pNewPoly->SetVertex( 2, mesh.vertices[ currTri[2] ] );

		pNewPoly->next = prevFace;
		prevFace = pNewPoly;
	}

	return prevFace;
}

BSPNode * SolidBSP::BuildTree_R( HPoly* pFaces )
{
	HPoly * splitter = SelectSplitter( pFaces );

	BSPNode * pNewNode = AllocNode( splitter );

	HPoly * pFrontPolys = null;
	HPoly * pBackPolys = null;

	HPoly * pCurrPoly = pFaces;

	while( pCurrPoly != null )
	{
		HPoly * pNextPoly = pCurrPoly->next;

		SplitFace( pCurrPoly, pNewNode->plane,
			pFrontPolys, pBackPolys, pNewNode->polys,
			SPLIT_THRESHOLD );

		// Update node bounds.
		pNewNode->polys->ExpandBounds( pNewNode->bounds );
	
		pCurrPoly = pNextPoly;
	}

	pNewNode->front	= pFrontPolys ?	BuildTree_R( pFrontPolys )	: & emptyLeaf;
	pNewNode->back	= pBackPolys ?	BuildTree_R( pBackPolys )	: & solidLeaf;

	pNewNode->bounds.AddBounds( pNewNode->front->bounds );
	pNewNode->bounds.AddBounds( pNewNode->back->bounds );

	return pNewNode;
}

//
//	SolidBSP::SelectSplitter
//
HPoly * SolidBSP::SelectSplitter( HPoly *& polygons )
{
	AssertPtr( polygons );

	//HPoly * currPoly = polygons;
	//HPoly * bestSplitter = null;
	//while ( currPoly != null )
	//{
	//	TODO: ...
	//}

	// Debugging.
	// Take the first poly from the list as a splitter.
	HPoly * splitter = polygons;

	// Remove splitter from the given list of polygons.
	polygons = splitter->next;
	splitter->next = null;

	return splitter;
}

//
//	If the specified faces straddles the splitting plane,
//	then two new faces are created and the original face is deleted.
//
void SolidBSP::SplitFace( HPoly* inFace, const mxPlane& splitPlane,
			HPoly *& pOutFrontPolys, HPoly *& pOutBackPolys, HPoly *& pOutCoplanars,
			const FLOAT epsilon /*  = SPLIT_THRESHOLD */ )
{
	FLOAT  dists[ HPoly::MAX_VERTICES + 4 ];
	TEnum<EPlaneSide,mxUInt>  sides[ HPoly::MAX_VERTICES + 4 ];
	mxUInt  counts[ 3 ] = { 0 };

	// Classify individual vertices with respect to the splitting plane.
	{
		mxUInt iVertex;
		for( iVertex = 0; iVertex < inFace->NumVertices(); iVertex++ )
		{
			FLOAT  fDistance = splitPlane.Distance( inFace->GetPosition(iVertex) );
			dists[ iVertex ] = fDistance;

			if ( fDistance > +epsilon )
			{
				sides[ iVertex ] = PLANESIDE_FRONT;
			}
			else if ( fDistance < -epsilon )
			{
				sides[ iVertex ] = PLANESIDE_BACK;
			}
			else {
				sides[ iVertex ] = PLANESIDE_ON;
			}

			++counts[ sides[ iVertex ] ];
		}

		sides[ iVertex ] = sides[ 0 ];
		dists[ iVertex ] = dists[ 0 ];
	}

	if ( counts[ PLANESIDE_ON ] == inFace->NumVertices() ) {
		PrependItem< HPoly >( pOutCoplanars, inFace );	// the poly is aligned with the splitPlane.
		return;
	}
	if ( 0 == counts[ PLANESIDE_FRONT ] ) {
		PrependItem< HPoly >( pOutBackPolys, inFace );	// the poly is completely behind the splitPlane.
		return;
	}
	if ( 0 == counts[ PLANESIDE_BACK ] ) {
		PrependItem< HPoly >( pOutFrontPolys, inFace );	// the poly is completely in front of the splitPlane.
		return;
	}

	// Straddles the splitPlane - we must clip.

	BSP_STATS( bspStats.numSplits++ );

	const mxUInt  maxPoints = inFace->NumVertices() + 4; // Estimated number of points.

	if ( maxPoints >= HPoly::SPLIT_THRESHOLD )
	{
		Unimplemented2( "polygons with more than 16 vertices" );
	}

	HPoly * frontPoly = new (this->polys) HPoly();
	HPoly * backPoly = new (this->polys) HPoly();

	BSP_STATS( bspStats.numPolygons++ );

	for( mxUInt iVertex = 0; iVertex < inFace->NumVertices(); iVertex++ )
	{
		const HVertex & v1 = inFace->vertices[iVertex];

		if ( sides[iVertex] == PLANESIDE_ON ) {
			frontPoly->AddVertex( v1 );
			backPoly->AddVertex( v1 );
			continue;
		}
		if ( sides[iVertex] == PLANESIDE_FRONT ) {
			frontPoly->AddVertex( v1 );
		}
		if ( sides[iVertex] == PLANESIDE_BACK ) {
			backPoly->AddVertex( v1 );
		}

		if ( (sides[iVertex+1] == PLANESIDE_ON) || (sides[iVertex+1] == sides[iVertex]) ) {
			continue;
		}

		// Generate a split point.
		HVertex  mid;

		const HVertex & v2 = inFace->vertices[ (iVertex+1) % inFace->NumVertices() ];

		if( bUsePrecisePolyClip )	// More precise (and slower).
		{
			// Always calculate the split going from the same side
			// or minor epsilon issues can happen.
			if ( sides[iVertex] == PLANESIDE_FRONT )
			{
				const FLOAT dot = dists[iVertex] / ( dists[iVertex] - dists[iVertex+1] );
				for ( UINT j = 0; j < 3; j++ ) {
					// avoid round off error when possible
					if ( splitPlane.Normal()[j] == 1.0f ) {
						mid[j] = splitPlane.Dist();
					} else if ( splitPlane.Normal()[j] == -1.0f ) {
						mid[j] = -splitPlane.Dist();
					} else {
						mid[j] = v1[j] + dot * ( v2[j] - v1[j] );
					}
				}
				mid.UV.Lerp( v1.UV, v2.UV, dot );
				mid.Normal.Lerp( v1.Normal, v2.Normal, dot );
				mid.Tangent.Lerp( v1.Tangent, v2.Tangent, dot );
			}
			else
			{
				const FLOAT dot = dists[iVertex+1] / ( dists[iVertex+1] - dists[iVertex] );
				for ( UINT j = 0; j < 3; j++ ) {	
					// avoid round off error when possible
					if ( splitPlane.Normal()[j] == 1.0f ) {
						mid[j] = splitPlane.Dist();
					} else if ( splitPlane.Normal()[j] == -1.0f ) {
						mid[j] = -splitPlane.Dist();
					} else {
						mid[j] = v2[j] + dot * ( v1[j] - v2[j] );
					}
				}
				mid.UV.Lerp( v2.UV, v1.UV, dot );
				mid.Normal.Lerp( v2.Normal, v1.Normal, dot );
				mid.Tangent.Lerp( v2.Tangent, v1.Tangent, dot );
			}
		}
		else
		{
			// Simply interpolate new vertex's attributes.
			const FLOAT dot = -( splitPlane.Normal().Dot( v2.XYZ - v1.XYZ ) );
			const FLOAT fraction = ( splitPlane.Normal().Dot( v1.XYZ ) + splitPlane.d ) / dot;
			mid.Lerp( v1, v2, fraction );
		
		}//End of polyclip

		frontPoly->AddVertex( mid );
		backPoly->AddVertex( mid );
	}

	Assert( frontPoly->NumVertices() < maxPoints
		&& backPoly->NumVertices() < maxPoints );

	PrependItem< HPoly >( pOutFrontPolys, frontPoly );
	PrependItem< HPoly >( pOutBackPolys, backPoly );
	DiscardPoly( inFace );
}

BSPNode * SolidBSP::AllocNode( HPoly* splitter )
{
	BSPNode * newNode = new (this->nodes) BSPNode(_NoInit);

	newNode->type = BSPNode::ENodeType::Internal;

	// Initialize splitting plane.
#if 0
	const Vec3D n( (v1 - v2).Cross( p3 - v2 ) );
	newNode->plane.SetNormal( n );
	newNode->plane.SetDist( -( Normal() * v2 ) );
#else
	// counter-clockwise polys are front-facing
	newNode->plane.FromPoints(
		splitter->GetPosition( 2 ),
		splitter->GetPosition( 1 ),
		splitter->GetPosition( 0 )
	);
#endif

	newNode->front = null;
	newNode->back = null;

	// Update node bounds.
	splitter->CalculateBounds( newNode->bounds );

	newNode->polys = splitter;

	newNode->flags = 0;

	BSP_STATS( bspStats.numInternalNodes++ );

	return newNode;
}

void SolidBSP::Discard( BSPNode* node )
{
	(void) node;
}

void SolidBSP::DiscardPoly( HPoly* poly )
{
	(void) poly;
}

void SolidBSP::DiscardPolyList( HPoly* head )
{
	(void) head;
}

bool SolidBSP::RayIntersection( const Vec3D &start, const Vec3D &dir, FLOAT &OutScale ) const
{
	Assert( dir.IsNormalized() );

#if 1
	const Vec3D end = start + dir * MAX_SCENE_SIZE;
	Vec3D vHit;
	if( RayIntersection_R( this->root.get(), start, end, vHit ) ) {
		OutScale = (vHit - start).Length();
		return true;
	} else {
		return false;
	}
#else
	return this->root->bounds.RayIntersection( start, dir, OutScale );
	FLOAT min = 0.0f;
	FLOAT max = +MAX_SCENE_SIZE;
	return RayIntersection_R( this->root.get(), start, dir, min, max, OutScale );
#endif
}

bool SolidBSP::RayIntersection_R( const BSPNode* node, const Vec3D &start, const Vec3D &end, Vec3D &OutHit ) const
{
	AssertPtr( node );

	if ( node->IsLeaf() ) {
		return false;
	}

	const FLOAT  dStart = node->plane.Distance( start );
	const FLOAT  dEnd   = node->plane.Distance( end );

	if ( dStart > 0 && dEnd > 0 ) {
		return RayIntersection_R( node->front, start, end, OutHit );
	}
	else if ( dStart < 0 && dEnd < 0 ) {
		return RayIntersection_R( node->back, start, end, OutHit );
	}

	// The line spans the splitting plane.

	// Variables to hold which side to traverse first and which second.
	BSPNode *  pFirstSide = null;
	BSPNode *  pSecondSide = null;

	// If we know that one is positive and the other is negative, and dStart < dEnd, so 
	// dStart is negative and dEnd is positive.
	if ( dStart < dEnd )
	{
		pFirstSide = node->back;	// The back side contains the start point so the back will be the first.
		pSecondSide = node->front;
	}
	// If dEnd is negative and dStart is positive.
	else if ( dEnd < dStart )
	{
		pFirstSide = node->front;	// The front side contains the start point so the front will be the first.
		pSecondSide = node->back;
	}
	else {
		pFirstSide = node->front;
		pSecondSide = node->back;
	}

	Vec3D  midPoint;	// intersection
	const FLOAT fraction = dStart / ( dStart - dEnd );
	midPoint = start + (end - start) * fraction;

	// Check the first side.
	if ( RayIntersection_R( pFirstSide, start, midPoint, OutHit ) ) {
		return true;
	}

	if( !bHitBackfacingPolys && dStart < 0.0f )
		goto L_CheckFartherSide;

	{
		const HPoly * pCurrPoly = node->polys;
		while( pCurrPoly != null )
		{
			if ( pCurrPoly->PointInside( midPoint ) )
			{
				OutHit = midPoint;
				return true;
			}
			pCurrPoly = pCurrPoly->next;
		}
	}

L_CheckFartherSide:
	// Check the second side.
	return RayIntersection_R( pSecondSide, midPoint, end, OutHit );
}

//
//	SolidBSP::RayIntersection_R
//
//	Intersects the given ray/segment R(t) = origin + t * direction, where min <= t <= max,
//	against the solid-leaf BSP tree, returning the time 'OutHit' of the first intersection
//	with a solid leaf, if any.
//
bool SolidBSP::RayIntersection_R( const BSPNode* node, const Vec3D &start, const Vec3D &dir, FLOAT min, FLOAT max, FLOAT &OutHit ) const
{
	Unimplemented;
#if 1
	if( node->IsInternal() )
	{
		FLOAT denom = node->plane.Normal().Dot( dir );	// cos( plane.N^Dir )
		FLOAT dist = node->plane.Distance( start ); //node->plane.d - node->plane.Normal().Dot( start );
		UINT nearIndex = (dist > 0.0f) ? 1 : 0;	// '1' if start in front of plane, '0' otherwise

		// If denom is zero, ray runs parallel to plane.
		// In this case, just fall through to visit the near side (the one p lies on).
		if( denom != 0.0f )
		{
			FLOAT t = dist / denom;
			if( 0.0f <= t && t <= max )
			{
				if( t >= min )
				{
					// Straddling, visit near side
					return RayIntersection_R( node->GetChild( nearIndex ), start, dir, min, t, OutHit );
				}
				else
				{
					// 0 <= t < min, visit far side
					return RayIntersection_R( node->GetChild( 1 ^ nearIndex ), start, dir, t, min, OutHit );
				}
			}
			else {
			//	Unimplemented;
				return false;
			}
		}
		else {
			return RayIntersection_R( node->GetChild( nearIndex ), start, dir, min, max, OutHit );
		}
	}
	else
	{
		if( node->IsSolid() )
		{
			OutHit = min;
			return true;
		}
	}

	return false;
#else
#endif
}

const mxBounds & SolidBSP::GetBoundsLocal() const
{
	return this->root->bounds;
}

//
//	SolidBSP::Clear
//
void SolidBSP::Clear()
{
	this->root = null;
	this->nodes.Clear();
	this->polys.Clear();

	BSP_STATS( bspStats.Reset() );
	CSG_STATS( csgStats.Reset() );
}

void SolidBSP::DoCSG( ESetOp setOp, const DynamicMesh& mesh )
{
	CSG_STATS( csgStats.Reset(); );

	mxUInt  numPolys;
	HPoly * polys = BuildPolygonList( mesh, numPolys );
	BSPNode * node = BuildTree_R( polys );

	switch( setOp )
	{
	case ESetOp::CSG_Difference :
		this->MergeSubtract_R( this->root.get_ref(), node );
		break;

	case ESetOp::CSG_Union :
		this->MergeUnion_R( this->root.get_ref(), node );
		break;

	default:
		Unimplemented;
	}//switch

	CSG_STATS( csgStats.Stop() );
	CSG_STATS( csgStats.Dump() );
}

//
//	SolidBSP::MergeSubtract_R
//
void SolidBSP::MergeSubtract_R( BSPNode*& pMyNode, BSPNode* pOtherNode )
{
	if ( pMyNode->IsSolid() ) {
		pMyNode = pOtherNode;
		return;
	}
	if ( pMyNode->IsSpace() ) {
		Discard( pOtherNode );
		return;
	}

	const EPlaneSide  eRelation = pOtherNode->bounds.PlaneSide( pMyNode->plane );

	if ( eRelation == PLANESIDE_FRONT ) {
		MergeSubtract_R( pMyNode->front, pOtherNode );
	}
	else if ( eRelation == PLANESIDE_BACK ) {
		MergeSubtract_R( pMyNode->back, pOtherNode );
	}
	else // SIDE_CROSS
	{
		// Clip my polygons in the first node with the other tree.
		//TODO: move this into PartitionNodeWithPlane() when it's debugged.
		// Perform clipping only if polygons from both nodes intersect each other !

		RemoveFacesOutsideNode( pMyNode->polys, pOtherNode );

		// Partition the other tree and merge with the corresponding pieces.
		BSPNode * t2_partitioned_Front = null;
		BSPNode * t2_partitioned_Back = null;

		PartitionNodeWithPlane( pMyNode->plane, pOtherNode,
				t2_partitioned_Front, t2_partitioned_Back );

		MergeSubtract_R( pMyNode->front, t2_partitioned_Front );
		MergeSubtract_R( pMyNode->back, t2_partitioned_Back );
	}
}

//
//	SolidBSP::MergeUnion_R
//
void SolidBSP::MergeUnion_R( BSPNode*& pMyNode, BSPNode* pOtherNode )
{
	if ( pMyNode->IsSolid() ) {
		Discard( pOtherNode );
		return;
	}
	if ( pMyNode->IsSpace() ) {
		pMyNode = pOtherNode;
		return;
	}

	const EPlaneSide  eRelation = pOtherNode->bounds.PlaneSide( pMyNode->plane );

	if ( eRelation == PLANESIDE_FRONT ) {
		MergeUnion_R( pMyNode->front, pOtherNode );
	}
	else if ( eRelation == PLANESIDE_BACK ) {
		MergeUnion_R( pMyNode->back, pOtherNode );
	}
	else // SIDE_CROSS
	{
		// Clip my polygons in the first node with the other tree.
		//TODO: move this into PartitionNodeWithPlane() when it's debugged.
		// Perform clipping only if polygons from both nodes intersect each other !

		MX_HACK("we don't remove polys inside the additive model because they cannot be seen")
	//	if( ! bCSGUnionNoSplitHack )
	//	RemoveFacesInsideNode( pMyNode->polys, pOtherNode );

		// Partition the other tree and merge with the corresponding pieces.
		BSPNode * t2_partitioned_Front = null;
		BSPNode * t2_partitioned_Back = null;

		PartitionNodeWithPlane( pMyNode->plane, pOtherNode,
				t2_partitioned_Front, t2_partitioned_Back );

		MergeUnion_R( pMyNode->front, t2_partitioned_Front );
		MergeUnion_R( pMyNode->back, t2_partitioned_Back );
	}
}

//
//	SolidBSP::PartitionNodeWithPlane
//
void SolidBSP::PartitionNodeWithPlane( const mxPlane& splitPlane, BSPNode* node,
		BSPNode*& OutFront, BSPNode*& OutBack )
{
	CSG_STATS( csgStats.numRecursiveCalls++ );

	if ( node->IsLeaf() ) {
		OutFront = node;
		OutBack = node;
		return;
	}

	// Check for nodes with zero polygons.
	if( ! bSplitBothHack )
	{
		if ( ! node->polys ) {
			sys::Warning( "\nInternal node with no polygons! Shouldn't happen!" );
		}
	}

	const FLOAT  dot = splitPlane.Normal().Dot( node->plane.Normal() );

	// Variable to hold if the normal vectors are facing the same direction.
	const bool  bNormalsInOneDir = ( dot > 0.0f );

	HPoly * frontPolys = null;
	HPoly * backPolys = null;
	HPoly * coplanars = null;

	HPoly * currFace = node->polys;
	while ( currFace != null )
	{
		HPoly * next = currFace->next;

		SplitFace( currFace, splitPlane,
			frontPolys, backPolys, coplanars,
			SPLIT_POLY );

		currFace = next;
	}

	if( bSplitBothHack )
	{
		goto L_Split_Both;	//HACK to compensate for finite numerical precision
	}

	if ( frontPolys && backPolys ) {
		Assert( null == coplanars );
		goto L_Split_Both;
	}

	// If planes are coplanar.
	if ( !frontPolys && !backPolys )
	{
		Assert( coplanars != null );
		// case "On"
		if ( bNormalsInOneDir )
		{
			// "parallel-on"
			OutFront = node->front;
			OutBack = node->back;
		}
		else
		{
			// "antiparallel-on"
			OutFront = node->back;
			OutBack = node->front;
		}
		return;
	}//End of case if ( !frontPolys && !backPolys )

	if( coplanars != null )
	{
		sys::DbgOut( TEXT("\nWarning: cracks and holes appeared!") );
		
		//TODO: do something with coplanar polys!
		coplanars = null; // HACK: <= set to null to avoid further negative asserts
	}

	if ( ! frontPolys )
	{
		Assert2( backPolys, "All polygons of the node must be behind the splitting splitPlane" );
		Assert2( coplanars == null, "Coplanar polys are not supported!" );

		node->polys = backPolys;

		if ( bNormalsInOneDir )
		{
			// Only the front child of the node needs to be partitioned.
			BSPNode *  partitioned_front_F = null;
			BSPNode *  partitioned_front_B = null;

			PartitionNodeWithPlane( splitPlane, node->front, partitioned_front_F, partitioned_front_B );

			OutFront = partitioned_front_F;
			OutFront->bounds = node->bounds;// TODO: <= is this needed ?

			OutBack = node;
			OutBack->front = ( partitioned_front_B );
			// node->backChild remains intact...
		}
		else
		{
			// Only the back child of the node is partitioned.
			BSPNode *  partitioned_back_F = null;
			BSPNode *  partitioned_back_B = null;

			PartitionNodeWithPlane( splitPlane, node->back, partitioned_back_F, partitioned_back_B );

			OutFront = partitioned_back_F;
			OutFront->bounds = node->bounds; // TODO: <= is this needed ?

			OutBack = node;
			OutBack->back = ( partitioned_back_B );
			// node->frontChild remains intact...
		}
		return;
	}//End of case "No front polys"


	if ( ! backPolys )
	{
		Assert2( frontPolys, "All the polygons of the node must be in front of the splitting splitPlane" );
		Assert2( coplanars == null, "Coplanar polys are not supported!" );

		node->polys = frontPolys;

		if ( bNormalsInOneDir )
		{
			// Only the back child of the node needs to be partitioned.
			BSPNode *  partitioned_back_F = null;
			BSPNode *  partitioned_back_B = null;

			PartitionNodeWithPlane( splitPlane, node->back, partitioned_back_F, partitioned_back_B );

			OutFront = node;
			OutFront->back = ( partitioned_back_F );
			// node->frontChild remains intact...

			OutBack = partitioned_back_B;
			OutBack->bounds = node->bounds;//TODO: <= is this needed
		}
		else
		{
			// Only the front child of the node has to be partitioned.
			BSPNode *  partitioned_front_F = null;
			BSPNode *  partitioned_front_B = null;

			PartitionNodeWithPlane( splitPlane, node->front, partitioned_front_F, partitioned_front_B );

			OutFront = node;
			OutFront->front = ( partitioned_front_F );
			// node->backChild remains intact...

			OutBack = partitioned_front_B;
			OutBack->bounds = node->bounds;// TODO: <= is this needed ?
		}
		return;
	}//End of case "No polys behind the splitPlane"


L_Split_Both:

	// Split both children of the node.

	OutFront = new (nodes) BSPNode(_NoInit);
	MemSet( OutFront, 0, sizeof( BSPNode ) );

	OutFront->plane = node->plane;
	OutFront->polys = frontPolys;
	OutFront->bounds = node->bounds;


	OutBack = new (nodes) BSPNode(_NoInit);
	MemSet( OutBack, 0, sizeof( BSPNode ) );

	OutBack->plane = node->plane;
	OutBack->polys = backPolys;
	OutBack->bounds = node->bounds;


	BSPNode *  partitioned_front_F = null;
	BSPNode *  partitioned_front_B = null;

	BSPNode *  partitioned_back_F = null;
	BSPNode *  partitioned_back_B = null;

	PartitionNodeWithPlane( splitPlane, node->front, partitioned_front_F, partitioned_front_B );
	PartitionNodeWithPlane( splitPlane, node->back, partitioned_back_F, partitioned_back_B );

	OutFront->front = ( partitioned_front_F );
	OutFront->back = ( partitioned_back_F );

	OutBack->front = ( partitioned_front_B );
	OutBack->back = ( partitioned_back_B );
}

//
//	SolidBSP::RemoveFacesOutsideNode
//
void SolidBSP::RemoveFacesOutsideNode( HPoly *& inFaces, const BSPNode* pNode )
{
	HPoly * facesToSplit = null;

	// Construct bounding boxes for each face and check if splits are necessary.
	HPoly * face = inFaces;
	while( face )
	{
		HPoly * next = face->next;

		mxBounds  polyBounds;
		face->CalculateBounds( polyBounds );
#if 1
		if ( pNode->bounds.IntersectsBounds( polyBounds ) )
		{
			PrependItem< HPoly >( facesToSplit, face );
		}
#else
		mxBounds  nodeBounds;
		pNode->CalculateBounds( nodeBounds );
		if ( nodeBounds.IntersectsBounds( polyBounds ) )
		{
			PrependItem< HPoly >( facesToSplit, face );
		}
#endif
		face = next;
	}

	if( facesToSplit )
	{
		HPoly * resultFaces = null;
		RemoveFacesOutsideNode_R( facesToSplit, pNode, resultFaces );
		inFaces = resultFaces;
	}
}

//
//	SolidBSP::RemoveFacesOutsideNode_R
//
void SolidBSP::RemoveFacesOutsideNode_R( HPoly* inFaces, const BSPNode* pNode, HPoly *& OutFaces )
{
	if( pNode->IsInternal() )
	{
		HPoly * frontPolys = null;
		HPoly * backPolys = null;
		HPoly * face = inFaces;
		while( face != null )
		{
			HPoly * next = face->next;

			MX_FIXME(" what to do with coplanars?")

			SplitFace( face, pNode->plane,
				frontPolys, backPolys, OutFaces,
				SPLIT_POLY );

			face = next;
		}

		if ( frontPolys ) {
			RemoveFacesOutsideNode_R( frontPolys, pNode->front, OutFaces );
		}
		if ( backPolys ) {
			RemoveFacesOutsideNode_R( backPolys, pNode->back, OutFaces );
		}
	}
	else if( pNode->IsSolid() )
	{
		// Prepend inFaces to OutFaces.
		HPoly * poly = inFaces;
		while( poly )
		{
			HPoly * next = poly->next;
			PrependItem< HPoly >( OutFaces, poly );
			poly = next;
		}
	}
	else {
		// The node represents the empty space.
		DiscardPolyList( inFaces );
	}
}

//
//	SolidBSP::EmitMesh
//
void SolidBSP::EmitMesh( DynamicMesh &OutMesh )
{
	DEBUG_CODE( mxUInt	startTime = sys::GetMilliseconds() );

	OutMesh.Reset();
	EmitNode_R( this->root, OutMesh );

	DEBUG_CODE(
		mxUInt	elapsedTime = sys::GetMilliseconds() - startTime;
		sys::Print("\nMesh Gen: %u msec\n", elapsedTime );

		sys::Print("\nNew mesh: verts: %u, tris: %u\n",
				OutMesh.vertices.Num(), OutMesh.triangles.Num() );
	);

/*
	DEBUG_CODE(
		sys::Print("\nVertices:\n");
		for( UINT i=0; i<OutMesh.vertices.Num(); i++ )
		{
			sys::Print("V[%u] = %s\n", i, OutMesh.vertices[i].XYZ.ToString() );
		}

		sys::Print("\nIndices:\n");
		for( UINT i=0; i<OutMesh.triangles.Num(); i++ )
		{
			sys::Print("%u %u %u\n",
				OutMesh.triangles[i].iA,OutMesh.triangles[i].iB,OutMesh.triangles[i].iC );
		}
	)
*/
}

//
//	CreateRenderVertex - creates a new vertex, appends to the mesh and returns a new index to that vertex.
//
rxIndex CreateRenderVertex( const HVertex& v, DynamicMesh &OutMesh )
{
	HVertex & rNewVertex = OutMesh.vertices.Alloc();

	rNewVertex.XYZ		= v.XYZ;
	rNewVertex.Normal	= v.Normal;
	rNewVertex.Tangent	= v.Tangent;
	rNewVertex.UV		= v.UV;

	return OutMesh.vertices.Num() - 1;
}

void SolidBSP::EmitNode_R( BSPNode* pNode, DynamicMesh &OutMesh )
{
#if 1
	if( pNode->IsInternal() )
	{
		// TODO: don't emit this node if the data in this node hasn't been changed.

		EmitNode_R( pNode->back, OutMesh );
		EmitNode_R( pNode->front, OutMesh );

		// Update bounding box of this node.
		pNode->bounds.SetZero();
		pNode->bounds.AddBounds( pNode->front->bounds );
		pNode->bounds.AddBounds( pNode->back->bounds );

		// Loop through all faces of this node.
		const HPoly * poly = pNode->polys;
		while( poly != null )
		{
			Assert( poly->NumVertices() >= 3 );

			const UINT numTriangles = poly->NumVertices() - 2;

			// Triangulate the current convex polygon...
			
			const HVertex & basePoint = poly->vertices[ 0 ];

			// Update bounding box of this node.
			pNode->bounds.AddPoint( basePoint.XYZ );

			const rxIndex iBasePoint = CreateRenderVertex( basePoint, OutMesh );

			for ( UINT i = 1; i < numTriangles + 1; i++ )
			{
				IndexTriple & rNewTriangle = OutMesh.triangles.Alloc();

				rNewTriangle.iA = iBasePoint;
				rNewTriangle.iB = CreateRenderVertex( poly->vertices[ i ],	OutMesh );
				rNewTriangle.iC = CreateRenderVertex( poly->vertices[ i+1 ],OutMesh );

				// Update bounding box of this node.
				pNode->bounds.AddPoint( poly->vertices[ i ].XYZ );
				pNode->bounds.AddPoint( poly->vertices[ i+1 ].XYZ );
			}

			poly = poly->GetNext();
		}
	}
#else
	if( pNode->IsInternal() )
	{
		// TODO: don't emit this node if the data in this node hasn't been changed.

		EmitNode_R( pNode->back, OutMesh );
		EmitNode_R( pNode->front, OutMesh );

		// Update bounding box of this node.
		pNode->bounds.SetZero();
		pNode->bounds.AddBounds( pNode->front->bounds );
		pNode->bounds.AddBounds( pNode->back->bounds );

		// Loop through all faces of this node.
		const HPoly * poly = pNode->polys;
		while( poly != null )
		{
			Assert( poly->NumVertices() >= 3 );

			const UINT numTriangles = poly->NumVertices() - 2;

			// Triangulate the current convex polygon...
			
			const HVertex & basePoint = poly->vertices[ 0 ];

			for ( UINT i = 1; i < numTriangles + 1; i++ )
			{
				IndexTriple & rNewTriangle = OutMesh.triangles.Alloc();

				rNewTriangle.iA = CreateRenderVertex( basePoint,			OutMesh );
				rNewTriangle.iB = CreateRenderVertex( poly->vertices[ i ],	OutMesh );
				rNewTriangle.iC = CreateRenderVertex( poly->vertices[ i+1 ],OutMesh );

				// Update bounding box of this node.
				pNode->bounds.AddPoint( basePoint.XYZ );
				pNode->bounds.AddPoint( poly->vertices[ i ].XYZ );
				pNode->bounds.AddPoint( poly->vertices[ i+1 ].XYZ );
			}

			poly = poly->GetNext();
		}
	}
#endif
}

/*================================
			mxSolid
================================*/

DEFINE_CLASS( mxSolid, 'BSPS', CSGModel );

mxSolid::mxSolid()
	: mLocalToWorld( null )
{
	this->hitFilterMask = HM_Solid;
}

mxSolid::~mxSolid()
{
	Shutdown();
}

void mxSolid::Setup( const CSGInfo& buildInfo )
{
	Assert( buildInfo.IsOk() );

	this->srcMesh.Copy( buildInfo.mesh );
	this->bsp.Build( buildInfo.mesh );
}

void mxSolid::Shutdown()
{
	this->mLocalToWorld = null;
	this->bsp.Clear();
	this->srcMesh.Clear();
	this->srcMesh.Clear();
}

void mxSolid::SetTransform( const Matrix4* worldTransform )
{
	mLocalToWorld = worldTransform;
}

void mxSolid::Apply( const CSGInput& csgInput, CSGOutput &Out )
{
	Assert( csgInput.IsOk() );

	mxSolid * other = checked_cast< mxSolid*, CSGModel* >( csgInput.operand );

	// Identify the 'lesser' solid (i.e. less complex, with fewer planes, verts, etc);
	// the second operand is assumed to be the 'lesser' solid.

	// Compute the matrix to transform the 'lesser' solid into my local coordinate space, where my bsp was built.

	Matrix4  mat;
	if( other->mLocalToWorld ) {
		mat = *( other->mLocalToWorld );
	} else {
		mat = Matrix4::mat4_identity;
	}

	if( this->mLocalToWorld ) {
		mat *= this->mLocalToWorld->Inverse();
	}

	// transform the less complex geometry

	other->dynMesh.Copy( other->srcMesh );
	other->dynMesh.Transform( mat );

	DEBUG_CODE(
	sys::Print(TEXT("SetOp '%s': %u tris <- %u tris\n"),
		GetSetOpName( csgInput.type ), this->srcMesh.NumTriangles(), other->srcMesh.NumTriangles() );
	);

	this->bsp.DoCSG( csgInput.type, other->dynMesh );

	this->bsp.EmitMesh( this->srcMesh );


	Out.flags = CSGOutput::EFlags::MeshChanged;
	this->srcMesh.ToRenderMesh( Out.meshData );
}

void mxSolid::GetBoundsLocal( mxBounds & OutBounds ) const
{
	OutBounds = this->bsp.GetBoundsLocal();
}

void mxSolid::GetBoundsWorld( mxBounds & OutBounds ) const
{
	mxBounds  aabbWorld;
	this->GetBoundsLocal( aabbWorld );

	if( mLocalToWorld ) {
		aabbWorld.TrasformSelf( *mLocalToWorld );
	}

	OutBounds = aabbWorld;
}

bool mxSolid::CastRay( const Vec3D& origin, const Vec3D& direction, FLOAT &fraction ) const
{
	Vec3D localOrigin( origin );
	Vec3D localDirection( direction );

	if( mLocalToWorld ) {
		Matrix4 invLocalToWorld( mLocalToWorld->Inverse() );
		invLocalToWorld.TransformVector( localOrigin );
		invLocalToWorld.TransformNormal( localDirection );
		localDirection.Normalize();
	}

	return this->bsp.RayIntersection( localOrigin, localDirection, fraction );
}

/*
================================
		NewCSGModel
================================
*/
CSGModel * NewCSGModel( const CSGInfo& buildInfo )
{
	Assert( buildInfo.IsOk() );
	mxSolid * pNewSolid = MX_NEW mxSolid();
	pNewSolid->Setup( buildInfo );
	return pNewSolid;
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
