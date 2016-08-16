/*
=============================================================================
	File:	BoundingVolumes.h
	Desc:	Under construction.
=============================================================================
*/

#ifndef __BOUNDING_VOLUMES_H__
#define __BOUNDING_VOLUMES_H__

namespace abc {

//
//	ESpatialRelation
//
enum ESpatialRelation
{
	Outside		= 0,	// <= Must be equal to zero.
	Intersects,
	Inside,
};

//
//	IBoundingVolume
//
class IBoundingVolume {
public:
	virtual ~IBoundingVolume() {}

					// Test for containment of a point.
    virtual bool	Contains ( const Vec3D& point ) const = 0;

protected:
	IBoundingVolume() {}
};

}//End of namespace abc

#endif // ! __BOUNDING_VOLUMES_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
