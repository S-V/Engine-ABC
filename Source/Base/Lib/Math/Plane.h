/*
=============================================================================
	File:	Plane.h
	Desc:	3D plane class.
	Note:	Originally written by Id Software.
	Copyright (C) 2004 Id Software, Inc. ( idPlane )
=============================================================================
*/

#ifndef __MATH_PLANE_H__
#define __MATH_PLANE_H__

/*
=============================================================================

	3D plane with equation: a * x + b * y + c * z + d = 0

=============================================================================
*/

namespace abc {

class Vec3D;
class Vec4D;
class Matrix3;

const FLOAT ON_EPSILON				= 0.1f;		// Plane side epsilon.
const FLOAT DEGENERATE_DIST_EPSILON	= 1e-4f;

//
//	EPlaneSide - spatial relation to a plane.
//
enum EPlaneSide
{
	PLANESIDE_FRONT	= 0,
	PLANESIDE_BACK	= 1,
	PLANESIDE_ON	= 2,
	PLANESIDE_CROSS	= 3,
};

//
// EPlaneType
//
enum EPlaneType
{
	PLANETYPE_X,
	PLANETYPE_Y,
	PLANETYPE_Z,
	PLANETYPE_NEGX,
	PLANETYPE_NEGY,
	PLANETYPE_NEGZ,
	PLANETYPE_TRUEAXIAL,	// all types < 6 are true axial planes
	PLANETYPE_ZEROX,
	PLANETYPE_ZEROY,
	PLANETYPE_ZEROZ,
	PLANETYPE_NONAXIAL,
};

//
//	mxPlane - consists of plane normal and distance from the origin.
//
class mxPlane {
public:
	FLOAT		a;
	FLOAT		b;
	FLOAT		c;
	FLOAT		d;	// distance from the origin

public:
					mxPlane( void );
					mxPlane( FLOAT a, FLOAT b, FLOAT c, FLOAT d );
					mxPlane( const Vec4D& vec );
					mxPlane( const Vec3D &normal, const FLOAT dist );

	FLOAT			operator[]( mxUInt index ) const;
	FLOAT &			operator[]( mxUInt index );
	mxPlane			operator-() const;						// flips plane
	mxPlane &		operator=( const Vec3D &v );			// sets normal and sets mxPlane::d to zero
	mxPlane			operator+( const mxPlane &p ) const;	// add plane equations
	mxPlane			operator-( const mxPlane &p ) const;	// subtract plane equations
	mxPlane &		operator*=( const Matrix3 &m );			// Normal() *= m

	bool			Compare( const mxPlane &p ) const;						// exact compare, no epsilon
	bool			Compare( const mxPlane &p, const FLOAT epsilon ) const;	// compare with epsilon
	bool			Compare( const mxPlane &p, const FLOAT normalEps, const FLOAT distEps ) const;	// compare with epsilon
	bool			operator==(	const mxPlane &p ) const;					// exact compare, no epsilon
	bool			operator!=(	const mxPlane &p ) const;					// exact compare, no epsilon

	void			SetZero( void );						// zero plane
	void			SetNormal( const Vec3D &normal );		// sets the normal
	void			Set( const Vec4D& vec );
	const Vec3D &	Normal( void ) const;					// reference to const normal
	Vec3D &			Normal( void );							// reference to normal
	FLOAT			Normalize( bool fixDegenerate = true );	// only normalizes the plane normal, does not adjust d
	bool			FixDegenerateNormal( void );			// fix degenerate normal
	bool			FixDegeneracies( FLOAT distEpsilon );	// fix degenerate normal and dist
	FLOAT			Dist( void ) const;						// returns: -d
	void			SetDist( const FLOAT dist );			// sets: d = -dist
	EPlaneType		Type( void ) const;						// returns plane type

	bool			FromPoints( const Vec3D &p1, const Vec3D &p2, const Vec3D &p3, bool fixDegenerate = true );
	bool			FromVecs( const Vec3D &dir1, const Vec3D &dir2, const Vec3D &p, bool fixDegenerate = true );
	void			FitThroughPoint( const Vec3D &p );	// assumes normal is valid
	bool			HeightFit( const Vec3D *points, const mxUInt numPoints );
	mxPlane			Translate( const Vec3D &translation ) const;
	mxPlane &		TranslateSelf( const Vec3D &translation );
	mxPlane			Rotate( const Vec3D &origin, const Matrix3 &axis ) const;
	mxPlane &		RotateSelf( const Vec3D &origin, const Matrix3 &axis );

	FLOAT			Distance( const Vec3D &v ) const;
	EPlaneSide		Side( const Vec3D &v, const FLOAT epsilon = 0.0f ) const;

	bool			LineIntersection( const Vec3D &start, const Vec3D &end ) const;
					// intersection point is start + dir * scale
	bool			RayIntersection( const Vec3D &start, const Vec3D &dir, FLOAT &scale ) const;
	bool			PlaneIntersection( const mxPlane &plane, Vec3D &start, Vec3D &dir ) const;

