/*
=============================================================================
	File:	Sphere.h
	Desc:
=============================================================================
*/

#ifndef __BOUNDING_VOLUMES_SPHERE_H__
#define __BOUNDING_VOLUMES_SPHERE_H__

namespace abc {

//
//	Sphere
//
class Sphere {
public:
					Sphere( void );
					explicit Sphere( const Vec3D &point );
					explicit Sphere( const Vec3D &point, const FLOAT r );

	FLOAT			operator[]( const mxUInt index ) const;
	FLOAT &			operator[]( const mxUInt index );
	Sphere			operator+( const Vec3D &t ) const;				// returns tranlated sphere
	Sphere &		operator+=( const Vec3D &t );					// translate the sphere
	Sphere			operator+( const Sphere &s ) const;
	Sphere &		operator+=( const Sphere &s );

	bool			Compare( const Sphere &a ) const;							// exact compare, no epsilon
	bool			Compare( const Sphere &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool			operator==(	const Sphere &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const Sphere &a ) const;						// exact compare, no epsilon

	void			Clear( void );									// inside out sphere
	void			SetZero( void );									// single point at m_origin
	void			SetOrigin( const Vec3D &o );					// set m_origin of sphere
	void			SetRadius( const FLOAT r );						// set square m_radius

	const Vec3D &	GetOrigin( void ) const;						// returns m_origin of sphere
	FLOAT			GetRadius( void ) const;						// returns sphere m_radius
	bool			IsCleared( void ) const;						// returns true if sphere is inside out

	bool			AddPoint( const Vec3D &p );					// add the point, returns true if the sphere expanded
	bool			AddSphere( const Sphere &s );					// add the sphere, returns true if the sphere expanded
	Sphere			Expand( const FLOAT d ) const;					// return bounds expanded in all directions with the given value
	Sphere &		ExpandSelf( const FLOAT d );					// expand bounds in all directions with the given value
	Sphere			Translate( const Vec3D &translation ) const;
	Sphere &		TranslateSelf( const Vec3D &translation );

	FLOAT			PlaneDistance( const mxPlane &plane ) const;
	EPlaneSide		PlaneSide( const mxPlane &plane, const FLOAT epsilon = ON_EPSILON ) const;

	bool			ContainsPoint( const Vec3D &p ) const;			// includes touching
	bool			IntersectsSphere( const Sphere &s ) const;	// includes touching
	bool			LineIntersection( const Vec3D &start, const Vec3D &end ) const;
					// intersection points are (start + dir * scale1) and (start + dir * scale2)
	bool			RayIntersection( const Vec3D &start, const Vec3D &dir, FLOAT &scale1, FLOAT &scale2 ) const;

					// construct from the given three points
	void			FromPoints( const Vec3D& a, const Vec3D& b, const Vec3D& c );
					// Tight sphere for a point set.
	void			FromPoints( const Vec3D *points, const mxUInt numPoints );
					// Most tight sphere for a translation.
	void			FromPointTranslation( const Vec3D &point, const Vec3D &translation );
	void			FromSphereTranslation( const Sphere &sphere, const Vec3D &start, const Vec3D &translation );
					// Most tight sphere for a rotation.
	void			FromPointRotation( const Vec3D &point, const mxRotation &rotation );
	void			FromSphereRotation( const Sphere &sphere, const Vec3D &start, const mxRotation &rotation );

	void			AxisProjection( const Vec3D &dir, FLOAT &min, FLOAT &max ) const;

	bool			IsValid() const;

public:
	static const Sphere	ZERO_SPHERE;
	static const Sphere	UNIT_SPHERE;

private:
	Vec3D			m_origin;
	FLOAT			m_radius;
};

FORCEINLINE Sphere::Sphere( void ) {
}

FORCEINLINE Sphere::Sphere( const Vec3D &point ) {
	m_origin = point;
	m_radius = 0.0f;
}

FORCEINLINE Sphere::Sphere( const Vec3D &point, const FLOAT r ) {
	m_origin = point;
	m_radius = r;
}

FORCEINLINE FLOAT Sphere::operator[]( const mxUInt index ) const {
	return ((FLOAT *) &m_origin)[index];
}

FORCEINLINE FLOAT &Sphere::operator[]( const mxUInt index ) {
	return ((FLOAT *) &m_origin)[index];
}

FORCEINLINE Sphere Sphere::operator+( const Vec3D &t ) const {
	return Sphere( m_origin + t, m_radius );
}

FORCEINLINE Sphere &Sphere::operator+=( const Vec3D &t ) {
	m_origin += t;
	return *this;
}

FORCEINLINE bool Sphere::Compare( const Sphere &a ) const {
	return ( m_origin.Compare( a.m_origin ) && m_radius == a.m_radius );
}

FORCEINLINE bool Sphere::Compare( const Sphere &a, const FLOAT epsilon ) const {
	return ( m_origin.Compare( a.m_origin, epsilon ) && mxMath::Fabs( m_radius - a.m_radius ) <= epsilon );
}

FORCEINLINE bool Sphere::operator==( const Sphere &a ) const {
	return Compare( a );
}

FORCEINLINE bool Sphere::operator!=( const Sphere &a ) const {
	return !Compare( a );
}

FORCEINLINE void Sphere::Clear( void ) {
	m_origin.SetZero();
	m_radius = -1.0f;
}

FORCEINLINE void Sphere::SetZero( void ) {
	m_origin.SetZero();
	m_radius = 0.0f;
}

FORCEINLINE void Sphere::SetOrigin( const Vec3D &o ) {
	m_origin = o;
}

FORCEINLINE void Sphere::SetRadius( const FLOAT r ) {
	m_radius = r;
}

FORCEINLINE const Vec3D &Sphere::GetOrigin( void ) const {
	return m_origin;
}

FORCEINLINE FLOAT Sphere::GetRadius( void ) const {
	return m_radius;
}

FORCEINLINE bool Sphere::IsCleared( void ) const {
	return ( m_radius < 0.0f );
}

FORCEINLINE bool Sphere::AddPoint( const Vec3D &p ) {
	if ( m_radius < 0.0f ) {
		m_origin = p;
		m_radius = 0.0f;
		return true;
	}
	else {
		FLOAT r = ( p - m_origin ).LengthSqr();
		if ( r > m_radius * m_radius ) {
			r = mxMath::Sqrt( r );
			m_origin += ( p - m_origin ) * 0.5f * (1.0f - m_radius / r );
			m_radius += 0.5f * ( r - m_radius );
			return true;
		}
		return false;
	}
}

FORCEINLINE bool Sphere::AddSphere( const Sphere &s ) {
	if ( m_radius < 0.0f ) {
		m_origin = s.m_origin;
		m_radius = s.m_radius;
		return true;
	}
	else {
		FLOAT r = ( s.m_origin - m_origin ).LengthSqr();
		if ( r > ( m_radius + s.m_radius ) * ( m_radius + s.m_radius ) ) {
			r = mxMath::Sqrt( r );
			m_origin += ( s.m_origin - m_origin ) * 0.5f * (1.0f - m_radius / ( r + s.m_radius ) );
			m_radius += 0.5f * ( ( r + s.m_radius ) - m_radius );
			return true;
		}
		return false;
	}
}

FORCEINLINE Sphere Sphere::Expand( const FLOAT d ) const {
	return Sphere( m_origin, m_radius + d );
}

FORCEINLINE Sphere &Sphere::ExpandSelf( const FLOAT d ) {
	m_radius += d;
	return *this;
}

FORCEINLINE Sphere Sphere::Translate( const Vec3D &translation ) const {
	return Sphere( m_origin + translation, m_radius );
}

FORCEINLINE Sphere &Sphere::TranslateSelf( const Vec3D &translation ) {
	m_origin += translation;
	return *this;
}

FORCEINLINE bool Sphere::ContainsPoint( const Vec3D &p ) const {
	if ( ( p - m_origin ).LengthSqr() > m_radius * m_radius ) {
		return false;
	}
	return true;
}

FORCEINLINE bool Sphere::IntersectsSphere( const Sphere &s ) const {
	FLOAT r = s.m_radius + m_radius;
	if ( ( s.m_origin - m_origin ).LengthSqr() > r * r ) {
		return false;
	}
	return true;
}

FORCEINLINE void Sphere::FromPointTranslation( const Vec3D &point, const Vec3D &translation ) {
	m_origin = point + 0.5f * translation;
	m_radius = mxMath::Sqrt( 0.5f * translation.LengthSqr() );
}

FORCEINLINE void Sphere::FromSphereTranslation( const Sphere &sphere, const Vec3D &start, const Vec3D &translation ) {
	m_origin = start + sphere.m_origin + 0.5f * translation;
	m_radius = mxMath::Sqrt( 0.5f * translation.LengthSqr() ) + sphere.m_radius;
}

FORCEINLINE void Sphere::FromPointRotation( const Vec3D &point, const mxRotation &rotation ) {
	Vec3D end = rotation * point;
	m_origin = ( point + end ) * 0.5f;
	m_radius = mxMath::Sqrt( 0.5f * ( end - point ).LengthSqr() );
}

FORCEINLINE void Sphere::FromSphereRotation( const Sphere &sphere, const Vec3D &start, const mxRotation &rotation ) {
	Vec3D end = rotation * sphere.m_origin;
	m_origin = start + ( sphere.m_origin + end ) * 0.5f;
	m_radius = mxMath::Sqrt( 0.5f * ( end - sphere.m_origin ).LengthSqr() ) + sphere.m_radius;
}

FORCEINLINE void Sphere::AxisProjection( const Vec3D &dir, FLOAT &min, FLOAT &max ) const {
	FLOAT d;
	d = dir * m_origin;
	min = d - m_radius;
	max = d + m_radius;
}

FORCEINLINE bool Sphere::IsValid() const {
	return (! FLOAT_IS_INVALID( m_radius )) && ( m_origin.IsValid() );
}

}//End of namespace abc

#endif // ! __BOUNDING_VOLUMES_SPHERE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
