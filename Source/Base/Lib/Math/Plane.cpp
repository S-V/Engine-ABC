/*
=============================================================================
	File:	Plane.cpp
	Desc:
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
//#include <Base.h>

namespace abc {

const mxPlane mxPlane::PLANE_ORIGIN( 0.0f, 0.0f, 0.0f, 0.0f );

const mxPlane mxPlane::PLANE_ZERO_X( 1.0f, 0.0f, 0.0f, 0.0f );
const mxPlane mxPlane::PLANE_ZERO_Y( 0.0f, 1.0f, 0.0f, 0.0f );
const mxPlane mxPlane::PLANE_ZERO_Z( 0.0f, 0.0f, 1.0f, 0.0f );

/*
================
mxPlane::Type
================
*/
EPlaneType mxPlane::Type( void ) const {
	if ( Normal()[0] == 0.0f ) {
		if ( Normal()[1] == 0.0f ) {
			return Normal()[2] > 0.0f ? EPlaneType::PLANETYPE_Z : EPlaneType::PLANETYPE_NEGZ;
		}
		else if ( Normal()[2] == 0.0f ) {
			return Normal()[1] > 0.0f ? EPlaneType::PLANETYPE_Y : EPlaneType::PLANETYPE_NEGY;
		}
		else {
			return EPlaneType::PLANETYPE_ZEROX;
		}
	}
	else if ( Normal()[1] == 0.0f ) {
		if ( Normal()[2] == 0.0f ) {
			return Normal()[0] > 0.0f ? EPlaneType::PLANETYPE_X : EPlaneType::PLANETYPE_NEGX;
		}
		else {
			return EPlaneType::PLANETYPE_ZEROY;
		}
	}
	else if ( Normal()[2] == 0.0f ) {
		return EPlaneType::PLANETYPE_ZEROZ;
	}
	else {
		return EPlaneType::PLANETYPE_NONAXIAL;
	}
}

/*
================
mxPlane::HeightFit
================
*/
bool mxPlane::HeightFit( const Vec3D *points, const mxUInt numPoints ) {
	mxUInt i;
	FLOAT sumXX = 0.0f, sumXY = 0.0f, sumXZ = 0.0f;
	FLOAT sumYY = 0.0f, sumYZ = 0.0f;
	Vec3D sum, average, dir;

	if ( numPoints == 1 ) {
		a = 0.0f;
		b = 0.0f;
		c = 1.0f;
		d = -points[0].z;
		return true;
	}
	if ( numPoints == 2 ) {
		dir = points[1] - points[0];
		Normal() = dir.Cross( Vec3D( 0, 0, 1 ) ).Cross( dir );
		Normalize();
		d = -( Normal() * points[0] );
		return true;
	}

	sum.SetZero();
	for ( i = 0; i < numPoints; i++) {
		sum += points[i];
	}
	average = sum / numPoints;

	for ( i = 0; i < numPoints; i++ ) {
		dir = points[i] - average;
		sumXX += dir.x * dir.x;
		sumXY += dir.x * dir.y;
		sumXZ += dir.x * dir.z;
		sumYY += dir.y * dir.y;
		sumYZ += dir.y * dir.z;
	}

	Matrix2 m( sumXX, sumXY, sumXY, sumYY );
	if ( !m.InverseSelf() ) {
		return false;
	}

	a = - sumXZ * m[0][0] - sumYZ * m[0][1];
	b = - sumXZ * m[1][0] - sumYZ * m[1][1];
	c = 1.0f;
	Normalize();
	d = -( a * average.x + b * average.y + c * average.z );
	return true;
}

/*
================
mxPlane::PlaneIntersection
================
*/
bool mxPlane::PlaneIntersection( const mxPlane &plane, Vec3D &start, Vec3D &dir ) const {
	DOUBLE n00, n01, n11, det, invDet, f0, f1;

	n00 = Normal().LengthSqr();
	n01 = Normal() * plane.Normal();
	n11 = plane.Normal().LengthSqr();
	det = n00 * n11 - n01 * n01;

	if ( mxMath::Fabs(det) < 1e-6f ) {
		return false;
	}

	invDet = 1.0f / det;
	f0 = ( n01 * plane.d - n11 * d ) * invDet;
	f1 = ( n01 * d - n00 * plane.d ) * invDet;

	dir = Normal().Cross( plane.Normal() );
	start = f0 * Normal() + f1 * plane.Normal();
	return true;
}

/*
=============
mxPlane::ToString
=============
*/
const char *mxPlane::ToString( mxUInt precision ) const {
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
