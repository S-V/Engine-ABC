/*
=============================================================================
	File:	Vec2D.h
	Desc:	2D vector.
			Copyright (C) 2004 Id Software, Inc. ( idVec2 )
=============================================================================
*/

#ifndef __MATH_VEC2D_H__
#define __MATH_VEC2D_H__

namespace abc {

//
//	Vec2D
//
class Vec2D {
public:
	FLOAT			x;
	FLOAT			y;

public:
					Vec2D( void );
					explicit Vec2D( const FLOAT x, const FLOAT y );
					explicit Vec2D( const FLOAT xy );

	void 			Set( const FLOAT x, const FLOAT y );
	void			SetZero( void );

	FLOAT			operator[]( INT index ) const;
	FLOAT &			operator[]( INT index );
	Vec2D			operator-() const;
	FLOAT			operator*( const Vec2D &a ) const;
	Vec2D			operator*( const FLOAT a ) const;
	Vec2D			operator/( const FLOAT a ) const;
	Vec2D			operator+( const Vec2D &a ) const;
	Vec2D			operator-( const Vec2D &a ) const;
	Vec2D &			operator+=( const Vec2D &a );
	Vec2D &			operator-=( const Vec2D &a );
	Vec2D &			operator/=( const Vec2D &a );
	Vec2D &			operator/=( const FLOAT a );
	Vec2D &			operator*=( const FLOAT a );

	friend Vec2D	operator*( const FLOAT a, const Vec2D b );

	bool			Compare( const Vec2D &a ) const;							// exact compare, no epsilon
	bool			Compare( const Vec2D &a, const FLOAT epsilon ) const;		// compare with epsilon
	bool			operator==(	const Vec2D &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const Vec2D &a ) const;						// exact compare, no epsilon

	FLOAT			Length( void ) const;
	FLOAT			LengthFast( void ) const;
	FLOAT			LengthSqr( void ) const;
	FLOAT			Normalize( void );			// returns length
	FLOAT			NormalizeFast( void );		// returns length
	Vec2D &			Truncate( FLOAT length );	// cap length
	void			Clamp( const Vec2D &min, const Vec2D &max );
	void			Snap( void );				// snap to closest integer value
	void			SnapInt( void );			// snap towards integer (floor)

	INT				GetDimension( void ) const;

	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToString( INT precision = 2 ) const;

	void			Lerp( const Vec2D &v1, const Vec2D &v2, const FLOAT l );

	Vec2D &			Rotate( FLOAT angle );

public:
	static const Vec2D vec2_origin;
	static const Vec2D vec2_unit;	// (1,1)
};

FORCEINLINE Vec2D::Vec2D( void ) {
}

FORCEINLINE Vec2D::Vec2D( const FLOAT x, const FLOAT y ) {
	this->x = x;
	this->y = y;
}

FORCEINLINE Vec2D::Vec2D( const FLOAT xy ) {
	this->x = xy;
	this->y = xy;
}

FORCEINLINE void Vec2D::Set( const FLOAT x, const FLOAT y ) {
	this->x = x;
	this->y = y;
}

FORCEINLINE void Vec2D::SetZero( void ) {
	x = y = 0.0f;
}

FORCEINLINE bool Vec2D::Compare( const Vec2D &a ) const {
	return ( ( x == a.x ) && ( y == a.y ) );
}

FORCEINLINE bool Vec2D::Compare( const Vec2D &a, const FLOAT epsilon ) const {
	if ( mxMath::Fabs( x - a.x ) > epsilon ) {
		return false;
	}
			
	if ( mxMath::Fabs( y - a.y ) > epsilon ) {
		return false;
	}

	return true;
}

FORCEINLINE bool Vec2D::operator==( const Vec2D &a ) const {
	return Compare( a );
}

FORCEINLINE bool Vec2D::operator!=( const Vec2D &a ) const {
	return !Compare( a );
}

FORCEINLINE FLOAT Vec2D::operator[]( INT index ) const {
	return ( &x )[ index ];
}

FORCEINLINE FLOAT& Vec2D::operator[]( INT index ) {
	return ( &x )[ index ];
}

FORCEINLINE FLOAT Vec2D::Length( void ) const {
	return ( FLOAT )mxMath::Sqrt( x * x + y * y );
}

FORCEINLINE FLOAT Vec2D::LengthFast( void ) const {
	FLOAT sqrLength;

	sqrLength = x * x + y * y;
	return sqrLength * mxMath::RSqrt( sqrLength );
}

FORCEINLINE FLOAT Vec2D::LengthSqr( void ) const {
	return ( x * x + y * y );
}

FORCEINLINE FLOAT Vec2D::Normalize( void ) {
	FLOAT sqrLength, invLength;

	sqrLength = x * x + y * y;
	invLength = mxMath::InvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	return invLength * sqrLength;
}

FORCEINLINE FLOAT Vec2D::NormalizeFast( void ) {
	FLOAT lengthSqr, invLength;

	lengthSqr = x * x + y * y;
	invLength = mxMath::RSqrt( lengthSqr );
	x *= invLength;
	y *= invLength;
	return invLength * lengthSqr;
}

FORCEINLINE Vec2D &Vec2D::Truncate( FLOAT length ) {
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
		}
	}

	return *this;
}

