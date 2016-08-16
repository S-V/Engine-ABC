/*
=============================================================================
	File:	Quat.h
	Desc:	Quaternion class.
	Note:	Originally written by Id Software.
	Copyright (C) 2004 Id Software, Inc. ( idQuat )
=============================================================================
*/

#ifndef __MATH_QUAT_H__
#define __MATH_QUAT_H__

/*
=============================================================================

	Quaternion

=============================================================================
*/

namespace abc {

class Vec3D;
class mxAngles;
class mxRotation;
class Matrix3;
class Matrix4;
class mxCQuat;

//
//	Quat
//
class Quat {
public:
	FLOAT			x;
	FLOAT			y;
	FLOAT			z;
	FLOAT			w;

public:
					Quat( void );
					Quat( FLOAT x, FLOAT y, FLOAT z, FLOAT w );
					Quat( const Vec3D& axis, FLOAT angle );
					Quat( EInitIdentity );

	void 			Set( FLOAT x, FLOAT y, FLOAT z, FLOAT w );
	void			SetZero();
	void			SetIdentity();

	FLOAT			operator[]( INT index ) const;
	FLOAT &			operator[]( INT index );
	Quat			operator-() const;
	Quat &			operator=( const Quat &a );
	Quat			operator+( const Quat &a ) const;
	Quat &			operator+=( const Quat &a );
	Quat			operator-( const Quat &a ) const;
	Quat &			operator-=( const Quat &a );
	Quat			operator*( const Quat &a ) const;
	Vec3D			operator*( const Vec3D &a ) const;
	Quat			operator*( FLOAT a ) const;
	Quat &			operator*=( const Quat &a );
	Quat &			operator*=( FLOAT a );

	friend Quat		operator*( const FLOAT a, const Quat &b );
	friend Vec3D	operator*( const Vec3D &a, const Quat &b );

