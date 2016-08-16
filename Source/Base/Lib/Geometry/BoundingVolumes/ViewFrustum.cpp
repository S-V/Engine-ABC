/*
=============================================================================
	File:	ViewFrustum.cpp
	Desc:	A simple view frustum class.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <Base.h>

namespace abc {

/*================================
		mxViewFrustum
================================*/

mxViewFrustum::mxViewFrustum()
{}

//
//	NOTE: This test assumes frustum planes face inward.
//
FASTBOOL mxViewFrustum::PointInFrustum( const Vec3D& point ) const
{
	for( IndexT iPlane = 0; iPlane < NUM_PLANES_TO_USE; ++iPlane )
	{
		// if point is behind the plane
		if ( mPlanes[ iPlane ].Distance( point ) < 0 ) {
			return False;
		}
	}
	return True;
}

//
//	NOTE: This test assumes frustum planes face inward.
//
ESpatialRelation mxViewFrustum::Classify( const AABB& box ) const
{
	MX_BUG("untested:")
#if 0
	Vec3D  corners[ AABB::NUM_CORNERS ];
	box.GetCorners( corners );

	// If all corners of the box are outside the frustum, the box is likely be outside the frustum.
	for ( mxUInt iPoint = 0; iPoint < AABB::NUM_CORNERS; iPoint++ )
	{
		if ( PointInFrustum( corners[iPoint] ) ) {
			return ESpatialRelation::Intersects;
		}
	}

	// This check is needed in case all box corners are outside the frustum but the frustum still intersects the box.
	if ( IntersectSphere( box.GetCenter(), box.GetHalfSize().Length() ) ) {
		return ESpatialRelation::Intersects;
	}

	return ESpatialRelation::Outside;


#elif 1
	//	Test two extreme points of the AABB against frustum planes.
	//	It will occasionally give false positives ( at the expense of efficiency ).

	for( mxUInt iPlane = 0; iPlane < NUM_PLANES_TO_USE; ++iPlane )
	{
		const mxUInt signX = mPlaneNormalSigns[ iPlane ][ AXIS_X ];
		const mxUInt signY = mPlaneNormalSigns[ iPlane ][ AXIS_Y ];
		const mxUInt signZ = mPlaneNormalSigns[ iPlane ][ AXIS_Z ];

		const Vec3D  positiveVertex( box[ signX ].x,	box[ signY ].y,		box[ signZ ].z );
		const Vec3D  negativeVertex( box[ 1-signX ].x,	box[ 1-signY ].y,	box[ 1-signZ ].z );

		const mxPlane  currentPlane( mPlanes[ iPlane ] );

		if ( currentPlane.Distance( positiveVertex ) < 0 )
		{
			return ESpatialRelation::Outside;
		}
		else if ( currentPlane.Distance( negativeVertex ) < 0 )
		{
			return ESpatialRelation::Intersects;
		}
	}
	return ESpatialRelation::Inside;

#elif 1
	Vec3D  corners[ AABB::NUM_CORNERS ];
	box.GetCorners( corners );
    for( mxUInt iPlane = 0; iPlane < NUM_PLANES_TO_USE; ++iPlane )
	{
		for ( mxUInt iPoint = 0; iPoint < AABB::NUM_CORNERS; iPoint++ )
		{
			if ( mPlanes[iPlane].Distance( corners[iPoint] ) > 0 ) {
				continue;
			}
			return ESpatialRelation::Outside;
		}
    }
	return ESpatialRelation::Inside;

#elif 0

	for( mxUInt iPlane = 0; iPlane < NUM_PLANES_TO_USE; ++iPlane )
	{
		Vec3D P;
		Vec3D Q;

		// For each coordinate axis x, y, z...
		for(int j = 0; j < 3; ++j)
		{
			// Make PQ point in the same direction as
			// the plane normal on this axis.
			if( mPlanes[iPlane][j] >= 0.0f )
			{
				P[j] = box.GetMin()[j];
				Q[j] = box.GetMax()[j];
			}
			else
			{
				P[j] = box.GetMax()[j];
				Q[j] = box.GetMin()[j];
			}
		}

		if( mPlanes[iPlane].Distance( Q ) < 0 ) {
			return ESpatialRelation::Outside;
		}
	}
	return ESpatialRelation::Inside;

#else

	// This code is incorrect!!!

	// true if any frustum planes instersect the box,
	// i.e. minimima and maxima points lie on different plane sides.
	bool bIntersect = false;

	for ( mxUInt iPlane = 0; iPlane < NUM_PLANES_TO_USE; ++iPlane )
	{
		const mxUInt signX = mPlaneNormalSigns[ iPlane ][ AXIS_X ];
		const mxUInt signY = mPlaneNormalSigns[ iPlane ][ AXIS_Y ];
		const mxUInt signZ = mPlaneNormalSigns[ iPlane ][ AXIS_Z ];

		const Vec3D  maxExtreme	( box[ signX ].x,	box[ signY ].y,		box[ signZ ].z );
		const Vec3D  minExtreme	( box[ 1-signX ].x,	box[ 1-signY ].y,	box[ 1-signZ ].z );

		const mxPlane & currentPlane( mPlanes[ iPlane ] );

		if ( currentPlane.Distance( minExtreme ) > 0 )
		{
			return ESpatialRelation::Outside;
		}

		if ( currentPlane.Distance( maxExtreme ) >= 0 ) {
			bIntersect = true;
		}
	}

	if ( bIntersect ) {
		return ESpatialRelation::Intersects;
	}
	return ESpatialRelation::Inside;

#endif
}

