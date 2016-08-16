/*
=============================================================================
	File:	ViewFrustum.h
	Desc:	A simple view frustum class.
=============================================================================
*/

#ifndef __MX_VIEW_FRUSTUM_H__
#define __MX_VIEW_FRUSTUM_H__

namespace abc {

//
//	EFrustumPlane
//
//	Note: The order in which frustum planes are tested 
//	can slightly improve performance (?).
//	That usually depends on the application.
//	The version below, i think, is 'optimized'(?) for common cases.
//
enum EFrustumPlane
{
	LEFT_PLANE,
	RIGHT_PLANE,

	BOTTOM_PLANE,
	TOP_PLANE,

	NEAR_PLANE,
	FAR_PLANE,

	NUM_FRUSTUM_PLANES
};

//
//	mxViewFrustum
//
class mxViewFrustum {
public:
	mxViewFrustum();	// empty ctor leaves data uninitialized!

	FASTBOOL	PointInFrustum( const Vec3D& point ) const;

	ESpatialRelation	Classify( const AABB& aabb ) const;

	FASTBOOL	IntersectsAABB( const AABB& aabb ) const;

	FASTBOOL	IntersectSphere( const Sphere& theSphere ) const;

	//
	// Builds frustum planes from the given matrix.
	//
	//   If the given matrix is equal to the camera projection matrix
	// then the resulting planes will be given in view space ( i.e., camera space ).
	//
	//   If the matrix is equal to the combined view and projection matrices,
	// then the resulting planes will be given in world space.
	//
	//   If the matrix is equal to the combined world, view and projection matrices,
	// then the resulting planes will be given in object space.
	//
	// NOTE: Frustum planes' normals point towards the inside of the frustum.
	//
	void		ExtractFrustumPlanes( const Matrix4& mat );

private:
	// Against how many frustum planes test an object ?
	// ( How many frustum planes to consider for culling with ? )
	enum {
		NUM_PLANES_TO_USE = FAR_PLANE	// Don't test against the far clipping plane.
	};

	mxPlane   mPlanes[ NUM_PLANES_TO_USE ];	// Actual frustum planes (the normals point inwards).

	// For referring to the components of a vector by name rather than by an integer.
	enum EAxis
	{
		AXIS_X,
		AXIS_Y,
		AXIS_Z,

		NUM_AXES,
	};

	// Cached signs of frustum planes' normals ( e.g. signX = 0 if normal.x < 0, otherwise, signX = 1 ).
	// They are used for deciding which corners of an AABB should be used
	// as near and far vertices during culling.
	//
	UINT	mPlaneNormalSigns[ NUM_FRUSTUM_PLANES ][ NUM_AXES ];
};

}//End of namespace abc

#endif // ! __MX_VIEW_FRUSTUM_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
