/*
=============================================================================
	File:	Utils.h
	Desc:	Math helpers.
=============================================================================
*/

#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

namespace abc {

//-------------------------------------------------------------------
//	Typedefs
//-------------------------------------------------------------------

typedef Vec2D		float2;
typedef Vec3D		float3;
typedef Vec4D		float4;

typedef Matrix2		float2x2;
typedef Matrix3		float3x3;
typedef Matrix4		float4x4;

//-------------------------------------------------------------------
//	Functions
//-------------------------------------------------------------------

/*
 * A function for creating a rotation matrix that rotates a vector called
 * "from" into another vector called "to".
 * Input : from[3], to[3] which both must be *normalized* non-zero vectors
 * Output: mtx[3][3] -- a 3x3 matrix in colum-major form
 * Authors: Tomas Mueller, John Hughes
 *          "Efficiently Building a Matrix to Rotate One Vector to Another"
 *          Journal of Graphics Tools, 4(4):1-4, 1999
 */
void FromToRotation( const FLOAT from[3], const FLOAT to[3], FLOAT mtx[3][3] );

}//End of namespace abc

#endif /* !__MATH_UTILS_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
