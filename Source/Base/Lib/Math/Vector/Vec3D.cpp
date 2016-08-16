/*
=============================================================================
	File:	Vec3D.cpp
	Desc:.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop

namespace abc {

//
//	Static members.
//
const Vec3D Vec3D::vec3_origin( 0.0f, 0.0f, 0.0f );

const Vec3D Vec3D::ZERO( 0, 0, 0 );

const Vec3D Vec3D::UNIT_SCALE( 1, 1, 1 );

const Vec3D Vec3D::UNIT_X( 1, 0, 0 );
const Vec3D Vec3D::UNIT_Y( 0, 1, 0 );
const Vec3D Vec3D::UNIT_Z( 0, 0, 1 );
const Vec3D Vec3D::NEGATIVE_UNIT_X( -1, 0, 0 );
const Vec3D Vec3D::NEGATIVE_UNIT_Y( 0, -1, 0 );
const Vec3D Vec3D::NEGATIVE_UNIT_Z( 0, 0, -1 );

//const Vec3D Vec3D::POSITIVE_INFINITY( +mxMath::INFINITY, +mxMath::INFINITY, +mxMath::INFINITY );
//const Vec3D Vec3D::NEGATIVE_INFINITY( -mxMath::INFINITY, -mxMath::INFINITY, -mxMath::INFINITY );
const Vec3D Vec3D::POSITIVE_INFINITY( +1e6, +1e6, +1e6 );
const Vec3D Vec3D::NEGATIVE_INFINITY( -1e6, -1e6, -1e6 );

/*
=============
Vec3D::ToYaw
=============
*/
FLOAT Vec3D::ToYaw( void ) const {
	FLOAT yaw;
	
	if ( ( y == 0.0f ) && ( x == 0.0f ) ) {
		yaw = 0.0f;
	} else {
		yaw = RAD2DEG( mxMath::ATan( y, x ) );
		if ( yaw < 0.0f ) {
			yaw += 360.0f;
		}
	}

	return yaw;
}

/*
=============
Vec3D::ToPitch
=============
*/
FLOAT Vec3D::ToPitch( void ) const {
	FLOAT	forward;
	FLOAT	pitch;
	
	if ( ( x == 0.0f ) && ( y == 0.0f ) ) {
		if ( z > 0.0f ) {
			pitch = 90.0f;
		} else {
			pitch = 270.0f;
		}
	} else {
		forward = ( FLOAT )mxMath::Sqrt( x * x + y * y );
		pitch = RAD2DEG( mxMath::ATan( z, forward ) );
		if ( pitch < 0.0f ) {
			pitch += 360.0f;
		}
	}

	return pitch;
}

/*
=============
Vec3D::ToAngles
=============
*/
mxAngles Vec3D::ToAngles( void ) const {
	FLOAT forward;
	FLOAT yaw;
	FLOAT pitch;
	
	if ( ( x == 0.0f ) && ( y == 0.0f ) ) {
		yaw = 0.0f;
		if ( z > 0.0f ) {
			pitch = 90.0f;
		} else {
			pitch = 270.0f;
		}
	} else {
		yaw = RAD2DEG( mxMath::ATan( y, x ) );
		if ( yaw < 0.0f ) {
			yaw += 360.0f;
		}

		forward = ( FLOAT )mxMath::Sqrt( x * x + y * y );
		pitch = RAD2DEG( mxMath::ATan( z, forward ) );
		if ( pitch < 0.0f ) {
			pitch += 360.0f;
		}
	}

	return mxAngles( -pitch, yaw, 0.0f );
}

/*
=============
Vec3D::ToPolar
=============
*/
mxPolar3 Vec3D::ToPolar( void ) const {
	FLOAT forward;
	FLOAT yaw;
	FLOAT pitch;
	
	if ( ( x == 0.0f ) && ( y == 0.0f ) ) {
		yaw = 0.0f;
		if ( z > 0.0f ) {
			pitch = 90.0f;
		} else {
			pitch = 270.0f;
		}
	} else {
		yaw = RAD2DEG( mxMath::ATan( y, x ) );
		if ( yaw < 0.0f ) {
			yaw += 360.0f;
		}

		forward = ( FLOAT )mxMath::Sqrt( x * x + y * y );
		pitch = RAD2DEG( mxMath::ATan( z, forward ) );
		if ( pitch < 0.0f ) {
			pitch += 360.0f;
		}
	}
	return mxPolar3( mxMath::Sqrt( x * x + y * y + z * z ), yaw, -pitch );
}

/*
=============
Vec3D::ToMat3
=============
*/
Matrix3 Vec3D::ToMat3( void ) const {
	Matrix3	mat;
	FLOAT	d;

	mat[0] = *this;
	d = x * x + y * y;
	if ( !d ) {
		mat[1][0] = 1.0f;
		mat[1][1] = 0.0f;
		mat[1][2] = 0.0f;
	} else {
		d = mxMath::InvSqrt( d );
		mat[1][0] = -y * d;
		mat[1][1] = x * d;
		mat[1][2] = 0.0f;
	}
	mat[2] = Cross( mat[1] );

	return mat;
}

