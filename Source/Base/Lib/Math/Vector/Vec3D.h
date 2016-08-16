/*
=============================================================================
	File:	Vec3D.h
	Desc:	3D vector.
			Copyright (C) 2004 Id Software, Inc. ( idVec3 )
=============================================================================
*/

#ifndef __MATH_VEC3D_H__
#define __MATH_VEC3D_H__

namespace abc {

class mxAngles;
class mxPolar3;
class Matrix3;

//
//	Vec3D
//

class Vec3D {
public:
	FLOAT		x;
	FLOAT		y;
	FLOAT		z;

public:
					Vec3D( void );	// empty ctor leaves data uninitialized
					explicit Vec3D( const FLOAT f );	// initializes all three componenets to the given value
					explicit Vec3D( const Vec2D& xy, const FLOAT z );
					explicit Vec3D( const FLOAT x, const FLOAT y, const FLOAT z );

	void 			Set( const FLOAT x, const FLOAT y, const FLOAT z );
	void			SetZero( void );

	FLOAT			operator[]( const INT index ) const;
	FLOAT &			operator[]( const INT index );
	Vec3D			operator-() const;
	Vec3D &			operator=( const Vec3D &a );		// required because of a msvc 6 & 7 bug
	FLOAT			operator*( const Vec3D &a ) const;	// Dot product.
	Vec3D			operator^( const Vec3D &a ) const;	// Cross product.
	Vec3D			operator*( const FLOAT a ) const;
	Vec3D			operator/( const FLOAT a ) const;
	Vec3D			operator+( const Vec3D &a ) const;
	Vec3D			operator-( const Vec3D &a ) const;
	Vec3D &			operator+=( const Vec3D &a );
	Vec3D &			operator-=( const Vec3D &a );
	Vec3D &			operator/=( const Vec3D &a );
	Vec3D &			operator/=( const FLOAT a );
	Vec3D &			operator*=( const FLOAT a );

	friend Vec3D	operator*( const FLOAT a, const Vec3D b );

	bool			Compare( const Vec3D &a ) const;							// exact compare, no epsilon
	bool			Compare( const Vec3D &a, const FLOAT epsilon ) const;		// compare with epsilon
	bool			operator==(	const Vec3D &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const Vec3D &a ) const;						// exact compare, no epsilon

	bool			FixDegenerateNormal( void );	// fix degenerate axial cases
	bool			FixDenormals( void );			// change tiny numbers to zero
	bool			IsNormalized( const FLOAT epsilon = VECTOR_EPSILON ) const;
	bool			IsZero() const;

	FLOAT			Dot( const Vec3D &a ) const;
	Vec3D			Cross( const Vec3D &a ) const;
	Vec3D &			Cross( const Vec3D &a, const Vec3D &b );
	FLOAT			Length( void ) const;
	FLOAT			LengthSqr( void ) const;
	FLOAT			LengthFast( void ) const;
	Vec3D			GetNormalized() const;
	FLOAT			Normalize( void );				// returns length
	FLOAT			NormalizeFast( void );			// returns length
	Vec3D &			Truncate( FLOAT length );		// cap length
	void			Clamp( const Vec3D &min, const Vec3D &max );
	void			Snap( void );					// snap to closest integer value
	void			SnapInt( void );				// snap towards integer (floor)

	INT				GetDimension( void ) const;

	FLOAT			ToYaw( void ) const;
	FLOAT			ToPitch( void ) const;
	mxAngles		ToAngles( void ) const;
	mxPolar3		ToPolar( void ) const;
	Matrix3			ToMat3( void ) const;		// vector should be normalized
	Matrix3			ToMat3( INT axis ) const;		// vector should be normalized
	const Vec2D &	ToVec2( void ) const;
	Vec2D &			ToVec2( void );
	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToString( INT precision = 2 ) const;

	void			NormalVectors( Vec3D &left, Vec3D &down ) const;	// vector should be normalized
	void			OrthogonalBasis( Vec3D &left, Vec3D &up ) const;

	void			ProjectOntoPlane( const Vec3D &normal, const FLOAT overBounce = 1.0f );
	bool			ProjectAlongPlane( const Vec3D &normal, const FLOAT epsilon, const FLOAT overBounce = 1.0f );
	void			ProjectSelfOntoSphere( const FLOAT radius );

