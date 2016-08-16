/*
=============================================================================
	File:	SpatialDatabase_Simple.h
	Desc:	Spatial database that uses no acceleration, no spatial subdivision
			structures and should only be used for debugging.
=============================================================================
*/

#ifndef __MX_SCENE_COMPONENT_SPATIAL_DATABASE_SIMPLE_H__
#define __MX_SCENE_COMPONENT_SPATIAL_DATABASE_SIMPLE_H__

namespace abc {

//
//	mxSpatialDatabase_Simple
//
//	No spatial subdivision.
//	All entities are processed sequentially.
//	(Uses view frustum culling only.)
//
class mxSpatialDatabase_Simple : public mxSpatialDatabase {
public:
			mxSpatialDatabase_Simple();
			~mxSpatialDatabase_Simple();

	virtual void	Setup( mxScene& theParentScene );
	virtual void	Close();

	//
	//	Query.
	//
	virtual void	TraceRay( const mxTraceInput& traceInput, mxTraceResult &OutTraceResult );

	virtual void	GetEntitiesInPoint( const Vec3D& point, mxEntityCache &OutEntities );
	virtual void	GetEntitiesAlongLine( const Vec3D& start, const Vec3D& end, mxEntityCache &OutEntities );
	virtual void	GetEntitiesAlongRay( const Vec3D& origin, const Vec3D& direction, mxEntityCache &OutEntities );
	virtual void	GetEntitiesInBox( const Vec3D& min, const Vec3D& max, mxEntityCache &OutEntities );
	virtual void	GetEntitiesInSphere( const Vec3D& origin, FLOAT radius, mxEntityCache &OutEntities );

	virtual mxEntity *	CastRay( const Vec3D& origin, const Vec3D& direction, Vec3D& hitPosition ) const;

	//
	// Visibility information and high-level culling.
	//
	virtual mxBool	IsPotentiallyVisible( const mxSceneView& rView, const mxSpatialProxy* obj ) const;
	virtual void	GetVisibleSet( const mxSceneView& rView, mxVisibleSet &OutVisibleSet );

	//
	//	Override ( mxSceneComponent ) :
	//
	virtual void	Add( mxEntity* entity );
	virtual void	Remove( mxEntity* entity );

	//
	//	Override ( mxSpatialProxy ) :
	//
	void GetBoundsLocal( mxBounds & OutBounds ) const;
	void GetBoundsWorld( mxBounds & OutBounds ) const;
	bool CastRay( const Vec3D& origin, const Vec3D& direction, FLOAT &fraction ) const;

private:
	TArray< mxSpatialProxy* >	objects;
	mxBounds					bounds;
};

} //end of namespace abc

#endif // ! __MX_SCENE_COMPONENT_SPATIAL_DATABASE_SIMPLE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
