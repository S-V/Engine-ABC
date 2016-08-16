/*
=============================================================================
	File:	Matrix_Common.h
	Desc:	Common stuff used by matrix classes.
=============================================================================
*/

#ifndef __MATH_MATRIX_COMMON_H__
#define __MATH_MATRIX_COMMON_H__

/*
=============================================================================

  Matrix classes, all matrices are row-major except Matrix3.

=============================================================================
*/

namespace abc {

const DOUBLE MATRIX_INVERSE_EPSILON	= 1e-14;
const DOUBLE MATRIX_EPSILON			= 1e-6;

class mxAngles;
class Quat;
class mxCQuat;
class mxRotation;

class mxPlane;

class Matrix4;

}//End of namespace abc

#endif /* !__MATH_MATRIX_COMMON_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