/*
=============
Vec3D::ToMat3
=============
*/
Matrix3 Vec3D::ToMat3( INT axis ) const {
	Matrix3	mat;
	FLOAT	d;
	INT		index_x = axis % GetDimension();
	INT		index_y = (axis + 1) % GetDimension();
	INT		index_z = (axis + 2) % GetDimension();
	FLOAT	local_x = (*this)[index_x];
	FLOAT	local_y = (*this)[index_y];

	mat[axis] = *this;
	d = local_x * local_x + local_y * local_y;
	if ( !d ) {
		mat[index_y][index_x] = 1.0f;
		mat[index_y][index_y] = 0.0f;
		mat[index_y][index_z] = 0.0f;
	} else {
		d = mxMath::InvSqrt( d );
		mat[index_y][index_x] = -local_y * d;
		mat[index_y][index_y] = local_x * d;
		mat[index_y][index_z] = 0.0f;
	}
	mat[index_z] = Cross( mat[index_y] );

	return mat;
}

/*
=============
Vec3D::RotateAboutX
=============
*/
Vec3D& Vec3D::RotateAboutX( FLOAT angle )
{
   FLOAT s, c;
	mxMath::SinCos( angle, s, c );
    
    FLOAT ny = c * y - s * z;
    FLOAT nz = c * z + s * y;
    
    this->y = ny;
    this->z = nz;
    
    return *this;
}

/*
=============
Vec3D::RotateAboutY
=============
*/
Vec3D& Vec3D::RotateAboutY( FLOAT angle )
{
    FLOAT s, c;
	mxMath::SinCos( angle, s, c );

    FLOAT nx = c * x + s * z;
    FLOAT nz = c * z - s * x;
    
    this->x = nx;
    this->z = nz;
    
    return *this;
}

/*
=============
Vec3D::RotateAboutZ
=============
*/
Vec3D& Vec3D::RotateAboutZ( FLOAT angle )
{
    FLOAT s, c;
	mxMath::SinCos( angle, s, c );
    
    FLOAT nx = c * x - s * y;
    FLOAT ny = c * y + s * x;
    
    this->x = nx;
    this->y = ny;
    
    return *this;
}

/*
=============
Vec3D::RotateAboutAxis
=============
*/
Vec3D& Vec3D::RotateAboutAxis( FLOAT angle, const Vec3D& axis )
{
	FLOAT s, c;
	mxMath::SinCos( angle, s, c );

    FLOAT k = 1.0F - c;
    
    FLOAT nx = x * (c + k * axis.x * axis.x) + y * (k * axis.x * axis.y - s * axis.z)
            + z * (k * axis.x * axis.z + s * axis.y);
    FLOAT ny = x * (k * axis.x * axis.y + s * axis.z) + y * (c + k * axis.y * axis.y)
            + z * (k * axis.y * axis.z - s * axis.x);
    FLOAT nz = x * (k * axis.x * axis.z - s * axis.y) + y * (k * axis.y * axis.z + s * axis.x)
            + z * (c + k * axis.z * axis.z);
    
    this->x = nx;
    this->y = ny;
    this->z = nz;
    
    return *this;
}

/*
=============
Vec3D::ToString
=============
*/
const char *Vec3D::ToString( int precision ) const {
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

/*
=============
Lerp

Linearly inperpolates one vector to another.
=============
*/
void Vec3D::Lerp( const Vec3D &v1, const Vec3D &v2, const FLOAT l ) {
	if ( l <= 0.0f ) {
		(*this) = v1;
	} else if ( l >= 1.0f ) {
		(*this) = v2;
	} else {
		(*this) = v1 + l * ( v2 - v1 );
	}
}

/*
=============
SLerp

Spherical linear interpolation from v1 to v2.
Vectors are expected to be normalized.
=============
*/
#define LERP_DELTA 1e-6

void Vec3D::SLerp( const Vec3D &v1, const Vec3D &v2, const FLOAT t ) {
	FLOAT omega, cosom, sinom, scale0, scale1;

	if ( t <= 0.0f ) {
		(*this) = v1;
		return;
	} else if ( t >= 1.0f ) {
		(*this) = v2;
		return;
	}

	cosom = v1 * v2;
	if ( ( 1.0f - cosom ) > LERP_DELTA ) {
		omega = mxMath::ACos( cosom );
		sinom = mxMath::Sin( omega );
		scale0 = mxMath::Sin( ( 1.0f - t ) * omega ) / sinom;
		scale1 = mxMath::Sin( t * omega ) / sinom;
	} else {
		scale0 = 1.0f - t;
		scale1 = t;
	}

	(*this) = ( v1 * scale0 + v2 * scale1 );
}

/*
=============
ProjectSelfOntoSphere

Projects the z component onto a sphere.
=============
*/
void Vec3D::ProjectSelfOntoSphere( const FLOAT radius ) {
	FLOAT rsqr = radius * radius;
	FLOAT len = Length();
	if ( len  < rsqr * 0.5f ) {
		z = mxMath::Sqrt( rsqr - len );
	} else {
		z = rsqr / ( 2.0f * mxMath::Sqrt( len ) );
	}
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
