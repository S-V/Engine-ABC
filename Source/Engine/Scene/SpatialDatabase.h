/*
=============================================================================
	File:	SpatialDatabase.h
	Desc:	Interfaces for executing spatial queries.
=============================================================================
*/

#ifndef __MX_SCENE_COMPONENT_SPATIAL_DATABASE_H__
#define __MX_SCENE_COMPONENT_SPATIAL_DATABASE_H__

namespace abc {

class mxSceneView;
class mxEntity;

class mxTraceInput;
class mxTraceResult;

class mxVisibleSet;

//
//	mxSpatialDatabase - is used to accelerate spatial queries on the scene.
//
//	Responsibilities: spatial organization of the scene for efficient culling,
//	tracing against visual geometry, etc.
//
//
class mxSpatialDatabase
	: public mxSpatialProxy		// for being able to be inserted into spatial databases (and be attached to entities too)
	, public mxSceneComponent	// this is a scene component
{
public:
	virtual		~mxSpatialDatabase() {}

	//--- Public spatial database interface ---------------------------------------------.

	//
	//	Query.
	//
	virtual void	TraceRay( const mxTraceInput& traceInput, mxTraceResult &OutTraceResult ) = 0;

	virtual void	GetEntitiesInPoint( const Vec3D& point, mxEntityCache &OutEntities ) = 0;
	virtual void	GetEntitiesAlongLine( const Vec3D& start, const Vec3D& end, mxEntityCache &OutEntities ) = 0;
	virtual void	GetEntitiesAlongRay( const Vec3D& origin, const Vec3D& direction, mxEntityCache &OutEntities ) = 0;
	virtual void	GetEntitiesInBox( const Vec3D& min, const Vec3D& max, mxEntityCache &OutEntities ) = 0;
	virtual void	GetEntitiesInSphere( const Vec3D& origin, FLOAT radius, mxEntityCache &OutEntities ) = 0;

	virtual mxEntity *	CastRay( const Vec3D& origin, const Vec3D& direction, Vec3D& hitPosition ) const { return null; }

	//
	// Visibility information and high-level culling.
	//
	virtual mxBool	IsPotentiallyVisible( const mxSceneView& rView, const mxSpatialProxy* obj ) const = 0;
	virtual void	GetVisibleSet( const mxSceneView& rView, mxVisibleSet &OutVisibleSet ) = 0;

	//
	//	Override ( mxSpatialProxy ) :
	//
	void GetBoundsLocal( mxBounds & OutBounds ) const	{ OutBounds = mxBounds::INFINITE_EXTENT; }
	void GetBoundsWorld( mxBounds & OutBounds ) const	{ OutBounds = mxBounds::INFINITE_EXTENT; }
};

} //end of namespace abc

#endif // ! __MX_SCENE_COMPONENT_SPATIAL_DATABASE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
