/*
=============================================================================
	File:	Rotation.h
	Desc:	Class for representing rotations around an arbitrary axis.
	Note:	Originally written by Id Software.
	Copyright (C) 2004 Id Software, Inc. ( idRotation )
=============================================================================
*/

#ifndef __MATH_ROTATION_H__
#define __MATH_ROTATION_H__

/*
=============================================================================

	Describes a complete rotation in degrees about an arbitrary axis.
	A local rotation matrix is stored for fast rotation of multiple points.

=============================================================================
*/

namespace abc {

class mxAngles;
class Quat;
class Matrix3;

//
//	mxRotation
//
class mxRotation {

	friend class mxAngles;
	friend class Quat;
	friend class Matrix3;

public:
						mxRotation( void );
						mxRotation( const Vec3D &rotationOrigin, const Vec3D &rotationVec, const FLOAT rotationAngle );

	void				Set( const Vec3D &rotationOrigin, const Vec3D &rotationVec, const FLOAT rotationAngle );
	void				SetOrigin( const Vec3D &rotationOrigin );
	void				SetVec( const Vec3D &rotationVec );					// has to be normalized
	void				SetVec( const FLOAT x, const FLOAT y, const FLOAT z );	// has to be normalized
	void				SetAngle( const FLOAT rotationAngle );
	void				Scale( const FLOAT s );
	void				ReCalculateMatrix( void );
	const Vec3D &		GetOrigin( void ) const;
	const Vec3D &		GetVec( void ) const;
	FLOAT				GetAngle( void ) const;

	mxRotation			operator-() const;										// flips rotation
	mxRotation			operator*( const FLOAT s ) const;						// scale rotation
	mxRotation			operator/( const FLOAT s ) const;						// scale rotation
	mxRotation &		operator*=( const FLOAT s );							// scale rotation
	mxRotation &		operator/=( const FLOAT s );							// scale rotation
	Vec3D				operator*( const Vec3D &v ) const;						// rotate vector

	friend mxRotation	operator*( const FLOAT s, const mxRotation &r );		// scale rotation
	friend Vec3D		operator*( const Vec3D &v, const mxRotation &r );		// rotate vector
	friend Vec3D &		operator*=( Vec3D &v, const mxRotation &r );			// rotate vector

	mxAngles			ToAngles( void ) const;
	Quat				ToQuat( void ) const;
	const Matrix3 &		ToMat3( void ) const;
	Matrix4				ToMat4( void ) const;
	Vec3D				ToAngularVelocity( void ) const;

	void				RotatePoint( Vec3D &point ) const;

	void				Normalize180( void );
	void				Normalize360( void );

private:
	Vec3D				origin;			// origin of rotation
	Vec3D				vec;			// normalized vector to rotate around
	FLOAT				angle;			// angle of rotation in degrees
	mutable Matrix3		axis;			// rotation axis
	mutable bool		axisValid;		// true if rotation axis is valid
};


FORCEINLINE mxRotation::mxRotation( void ) {
}

FORCEINLINE mxRotation::mxRotation( const Vec3D &rotationOrigin, const Vec3D &rotationVec, const FLOAT rotationAngle ) {
	Assert( rotationVec.IsNormalized() );
	this->origin	= rotationOrigin;
	this->vec		= rotationVec;
	this->angle		= rotationAngle;
	this->axisValid = false;
}

FORCEINLINE void mxRotation::Set( const Vec3D &rotationOrigin, const Vec3D &rotationVec, const FLOAT rotationAngle ) {
	Assert( rotationVec.IsNormalized() );
	this->origin	= rotationOrigin;
	this->vec		= rotationVec;
	this->angle		= rotationAngle;
	this->axisValid = false;
}

FORCEINLINE void mxRotation::SetOrigin( const Vec3D &rotationOrigin ) {
	origin = rotationOrigin;
}

FORCEINLINE void mxRotation::SetVec( const Vec3D &rotationVec ) {
	vec = rotationVec;
	axisValid = false;
}

FORCEINLINE void mxRotation::SetVec( FLOAT x, FLOAT y, FLOAT z ) {
	vec[0] = x;
	vec[1] = y;
	vec[2] = z;
	axisValid = false;
}

FORCEINLINE void mxRotation::SetAngle( const FLOAT rotationAngle ) {
	angle = rotationAngle;
	axisValid = false;
}

FORCEINLINE void mxRotation::Scale( const FLOAT s ) {
	angle *= s;
	axisValid = false;
}

FORCEINLINE void mxRotation::ReCalculateMatrix( void ) {
	axisValid = false;
	ToMat3();
}

FORCEINLINE const Vec3D &mxRotation::GetOrigin( void ) const {
	return origin;
}

FORCEINLINE const Vec3D &mxRotation::GetVec( void ) const  {
	return vec;
}

FORCEINLINE FLOAT mxRotation::GetAngle( void ) const  {
	return angle;
}

FORCEINLINE mxRotation mxRotation::operator-() const {
	return mxRotation( origin, vec, -angle );
}

FORCEINLINE mxRotation mxRotation::operator*( const FLOAT s ) const {
	return mxRotation( origin, vec, angle * s );
}

FORCEINLINE mxRotation mxRotation::operator/( const FLOAT s ) const {
	Assert( s != 0.0f );
	return mxRotation( origin, vec, angle / s );
}

FORCEINLINE mxRotation &mxRotation::operator*=( const FLOAT s ) {
	angle *= s;
	axisValid = false;
	return *this;
}

FORCEINLINE mxRotation &mxRotation::operator/=( const FLOAT s ) {
	Assert( s != 0.0f );
	angle /= s;
	axisValid = false;
	return *this;
}

FORCEINLINE Vec3D mxRotation::operator*( const Vec3D &v ) const {
	if ( !axisValid ) {
		ToMat3();
	}
	return ((v - origin) * axis + origin);
}

FORCEINLINE mxRotation operator*( const FLOAT s, const mxRotation &r ) {
	return r * s;
}

FORCEINLINE Vec3D operator*( const Vec3D &v, const mxRotation &r ) {
	return r * v;
}

FORCEINLINE Vec3D &operator*=( Vec3D &v, const mxRotation &r ) {
	v = r * v;
	return v;
}

FORCEINLINE void mxRotation::RotatePoint( Vec3D &point ) const {
	if ( !axisValid ) {
		ToMat3();
	}
	point = ((point - origin) * axis + origin);
}

}//End of namespace abc

#endif /* !__MATH_ROTATION_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
