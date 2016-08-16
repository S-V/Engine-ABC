/*
=============================================================================
	File:	Sphere.cpp
	Desc:
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
//#include <Base.h>

namespace abc {

/*================================
		Sphere
================================*/

const Sphere Sphere::ZERO_SPHERE( Vec3D::vec3_origin, 0.0f );
const Sphere Sphere::UNIT_SPHERE( Vec3D::vec3_origin, 1.0f );

/*
================
Sphere::PlaneDistance
================
*/
FLOAT Sphere::PlaneDistance( const mxPlane &plane ) const {
	FLOAT d;

	d = plane.Distance( m_origin );
	if ( d > m_radius ) {
		return d - m_radius;
	}
	if ( d < -m_radius ) {
		return d + m_radius;
	}
	return 0.0f;
}

/*
================
Sphere::PlaneSide
================
*/
EPlaneSide Sphere::PlaneSide( const mxPlane &plane, const FLOAT epsilon ) const {
	FLOAT d;

	d = plane.Distance( m_origin );
	if ( d > m_radius + epsilon ) {
		return EPlaneSide::PLANESIDE_FRONT;
	}
	if ( d < -m_radius - epsilon ) {
		return EPlaneSide::PLANESIDE_BACK;
	}
	return EPlaneSide::PLANESIDE_CROSS;
}

/*
============
Sphere::LineIntersection

  Returns true if the line intersects the sphere between the start and end point.
============
*/
bool Sphere::LineIntersection( const Vec3D &start, const Vec3D &end ) const {
	Vec3D r, s, e;
	FLOAT a;

	s = start - m_origin;
	e = end - m_origin;
	r = e - s;
	a = -s * r;
	if ( a <= 0 ) {
		return ( s * s < m_radius * m_radius );
	}
	else if ( a >= r * r ) {
		return ( e * e < m_radius * m_radius );
	}
	else {
		r = s + ( a / ( r * r ) ) * r;
		return ( r * r < m_radius * m_radius );
	}
}

/*
============
Sphere::RayIntersection

  Returns true if the ray intersects the sphere.
  The ray can intersect the sphere in both directions from the start point.
  If start is inside the sphere then scale1 < 0 and scale2 > 0.
============
*/
bool Sphere::RayIntersection( const Vec3D &start, const Vec3D &dir, FLOAT &scale1, FLOAT &scale2 ) const {
	DOUBLE a, b, c, d, sqrtd;
	Vec3D p;

	p = start - m_origin;
	a = dir * dir;
	b = dir * p;
	c = p * p - m_radius * m_radius;
	d = b * b - c * a;

	if ( d < 0.0f ) {
		return false;	// A negative discriminant corresponds to ray missing sphere.
	}

	sqrtd = mxMath::Sqrt( d );
	a = 1.0f / a;

	scale1 = ( -b + sqrtd ) * a;
	scale2 = ( -b - sqrtd ) * a;

	return true;
}

/*
============
Sphere::FromPoints
============
*/
void Sphere::FromPoints( const Vec3D& a, const Vec3D& b, const Vec3D& c )
{
	Unimplemented;
}

/*
============
Sphere::FromPoints

  Tight sphere for a point set.
============
*/
#if 0
void Sphere::FromPoints( const Vec3D *points, const mxUInt numPoints ) {
	mxUInt i;
	FLOAT radiusSqr, dist;
	Vec3D mins, maxs;

	SIMDProcessor->MinMax( mins, maxs, points, numPoints );

	m_origin = ( mins + maxs ) * 0.5f;

	radiusSqr = 0.0f;
	for ( i = 0; i < numPoints; i++ ) {
		dist = ( points[i] - m_origin ).LengthSqr();
		if ( dist > radiusSqr ) {
			radiusSqr = dist;
		}
	}
	m_radius = mxMath::Sqrt( radiusSqr );
}
#endif

}//End of namespace

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