	void			Lerp( const Vec3D &v1, const Vec3D &v2, const FLOAT l );
	void			SLerp( const Vec3D &v1, const Vec3D &v2, const FLOAT l );

	Vec3D & 		RotateAboutX( FLOAT angle );
	Vec3D & 		RotateAboutY( FLOAT angle );
	Vec3D & 		RotateAboutZ( FLOAT angle );
	Vec3D & 		RotateAboutAxis( FLOAT angle, const Vec3D& axis );

	bool			IsValid() const;

public:
	static Vec3D	GetRandom( INT seed = 0 );
	static Vec3D	GetRandomNormalized( INT seed = 0 );

public:
	static const Vec3D   vec3_origin;		// ( 0, 0, 0 ).

	static const Vec3D   ZERO;			// ( 0, 0, 0 ).

	static const Vec3D   UNIT_SCALE;	// ( 1, 1, 1 ).

	static const Vec3D   UNIT_X;
	static const Vec3D   UNIT_Y;
	static const Vec3D   UNIT_Z;
	static const Vec3D   NEGATIVE_UNIT_X;
	static const Vec3D   NEGATIVE_UNIT_Y;
	static const Vec3D   NEGATIVE_UNIT_Z;

	static const Vec3D   POSITIVE_INFINITY;
	static const Vec3D   NEGATIVE_INFINITY;
};

FORCEINLINE Vec3D::Vec3D( void ) {
}

FORCEINLINE Vec3D::Vec3D( const FLOAT f ) {
	this->x = f;
	this->y = f;
	this->z = f;
}

FORCEINLINE Vec3D::Vec3D( const Vec2D& xy, const FLOAT z ) {
	this->x = xy.x;
	this->y = xy.y;
	this->z = z;
}

FORCEINLINE Vec3D::Vec3D( const FLOAT x, const FLOAT y, const FLOAT z ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

FORCEINLINE FLOAT Vec3D::operator[]( const INT index ) const {
	return ( &x )[ index ];
}

FORCEINLINE FLOAT &Vec3D::operator[]( const INT index ) {
	return ( &x )[ index ];
}

FORCEINLINE void Vec3D::Set( const FLOAT x, const FLOAT y, const FLOAT z ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

FORCEINLINE void Vec3D::SetZero( void ) {
	x = y = z = 0.0f;
}

FORCEINLINE Vec3D Vec3D::operator-() const {
	return Vec3D( -x, -y, -z );
}

FORCEINLINE Vec3D &Vec3D::operator=( const Vec3D &a ) {
	x = a.x;
	y = a.y;
	z = a.z;
	return *this;
}

FORCEINLINE Vec3D Vec3D::operator-( const Vec3D &a ) const {
	return Vec3D( x - a.x, y - a.y, z - a.z );
}

FORCEINLINE FLOAT Vec3D::operator*( const Vec3D &a ) const {
	return Dot( a );
}

FORCEINLINE FLOAT Vec3D::Dot( const Vec3D &a ) const {
	return x * a.x + y * a.y + z * a.z;
}

FORCEINLINE Vec3D Vec3D::operator*( const FLOAT a ) const {
	return Vec3D( x * a, y * a, z * a );
}

FORCEINLINE Vec3D Vec3D::operator^( const Vec3D &a ) const {
	return Vec3D(
		y * a.z - z * a.y,
		z * a.x - x * a.z,
		x * a.y - y * a.x );
}

FORCEINLINE Vec3D Vec3D::operator/( const FLOAT a ) const {
	FLOAT inva = 1.0f / a;
	return Vec3D( x * inva, y * inva, z * inva );
}

FORCEINLINE Vec3D operator*( const FLOAT a, const Vec3D b ) {
	return Vec3D( b.x * a, b.y * a, b.z * a );
}

FORCEINLINE Vec3D Vec3D::operator+( const Vec3D &a ) const {
	return Vec3D( x + a.x, y + a.y, z + a.z );
}

FORCEINLINE Vec3D &Vec3D::operator+=( const Vec3D &a ) {
	x += a.x;
	y += a.y;
	z += a.z;

	return *this;
}

FORCEINLINE Vec3D &Vec3D::operator/=( const Vec3D &a ) {
	x /= a.x;
	y /= a.y;
	z /= a.z;

	return *this;
}

FORCEINLINE Vec3D &Vec3D::operator/=( const FLOAT a ) {
	FLOAT inva = 1.0f / a;
	x *= inva;
	y *= inva;
	z *= inva;

	return *this;
}

FORCEINLINE Vec3D &Vec3D::operator-=( const Vec3D &a ) {
	x -= a.x;
	y -= a.y;
	z -= a.z;

	return *this;
}

FORCEINLINE Vec3D &Vec3D::operator*=( const FLOAT a ) {
	x *= a;
	y *= a;
	z *= a;

	return *this;
}

FORCEINLINE bool Vec3D::Compare( const Vec3D &a ) const {
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) );
}

FORCEINLINE bool Vec3D::Compare( const Vec3D &a, const FLOAT epsilon ) const {
	if ( mxMath::Fabs( x - a.x ) > epsilon ) {
		return false;
	}

	if ( mxMath::Fabs( y - a.y ) > epsilon ) {
		return false;
	}

	if ( mxMath::Fabs( z - a.z ) > epsilon ) {
		return false;
	}

	return true;
}

FORCEINLINE bool Vec3D::operator==( const Vec3D &a ) const {
	return Compare( a );
}

FORCEINLINE bool Vec3D::operator!=( const Vec3D &a ) const {
	return !Compare( a );
}

FORCEINLINE FLOAT Vec3D::NormalizeFast( void ) {
	FLOAT sqrLength, invLength;

	sqrLength = x * x + y * y + z * z;
	invLength = mxMath::RSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return invLength * sqrLength;
}

FORCEINLINE bool Vec3D::FixDegenerateNormal( void ) {
	if ( x == 0.0f ) {
		if ( y == 0.0f ) {
			if ( z > 0.0f ) {
				if ( z != 1.0f ) {
					z = 1.0f;
					return true;
				}
			} else {
				if ( z != -1.0f ) {
					z = -1.0f;
					return true;
				}
			}
			return false;
		} else if ( z == 0.0f ) {
			if ( y > 0.0f ) {
				if ( y != 1.0f ) {
					y = 1.0f;
					return true;
				}
			} else {
				if ( y != -1.0f ) {
					y = -1.0f;
					return true;
				}
			}
			return false;
		}
	} else if ( y == 0.0f ) {
		if ( z == 0.0f ) {
			if ( x > 0.0f ) {
				if ( x != 1.0f ) {
					x = 1.0f;
					return true;
				}
			} else {
				if ( x != -1.0f ) {
					x = -1.0f;
					return true;
				}
			}
			return false;
		}
	}
	if ( mxMath::Fabs( x ) == 1.0f ) {
		if ( y != 0.0f || z != 0.0f ) {
			y = z = 0.0f;
			return true;
		}
		return false;
	} else if ( mxMath::Fabs( y ) == 1.0f ) {
		if ( x != 0.0f || z != 0.0f ) {
			x = z = 0.0f;
			return true;
		}
		return false;
	} else if ( mxMath::Fabs( z ) == 1.0f ) {
		if ( x != 0.0f || y != 0.0f ) {
			x = y = 0.0f;
			return true;
		}
		return false;
	}
	return false;
}

FORCEINLINE bool Vec3D::FixDenormals( void ) {
	bool denormal = false;
	if ( mxMath::Fabs( x ) < 1e-30f ) {
		x = 0.0f;
		denormal = true;
	}
	if ( mxMath::Fabs( y ) < 1e-30f ) {
		y = 0.0f;
		denormal = true;
	}
	if ( mxMath::Fabs( z ) < 1e-30f ) {
		z = 0.0f;
		denormal = true;
	}
	return denormal;
}

FORCEINLINE bool Vec3D::IsNormalized( const FLOAT epsilon /* = VECTOR_EPSILON */ ) const {
	return mxMath::Fabs( LengthSqr() - 1.0f ) < epsilon;
}

FORCEINLINE Vec3D Vec3D::Cross( const Vec3D &a ) const {
	return Vec3D(
		y * a.z - z * a.y,
		z * a.x - x * a.z,
		x * a.y - y * a.x );
}

FORCEINLINE Vec3D & Vec3D::Cross( const Vec3D &a, const Vec3D &b ) {
	x = a.y * b.z - a.z * b.y;
	y = a.z * b.x - a.x * b.z;
	z = a.x * b.y - a.y * b.x;

	return *this;
}

FORCEINLINE FLOAT Vec3D::Length( void ) const {
	return ( FLOAT )mxMath::Sqrt( x * x + y * y + z * z );
}

FORCEINLINE FLOAT Vec3D::LengthSqr( void ) const {
	return ( x * x + y * y + z * z );
}

FORCEINLINE FLOAT Vec3D::LengthFast( void ) const {
	FLOAT sqrLength;

	sqrLength = x * x + y * y + z * z;
	return sqrLength * mxMath::RSqrt( sqrLength );
}

FORCEINLINE Vec3D Vec3D::GetNormalized() const {
	Vec3D  v( *this );
	v.Normalize();
	return v;
}

FORCEINLINE FLOAT Vec3D::Normalize( void ) {
	FLOAT sqrLength, invLength;

	sqrLength = x * x + y * y + z * z;
	invLength = mxMath::InvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return invLength * sqrLength;
}

FORCEINLINE Vec3D & Vec3D::Truncate( FLOAT length ) {
	FLOAT length2;
	FLOAT ilength;

	if ( !length ) {
		SetZero();
	}
	else {
		length2 = LengthSqr();
		if ( length2 > length * length ) {
			ilength = length * mxMath::InvSqrt( length2 );
			x *= ilength;
			y *= ilength;
			z *= ilength;
		}
	}

	return *this;
}

FORCEINLINE void Vec3D::Clamp( const Vec3D &min, const Vec3D &max ) {
	if ( x < min.x ) {
		x = min.x;
	} else if ( x > max.x ) {
		x = max.x;
	}
	if ( y < min.y ) {
		y = min.y;
	} else if ( y > max.y ) {
		y = max.y;
	}
	if ( z < min.z ) {
		z = min.z;
	} else if ( z > max.z ) {
		z = max.z;
	}
}

FORCEINLINE void Vec3D::Snap( void ) {
	x = mxMath::Floor( x + 0.5f );
	y = mxMath::Floor( y + 0.5f );
	z = mxMath::Floor( z + 0.5f );
}

FORCEINLINE void Vec3D::SnapInt( void ) {
	x = FLOAT( INT( x ) );
	y = FLOAT( INT( y ) );
	z = FLOAT( INT( z ) );
}

FORCEINLINE INT Vec3D::GetDimension( void ) const {
	return 3;
}

FORCEINLINE const Vec2D &Vec3D::ToVec2( void ) const {
	return *reinterpret_cast<const Vec2D *>(this);
}

FORCEINLINE Vec2D &Vec3D::ToVec2( void ) {
	return *reinterpret_cast<Vec2D *>(this);
}

FORCEINLINE const FLOAT *Vec3D::ToFloatPtr( void ) const {
	return &x;
}

FORCEINLINE FLOAT *Vec3D::ToFloatPtr( void ) {
	return &x;
}

FORCEINLINE void Vec3D::NormalVectors( Vec3D &left, Vec3D &down ) const {
	FLOAT d;

	d = x * x + y * y;
	if ( !d ) {
		left[0] = 1;
		left[1] = 0;
		left[2] = 0;
	} else {
		d = mxMath::InvSqrt( d );
		left[0] = -y * d;
		left[1] = x * d;
		left[2] = 0;
	}
	down = left.Cross( *this );
}

FORCEINLINE void Vec3D::OrthogonalBasis( Vec3D &left, Vec3D &up ) const {
	FLOAT l, s;

	if ( mxMath::Fabs( z ) > 0.7f ) {
		l = y * y + z * z;
		s = mxMath::InvSqrt( l );
		up[0] = 0;
		up[1] = z * s;
		up[2] = -y * s;
		left[0] = l * s;
		left[1] = -x * up[2];
		left[2] = x * up[1];
	}
	else {
		l = x * x + y * y;
		s = mxMath::InvSqrt( l );
		left[0] = -y * s;
		left[1] = x * s;
		left[2] = 0;
		up[0] = -z * left[1];
		up[1] = z * left[0];
		up[2] = l * s;
	}
}

FORCEINLINE void Vec3D::ProjectOntoPlane( const Vec3D &normal, const FLOAT overBounce ) {
	FLOAT backoff;
	
	backoff = *this * normal;
	
	if ( overBounce != 1.0 ) {
		if ( backoff < 0 ) {
			backoff *= overBounce;
		} else {
			backoff /= overBounce;
		}
	}

	*this -= backoff * normal;
}

FORCEINLINE bool Vec3D::ProjectAlongPlane( const Vec3D &normal, const FLOAT epsilon, const FLOAT overBounce ) {
	Vec3D cross;
	FLOAT len;

	cross = this->Cross( normal ).Cross( (*this) );
	// normalize so a fixed epsilon can be used
	cross.Normalize();
	len = normal * cross;
	if ( mxMath::Fabs( len ) < epsilon ) {
		return false;
	}
	cross *= overBounce * ( normal * (*this) ) / len;
	(*this) -= cross;
	return true;
}

FORCEINLINE bool Vec3D::IsValid() const
{
	return ! FLOAT_IS_INVALID( x )
		&& ! FLOAT_IS_INVALID( y )
		&& ! FLOAT_IS_INVALID( z );
}

FORCEINLINE bool Vec3D::IsZero() const
{
	return ( x == 0.0f ) && ( y == 0.0f ) && ( z == 0.0f );
}

FORCEINLINE Vec3D Vec3D::GetRandom( INT seed ) {
	mxRandom  random( seed );
	Vec3D v( random.CRandomFloat(), random.CRandomFloat(), random.CRandomFloat() );
	return v;
}

FORCEINLINE Vec3D Vec3D::GetRandomNormalized( INT seed ) {
	return GetRandom( seed ).GetNormalized();
}

//===============================================================
//
//	mxPolar3
//
//===============================================================

class mxPolar3 {
public:	
	FLOAT			radius, theta, phi;