//
//	This code was lifted and modified from Opcode library (written by Pierre Terdiman,
//	who commented that the original code was taken from Umbra/dPVS (by Ville Miettinen)).
//
FASTBOOL mxViewFrustum::IntersectsAABB( const AABB& aabb ) const
{
	MX_BUG("untested")
#if 1
	const Vec3D boxExtent( aabb.Size() * 0.5f );
	const Vec3D boxCenter( aabb.GetCenter() );

	const mxPlane * planes = this->mPlanes;
	UINT planeIndex = 0;

	while( planeIndex < NUM_PLANES_TO_USE )
	{
		const mxPlane & p = planes[planeIndex];

		MX_OPTIMIZE( "fabsf could be precomputed" )

		FLOAT NP = boxExtent.x * mxMath::Fabs( p.a )
					+ boxExtent.y * mxMath::Fabs( p.b )
						+ boxExtent.z * mxMath::Fabs( p.c );

		FLOAT MP = boxCenter.x * p.a
					+ boxCenter.y * p.b
						+ boxCenter.z * p.c
							+ p.d;

		if( NP < MP ) {
			// near vertex behind the clip plane so there is no intersection
			return False;
		}
		if( (-NP) < MP ) {
			// near and far vertices on different sides of plane
			return True;
		}

		planeIndex++;	// advance to next plane
	}

	return True;

#else
	// Idea for optimized AABB testing from www.lighthouse3d.com
	for( IndexT iPlane = 0; iPlane < NUM_PLANES_TO_USE; ++iPlane )
	{
		const Vec3D n = mPlanes[ iPlane ].Normal();
		
		Vec3D positive = aabb.GetMin();
		if( n.x <= 0 ) positive.x = aabb.GetMax().x;
		if( n.y <= 0 ) positive.y = aabb.GetMax().y;
		if( n.z <= 0 ) positive.z = aabb.GetMax().z;

		if( mPlanes[ iPlane ].Distance( positive ) < 0 ) return False;
//		else
//		if( mPlanes[ iPlane ].Distance( negative ) < 0 ) return True;
	}
	return True;
#endif
}

//
//	mxViewFrustum::IntersectSphere
//
FASTBOOL mxViewFrustum::IntersectSphere( const Sphere& theSphere ) const
{
	for( IndexT iPlane = 0; iPlane < NUM_PLANES_TO_USE; ++iPlane )
	{
		const mxPlane & rCurrPlane( mPlanes[ iPlane ] );
		if ( rCurrPlane.Distance( theSphere.GetOrigin() ) < -theSphere.GetRadius() ) {
			return False;
		}
	}
    return True;
}

//
//	mxViewFrustum::ExtractFrustumPlanes
//
void mxViewFrustum::ExtractFrustumPlanes( const Matrix4& mat )
{
	Vec4D  col0( mat[0][0], mat[1][0], mat[2][0], mat[3][0] );
	Vec4D  col1( mat[0][1], mat[1][1], mat[2][1], mat[3][1] );
	Vec4D  col2( mat[0][2], mat[1][2], mat[2][2], mat[3][2] );
	Vec4D  col3( mat[0][3], mat[1][3], mat[2][3], mat[3][3] );

	// Planes face inward.
	mPlanes[ LEFT_PLANE ]	.Set( col3 + col0 ); // left
	mPlanes[ RIGHT_PLANE ]	.Set( col3 - col0 ); // right

	mPlanes[ BOTTOM_PLANE ].Set( col3 + col1 ); // bottom
	mPlanes[ TOP_PLANE	]	.Set( col3 - col1 ); // top

	mPlanes[ NEAR_PLANE ]	.Set( col2 );        // near
	mPlanes[ FAR_PLANE ]	.Set( col3 - col2 ); // far


	for( IndexT iPlane = 0; iPlane < NUM_PLANES_TO_USE; iPlane++)
	{
		mPlanes[ iPlane ].d /= mPlanes[ iPlane ].Normal().Length();
		mPlanes[ iPlane ].Normalize(false);

		// Get the signs of the frustum planes' normals.
		for( IndexT iAxis = 0; iAxis < NUM_AXES; iAxis++ )
		{
#if 1
		mPlaneNormalSigns[ iPlane ][ iAxis ] =
			( mPlanes[ iPlane ].Normal()[ iAxis ] > 0.f ) ? 1 : 0;
#else
		MX_TODO("look at IceFPU.h, make a FLT_SIGN_BITMASK = 0x8000000 in Math.h" );
		MX_BUG( "this doesn't work correctly with the current view-frustum culling code" );

		// Extract the sign bit from the floating point value.
		UDWORD value = *(UDWORD*) & (mPlanes[ iPlane ][ iAxis ]);
		mPlaneNormalSigns[ iPlane ][ iAxis ] = (value & 0x80000000L);
#endif
		}
	}
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
