/*
=============================================================================
	File:	Angles.cpp
	Desc:	
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
//#include <Base.h>

#include <float.h>

namespace abc {

const mxAngles mxAngles::ZERO( 0.0f, 0.0f, 0.0f );

/*
=================
mxAngles::Normalize360

returns angles normalized to the range [0 <= angle < 360]
=================
*/
mxAngles& mxAngles::Normalize360( void ) {
	INT i;

	for ( i = 0; i < 3; i++ ) {
		if ( ( (*this)[i] >= 360.0f ) || ( (*this)[i] < 0.0f ) ) {
			(*this)[i] -= floor( (*this)[i] / 360.0f ) * 360.0f;

			if ( (*this)[i] >= 360.0f ) {
				(*this)[i] -= 360.0f;
			}
			if ( (*this)[i] < 0.0f ) {
				(*this)[i] += 360.0f;
			}
		}
	}

	return *this;
}

/*
=================
mxAngles::Normalize180

returns angles normalized to the range [-180 < angle <= 180]
=================
*/
mxAngles& mxAngles::Normalize180( void ) {
	Normalize360();

	if ( pitch > 180.0f ) {
		pitch -= 360.0f;
	}
	
	if ( yaw > 180.0f ) {
		yaw -= 360.0f;
	}

	if ( roll > 180.0f ) {
		roll -= 360.0f;
	}
	return *this;
}

/*
=================
mxAngles::ToVectors
=================
*/
void mxAngles::ToVectors( Vec3D *forward, Vec3D *right, Vec3D *up ) const {
	FLOAT sr, sp, sy, cr, cp, cy;
	
	mxMath::SinCos( DEG2RAD( yaw ), sy, cy );
	mxMath::SinCos( DEG2RAD( pitch ), sp, cp );
	mxMath::SinCos( DEG2RAD( roll ), sr, cr );

	if ( forward ) {
		forward->Set( cp * cy, cp * sy, -sp );
	}

	if ( right ) {
		right->Set( -sr * sp * cy + cr * sy, -sr * sp * sy + -cr * cy, -sr * cp );
	}

	if ( up ) {
		up->Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );
	}
}

/*
=================
mxAngles::ToForward
=================
*/
Vec3D mxAngles::ToForward( void ) const {
	FLOAT sp, sy, cp, cy;

	mxMath::SinCos( DEG2RAD( yaw ), sy, cy );
	mxMath::SinCos( DEG2RAD( pitch ), sp, cp );

	return Vec3D( cp * cy, cp * sy, -sp );
}

/*
=================
mxAngles::ToQuat
=================
*/
Quat mxAngles::ToQuat( void ) const {
	FLOAT sx, cx, sy, cy, sz, cz;
	FLOAT sxcy, cxcy, sxsy, cxsy;

	mxMath::SinCos( DEG2RAD( yaw ) * 0.5f, sz, cz );
	mxMath::SinCos( DEG2RAD( pitch ) * 0.5f, sy, cy );
	mxMath::SinCos( DEG2RAD( roll ) * 0.5f, sx, cx );

	sxcy = sx * cy;
	cxcy = cx * cy;
	sxsy = sx * sy;
	cxsy = cx * sy;

	return Quat( cxsy*sz - sxcy*cz, -cxsy*cz - sxcy*sz, sxsy*cz - cxcy*sz, cxcy*cz + sxsy*sz );
}

/*
=================
mxAngles::ToRotation
=================
*/
mxRotation mxAngles::ToRotation( void ) const {
	Vec3D vec;
	FLOAT angle, w;
	FLOAT sx, cx, sy, cy, sz, cz;
	FLOAT sxcy, cxcy, sxsy, cxsy;

	if ( pitch == 0.0f ) {
		if ( yaw == 0.0f ) {
			return mxRotation( Vec3D::vec3_origin, Vec3D( -1.0f, 0.0f, 0.0f ), roll );
		}
		if ( roll == 0.0f ) {
			return mxRotation( Vec3D::vec3_origin, Vec3D( 0.0f, 0.0f, -1.0f ), yaw );
		}
	} else if ( yaw == 0.0f && roll == 0.0f ) {
		return mxRotation( Vec3D::vec3_origin, Vec3D( 0.0f, -1.0f, 0.0f ), pitch );
	}

	mxMath::SinCos( DEG2RAD( yaw ) * 0.5f, sz, cz );
	mxMath::SinCos( DEG2RAD( pitch ) * 0.5f, sy, cy );
	mxMath::SinCos( DEG2RAD( roll ) * 0.5f, sx, cx );

	sxcy = sx * cy;
	cxcy = cx * cy;
	sxsy = sx * sy;
	cxsy = cx * sy;

	vec.x =  cxsy * sz - sxcy * cz;
	vec.y = -cxsy * cz - sxcy * sz;
	vec.z =  sxsy * cz - cxcy * sz;
	w =		 cxcy * cz + sxsy * sz;
	angle = mxMath::ACos( w );
	if ( angle == 0.0f ) {
		vec.Set( 0.0f, 0.0f, 1.0f );
	} else {
		//vec *= (1.0f / sin( angle ));
		vec.Normalize();
		vec.FixDegenerateNormal();
		angle *= 2.0f * mxMath::M_RAD2DEG;
	}
	return mxRotation( Vec3D::vec3_origin, vec, angle );
}

/*
=================
mxAngles::ToMat3
=================
*/
Matrix3 mxAngles::ToMat3( void ) const {
	Matrix3 mat;
	FLOAT sr, sp, sy, cr, cp, cy;

	mxMath::SinCos( DEG2RAD( yaw ), sy, cy );
	mxMath::SinCos( DEG2RAD( pitch ), sp, cp );
	mxMath::SinCos( DEG2RAD( roll ), sr, cr );

	mat[ 0 ].Set( cp * cy, cp * sy, -sp );
	mat[ 1 ].Set( sr * sp * cy + cr * -sy, sr * sp * sy + cr * cy, sr * cp );
	mat[ 2 ].Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );

	return mat;
}

/*
=================
mxAngles::ToMat4
=================
*/
Matrix4 mxAngles::ToMat4( void ) const {
	return ToMat3().ToMat4();
}

/*
=================
mxAngles::ToAngularVelocity
=================
*/
Vec3D mxAngles::ToAngularVelocity( void ) const {
	mxRotation rotation = mxAngles::ToRotation();
	return rotation.GetVec() * DEG2RAD( rotation.GetAngle() );
}

/*
=============
mxAngles::ToString
=============
*/
const char *mxAngles::ToString( INT precision ) const {
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