FORCEINLINE void Vec2D::Clamp( const Vec2D &min, const Vec2D &max ) {
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
}

FORCEINLINE void Vec2D::Snap( void ) {
	x = floor( x + 0.5f );
	y = floor( y + 0.5f );
}

FORCEINLINE void Vec2D::SnapInt( void ) {
	x = FLOAT( INT( x ) );
	y = FLOAT( INT( y ) );
}

FORCEINLINE Vec2D Vec2D::operator-() const {
	return Vec2D( -x, -y );
}
	
FORCEINLINE Vec2D Vec2D::operator-( const Vec2D &a ) const {
	return Vec2D( x - a.x, y - a.y );
}

FORCEINLINE FLOAT Vec2D::operator*( const Vec2D &a ) const {
	return x * a.x + y * a.y;
}

FORCEINLINE Vec2D Vec2D::operator*( const FLOAT a ) const {
	return Vec2D( x * a, y * a );
}

FORCEINLINE Vec2D Vec2D::operator/( const FLOAT a ) const {
	FLOAT inva = 1.0f / a;
	return Vec2D( x * inva, y * inva );
}

FORCEINLINE Vec2D operator*( const FLOAT a, const Vec2D b ) {
	return Vec2D( b.x * a, b.y * a );
}

FORCEINLINE Vec2D Vec2D::operator+( const Vec2D &a ) const {
	return Vec2D( x + a.x, y + a.y );
}

FORCEINLINE Vec2D &Vec2D::operator+=( const Vec2D &a ) {
	x += a.x;
	y += a.y;

	return *this;
}

FORCEINLINE Vec2D &Vec2D::operator/=( const Vec2D &a ) {
	x /= a.x;
	y /= a.y;

	return *this;
}

FORCEINLINE Vec2D &Vec2D::operator/=( const FLOAT a ) {
	FLOAT inva = 1.0f / a;
	x *= inva;
	y *= inva;

	return *this;
}

FORCEINLINE Vec2D &Vec2D::operator-=( const Vec2D &a ) {
	x -= a.x;
	y -= a.y;

	return *this;
}

FORCEINLINE Vec2D &Vec2D::operator*=( const FLOAT a ) {
	x *= a;
	y *= a;

	return *this;
}

FORCEINLINE INT Vec2D::GetDimension( void ) const {
	return 2;
}

FORCEINLINE const FLOAT *Vec2D::ToFloatPtr( void ) const {
	return &x;
}

FORCEINLINE FLOAT *Vec2D::ToFloatPtr( void ) {
	return &x;
}

}//End of namespace abc

#endif /* !__MATH_VEC2D_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
