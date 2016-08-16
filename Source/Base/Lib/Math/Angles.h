/*
=============================================================================
	File:	Angles.h
	Desc:	Euler angles.
	Note:	Originally written by Id Software.
	Copyright (C) 2004 Id Software, Inc. ( idAngles )
=============================================================================
*/

#ifndef __MATH_ANGLES_H__
#define __MATH_ANGLES_H__

/*
=============================================================================

	Euler angles

=============================================================================
*/

namespace abc {

enum EAngleIndices
{
	PITCH	= 0,	// up / down
	YAW		= 1,	// left / right
	ROLL	= 2		// fall over
};

class Vec3D;
class Quat;
class mxRotation;
class Matrix3;
class Matrix4;

//
//	mxAngles - Euler angles ( in degrees ).
//
class mxAngles {
public:
	FLOAT			pitch;	// In degrees.
	FLOAT			yaw;	// In degrees.
	FLOAT			roll;	// In degrees.

public:
					mxAngles( void );
					mxAngles( FLOAT pitch, FLOAT yaw, FLOAT roll );
					explicit mxAngles( const Vec3D &v );

	void 			Set( FLOAT pitch, FLOAT yaw, FLOAT roll );
	mxAngles &		Zero( void );

	FLOAT			operator[]( INT index ) const;
	FLOAT &			operator[]( INT index );
	mxAngles		operator-() const;			// negate angles, in general not the inverse rotation
	mxAngles &		operator=( const mxAngles &a );
	mxAngles		operator+( const mxAngles &a ) const;
	mxAngles &		operator+=( const mxAngles &a );
	mxAngles		operator-( const mxAngles &a ) const;
	mxAngles &		operator-=( const mxAngles &a );
	mxAngles		operator*( const FLOAT a ) const;
	mxAngles &		operator*=( const FLOAT a );
	mxAngles		operator/( const FLOAT a ) const;
	mxAngles &		operator/=( const FLOAT a );

	friend mxAngles	operator*( const FLOAT a, const mxAngles &b );

	bool			Compare( const mxAngles &a ) const;							// exact compare, no epsilon
	bool			Compare( const mxAngles &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool			operator==(	const mxAngles &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const mxAngles &a ) const;						// exact compare, no epsilon

	mxAngles &		Normalize360( void );	// normalizes 'this'
	mxAngles &		Normalize180( void );	// normalizes 'this'

	void			Clamp( const mxAngles &min, const mxAngles &max );

	INT				GetDimension( void ) const;

	void			ToVectors( Vec3D *forward, Vec3D *right = NULL, Vec3D *up = NULL ) const;
	Vec3D			ToForward( void ) const;
	Quat			ToQuat( void ) const;
	mxRotation		ToRotation( void ) const;
	Matrix3			ToMat3( void ) const;
	Matrix4			ToMat4( void ) const;
	Vec3D			ToAngularVelocity( void ) const;
	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToString( INT precision = 2 ) const;

public:
	static const mxAngles ZERO;
};

FORCEINLINE mxAngles::mxAngles( void ) {
}

FORCEINLINE mxAngles::mxAngles( FLOAT pitch, FLOAT yaw, FLOAT roll ) {
	this->pitch = pitch;
	this->yaw	= yaw;
	this->roll	= roll;
}

FORCEINLINE mxAngles::mxAngles( const Vec3D &v ) {
	this->pitch = v[0];
	this->yaw	= v[1];
	this->roll	= v[2];
}

FORCEINLINE void mxAngles::Set( FLOAT pitch, FLOAT yaw, FLOAT roll ) {
	this->pitch = pitch;
	this->yaw	= yaw;
	this->roll	= roll;
}

FORCEINLINE mxAngles &mxAngles::Zero( void ) {
	pitch = yaw = roll = 0.0f;
	return *this;
}

FORCEINLINE FLOAT mxAngles::operator[]( INT index ) const {
	Assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &pitch )[ index ];
}

FORCEINLINE FLOAT &mxAngles::operator[]( INT index ) {
	Assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &pitch )[ index ];
}