	bool			Compare( const Quat &a ) const;						// exact compare, no epsilon
	bool			Compare( const Quat &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool			operator==(	const Quat &a ) const;					// exact compare, no epsilon
	bool			operator!=(	const Quat &a ) const;					// exact compare, no epsilon

	Quat			Inverse( void ) const;
	FLOAT			Length( void ) const;
	Quat &			Normalize( void );

	FLOAT			CalcW( void ) const;
	INT				GetDimension( void ) const;

	mxAngles		ToAngles( void ) const;
	mxRotation		ToRotation( void ) const;
	Matrix3			ToMat3( void ) const;	// NOTE: this quaternion must be normalized.
	Matrix4			ToMat4( void ) const;	// NOTE: this quaternion must be normalized.
	mxCQuat			ToCQuat( void ) const;
	Vec3D			ToAngularVelocity( void ) const;
	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToString( INT precision = 2 ) const;

	Quat &		Slerp( const Quat &from, const Quat &to, FLOAT t );

	// Testing & Debugging.

	bool	IsNormalized( const FLOAT tolerance = 0.0f ) const;

public:
	static const Quat	ZERO;			// Addition identity quaternion.
	static const Quat	QUAT_IDENTITY;	// Multiplication identity quaternion.
};

FORCEINLINE Quat::Quat( void ) {
}

FORCEINLINE Quat::Quat( FLOAT x, FLOAT y, FLOAT z, FLOAT w ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

FORCEINLINE Quat::Quat( const Vec3D& axis, FLOAT angle )
{
	Assert( axis.IsNormalized() );
	FLOAT halfAngle = angle * 0.5f;
	FLOAT sine = mxMath::Sin( halfAngle );

	this->x = sine * axis.x;
	this->y = sine * axis.y;
	this->z = sine * axis.z;
	this->w = mxMath::Cos( halfAngle );
}

FORCEINLINE Quat::Quat( EInitIdentity )
{
	*this = QUAT_IDENTITY;
}

FORCEINLINE FLOAT Quat::operator[]( INT index ) const {
	Assert( ( index >= 0 ) && ( index < 4 ) );
	return ( &x )[ index ];
}

FORCEINLINE FLOAT& Quat::operator[]( INT index ) {
	Assert( ( index >= 0 ) && ( index < 4 ) );
	return ( &x )[ index ];
}

FORCEINLINE Quat Quat::operator-() const {
	return Quat( -x, -y, -z, -w );
}

FORCEINLINE Quat &Quat::operator=( const Quat &a ) {
	x = a.x;
	y = a.y;
	z = a.z;
	w = a.w;

	return *this;
}

FORCEINLINE Quat Quat::operator+( const Quat &a ) const {
	return Quat( x + a.x, y + a.y, z + a.z, w + a.w );
}

FORCEINLINE Quat& Quat::operator+=( const Quat &a ) {
	x += a.x;
	y += a.y;
	z += a.z;
	w += a.w;

	return *this;
}

FORCEINLINE Quat Quat::operator-( const Quat &a ) const {
	return Quat( x - a.x, y - a.y, z - a.z, w - a.w );
}

FORCEINLINE Quat& Quat::operator-=( const Quat &a ) {
	x -= a.x;
	y -= a.y;
	z -= a.z;
	w -= a.w;

	return *this;
}

FORCEINLINE Quat Quat::operator*( const Quat &a ) const {
	return Quat(	w*a.x + x*a.w + y*a.z - z*a.y,
					w*a.y + y*a.w + z*a.x - x*a.z,
					w*a.z + z*a.w + x*a.y - y*a.x,
					w*a.w - x*a.x - y*a.y - z*a.z );
}

FORCEINLINE Vec3D Quat::operator*( const Vec3D &a ) const {
#if 0
	MX_OPTIMIZE( is it faster to do the conversion to a 3x3 matrix and multiply the vector by this 3x3 matrix? )
	return ( ToMat3() * a );
#else
	// result = this->Inverse() * Quat( a.x, a.y, a.z, 0.0f ) * (*this)
	FLOAT xxzz = x*x - z*z;
	FLOAT wwyy = w*w - y*y;

	FLOAT xw2 = x*w*2.0f;
	FLOAT xy2 = x*y*2.0f;
	FLOAT xz2 = x*z*2.0f;
	FLOAT yw2 = y*w*2.0f;
	FLOAT yz2 = y*z*2.0f;
	FLOAT zw2 = z*w*2.0f;

	return Vec3D(
		(xxzz + wwyy)*a.x		+ (xy2 + zw2)*a.y		+ (xz2 - yw2)*a.z,
		(xy2 - zw2)*a.x			+ (y*y+w*w-x*x-z*z)*a.y	+ (yz2 + xw2)*a.z,
		(xz2 + yw2)*a.x			+ (yz2 - xw2)*a.y		+ (wwyy - xxzz)*a.z
	);
#endif
}

FORCEINLINE Quat Quat::operator*( FLOAT a ) const {
	return Quat( x * a, y * a, z * a, w * a );
}

FORCEINLINE Quat operator*( const FLOAT a, const Quat &b ) {
	return b * a;
}

FORCEINLINE Vec3D operator*( const Vec3D &a, const Quat &b ) {
	return b * a;
}

FORCEINLINE Quat& Quat::operator*=( const Quat &a ) {
	*this = *this * a;

	return *this;
}

FORCEINLINE Quat& Quat::operator*=( FLOAT a ) {
	x *= a;
	y *= a;
	z *= a;
	w *= a;

	return *this;
}

FORCEINLINE bool Quat::Compare( const Quat &a ) const {
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) && ( w == a.w ) );
}

FORCEINLINE bool Quat::Compare( const Quat &a, const FLOAT epsilon ) const {
	if ( mxMath::Fabs( x - a.x ) > epsilon ) {
		return false;
	}
	if ( mxMath::Fabs( y - a.y ) > epsilon ) {
		return false;
	}
	if ( mxMath::Fabs( z - a.z ) > epsilon ) {
		return false;
	}
	if ( mxMath::Fabs( w - a.w ) > epsilon ) {
		return false;
	}
	return true;
}

FORCEINLINE bool Quat::operator==( const Quat &a ) const {
	return Compare( a );
}

FORCEINLINE bool Quat::operator!=( const Quat &a ) const {
	return !Compare( a );
}

FORCEINLINE void Quat::Set( FLOAT x, FLOAT y, FLOAT z, FLOAT w ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

FORCEINLINE void Quat::SetZero() {
	*this = Quat::ZERO;
}

FORCEINLINE void Quat::SetIdentity() {
	*this = QUAT_IDENTITY;
}

FORCEINLINE Quat Quat::Inverse( void ) const {
	return Quat( -x, -y, -z, w );
}

FORCEINLINE FLOAT Quat::Length( void ) const {
	FLOAT len;

	len = x * x + y * y + z * z + w * w;
	return mxMath::Sqrt( len );
}

FORCEINLINE Quat& Quat::Normalize( void )
{
#if 0
	FLOAT len;
	FLOAT ilength;

	len = this->Length();
	if ( len ) {
		ilength = 1 / len;
		x *= ilength;
		y *= ilength;
		z *= ilength;
		w *= ilength;
	}
#else
	FLOAT len = this->Length();
	MX_WARNING( "Unsafe code:" )
	FLOAT ilength = mxMath::Reciprocal( len );
	x *= ilength;
	y *= ilength;
	z *= ilength;
	w *= ilength;
#endif
	return *this;
}

FORCEINLINE FLOAT Quat::CalcW( void ) const {
	// take the absolute value because floating point rounding may cause the dot of x,y,z to be larger than 1
	return mxMath::Sqrt( mxMath::Fabs( 1.0f - ( x * x + y * y + z * z ) ) );
}

FORCEINLINE INT Quat::GetDimension( void ) const {
	return 4;
}

FORCEINLINE const FLOAT *Quat::ToFloatPtr( void ) const {
	return &x;
}

FORCEINLINE FLOAT *Quat::ToFloatPtr( void ) {
	return &x;
}

//===========================================================================

// Multiply two quaternions.
//
FORCEINLINE Quat MulQuatQuat( const Quat& q1, const Quat& q2 )
{
	return Quat(
		q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
		q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z );
}

// Multiply a quaternion with a vector.
//
FORCEINLINE Quat MulQuatVec3( const Quat& q, const Vec3D& w )
{
	return Quat(
		q.w * w.x + q.y * w.z - q.z * w.y,
		q.w * w.y + q.z * w.x - q.x * w.z,
		q.w * w.z + q.x * w.y - q.y * w.x,
		-q.x * w.x - q.y * w.y - q.z * w.z );
}

// Multiply a vector with a quaternion.
//
FORCEINLINE Quat MulVec3Quat( const Vec3D& w, const Quat& q )
{
	return Quat(
		w.x * q.w + w.y * q.z - w.z * q.y,
		w.y * q.w + w.z * q.x - w.x * q.z,
		w.z * q.w + w.x * q.y - w.y * q.x,
		-w.x * q.x - w.y * q.y - w.z * q.z );
}

/*
=============================================================================

	Compressed quaternion

=============================================================================
*/

class mxCQuat {
public:
	FLOAT			x;
	FLOAT			y;
	FLOAT			z;

public:
					mxCQuat( void );
					mxCQuat( FLOAT x, FLOAT y, FLOAT z );

	void 			Set( FLOAT x, FLOAT y, FLOAT z );

	FLOAT			operator[]( INT index ) const;
	FLOAT &			operator[]( INT index );

	bool			Compare( const mxCQuat &a ) const;						// exact compare, no epsilon
	bool			Compare( const mxCQuat &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool			operator==(	const mxCQuat &a ) const;					// exact compare, no epsilon
	bool			operator!=(	const mxCQuat &a ) const;					// exact compare, no epsilon

	INT				GetDimension( void ) const;

	mxAngles		ToAngles( void ) const;
	mxRotation		ToRotation( void ) const;
	Matrix3			ToMat3( void ) const;
	Matrix4			ToMat4( void ) const;
	Quat			ToQuat( void ) const;
	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToString( INT precision = 2 ) const;
};

FORCEINLINE mxCQuat::mxCQuat( void ) {
}

FORCEINLINE mxCQuat::mxCQuat( FLOAT x, FLOAT y, FLOAT z ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

FORCEINLINE void mxCQuat::Set( FLOAT x, FLOAT y, FLOAT z ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

FORCEINLINE FLOAT mxCQuat::operator[]( INT index ) const {
	Assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &x )[ index ];
}

FORCEINLINE FLOAT& mxCQuat::operator[]( INT index ) {
	Assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &x )[ index ];
}

FORCEINLINE bool mxCQuat::Compare( const mxCQuat &a ) const {
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) );
}

FORCEINLINE bool mxCQuat::Compare( const mxCQuat &a, const FLOAT epsilon ) const {
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

FORCEINLINE bool mxCQuat::operator==( const mxCQuat &a ) const {
	return Compare( a );
}

FORCEINLINE bool mxCQuat::operator!=( const mxCQuat &a ) const {
	return !Compare( a );
}

FORCEINLINE INT mxCQuat::GetDimension( void ) const {
	return 3;
}

FORCEINLINE Quat mxCQuat::ToQuat( void ) const
{
	// Take the absolute value because floating point rounding may cause the dot of x,y,z to be larger than 1.
	return Quat( x, y, z, mxMath::Sqrt( mxMath::Fabs( 1.0f - ( x * x + y * y + z * z ) ) ) );
}

FORCEINLINE const FLOAT *mxCQuat::ToFloatPtr( void ) const {
	return &x;
}

FORCEINLINE FLOAT *mxCQuat::ToFloatPtr( void ) {
	return &x;
}

}//End of namespace abc

#endif /* !__MATH_QUAT_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
