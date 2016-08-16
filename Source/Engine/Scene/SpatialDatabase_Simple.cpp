/*
=============================================================================
	File:	SpatialDatabase_Simple.cpp
	Desc:	Spatial database that uses no acceleration, no spatial subdivision
			structures and should only be used for debugging.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>

namespace abc {

/*================================
	mxSpatialDatabase_Simple
================================*/

mxSpatialDatabase_Simple::mxSpatialDatabase_Simple()
	: bounds( mxBounds::INFINITE_EXTENT )
{
}

mxSpatialDatabase_Simple::~mxSpatialDatabase_Simple()
{
}

void mxSpatialDatabase_Simple::Setup( mxScene& theParentScene )
{
	mxSpatialDatabase::Setup( theParentScene );

	//this->bounds = theParentScene.GetBounds();
	this->bounds = mxBounds::INFINITE_EXTENT;
}

void mxSpatialDatabase_Simple::Close()
{
	mxSpatialDatabase::Close();
}

void mxSpatialDatabase_Simple::TraceRay( const mxTraceInput& traceInput, mxTraceResult &OutTraceResult )
{
	Unimplemented;
}

void mxSpatialDatabase_Simple::GetEntitiesInPoint( const Vec3D& point, mxEntityCache &OutEntities )
{
	//for ( mxUInt iObject = 0; iObject < this->objects.Num(); iObject++ )
	//{
	//	mxSpatialProxy * pObject = this->objects[ iObject ];

	//	Vec3D intersection;
	//	if ( pObject->GetSpatialProxy()->GetBoundsWorld().ContainsPoint( point ) ) {
	//		OutEntities.Append( pObject );
	//	}
	//}
	Unimplemented;
}

void mxSpatialDatabase_Simple::GetEntitiesAlongLine( const Vec3D& start, const Vec3D& end, mxEntityCache &OutEntities )
{
	//for ( mxUInt iObject = 0; iObject < this->objects.Num(); iObject++ )
	//{
	//	mxSpatialProxy * pObject = this->objects[ iObject ];

	//	if ( pObject->GetSpatialProxy()->GetBoundsWorld().LineIntersection( start, end ) )
	//	{
	//		OutEntities.Append( pObject );
	//	}
	//}
	Unimplemented;
}

void mxSpatialDatabase_Simple::GetEntitiesAlongRay( const Vec3D& origin, const Vec3D& direction, mxEntityCache &OutEntities )
{
	//for ( mxUInt iObject = 0; iObject < this->objects.Num(); iObject++ )
	//{
	//	mxSpatialProxy * pObject = this->objects[ iObject ];

	//	Vec3D intersection;
	//	if ( pObject->GetSpatialProxy()->CastRay( origin, direction, intersection ) )
	//	{
	//		OutEntities.Append( pObject );
	//	}
	//}
	Unimplemented;
}

void mxSpatialDatabase_Simple::GetEntitiesInBox( const Vec3D& min, const Vec3D& max, mxEntityCache &OutEntities )
{
	//for ( mxUInt iObject = 0; iObject < this->objects.Num(); iObject++ )
	//{
	//	mxSpatialProxy * pObject = this->objects[ iObject ];

	//	if ( pObject->GetSpatialProxy()->GetBoundsWorld().IntersectsBounds( mxBounds( min, max ) ) )
	//	{
	//		OutEntities.Append( pObject );
	//	}
	//}
	Unimplemented;
}

void mxSpatialDatabase_Simple::GetEntitiesInSphere( const Vec3D& origin, FLOAT radius, mxEntityCache &OutEntities )
{
	//for ( mxUInt iObject = 0; iObject < this->objects.Num(); iObject++ )
	//{
	//	mxSpatialProxy * pObject = this->objects[ iObject ];

	//	Vec3D     extreme( radius, radius, radius );
	//	mxBounds  bounds( origin - extreme, origin + extreme );
	//	if ( pObject->GetSpatialProxy()->GetBoundsWorld().IntersectsBounds( bounds ) )
	//	{
	//		OutEntities.Append( pObject );
	//	}
	//}
	Unimplemented;
}