FORCEINLINE mxAngles mxAngles::operator-() const {
	return mxAngles( -pitch, -yaw, -roll );
}

FORCEINLINE mxAngles &mxAngles::operator=( const mxAngles &a ) {
	pitch	= a.pitch;
	yaw		= a.yaw;
	roll	= a.roll;
	return *this;
}

FORCEINLINE mxAngles mxAngles::operator+( const mxAngles &a ) const {
	return mxAngles( pitch + a.pitch, yaw + a.yaw, roll + a.roll );
}

FORCEINLINE mxAngles& mxAngles::operator+=( const mxAngles &a ) {
	pitch	+= a.pitch;
	yaw		+= a.yaw;
	roll	+= a.roll;

	return *this;
}

FORCEINLINE mxAngles mxAngles::operator-( const mxAngles &a ) const {
	return mxAngles( pitch - a.pitch, yaw - a.yaw, roll - a.roll );
}

FORCEINLINE mxAngles& mxAngles::operator-=( const mxAngles &a ) {
	pitch	-= a.pitch;
	yaw		-= a.yaw;
	roll	-= a.roll;
	return *this;
}

FORCEINLINE mxAngles mxAngles::operator*( const FLOAT a ) const {
	return mxAngles( pitch * a, yaw * a, roll * a );
}

FORCEINLINE mxAngles& mxAngles::operator*=( FLOAT a ) {
	pitch	*= a;
	yaw		*= a;
	roll	*= a;
	return *this;
}

FORCEINLINE mxAngles mxAngles::operator/( const FLOAT a ) const {
	FLOAT inva = 1.0f / a;
	return mxAngles( pitch * inva, yaw * inva, roll * inva );
}

FORCEINLINE mxAngles& mxAngles::operator/=( FLOAT a ) {
	FLOAT inva = 1.0f / a;
	pitch	*= inva;
	yaw		*= inva;
	roll	*= inva;
	return *this;
}

FORCEINLINE mxAngles operator*( const FLOAT a, const mxAngles &b ) {
	return mxAngles( a * b.pitch, a * b.yaw, a * b.roll );
}

FORCEINLINE bool mxAngles::Compare( const mxAngles &a ) const {
	return ( ( a.pitch == pitch ) && ( a.yaw == yaw ) && ( a.roll == roll ) );
}

FORCEINLINE bool mxAngles::Compare( const mxAngles &a, const FLOAT epsilon ) const {
	if ( mxMath::Fabs( pitch - a.pitch ) > epsilon ) {
		return false;
	}
			
	if ( mxMath::Fabs( yaw - a.yaw ) > epsilon ) {
		return false;
	}

	if ( mxMath::Fabs( roll - a.roll ) > epsilon ) {
		return false;
	}

	return true;
}

FORCEINLINE bool mxAngles::operator==( const mxAngles &a ) const {
	return Compare( a );
}

FORCEINLINE bool mxAngles::operator!=( const mxAngles &a ) const {
	return !Compare( a );
}

FORCEINLINE void mxAngles::Clamp( const mxAngles &min, const mxAngles &max ) {
	if ( pitch < min.pitch ) {
		pitch = min.pitch;
	} else if ( pitch > max.pitch ) {
		pitch = max.pitch;
	}
	if ( yaw < min.yaw ) {
		yaw = min.yaw;
	} else if ( yaw > max.yaw ) {
		yaw = max.yaw;
	}
	if ( roll < min.roll ) {
		roll = min.roll;
	} else if ( roll > max.roll ) {
		roll = max.roll;
	}
}

FORCEINLINE INT mxAngles::GetDimension( void ) const {
	return 3;
}

FORCEINLINE const FLOAT *mxAngles::ToFloatPtr( void ) const {
	return &pitch;
}

FORCEINLINE FLOAT *mxAngles::ToFloatPtr( void ) {
	return &pitch;
}

}//End of namespace abc

#endif /* !__MATH_ANGLES_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
