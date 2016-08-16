/*
=============================================================================
	File:	SpatialProxy.h
	Desc:	Spatial proxy can be thought of
			as a spatial database of a single object.
=============================================================================
*/

#ifndef __MX_SPATIAL_PROXY_H__
#define __MX_SPATIAL_PROXY_H__

namespace abc {

//
//	mxSpatialProxy - interface for making spatial queries against visual geometry of a scene object.
//
class mxSpatialProxy : public mxEntityComponent
{
	DECLARE_ABSTRACT_CLASS( mxSpatialProxy );

public:
	UINT32		hitFilterMask;	// bits from EHitMask

public:

	// Returns the extent of the object in local space.
	//
	virtual void GetBoundsLocal( mxBounds & OutBounds ) const
	{
		OutBounds = mxBounds::INFINITE_EXTENT;
	}

	// Returns the extent of the object in world space.
	//
	virtual void GetBoundsWorld( mxBounds & OutBounds ) const
	{
		OutBounds = mxBounds::INFINITE_EXTENT;
	}

	// Returns the bounding sphere around the object in world space.
	//
	virtual void GetBoundingSphereWorld( Sphere & OutSphere ) const
	{
		mxBounds  worldBounds;
		GetBoundsWorld( worldBounds );
		OutSphere = worldBounds.ToSphere();
	}

	// Returns an _approximate_ point of intersection between the given ray and this object.
	// NOTE: Ray origin and direction are given in world space.
	// fraction - fraction of movement completed (fraction is in range [0..1], hitPosition = origin + direction * fraction).
	//
	virtual bool CastRay( const Vec3D& origin, const Vec3D& direction, FLOAT &fraction ) const
	{
		Assert( direction.IsNormalized() );

		// Check intersection with bounding box.
		mxBounds  worldBounds;
		GetBoundsWorld( worldBounds );
		
		return worldBounds.RayIntersection( origin, direction, fraction );
	}

	// Work in progress...

protected:
	mxSpatialProxy() : hitFilterMask(0) {}
	virtual	~mxSpatialProxy() {}
};

//===========================================================================

//
//	mxSpatialProxy_Infinite - object with infinite extents, used mainly for debugging.
//
class mxSpatialProxy_Infinite : public mxSpatialProxy {
public:
	DECLARE_CLASS( mxSpatialProxy_Infinite );

public:
	mxSpatialProxy_Infinite()
	{}

	virtual void GetBoundsLocal( mxBounds & OutBounds ) const
	{
		OutBounds = mxBounds::INFINITE_EXTENT;
	}
	virtual void GetBoundsWorld( mxBounds & OutBounds ) const
	{
		OutBounds = mxBounds::INFINITE_EXTENT;
	}

	bool CastRay( const Vec3D& origin, const Vec3D& direction, FLOAT &fraction ) const
	{
		Assert( direction.IsNormalized() );
		(void) origin, (void) direction, (void) fraction;
		return false;
	}
};

//
//	mxSpatialProxy_Sphere
//
class mxSpatialProxy_Sphere : public mxSpatialProxy {
public:
	DECLARE_CLASS( mxSpatialProxy_Sphere );

public:
	mxSpatialProxy_Sphere()
		: radius(0), worldTransform( Matrix4::mat4_identity )
	{}
	mxSpatialProxy_Sphere( FLOAT theRadius, const Matrix4& theWorldTransform )
		: radius( theRadius ), worldTransform( theWorldTransform )
	{}

	virtual void GetBoundsLocal( mxBounds & OutBounds ) const
	{
		OutBounds.FromSphere( Sphere( Vec3D(0,0,0), radius ) );
	}
	virtual void GetBoundsWorld( mxBounds & OutBounds ) const
	{
		OutBounds.FromSphere( Sphere( worldTransform.GetTranslation(), radius ) );
	}
	virtual void GetBoundingSphereWorld( Sphere & OutSphere ) const
	{
		OutSphere.SetOrigin( worldTransform.GetTranslation() );
		OutSphere.SetRadius( radius );
	}
	virtual bool CastRay( const Vec3D& origin, const Vec3D& direction, FLOAT &fraction ) const
	{
		Assert( direction.IsNormalized() );

		Sphere bs;
		GetBoundingSphereWorld( bs );

		if ( bs.ContainsPoint( origin ) ) {
			return false;
		} else {
			FLOAT scale1;
			return bs.RayIntersection( origin, direction, scale1, fraction );
		}
	}

private:
	const Matrix4 &	worldTransform;
	FLOAT			radius;
};

//
//	mxSpatialProxy_Box
//
class mxSpatialProxy_Box : public mxSpatialProxy {
public:
	DECLARE_CLASS( mxSpatialProxy_Box );

public:
	mxSpatialProxy_Box()
		: localAabb( AABB::BOUNDS_ZERO ), worldTransform( Matrix4::mat4_identity )
	{}
	mxSpatialProxy_Box( const AABB& theLocalAABB, const Matrix4& theWorldTransform )
		: localAabb( theLocalAABB ), worldTransform( theWorldTransform )
	{}

	virtual void GetBoundsLocal( mxBounds & OutBounds ) const
	{
		OutBounds = localAabb;
	}
	virtual void GetBoundsWorld( mxBounds & OutBounds ) const
	{
		OutBounds = localAabb.Trasform( worldTransform );
	}
	virtual void GetBoundingSphereWorld( Sphere & OutSphere ) const
	{
		mxBounds boundsWorld;
		GetBoundsWorld( boundsWorld );
		OutSphere = boundsWorld.ToSphere();
	}

private:
	AABB			localAabb;
	const Matrix4 &	worldTransform;
};

/*
class mxSpatialProxy_Sphere
class mxSpatialProxy_Box
class mxSpatialProxy_Cone
*/

}//End of namespace abc

#endif // ! __MX_SPATIAL_PROXY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