					mxPolar3( void );
					explicit mxPolar3( const FLOAT radius, const FLOAT theta, const FLOAT phi );

	void 			Set( const FLOAT radius, const FLOAT theta, const FLOAT phi );

	FLOAT			operator[]( const INT index ) const;
	FLOAT &			operator[]( const INT index );
	mxPolar3		operator-() const;
	mxPolar3 &		operator=( const mxPolar3 &a );

	Vec3D			ToVec3( void ) const;
};

FORCEINLINE mxPolar3::mxPolar3( void ) {
}

FORCEINLINE mxPolar3::mxPolar3( const FLOAT radius, const FLOAT theta, const FLOAT phi ) {
	Assert( radius > 0 );
	this->radius = radius;
	this->theta = theta;
	this->phi = phi;
}
	
FORCEINLINE void mxPolar3::Set( const FLOAT radius, const FLOAT theta, const FLOAT phi ) {
	Assert( radius > 0 );
	this->radius = radius;
	this->theta = theta;
	this->phi = phi;
}

FORCEINLINE FLOAT mxPolar3::operator[]( const INT index ) const {
	return ( &radius )[ index ];
}

FORCEINLINE FLOAT &mxPolar3::operator[]( const INT index ) {
	return ( &radius )[ index ];
}

FORCEINLINE mxPolar3 mxPolar3::operator-() const {
	return mxPolar3( radius, -theta, -phi );
}

FORCEINLINE mxPolar3 &mxPolar3::operator=( const mxPolar3 &a ) { 
	radius = a.radius;
	theta = a.theta;
	phi = a.phi;
	return *this;
}

FORCEINLINE Vec3D mxPolar3::ToVec3( void ) const {
	FLOAT sp, cp, st, ct;
	mxMath::SinCos( phi, sp, cp );
	mxMath::SinCos( theta, st, ct );
 	return Vec3D( cp * radius * ct, cp * radius * st, radius * sp );
}

}//End of namespace abc

#endif /* !__MATH_VEC3D_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
