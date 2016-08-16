/*
=============================================================================
	File:	Matrix4x4.cpp
	Desc:	Row-major float4x4 matrix.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
//#include <Base.h>

namespace abc {

//===============================================================
//
//	Matrix4
//
//===============================================================

const Matrix4	Matrix4::mat4_zero(
				   Vec4D( 0, 0, 0, 0 ),
				   Vec4D( 0, 0, 0, 0 ),
				   Vec4D( 0, 0, 0, 0 ),
				   Vec4D( 0, 0, 0, 0 ) );

const Matrix4	Matrix4::mat4_identity(
				   Vec4D( 1, 0, 0, 0 ),
				   Vec4D( 0, 1, 0, 0 ),
				   Vec4D( 0, 0, 1, 0 ),
				   Vec4D( 0, 0, 0, 1 ) );

/*
============
Matrix4::Transpose
============
*/
Matrix4 Matrix4::Transpose( void ) const {
	Matrix4	transpose;
	INT		i, j;
	MX_OPTIMIZE("unroll the loops and help RVO")
	for( i = 0; i < 4; i++ ) {
		for( j = 0; j < 4; j++ ) {
			transpose[ i ][ j ] = mRows[ j ][ i ];
        }
	}
	return transpose;
}

/*
============
Matrix4::TransposeSelf
============
*/
Matrix4 &Matrix4::TransposeSelf( void ) {
	FLOAT	temp;
	INT		i, j;
	MX_OPTIMIZE("unroll the loops and help RVO")
	for( i = 0; i < 4; i++ ) {
		for( j = i + 1; j < 4; j++ ) {
			temp = mRows[ i ][ j ];
			mRows[ i ][ j ] = mRows[ j ][ i ];
			mRows[ j ][ i ] = temp;
        }
	}
	return *this;
}

/*
============
Matrix4::Determinant
============
*/
FLOAT Matrix4::Determinant( void ) const {

	// 2x2 sub-determinants
	FLOAT det2_01_01 = mRows[0][0] * mRows[1][1] - mRows[0][1] * mRows[1][0];
	FLOAT det2_01_02 = mRows[0][0] * mRows[1][2] - mRows[0][2] * mRows[1][0];
	FLOAT det2_01_03 = mRows[0][0] * mRows[1][3] - mRows[0][3] * mRows[1][0];
	FLOAT det2_01_12 = mRows[0][1] * mRows[1][2] - mRows[0][2] * mRows[1][1];
	FLOAT det2_01_13 = mRows[0][1] * mRows[1][3] - mRows[0][3] * mRows[1][1];
	FLOAT det2_01_23 = mRows[0][2] * mRows[1][3] - mRows[0][3] * mRows[1][2];

	// 3x3 sub-determinants
	FLOAT det3_201_012 = mRows[2][0] * det2_01_12 - mRows[2][1] * det2_01_02 + mRows[2][2] * det2_01_01;
	FLOAT det3_201_013 = mRows[2][0] * det2_01_13 - mRows[2][1] * det2_01_03 + mRows[2][3] * det2_01_01;
	FLOAT det3_201_023 = mRows[2][0] * det2_01_23 - mRows[2][2] * det2_01_03 + mRows[2][3] * det2_01_02;
	FLOAT det3_201_123 = mRows[2][1] * det2_01_23 - mRows[2][2] * det2_01_13 + mRows[2][3] * det2_01_12;

	return ( - det3_201_123 * mRows[3][0] + det3_201_023 * mRows[3][1] - det3_201_013 * mRows[3][2] + det3_201_012 * mRows[3][3] );
}

