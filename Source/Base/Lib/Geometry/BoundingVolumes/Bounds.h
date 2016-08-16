/*
=============================================================================
	File:	Bounds.h
	Desc:	Default bounding volume type.
=============================================================================
*/

#ifndef __BOUNDING_VOLUMES_BOUNDS_H__
#define __BOUNDING_VOLUMES_BOUNDS_H__

namespace abc {

#if 1

// Default bounding volume type.
typedef AABB	mxBounds;

#elif 0

//
//	mxBounds - AABB and sphere with the same origin.
//	
class mxBounds {
public:
	Vec3D	origin;
	FLOAT	radius;		// sphere radius
	Vec3D	extent;		// aabb extent

public:
					mxBounds( ENoInit );
					explicit mxBounds( const Vec3D &mins, const Vec3D &maxs );
					explicit mxBounds( const Vec3D &point, FLOAT radius );
};

#endif

}//End of namespace abc

#endif // ! __BOUNDING_VOLUMES_BOUNDS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