//
//	mxSpatialDatabase_Simple::IsPotentiallyVisible
//
mxBool mxSpatialDatabase_Simple::IsPotentiallyVisible( const mxSceneView& view, const mxSpatialProxy* obj ) const
{
#if 0
	mxBounds  worldBounds;
	obj->GetBoundsWorld( worldBounds );
	return view.GetFrustum().IntersectsAABB( worldBounds );
#else
	Sphere  s;
	obj->GetBoundingSphereWorld( s );
	return view.GetFrustum().IntersectSphere( s );
#endif
}

//
//	mxSpatialDatabase_Simple::GetVisibleSet
//
void mxSpatialDatabase_Simple::GetVisibleSet( const mxSceneView& view, mxVisibleSet &OutVisibleSet )
{
	// Clear the output visible set first.
	OutVisibleSet.Empty();

	// Find all (potentially) visible objects.
	for ( IndexT iObject = 0; iObject < this->objects.Num(); iObject++ )
	{
		mxSpatialProxy * pObject = this->objects[ iObject ];

		if ( IsPotentiallyVisible( view, pObject ) ) {
			OutVisibleSet.Add( pObject->GetOwner() );
		}
	}
}

void mxSpatialDatabase_Simple::Add( mxEntity* entity )
{
	AssertPtr( entity );
#if 0
	TComponentPtr ptr( entity->FindComponent( mxSpatialProxy::Type ) );
	if ( ptr.IsValid() )
	{
		mxSpatialProxy * spatialProxy = checked_cast< mxSpatialProxy*, mxEntityComponent* >( ptr );
		this->objects.Append( spatialProxy );
	}
#else
	if ( mxSpatialProxy* spatialProxy = entity->GetSpatialProxy() )
	{
		this->objects.Append( spatialProxy );
	}
#endif
}

void mxSpatialDatabase_Simple::Remove( mxEntity* entity )
{
	AssertPtr( entity );
	Unimplemented;
	//TComponentPtr ptr( entity->FindComponent( mxSpatialProxy::Type ) );
	//if ( ptr.IsValid() )
	//{
	//	mxSpatialProxy * spatialProxy = checked_cast< mxSpatialProxy*, mxEntityComponent* >( ptr );
	//	const bool bRemoved = this->objects.Remove( spatialProxy );
	//	Assert( bRemoved );
	//}
}

void mxSpatialDatabase_Simple::GetBoundsLocal( mxBounds & OutBounds ) const
{
	OutBounds = this->bounds;
}

void mxSpatialDatabase_Simple::GetBoundsWorld( mxBounds & OutBounds ) const
{
	OutBounds = this->bounds;
}

bool mxSpatialDatabase_Simple::CastRay( const Vec3D& origin, const Vec3D& direction, FLOAT &fraction ) const
{
	Assert( direction.IsNormalized() );

	mxSpatialProxy * pHitObject = null;
	
	Assert2( fraction == MAX_SCENE_SIZE, "fraction must be init'ed to 'infinity' on entering" );

	FLOAT hitFraction = MAX_SCENE_SIZE;

	for ( IndexT iObject = 0; iObject < this->objects.Num(); iObject++ )
	{
		mxSpatialProxy * pObject = this->objects[ iObject ];

		if ( (pObject->hitFilterMask & HM_Solid)
			&& pObject->CastRay( origin, direction, hitFraction ) )
		{
			if ( hitFraction < fraction ) {
				pHitObject = pObject;
				fraction = hitFraction;
			}
		}
	}
	
	return (pHitObject != null);
}

mxEntity* mxSpatialDatabase_Simple::CastRay( const Vec3D& origin, const Vec3D& direction, Vec3D& hitPosition ) const
{
	Assert( direction.IsNormalized() );

	mxSpatialProxy * pHitObject = null;
	
	FLOAT fraction = MAX_SCENE_SIZE;
	FLOAT hitFraction;

	for ( IndexT iObject = 0; iObject < this->objects.Num(); iObject++ )
	{
		mxSpatialProxy * pObject = this->objects[ iObject ];

		if ( (pObject->hitFilterMask & HM_Solid)
			&& pObject->CastRay( origin, direction, hitFraction ) )
		{
			if ( hitFraction < fraction ) {
				pHitObject = pObject;
				fraction = hitFraction;
			}
		}
	}

	hitPosition = origin + direction * fraction;

	if( pHitObject ) {
		return pHitObject->GetOwner();
	}
	return null;
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