/*
============
Matrix4::InverseSelf
============
*/
bool Matrix4::InverseSelf( void ) {
	// 84+4+16 = 104 multiplications
	//			   1 division
	DOUBLE det, invDet;

	// 2x2 sub-determinants required to calculate 4x4 determinant
	FLOAT det2_01_01 = mRows[0][0] * mRows[1][1] - mRows[0][1] * mRows[1][0];
	FLOAT det2_01_02 = mRows[0][0] * mRows[1][2] - mRows[0][2] * mRows[1][0];
	FLOAT det2_01_03 = mRows[0][0] * mRows[1][3] - mRows[0][3] * mRows[1][0];
	FLOAT det2_01_12 = mRows[0][1] * mRows[1][2] - mRows[0][2] * mRows[1][1];
	FLOAT det2_01_13 = mRows[0][1] * mRows[1][3] - mRows[0][3] * mRows[1][1];
	FLOAT det2_01_23 = mRows[0][2] * mRows[1][3] - mRows[0][3] * mRows[1][2];

	// 3x3 sub-determinants required to calculate 4x4 determinant
	FLOAT det3_201_012 = mRows[2][0] * det2_01_12 - mRows[2][1] * det2_01_02 + mRows[2][2] * det2_01_01;
	FLOAT det3_201_013 = mRows[2][0] * det2_01_13 - mRows[2][1] * det2_01_03 + mRows[2][3] * det2_01_01;
	FLOAT det3_201_023 = mRows[2][0] * det2_01_23 - mRows[2][2] * det2_01_03 + mRows[2][3] * det2_01_02;
	FLOAT det3_201_123 = mRows[2][1] * det2_01_23 - mRows[2][2] * det2_01_13 + mRows[2][3] * det2_01_12;

	det = ( - det3_201_123 * mRows[3][0] + det3_201_023 * mRows[3][1] - det3_201_013 * mRows[3][2] + det3_201_012 * mRows[3][3] );

	if ( mxMath::Fabs( det ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	// remaining 2x2 sub-determinants
	FLOAT det2_03_01 = mRows[0][0] * mRows[3][1] - mRows[0][1] * mRows[3][0];
	FLOAT det2_03_02 = mRows[0][0] * mRows[3][2] - mRows[0][2] * mRows[3][0];
	FLOAT det2_03_03 = mRows[0][0] * mRows[3][3] - mRows[0][3] * mRows[3][0];
	FLOAT det2_03_12 = mRows[0][1] * mRows[3][2] - mRows[0][2] * mRows[3][1];
	FLOAT det2_03_13 = mRows[0][1] * mRows[3][3] - mRows[0][3] * mRows[3][1];
	FLOAT det2_03_23 = mRows[0][2] * mRows[3][3] - mRows[0][3] * mRows[3][2];

	FLOAT det2_13_01 = mRows[1][0] * mRows[3][1] - mRows[1][1] * mRows[3][0];
	FLOAT det2_13_02 = mRows[1][0] * mRows[3][2] - mRows[1][2] * mRows[3][0];
	FLOAT det2_13_03 = mRows[1][0] * mRows[3][3] - mRows[1][3] * mRows[3][0];
	FLOAT det2_13_12 = mRows[1][1] * mRows[3][2] - mRows[1][2] * mRows[3][1];
	FLOAT det2_13_13 = mRows[1][1] * mRows[3][3] - mRows[1][3] * mRows[3][1];
	FLOAT det2_13_23 = mRows[1][2] * mRows[3][3] - mRows[1][3] * mRows[3][2];

	// remaining 3x3 sub-determinants
	FLOAT det3_203_012 = mRows[2][0] * det2_03_12 - mRows[2][1] * det2_03_02 + mRows[2][2] * det2_03_01;
	FLOAT det3_203_013 = mRows[2][0] * det2_03_13 - mRows[2][1] * det2_03_03 + mRows[2][3] * det2_03_01;
	FLOAT det3_203_023 = mRows[2][0] * det2_03_23 - mRows[2][2] * det2_03_03 + mRows[2][3] * det2_03_02;
	FLOAT det3_203_123 = mRows[2][1] * det2_03_23 - mRows[2][2] * det2_03_13 + mRows[2][3] * det2_03_12;

	FLOAT det3_213_012 = mRows[2][0] * det2_13_12 - mRows[2][1] * det2_13_02 + mRows[2][2] * det2_13_01;
	FLOAT det3_213_013 = mRows[2][0] * det2_13_13 - mRows[2][1] * det2_13_03 + mRows[2][3] * det2_13_01;
	FLOAT det3_213_023 = mRows[2][0] * det2_13_23 - mRows[2][2] * det2_13_03 + mRows[2][3] * det2_13_02;
	FLOAT det3_213_123 = mRows[2][1] * det2_13_23 - mRows[2][2] * det2_13_13 + mRows[2][3] * det2_13_12;

	FLOAT det3_301_012 = mRows[3][0] * det2_01_12 - mRows[3][1] * det2_01_02 + mRows[3][2] * det2_01_01;
	FLOAT det3_301_013 = mRows[3][0] * det2_01_13 - mRows[3][1] * det2_01_03 + mRows[3][3] * det2_01_01;
	FLOAT det3_301_023 = mRows[3][0] * det2_01_23 - mRows[3][2] * det2_01_03 + mRows[3][3] * det2_01_02;
	FLOAT det3_301_123 = mRows[3][1] * det2_01_23 - mRows[3][2] * det2_01_13 + mRows[3][3] * det2_01_12;

	mRows[0][0] =	- det3_213_123 * invDet;
	mRows[1][0] = + det3_213_023 * invDet;
	mRows[2][0] = - det3_213_013 * invDet;
	mRows[3][0] = + det3_213_012 * invDet;

	mRows[0][1] = + det3_203_123 * invDet;
	mRows[1][1] = - det3_203_023 * invDet;
	mRows[2][1] = + det3_203_013 * invDet;
	mRows[3][1] = - det3_203_012 * invDet;

	mRows[0][2] = + det3_301_123 * invDet;
	mRows[1][2] = - det3_301_023 * invDet;
	mRows[2][2] = + det3_301_013 * invDet;
	mRows[3][2] = - det3_301_012 * invDet;

	mRows[0][3] = - det3_201_123 * invDet;
	mRows[1][3] = + det3_201_023 * invDet;
	mRows[2][3] = - det3_201_013 * invDet;
	mRows[3][3] = + det3_201_012 * invDet;

	return true;
}

/*
============
Matrix4::InverseFastSelf
============
*/
bool Matrix4::InverseFastSelf( void ) {
#if 0
	// 84+4+16 = 104 multiplications
	//			   1 division
	DOUBLE det, invDet;

	// 2x2 sub-determinants required to calculate 4x4 determinant
	FLOAT det2_01_01 = mRows[0][0] * mRows[1][1] - mRows[0][1] * mRows[1][0];
	FLOAT det2_01_02 = mRows[0][0] * mRows[1][2] - mRows[0][2] * mRows[1][0];
	FLOAT det2_01_03 = mRows[0][0] * mRows[1][3] - mRows[0][3] * mRows[1][0];
	FLOAT det2_01_12 = mRows[0][1] * mRows[1][2] - mRows[0][2] * mRows[1][1];
	FLOAT det2_01_13 = mRows[0][1] * mRows[1][3] - mRows[0][3] * mRows[1][1];
	FLOAT det2_01_23 = mRows[0][2] * mRows[1][3] - mRows[0][3] * mRows[1][2];

	// 3x3 sub-determinants required to calculate 4x4 determinant
	FLOAT det3_201_012 = mRows[2][0] * det2_01_12 - mRows[2][1] * det2_01_02 + mRows[2][2] * det2_01_01;
	FLOAT det3_201_013 = mRows[2][0] * det2_01_13 - mRows[2][1] * det2_01_03 + mRows[2][3] * det2_01_01;
	FLOAT det3_201_023 = mRows[2][0] * det2_01_23 - mRows[2][2] * det2_01_03 + mRows[2][3] * det2_01_02;
	FLOAT det3_201_123 = mRows[2][1] * det2_01_23 - mRows[2][2] * det2_01_13 + mRows[2][3] * det2_01_12;

	det = ( - det3_201_123 * mRows[3][0] + det3_201_023 * mRows[3][1] - det3_201_013 * mRows[3][2] + det3_201_012 * mRows[3][3] );

	if ( mxMath::Fabs( det ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	// remaining 2x2 sub-determinants
	FLOAT det2_03_01 = mRows[0][0] * mRows[3][1] - mRows[0][1] * mRows[3][0];
	FLOAT det2_03_02 = mRows[0][0] * mRows[3][2] - mRows[0][2] * mRows[3][0];
	FLOAT det2_03_03 = mRows[0][0] * mRows[3][3] - mRows[0][3] * mRows[3][0];
	FLOAT det2_03_12 = mRows[0][1] * mRows[3][2] - mRows[0][2] * mRows[3][1];
	FLOAT det2_03_13 = mRows[0][1] * mRows[3][3] - mRows[0][3] * mRows[3][1];
	FLOAT det2_03_23 = mRows[0][2] * mRows[3][3] - mRows[0][3] * mRows[3][2];

	FLOAT det2_13_01 = mRows[1][0] * mRows[3][1] - mRows[1][1] * mRows[3][0];
	FLOAT det2_13_02 = mRows[1][0] * mRows[3][2] - mRows[1][2] * mRows[3][0];
	FLOAT det2_13_03 = mRows[1][0] * mRows[3][3] - mRows[1][3] * mRows[3][0];
	FLOAT det2_13_12 = mRows[1][1] * mRows[3][2] - mRows[1][2] * mRows[3][1];
	FLOAT det2_13_13 = mRows[1][1] * mRows[3][3] - mRows[1][3] * mRows[3][1];
	FLOAT det2_13_23 = mRows[1][2] * mRows[3][3] - mRows[1][3] * mRows[3][2];

	// remaining 3x3 sub-determinants
	FLOAT det3_203_012 = mRows[2][0] * det2_03_12 - mRows[2][1] * det2_03_02 + mRows[2][2] * det2_03_01;
	FLOAT det3_203_013 = mRows[2][0] * det2_03_13 - mRows[2][1] * det2_03_03 + mRows[2][3] * det2_03_01;
	FLOAT det3_203_023 = mRows[2][0] * det2_03_23 - mRows[2][2] * det2_03_03 + mRows[2][3] * det2_03_02;
	FLOAT det3_203_123 = mRows[2][1] * det2_03_23 - mRows[2][2] * det2_03_13 + mRows[2][3] * det2_03_12;

	FLOAT det3_213_012 = mRows[2][0] * det2_13_12 - mRows[2][1] * det2_13_02 + mRows[2][2] * det2_13_01;
	FLOAT det3_213_013 = mRows[2][0] * det2_13_13 - mRows[2][1] * det2_13_03 + mRows[2][3] * det2_13_01;
	FLOAT det3_213_023 = mRows[2][0] * det2_13_23 - mRows[2][2] * det2_13_03 + mRows[2][3] * det2_13_02;
	FLOAT det3_213_123 = mRows[2][1] * det2_13_23 - mRows[2][2] * det2_13_13 + mRows[2][3] * det2_13_12;

	FLOAT det3_301_012 = mRows[3][0] * det2_01_12 - mRows[3][1] * det2_01_02 + mRows[3][2] * det2_01_01;
	FLOAT det3_301_013 = mRows[3][0] * det2_01_13 - mRows[3][1] * det2_01_03 + mRows[3][3] * det2_01_01;
	FLOAT det3_301_023 = mRows[3][0] * det2_01_23 - mRows[3][2] * det2_01_03 + mRows[3][3] * det2_01_02;
	FLOAT det3_301_123 = mRows[3][1] * det2_01_23 - mRows[3][2] * det2_01_13 + mRows[3][3] * det2_01_12;

	mRows[0][0] =	- det3_213_123 * invDet;
	mRows[1][0] = + det3_213_023 * invDet;
	mRows[2][0] = - det3_213_013 * invDet;
	mRows[3][0] = + det3_213_012 * invDet;

	mRows[0][1] = + det3_203_123 * invDet;
	mRows[1][1] = - det3_203_023 * invDet;
	mRows[2][1] = + det3_203_013 * invDet;
	mRows[3][1] = - det3_203_012 * invDet;

	mRows[0][2] = + det3_301_123 * invDet;
	mRows[1][2] = - det3_301_023 * invDet;
	mRows[2][2] = + det3_301_013 * invDet;
	mRows[3][2] = - det3_301_012 * invDet;

	mRows[0][3] = - det3_201_123 * invDet;
	mRows[1][3] = + det3_201_023 * invDet;
	mRows[2][3] = - det3_201_013 * invDet;
	mRows[3][3] = + det3_201_012 * invDet;

	return true;
#elif 0
	// 4*18 = 72 multiplications
	//		   4 divisions
	FLOAT *mRows = reinterpret_cast<FLOAT *>(this);
	FLOAT s;
	DOUBLE d, di;

	di = mRows[0];
	s = di;
	mRows[0] = d = 1.0f / di;
	mRows[1] *= d;
	mRows[2] *= d;
	mRows[3] *= d;
	d = -d;
	mRows[4] *= d;
	mRows[8] *= d;
	mRows[12] *= d;
	d = mRows[4] * di;
	mRows[5] += mRows[1] * d;
	mRows[6] += mRows[2] * d;
	mRows[7] += mRows[3] * d;
	d = mRows[8] * di;
	mRows[9] += mRows[1] * d;
	mRows[10] += mRows[2] * d;
	mRows[11] += mRows[3] * d;
	d = mRows[12] * di;
	mRows[13] += mRows[1] * d;
	mRows[14] += mRows[2] * d;
	mRows[15] += mRows[3] * d;
	di = mRows[5];
	s *= di;
	mRows[5] = d = 1.0f / di;
	mRows[4] *= d;
	mRows[6] *= d;
	mRows[7] *= d;
	d = -d;
	mRows[1] *= d;
	mRows[9] *= d;
	mRows[13] *= d;
	d = mRows[1] * di;
	mRows[0] += mRows[4] * d;
	mRows[2] += mRows[6] * d;
	mRows[3] += mRows[7] * d;
	d = mRows[9] * di;
	mRows[8] += mRows[4] * d;
	mRows[10] += mRows[6] * d;
	mRows[11] += mRows[7] * d;
	d = mRows[13] * di;
	mRows[12] += mRows[4] * d;
	mRows[14] += mRows[6] * d;
	mRows[15] += mRows[7] * d;
	di = mRows[10];
	s *= di;
	mRows[10] = d = 1.0f / di;
	mRows[8] *= d;
	mRows[9] *= d;
	mRows[11] *= d;
	d = -d;
	mRows[2] *= d;
	mRows[6] *= d;
	mRows[14] *= d;
	d = mRows[2] * di;
	mRows[0] += mRows[8] * d;
	mRows[1] += mRows[9] * d;
	mRows[3] += mRows[11] * d;
	d = mRows[6] * di;
	mRows[4] += mRows[8] * d;
	mRows[5] += mRows[9] * d;
	mRows[7] += mRows[11] * d;
	d = mRows[14] * di;
	mRows[12] += mRows[8] * d;
	mRows[13] += mRows[9] * d;
	mRows[15] += mRows[11] * d;
	di = mRows[15];
	s *= di;
	mRows[15] = d = 1.0f / di;
	mRows[12] *= d;
	mRows[13] *= d;
	mRows[14] *= d;
	d = -d;
	mRows[3] *= d;
	mRows[7] *= d;
	mRows[11] *= d;
	d = mRows[3] * di;
	mRows[0] += mRows[12] * d;
	mRows[1] += mRows[13] * d;
	mRows[2] += mRows[14] * d;
	d = mRows[7] * di;
	mRows[4] += mRows[12] * d;
	mRows[5] += mRows[13] * d;
	mRows[6] += mRows[14] * d;
	d = mRows[11] * di;
	mRows[8] += mRows[12] * d;
	mRows[9] += mRows[13] * d;
	mRows[10] += mRows[14] * d;

	return ( s != 0.0f && !FLOAT_IS_NAN( s ) );
#else
	//	6*8+2*6 = 60 multiplications
	//		2*1 =  2 divisions
	Matrix2 r0, r1, r2, r3;
	FLOAT a, det, invDet;
	FLOAT *mRows = reinterpret_cast<FLOAT *>(this);

	// r0 = m0.Inverse();
	det = mRows[0*4+0] * mRows[1*4+1] - mRows[0*4+1] * mRows[1*4+0];

	if ( mxMath::Fabs( det ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	r0[0][0] =   mRows[1*4+1] * invDet;
	r0[0][1] = - mRows[0*4+1] * invDet;
	r0[1][0] = - mRows[1*4+0] * invDet;
	r0[1][1] =   mRows[0*4+0] * invDet;

	// r1 = r0 * m1;
	r1[0][0] = r0[0][0] * mRows[0*4+2] + r0[0][1] * mRows[1*4+2];
	r1[0][1] = r0[0][0] * mRows[0*4+3] + r0[0][1] * mRows[1*4+3];
	r1[1][0] = r0[1][0] * mRows[0*4+2] + r0[1][1] * mRows[1*4+2];
	r1[1][1] = r0[1][0] * mRows[0*4+3] + r0[1][1] * mRows[1*4+3];

	// r2 = m2 * r1;
	r2[0][0] = mRows[2*4+0] * r1[0][0] + mRows[2*4+1] * r1[1][0];
	r2[0][1] = mRows[2*4+0] * r1[0][1] + mRows[2*4+1] * r1[1][1];
	r2[1][0] = mRows[3*4+0] * r1[0][0] + mRows[3*4+1] * r1[1][0];
	r2[1][1] = mRows[3*4+0] * r1[0][1] + mRows[3*4+1] * r1[1][1];

	// r3 = r2 - m3;
	r3[0][0] = r2[0][0] - mRows[2*4+2];
	r3[0][1] = r2[0][1] - mRows[2*4+3];
	r3[1][0] = r2[1][0] - mRows[3*4+2];
	r3[1][1] = r2[1][1] - mRows[3*4+3];

	// r3.InverseSelf();
	det = r3[0][0] * r3[1][1] - r3[0][1] * r3[1][0];

	if ( mxMath::Fabs( det ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	a = r3[0][0];
	r3[0][0] =   r3[1][1] * invDet;
	r3[0][1] = - r3[0][1] * invDet;
	r3[1][0] = - r3[1][0] * invDet;
	r3[1][1] =   a * invDet;

	// r2 = m2 * r0;
	r2[0][0] = mRows[2*4+0] * r0[0][0] + mRows[2*4+1] * r0[1][0];
	r2[0][1] = mRows[2*4+0] * r0[0][1] + mRows[2*4+1] * r0[1][1];
	r2[1][0] = mRows[3*4+0] * r0[0][0] + mRows[3*4+1] * r0[1][0];
	r2[1][1] = mRows[3*4+0] * r0[0][1] + mRows[3*4+1] * r0[1][1];

	// m2 = r3 * r2;
	mRows[2*4+0] = r3[0][0] * r2[0][0] + r3[0][1] * r2[1][0];
	mRows[2*4+1] = r3[0][0] * r2[0][1] + r3[0][1] * r2[1][1];
	mRows[3*4+0] = r3[1][0] * r2[0][0] + r3[1][1] * r2[1][0];
	mRows[3*4+1] = r3[1][0] * r2[0][1] + r3[1][1] * r2[1][1];

	// m0 = r0 - r1 * m2;
	mRows[0*4+0] = r0[0][0] - r1[0][0] * mRows[2*4+0] - r1[0][1] * mRows[3*4+0];
	mRows[0*4+1] = r0[0][1] - r1[0][0] * mRows[2*4+1] - r1[0][1] * mRows[3*4+1];
	mRows[1*4+0] = r0[1][0] - r1[1][0] * mRows[2*4+0] - r1[1][1] * mRows[3*4+0];
	mRows[1*4+1] = r0[1][1] - r1[1][0] * mRows[2*4+1] - r1[1][1] * mRows[3*4+1];

	// m1 = r1 * r3;
	mRows[0*4+2] = r1[0][0] * r3[0][0] + r1[0][1] * r3[1][0];
	mRows[0*4+3] = r1[0][0] * r3[0][1] + r1[0][1] * r3[1][1];
	mRows[1*4+2] = r1[1][0] * r3[0][0] + r1[1][1] * r3[1][0];
	mRows[1*4+3] = r1[1][0] * r3[0][1] + r1[1][1] * r3[1][1];

	// m3 = -r3;
	mRows[2*4+2] = -r3[0][0];
	mRows[2*4+3] = -r3[0][1];
	mRows[3*4+2] = -r3[1][0];
	mRows[3*4+3] = -r3[1][1];

	return true;
#endif
}

//
//	Matrix4::SetRotationInDegrees
//
void Matrix4::SetRotationInDegrees( const Vec3D& rotation ) {
	SetRotationInRadians( rotation * mxMath::M_DEG2RAD );
}

//
//	Matrix4::SetRotationInRadians
//
void Matrix4::SetRotationInRadians( const Vec3D& rotation ) {
	const FLOAT cr = mxMath::Cos( rotation.x );
	const FLOAT sr = mxMath::Sin( rotation.x );
	const FLOAT cp = mxMath::Cos( rotation.y );
	const FLOAT sp = mxMath::Sin( rotation.y );
	const FLOAT cy = mxMath::Cos( rotation.z );
	const FLOAT sy = mxMath::Sin( rotation.z);

	FLOAT * M = (FLOAT*) mRows;

	M[ 0 ] = cp * cy;
	M[ 1 ] = cp * sy;
	M[ 2 ] = -sp;

	const FLOAT srsp = sr * sp;
	const FLOAT crsp = cr * sp;

	M[ 4 ] = srsp * cy - cr * sy;
	M[ 5 ] = srsp * sy + cr * cy;
	M[ 6 ] = sr * cp;

	M[ 8 ] = crsp * cy + sr * sy;
	M[ 9 ] = crsp * sy - sr * cy;
	M[ 10 ] = cr * cp;
}

//
//	Matrix4::GetRotationInDegrees
//
Vec3D Matrix4::GetRotationInDegrees() const
{
	const Matrix4 & mat( *this );

	FLOAT Y = -mxMath::ASin( mat[0][2] );
	const FLOAT C = mxMath::Cos( Y );
	Y *= mxMath::M_RAD2DEG;

	FLOAT rotx, roty, X, Z;

	if ( mxMath::Fabs( C ) > mxMath::FLT_EPSILON )
	{
		const FLOAT invC = 1.0 / C;
		rotx = mat[2][2] * invC;
		roty = mat[1][2] * invC;
		X = mxMath::ATan( roty, rotx ) * mxMath::M_RAD2DEG;
		rotx = mat[0][0] * invC;
		roty = mat[0][1] * invC;
		Z = mxMath::ATan( roty, rotx ) * mxMath::M_RAD2DEG;
	}
	else
	{
		X = 0.0;
		rotx = mat[1][1];
		roty = -mat[1][0];
		Z = mxMath::ATan( roty, rotx ) * mxMath::M_RAD2DEG;
	}

	// fix values that get below zero
	// before it would set (!) values to 360
	// that where above 360:
	if (X < 0.0) X += 360.0;
	if (Y < 0.0) Y += 360.0;
	if (Z < 0.0) Z += 360.0;

	return Vec3D( X, Y, Z );
}

//
//	Matrix4::TransformPlane
//
//void Matrix4::TransformPlane( mxPlane & plane ) const
//{
//	plane = ((Vec4D&)plane) * this->Inverse().Transpose();
//}

//
//	Matrix4::SetRotationX
//
void Matrix4::SetRotationX( FLOAT angle )
{
	FLOAT fSin, fCos;
	mxMath::SinCos( angle, fSin, fCos );

	mRows[0][0] = 1.0f;		mRows[0][1] = 0.0f;		mRows[0][2] = 0.0f;		mRows[0][3] = 0.0f;
	mRows[1][0] = 0.0f;		mRows[1][1] = fCos;		mRows[1][2] = fSin;		mRows[1][3] = 0.0f;
	mRows[2][0] = 0.0f;		mRows[2][1] = -fSin;		mRows[2][2] = fCos;		mRows[2][3] = 0.0f;
	mRows[3][0] = 0.0f;		mRows[3][1] = 0.0f;		mRows[3][2] = 0.0f;		mRows[3][3] = 1.0f;
}

//
//	Matrix4::SetRotationY
//
void Matrix4::SetRotationY( FLOAT angle )
{
	FLOAT fSin, fCos;
	mxMath::SinCos( angle, fSin, fCos );

	mRows[0][0] = fCos;		mRows[0][1] = 0.0f;		mRows[0][2] = -fSin;	mRows[0][3] = 0.0f;
	mRows[1][0] = 0.0f;		mRows[1][1] = 1.0f;		mRows[1][2] = 0.0f;		mRows[1][3] = 0.0f;
	mRows[2][0] = fSin;		mRows[2][1] = 0.0f;		mRows[2][2] = fCos;		mRows[2][3] = 0.0f;
	mRows[3][0] = 0.0f;		mRows[3][1] = 0.0f;		mRows[3][2] = 0.0f;		mRows[3][3] = 1.0f;
}

//
//	Matrix4::SetRotationZ
//
void Matrix4::SetRotationZ( FLOAT angle )
{
	FLOAT fSin, fCos;
	mxMath::SinCos( angle, fSin, fCos );

	mRows[0][0] = fCos;		mRows[0][1] = fSin;		mRows[0][2] = 0.0f;		mRows[0][3] = 0.0f;
	mRows[1][0] = -fSin;	mRows[1][1] = fCos;		mRows[1][2] = 0.0f;		mRows[1][3] = 0.0f;
	mRows[2][0] = 0.0f;		mRows[2][1] = 0.0f;		mRows[2][2] = 1.0f;		mRows[2][3] = 0.0f;
	mRows[3][0] = 0.0f;		mRows[3][1] = 0.0f;		mRows[3][2] = 0.0f;		mRows[3][3] = 1.0f;
}

//
//	Matrix4::SetRotationAxis
//
void Matrix4::SetRotationAxis( FLOAT angle, const Vec3D& axis )
{
#ifdef USE_D3D_MATH_FUNC
	::D3DXMatrixRotationAxis( (D3DXMATRIX* ) mRows, (const D3DXVECTOR3* ) &axis,	angle );
#else
	Assert( axis.IsNormalized() );

	FLOAT s, c;
	mxMath::SinCos( angle, s, c );

	const FLOAT xy = axis.x * axis.y;
	const FLOAT yz = axis.y * axis.z;
	const FLOAT zx = axis.z * axis.x;
	const FLOAT xs = axis.x * s;
	const FLOAT ys = axis.y * s;
	const FLOAT zs = axis.z * s;

	FLOAT * M = (FLOAT*) mRows;

	M[0] = (1.0f - c) * axis.x * axis.x + c;
	M[1] = (1.0f - c) * xy + zs;
	M[2] = (1.0f - c) * zx - ys;
	M[3] = 0.0;

	M[4] = (1.0f - c) * xy - zs;
	M[5] = (1.0f - c) * axis.y * axis.y + c;
	M[6] = (1.0f - c) * yz + xs;
	M[7] = 0.0;

	M[8] = (1.0f - c) * zx + ys;
	M[9] = (1.0f - c) * yz - xs;
	M[10] = (1.0f - c) * axis.z * axis.z + c;
	M[11] = 0.0;

	M[12] = 0.0;
	M[13] = 0.0;
	M[14] = 0.0;
    M[15] = 1.0;
#endif
}

//
//	Matrix4::SetRotationAxis
//
void Matrix4::SetRotationAxis( FLOAT angle, const Vec3D& vFrom, const Vec3D& vTo )
{
	/*
		Note: swiped from Irrlicht ( file "matrix4.h" ).

		http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToMatrix/index.htm
	*/

	// unit vectors
	Vec3D f ( vFrom );
	Vec3D t ( vTo );
	f.Normalize();
	t.Normalize();

	// axis multiplication by sin
	const Vec3D vs ( t.Cross( f ) );

	// axis of rotation
	Vec3D v ( vs );
	v.Normalize();

	// cosinus angle
	const FLOAT ca = f.Dot( t );

	Vec3D vt ( v * (1.0f - ca) );

	FLOAT * M = (FLOAT*) mRows;

	M[0] = vt.x * v.x + ca;
	M[5] = vt.y * v.y + ca;
	M[10] = vt.z * v.z + ca;

	vt.x *= v.y;
	vt.z *= v.x;
	vt.y *= v.z;

	M[1] = vt.x - vs.z;
	M[2] = vt.z + vs.y;
	M[3] = 0.0f;

	M[4] = vt.x + vs.z;
	M[6] = vt.y - vs.x;
	M[7] = 0.0f;

	M[8] = vt.z - vs.y;
	M[9] = vt.y + vs.x;
	M[11] = 0.0f;

	M[12] = 0.0f;
	M[13] = 0.0f;
	M[14] = 0.0f;
	M[15] = 1.0f;
}

//
//	Matrix4::SetRotationAxis
//
Matrix4 Matrix4::GetAbsolute() const
{	
	MX_OPTIMIZE("unroll the loops and help RVO")
	Matrix4   mat;
	for ( mxUInt row = 0; row < 4; row++ ) {
		for ( mxUInt col = 0; col < 4; col++ ) {
			mat[row][col] = mxMath::Fabs( mRows[row][col] );
		}
	}
	return mat;
}

//
//	Matrix4::BuildTransform
//
void Matrix4::BuildTransform( const Vec3D& vTranslation, const Quat& qRotation, const Vec3D& vScale )
{
#if 0
	MX_OPTIMIZE( "build the matrix by hand" )

	Matrix4  translation( Matrix4::mat4_identity );
	translation.SetTranslation( vTranslation );

	Matrix4  rotation( qRotation.ToMat4() );

	Matrix4  scaling = Matrix4(
		vScale.x,	0,			0,			0,
		0,			vScale.y,	0,			0,
		0,			0,			vScale.z,	0,
		0,			0,			0,			1
	);

	// Transformations are ordered as follows:
	//	1) Scale;
	//	2) Rotate;
	//	3) Translate.
	*this = scaling * rotation * translation;

#else

	Matrix4  mat( qRotation.ToMat4() );
	mat.SetTranslation( vTranslation );

	Matrix4  scaling = Matrix4(
		vScale.x,	0,			0,			0,
		0,			vScale.y,	0,			0,
		0,			0,			vScale.z,	0,
		0,			0,			0,			1
	);

	*this = scaling * mat;

#endif
}

//
//	Matrix4::BuildTransform
//
void Matrix4::BuildTransform( const Vec3D& vTranslation, const Quat& qRotation )
{
	*this = qRotation.ToMat4();
	this->SetTranslation( vTranslation );
}

/*
=============
Matrix4::ToString
=============
*/
const char *Matrix4::ToString( INT precision ) const {
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