					// assumes lookDir is normalized
	bool			FrontFacing( const Vec3D& lookDir ) const;

	mxUInt			GetDimension( void ) const;

	const Vec4D &	ToVec4( void ) const;
	Vec4D &			ToVec4( void );
	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToString( mxUInt precision = 2 ) const;

public:
	static const mxPlane PLANE_ORIGIN;	// ( N = (0,0,0), D = 0 )

	static const mxPlane PLANE_ZERO_X;	// ( N = (1,0,0), D = 0 )
	static const mxPlane PLANE_ZERO_Y;	// ( N = (0,1,0), D = 0 )
	static const mxPlane PLANE_ZERO_Z;	// ( N = (0,0,1), D = 0 )
};

FORCEINLINE mxPlane::mxPlane( void ) {
}

FORCEINLINE mxPlane::mxPlane( FLOAT a, FLOAT b, FLOAT c, FLOAT d ) {
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
}

FORCEINLINE mxPlane::mxPlane( const Vec4D& vec )
	: a( vec.x ), b( vec.y ), c( vec.z ), d( vec.w )
{}

FORCEINLINE mxPlane::mxPlane( const Vec3D &normal, const FLOAT dist ) {
	this->a = normal.x;
	this->b = normal.y;
	this->c = normal.z;
	this->d = -dist;
}

FORCEINLINE FLOAT mxPlane::operator[]( mxUInt index ) const {
	return ( &a )[ index ];
}

FORCEINLINE FLOAT& mxPlane::operator[]( mxUInt index ) {
	return ( &a )[ index ];
}

FORCEINLINE mxPlane mxPlane::operator-() const {
	return mxPlane( -a, -b, -c, -d );
}

FORCEINLINE mxPlane &mxPlane::operator=( const Vec3D &v ) { 
	a = v.x;
	b = v.y;
	c = v.z;
	d = 0;
	return *this;
}

FORCEINLINE mxPlane mxPlane::operator+( const mxPlane &p ) const {
	return mxPlane( a + p.a, b + p.b, c + p.c, d + p.d );
}

FORCEINLINE mxPlane mxPlane::operator-( const mxPlane &p ) const {
	return mxPlane( a - p.a, b - p.b, c - p.c, d - p.d );
}

FORCEINLINE mxPlane &mxPlane::operator*=( const Matrix3 &m ) {
	Normal() *= m;
	return *this;
}

FORCEINLINE bool mxPlane::Compare( const mxPlane &p ) const {
	return ( a == p.a && b == p.b && c == p.c && d == p.d );
}

FORCEINLINE bool mxPlane::Compare( const mxPlane &p, const FLOAT epsilon ) const {
	if ( mxMath::Fabs( a - p.a ) > epsilon ) {
		return false;
	}
			
	if ( mxMath::Fabs( b - p.b ) > epsilon ) {
		return false;
	}

	if ( mxMath::Fabs( c - p.c ) > epsilon ) {
		return false;
	}

	if ( mxMath::Fabs( d - p.d ) > epsilon ) {
		return false;
	}

	return true;
}

FORCEINLINE bool mxPlane::Compare( const mxPlane &p, const FLOAT normalEps, const FLOAT distEps ) const {
	if ( mxMath::Fabs( d - p.d ) > distEps ) {
		return false;
	}
	if ( !Normal().Compare( p.Normal(), normalEps ) ) {
		return false;
	}
	return true;
}

FORCEINLINE bool mxPlane::operator==( const mxPlane &p ) const {
	return Compare( p );
}

FORCEINLINE bool mxPlane::operator!=( const mxPlane &p ) const {
	return !Compare( p );
}

FORCEINLINE void mxPlane::SetZero( void ) {
	a = b = c = d = 0.0f;
}

FORCEINLINE void mxPlane::SetNormal( const Vec3D &normal ) {
	a = normal.x;
	b = normal.y;
	c = normal.z;
}

FORCEINLINE void mxPlane::Set( const Vec4D& vec ) {
	a = vec.x;
	b = vec.y;
	c = vec.z;
	d = vec.w;
}

FORCEINLINE const Vec3D &mxPlane::Normal( void ) const {
	return *reinterpret_cast<const Vec3D *>(&a);
}

FORCEINLINE Vec3D &mxPlane::Normal( void ) {
	return *reinterpret_cast<Vec3D *>(&a);
}

FORCEINLINE FLOAT mxPlane::Normalize( bool fixDegenerate ) {
	FLOAT length = reinterpret_cast<Vec3D *>(&a)->Normalize();

	if ( fixDegenerate ) {
		FixDegenerateNormal();
	}
	return length;
}

FORCEINLINE bool mxPlane::FixDegenerateNormal( void ) {
	return Normal().FixDegenerateNormal();
}

FORCEINLINE bool mxPlane::FixDegeneracies( FLOAT distEpsilon ) {
	bool fixedNormal = FixDegenerateNormal();
	// only fix dist if the normal was degenerate
	if ( fixedNormal ) {
		if ( mxMath::Fabs( d - mxMath::Rint( d ) ) < distEpsilon ) {
			d = mxMath::Rint( d );
		}
	}
	return fixedNormal;
}

FORCEINLINE FLOAT mxPlane::Dist( void ) const {
	return -d;
}

FORCEINLINE void mxPlane::SetDist( const FLOAT dist ) {
	d = -dist;
}

FORCEINLINE bool mxPlane::FromPoints( const Vec3D &p1, const Vec3D &p2, const Vec3D &p3, bool fixDegenerate ) {
	Normal() = (p1 - p2).Cross( p3 - p2 );
	if ( Normalize( fixDegenerate ) == 0.0f ) {
		return false;
	}
	d = -( Normal() * p2 );
	return true;
}

FORCEINLINE bool mxPlane::FromVecs( const Vec3D &dir1, const Vec3D &dir2, const Vec3D &p, bool fixDegenerate ) {
	Normal() = dir1.Cross( dir2 );
	if ( Normalize( fixDegenerate ) == 0.0f ) {
		return false;
	}
	d = -( Normal() * p );
	return true;
}

FORCEINLINE void mxPlane::FitThroughPoint( const Vec3D &p ) {
	d = -( Normal() * p );
}

FORCEINLINE mxPlane mxPlane::Translate( const Vec3D &translation ) const {
	return mxPlane( a, b, c, d - translation * Normal() );
}

FORCEINLINE mxPlane &mxPlane::TranslateSelf( const Vec3D &translation ) {
	d -= translation * Normal();
	return *this;
}

FORCEINLINE mxPlane mxPlane::Rotate( const Vec3D &origin, const Matrix3 &axis ) const {
	mxPlane p;
	p.Normal() = Normal() * axis;
	p.d = d + origin * Normal() - origin * p.Normal();
	return p;
}

FORCEINLINE mxPlane &mxPlane::RotateSelf( const Vec3D &origin, const Matrix3 &axis ) {
	d += origin * Normal();
	Normal() *= axis;
	d -= origin * Normal();
	return *this;
}

FORCEINLINE FLOAT mxPlane::Distance( const Vec3D &v ) const {
	return a * v.x + b * v.y + c * v.z + d;
}

FORCEINLINE EPlaneSide mxPlane::Side( const Vec3D &v, const FLOAT epsilon ) const {
	FLOAT dist = Distance( v );
	if ( dist > epsilon ) {
		return PLANESIDE_FRONT;
	}
	else if ( dist < -epsilon ) {
		return PLANESIDE_BACK;
	}
	else {
		return PLANESIDE_ON;
	}
}

FORCEINLINE bool mxPlane::LineIntersection( const Vec3D &start, const Vec3D &end ) const {
	FLOAT d1, d2, fraction;

	d1 = Normal() * start + d;
	d2 = Normal() * end + d;
	if ( d1 == d2 ) {
		return false;
	}
	if ( d1 > 0.0f && d2 > 0.0f ) {
		return false;
	}
	if ( d1 < 0.0f && d2 < 0.0f ) {
		return false;
	}
	fraction = ( d1 / ( d1 - d2 ) );
	return ( fraction >= 0.0f && fraction <= 1.0f );
}

FORCEINLINE bool mxPlane::RayIntersection( const Vec3D &start, const Vec3D &dir, FLOAT &scale ) const {
	FLOAT d1, d2;

	d1 = Normal() * start + d;
	d2 = Normal() * dir;
	if ( d2 == 0.0f ) {
		return false;
	}
	scale = -( d1 / d2 );
	return true;
}

FORCEINLINE bool mxPlane::FrontFacing( const Vec3D& lookDir ) const
{
	Assert( lookDir.IsNormalized() );
	return Normal().Dot( lookDir ) <= 0.0f;
}

FORCEINLINE mxUInt mxPlane::GetDimension( void ) const {
	return 4;
}

FORCEINLINE const Vec4D &mxPlane::ToVec4( void ) const {
	return *reinterpret_cast<const Vec4D *>(&a);
}

FORCEINLINE Vec4D &mxPlane::ToVec4( void ) {
	return *reinterpret_cast<Vec4D *>(&a);
}

FORCEINLINE const FLOAT *mxPlane::ToFloatPtr( void ) const {
	return reinterpret_cast<const FLOAT *>(&a);
}

FORCEINLINE FLOAT *mxPlane::ToFloatPtr( void ) {
	return reinterpret_cast<FLOAT *>(&a);
}

}//End of namespace abc

#endif /* !__MATH_PLANE_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
