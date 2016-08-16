/*
=============================================================================
	File:	Matrix4x4.h
	Desc:	Row-major float4x4 matrix.

	Originally written by Id Software.
	Copyright (C) 2004 Id Software, Inc. ( idMat4 )
=============================================================================
*/

#ifndef __MATH_MATRIX_4x4_H__
#define __MATH_MATRIX_4x4_H__

namespace abc {

//===============================================================
//
//	Matrix4 - 4x4 matrix ( row-major, DirectX-style ).
//
//===============================================================

ALIGN_16 class Matrix4 {
public:
					Matrix4( void );
					explicit Matrix4( const Vec4D &x, const Vec4D &y, const Vec4D &z, const Vec4D &w );
					explicit Matrix4(const FLOAT xx, const FLOAT xy, const FLOAT xz, const FLOAT xw,
									const FLOAT yx, const FLOAT yy, const FLOAT yz, const FLOAT yw,
									const FLOAT zx, const FLOAT zy, const FLOAT zz, const FLOAT zw,
									const FLOAT wx, const FLOAT wy, const FLOAT wz, const FLOAT ww );
					explicit Matrix4( const Matrix3 &rotation, const Vec3D &translation );
					explicit Matrix4( const FLOAT src[ 4 ][ 4 ] );
					explicit Matrix4( const FLOAT * m );
					Matrix4( EInitIdentity );

	const Vec4D &	operator []( INT index ) const;
	Vec4D &			operator []( INT index );
	Matrix4			operator *( const FLOAT a ) const;
	Vec4D			operator *( const Vec4D &vec ) const;
	Vec3D			operator *( const Vec3D &vec ) const;
	Matrix4			operator *( const Matrix4 &a ) const;
	Matrix4			operator +( const Matrix4 &a ) const;
	Matrix4			operator -( const Matrix4 &a ) const;
	Matrix4 &		operator *=( const FLOAT a );
	Matrix4 &		operator *=( const Matrix4 &a );
	Matrix4 &		operator +=( const Matrix4 &a );
	Matrix4 &		operator -=( const Matrix4 &a );

	friend Matrix4	operator * ( const FLOAT a, const Matrix4 &mat );
	friend Vec4D	operator * ( const Vec4D &vec, const Matrix4 &mat );
	friend Vec3D	operator * ( const Vec3D &vec, const Matrix4 &mat );
	friend Vec4D &	operator *=( Vec4D &vec, const Matrix4 &mat );
	friend Vec3D &	operator *=( Vec3D &vec, const Matrix4 &mat );

	bool			Compare( const Matrix4 &a ) const;						// exact compare, no epsilon
	bool			Compare( const Matrix4 &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool			operator==( const Matrix4 &a ) const;					// exact compare, no epsilon
	bool			operator!=( const Matrix4 &a ) const;					// exact compare, no epsilon

	void			SetZero( void );
	void			SetIdentity( void );
	bool			IsIdentity( const FLOAT epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetric( const FLOAT epsilon = MATRIX_EPSILON ) const;
	bool			IsDiagonal( const FLOAT epsilon = MATRIX_EPSILON ) const;
	bool			IsRotated( void ) const;

	void			ProjectVector( const Vec4D &src, Vec4D &dst ) const;
	void			UnprojectVector( const Vec4D &src, Vec4D &dst ) const;

	FLOAT			Trace( void ) const;		// Returns the sum of diagonal components.
	FLOAT			Determinant( void ) const;
	Matrix4			Transpose( void ) const;	// returns transpose
	Matrix4 &		TransposeSelf( void );
	Matrix4			Inverse( void ) const;		// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseSelf( void );		// returns false if determinant is zero
	Matrix4			InverseFast( void ) const;	// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseFastSelf( void );	// returns false if determinant is zero
	Matrix4			TransposeMultiply( const Matrix4 &b ) const;
	Matrix4			GetAbsolute() const;		// Returns a matrix with non-negative values.
	void			SetRotationInDegrees( const Vec3D& rotation );
	void			SetRotationInRadians( const Vec3D& rotation );
	Vec3D			GetRotationInDegrees() const;

			// Builds a left-handed, look-at matrix.
	void	BuildView( const Vec3D& eyePosition, const Vec3D& lookAt, const Vec3D& upVector = Vec3D(0,1,0) );

			// Builds a left-handed perspective projection matrix based on a field of view.
	void	BuildProjection( FLOAT FOVy, FLOAT Aspect, FLOAT NearZ, FLOAT FarZ );

			// Builds a left-handed orthographic projection matrix.
	void	BuildOrtho( FLOAT Width, FLOAT Height, FLOAT NearZ, FLOAT FarZ );

			// Makes a transformation matrix with Scaling, Rotation and Translation.
	void	BuildTransform( const Vec3D& translation, const Quat& rotation, const Vec3D& scale );

			// Makes a translation and rotation matrix.
	void	BuildTransform( const Vec3D& translation, const Quat& rotation );

	//void			TransformPlane( mxPlane & /* in out */ plane ) const;

					// Transforms a point.
	void			TransformVector( Vec3D & /* in out */ point ) const;

					// Transforms a normal ( direction ).
	void			TransformNormal( Vec3D & /* in out */ normal ) const;

					// The same as TransformNormal(), provided for convenience.
	void			RotateVector( Vec3D & /* in out */ v ) const { TransformNormal( v ); }

	INT				GetDimension( void ) const;
	bool			IsAffine() const;	// An affine matrix has no projective coefficients.

	Matrix3			ToMat3() const;	// returns the rotation part of the matrix

	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToString( INT precision = 2 ) const;

	void			SetTranslation( const Vec3D& v );
	const Vec3D &	GetTranslation() const;

	void			SetDiagonal( const Vec3D& v );

	void	SetRotationX( FLOAT angle );						// NOTE: Angle in radians.
	void	SetRotationY( FLOAT angle );						// NOTE: Angle in radians.
	void	SetRotationZ( FLOAT angle );						// NOTE: Angle in radians.
	
	void	SetRotationAxis( FLOAT angle, const Vec3D& axis );	// NOTE: Angle in radians.

	// Builds a matrix than rotates from one vector to another vector.
	void	SetRotationAxis( FLOAT angle, const Vec3D& vFrom, const Vec3D& vTo );	// NOTE: Angle in radians.

public:
	static Matrix4	CreateScale( FLOAT uniformScaleFactor );
	static Matrix4	CreateScale( const Vec3D& scaleFactor );

	static Matrix4	CreateTranslation( const Vec3D& translation );

public:
	static const Matrix4	mat4_zero;
	static const Matrix4	mat4_identity;

private:
	Vec4D		mRows[ 4 ];
};

FORCEINLINE Matrix4::Matrix4( void ) {
}

FORCEINLINE Matrix4::Matrix4( const Vec4D &x, const Vec4D &y, const Vec4D &z, const Vec4D &w )
{
	mRows[ 0 ] = x;
	mRows[ 1 ] = y;
	mRows[ 2 ] = z;
	mRows[ 3 ] = w;
}

FORCEINLINE Matrix4::Matrix4( const FLOAT xx, const FLOAT xy, const FLOAT xz, const FLOAT xw,
							const FLOAT yx, const FLOAT yy, const FLOAT yz, const FLOAT yw,
							const FLOAT zx, const FLOAT zy, const FLOAT zz, const FLOAT zw,
							const FLOAT wx, const FLOAT wy, const FLOAT wz, const FLOAT ww )
{
	mRows[0][0] = xx; mRows[0][1] = xy; mRows[0][2] = xz; mRows[0][3] = xw;
	mRows[1][0] = yx; mRows[1][1] = yy; mRows[1][2] = yz; mRows[1][3] = yw;
	mRows[2][0] = zx; mRows[2][1] = zy; mRows[2][2] = zz; mRows[2][3] = zw;
	mRows[3][0] = wx; mRows[3][1] = wy; mRows[3][2] = wz; mRows[3][3] = ww;
}

FORCEINLINE Matrix4::Matrix4( const Matrix3 &rotation, const Vec3D &translation )
{
	// NOTE: Matrix3 is transposed because it is column-major.
	mRows[ 0 ][ 0 ] = rotation[0][0];
	mRows[ 0 ][ 1 ] = rotation[1][0];
	mRows[ 0 ][ 2 ] = rotation[2][0];
	mRows[ 0 ][ 3 ] = 0.0f;
	mRows[ 1 ][ 0 ] = rotation[0][1];
	mRows[ 1 ][ 1 ] = rotation[1][1];
	mRows[ 1 ][ 2 ] = rotation[2][1];
	mRows[ 1 ][ 3 ] = 0.0f;
	mRows[ 2 ][ 0 ] = rotation[0][2];
	mRows[ 2 ][ 1 ] = rotation[1][2];
	mRows[ 2 ][ 2 ] = rotation[2][2];
	mRows[ 2 ][ 3 ] = 0.0f;
	mRows[ 3 ][ 0 ] = translation[0];
	mRows[ 3 ][ 1 ] = translation[1];
	mRows[ 3 ][ 2 ] = translation[2];
	mRows[ 3 ][ 3 ] = 1.0f;
}

FORCEINLINE Matrix4::Matrix4( const FLOAT src[ 4 ][ 4 ] ) {
	MemCopy( mRows, src, 4 * 4 * sizeof( FLOAT ) );
}

FORCEINLINE Matrix4::Matrix4( const FLOAT * m )
{
	mRows[0][0] = m[ 0]; mRows[0][1] = m[ 4]; mRows[0][2] = m[ 8]; mRows[0][3] = m[12];
	mRows[1][0] = m[ 1]; mRows[1][1] = m[ 5]; mRows[1][2] = m[ 9]; mRows[1][3] = m[13];
	mRows[2][0] = m[ 2]; mRows[2][1] = m[ 6]; mRows[2][2] = m[10]; mRows[2][3] = m[14];
	mRows[3][0] = m[ 3]; mRows[3][1] = m[ 7]; mRows[3][2] = m[11]; mRows[3][3] = m[15];
}

FORCEINLINE Matrix4::Matrix4( EInitIdentity )
{
	*this = mat4_identity;
}

FORCEINLINE const Vec4D &Matrix4::operator[]( INT index ) const {
	//Assert( ( index >= 0 ) && ( index < 4 ) );
	return mRows[ index ];
}

FORCEINLINE Vec4D &Matrix4::operator[]( INT index ) {
	//Assert( ( index >= 0 ) && ( index < 4 ) );
	return mRows[ index ];
}

FORCEINLINE Matrix4 Matrix4::operator*( const FLOAT a ) const {
	return Matrix4(
		mRows[0].x * a, mRows[0].y * a, mRows[0].z * a, mRows[0].w * a,
		mRows[1].x * a, mRows[1].y * a, mRows[1].z * a, mRows[1].w * a,
		mRows[2].x * a, mRows[2].y * a, mRows[2].z * a, mRows[2].w * a,
		mRows[3].x * a, mRows[3].y * a, mRows[3].z * a, mRows[3].w * a );
}

FORCEINLINE Vec4D Matrix4::operator*( const Vec4D &vec ) const
{
	return Vec4D(
		mRows[ 0 ].x * vec.x + mRows[ 0 ].y * vec.y + mRows[ 0 ].z * vec.z + mRows[ 0 ].w * vec.w,
		mRows[ 1 ].x * vec.x + mRows[ 1 ].y * vec.y + mRows[ 1 ].z * vec.z + mRows[ 1 ].w * vec.w,
		mRows[ 2 ].x * vec.x + mRows[ 2 ].y * vec.y + mRows[ 2 ].z * vec.z + mRows[ 2 ].w * vec.w,
		mRows[ 3 ].x * vec.x + mRows[ 3 ].y * vec.y + mRows[ 3 ].z * vec.z + mRows[ 3 ].w * vec.w );
}

FORCEINLINE Vec3D Matrix4::operator*( const Vec3D &vec ) const
{
	FLOAT s = mRows[ 3 ].x * vec.x + mRows[ 3 ].y * vec.y + mRows[ 3 ].z * vec.z + mRows[ 3 ].w;
	if ( s == 0.0f ) {
		return Vec3D( 0.0f, 0.0f, 0.0f );
	}
	if ( s == 1.0f ) {
		return Vec3D(
			mRows[ 0 ].x * vec.x + mRows[ 0 ].y * vec.y + mRows[ 0 ].z * vec.z + mRows[ 0 ].w,
			mRows[ 1 ].x * vec.x + mRows[ 1 ].y * vec.y + mRows[ 1 ].z * vec.z + mRows[ 1 ].w,
			mRows[ 2 ].x * vec.x + mRows[ 2 ].y * vec.y + mRows[ 2 ].z * vec.z + mRows[ 2 ].w );
	}
	else {
		FLOAT invS = 1.0f / s;
		return Vec3D(
			(mRows[ 0 ].x * vec.x + mRows[ 0 ].y * vec.y + mRows[ 0 ].z * vec.z + mRows[ 0 ].w) * invS,
			(mRows[ 1 ].x * vec.x + mRows[ 1 ].y * vec.y + mRows[ 1 ].z * vec.z + mRows[ 1 ].w) * invS,
			(mRows[ 2 ].x * vec.x + mRows[ 2 ].y * vec.y + mRows[ 2 ].z * vec.z + mRows[ 2 ].w) * invS );
	}
}

FORCEINLINE Matrix4 Matrix4::operator*( const Matrix4 &a ) const {
	INT i, j;
	const FLOAT *m1Ptr, *m2Ptr;
	FLOAT *dstPtr;
	Matrix4 dst;

	m1Ptr = reinterpret_cast<const FLOAT *>(this);
	m2Ptr = reinterpret_cast<const FLOAT *>(&a);
	dstPtr = reinterpret_cast<FLOAT *>(&dst);

	for ( i = 0; i < 4; i++ ) {
		for ( j = 0; j < 4; j++ ) {
			*dstPtr = m1Ptr[0] * m2Ptr[ 0 * 4 + j ]
					+ m1Ptr[1] * m2Ptr[ 1 * 4 + j ]
					+ m1Ptr[2] * m2Ptr[ 2 * 4 + j ]
					+ m1Ptr[3] * m2Ptr[ 3 * 4 + j ];
			dstPtr++;
		}
		m1Ptr += 4;
	}
	return dst;
}

FORCEINLINE Matrix4 Matrix4::operator+( const Matrix4 &a ) const {
	return Matrix4( 
		mRows[0].x + a[0].x, mRows[0].y + a[0].y, mRows[0].z + a[0].z, mRows[0].w + a[0].w,
		mRows[1].x + a[1].x, mRows[1].y + a[1].y, mRows[1].z + a[1].z, mRows[1].w + a[1].w,
		mRows[2].x + a[2].x, mRows[2].y + a[2].y, mRows[2].z + a[2].z, mRows[2].w + a[2].w,
		mRows[3].x + a[3].x, mRows[3].y + a[3].y, mRows[3].z + a[3].z, mRows[3].w + a[3].w );
}
    
FORCEINLINE Matrix4 Matrix4::operator-( const Matrix4 &a ) const {
	return Matrix4( 
		mRows[0].x - a[0].x, mRows[0].y - a[0].y, mRows[0].z - a[0].z, mRows[0].w - a[0].w,
		mRows[1].x - a[1].x, mRows[1].y - a[1].y, mRows[1].z - a[1].z, mRows[1].w - a[1].w,
		mRows[2].x - a[2].x, mRows[2].y - a[2].y, mRows[2].z - a[2].z, mRows[2].w - a[2].w,
		mRows[3].x - a[3].x, mRows[3].y - a[3].y, mRows[3].z - a[3].z, mRows[3].w - a[3].w );
}

FORCEINLINE Matrix4 &Matrix4::operator*=( const FLOAT a ) {
	mRows[0].x *= a; mRows[0].y *= a; mRows[0].z *= a; mRows[0].w *= a;
	mRows[1].x *= a; mRows[1].y *= a; mRows[1].z *= a; mRows[1].w *= a;
	mRows[2].x *= a; mRows[2].y *= a; mRows[2].z *= a; mRows[2].w *= a;
	mRows[3].x *= a; mRows[3].y *= a; mRows[3].z *= a; mRows[3].w *= a;
    return *this;
}

FORCEINLINE Matrix4 &Matrix4::operator*=( const Matrix4 &a ) {
	*this = (*this) * a;
	return *this;
}

FORCEINLINE Matrix4 &Matrix4::operator+=( const Matrix4 &a ) {
	mRows[0].x += a[0].x; mRows[0].y += a[0].y; mRows[0].z += a[0].z; mRows[0].w += a[0].w;
	mRows[1].x += a[1].x; mRows[1].y += a[1].y; mRows[1].z += a[1].z; mRows[1].w += a[1].w;
	mRows[2].x += a[2].x; mRows[2].y += a[2].y; mRows[2].z += a[2].z; mRows[2].w += a[2].w;
	mRows[3].x += a[3].x; mRows[3].y += a[3].y; mRows[3].z += a[3].z; mRows[3].w += a[3].w;
    return *this;
}

FORCEINLINE Matrix4 &Matrix4::operator-=( const Matrix4 &a ) {
	mRows[0].x -= a[0].x; mRows[0].y -= a[0].y; mRows[0].z -= a[0].z; mRows[0].w -= a[0].w;
	mRows[1].x -= a[1].x; mRows[1].y -= a[1].y; mRows[1].z -= a[1].z; mRows[1].w -= a[1].w;
	mRows[2].x -= a[2].x; mRows[2].y -= a[2].y; mRows[2].z -= a[2].z; mRows[2].w -= a[2].w;
	mRows[3].x -= a[3].x; mRows[3].y -= a[3].y; mRows[3].z -= a[3].z; mRows[3].w -= a[3].w;
    return *this;
}

FORCEINLINE Matrix4 operator*( const FLOAT a, const Matrix4 &mat ) {
	return mat * a;
}

FORCEINLINE Vec4D operator*( const Vec4D &vec, const Matrix4 &mat ) {
	return mat * vec;
}

FORCEINLINE Vec3D operator*( const Vec3D &vec, const Matrix4 &mat ) {
	return mat * vec;
}

FORCEINLINE Vec4D &operator*=( Vec4D &vec, const Matrix4 &mat ) {
	vec = mat * vec;
	return vec;
}

FORCEINLINE Vec3D &operator*=( Vec3D &vec, const Matrix4 &mat ) {
	vec = mat * vec;
	return vec;
}

FORCEINLINE bool Matrix4::Compare( const Matrix4 &a ) const {
	UINT i;
	const FLOAT *ptr1, *ptr2;

	ptr1 = reinterpret_cast<const FLOAT *>(mRows);
	ptr2 = reinterpret_cast<const FLOAT *>(a.mRows);
	for ( i = 0; i < 4*4; i++ ) {
		if ( ptr1[i] != ptr2[i] ) {
			return false;
		}
	}
	return true;
}

FORCEINLINE bool Matrix4::Compare( const Matrix4 &a, const FLOAT epsilon ) const {
	dword i;
	const FLOAT *ptr1, *ptr2;

	ptr1 = reinterpret_cast<const FLOAT *>(mRows);
	ptr2 = reinterpret_cast<const FLOAT *>(a.mRows);
	for ( i = 0; i < 4*4; i++ ) {
		if ( mxMath::Fabs( ptr1[i] - ptr2[i] ) > epsilon ) {
			return false;
		}
	}
	return true;
}

FORCEINLINE bool Matrix4::operator==( const Matrix4 &a ) const {
	return Compare( a );
}

FORCEINLINE bool Matrix4::operator!=( const Matrix4 &a ) const {
	return !Compare( a );
}

FORCEINLINE void Matrix4::SetZero( void ) {
	memset( mRows, 0, sizeof( Matrix4 ) );
}

FORCEINLINE void Matrix4::SetIdentity( void ) {
	*this = mat4_identity;
}

FORCEINLINE bool Matrix4::IsIdentity( const FLOAT epsilon ) const {
	return Compare( mat4_identity, epsilon );
}

FORCEINLINE bool Matrix4::IsSymmetric( const FLOAT epsilon ) const {
	for ( INT i = 1; i < 4; i++ ) {
		for ( INT j = 0; j < i; j++ ) {
			if ( mxMath::Fabs( mRows[i][j] - mRows[j][i] ) > epsilon ) {
				return false;
			}
		}
	}
	return true;
}

FORCEINLINE bool Matrix4::IsDiagonal( const FLOAT epsilon ) const {
	for ( INT i = 0; i < 4; i++ ) {
		for ( INT j = 0; j < 4; j++ ) {
			if ( i != j && mxMath::Fabs( mRows[i][j] ) > epsilon ) {
				return false;
			}
		}
	}
	return true;
}

FORCEINLINE bool Matrix4::IsRotated( void ) const {
	if ( !mRows[ 0 ][ 1 ] && !mRows[ 0 ][ 2 ] &&
		!mRows[ 1 ][ 0 ] && !mRows[ 1 ][ 2 ] &&
		!mRows[ 2 ][ 0 ] && !mRows[ 2 ][ 1 ] ) {
		return false;
	}
	return true;
}

FORCEINLINE void Matrix4::ProjectVector( const Vec4D &src, Vec4D &dst ) const {
	dst.x = src * mRows[ 0 ];
	dst.y = src * mRows[ 1 ];
	dst.z = src * mRows[ 2 ];
	dst.w = src * mRows[ 3 ];
}

FORCEINLINE void Matrix4::UnprojectVector( const Vec4D &src, Vec4D &dst ) const {
	dst = mRows[ 0 ] * src.x + mRows[ 1 ] * src.y + mRows[ 2 ] * src.z + mRows[ 3 ] * src.w;
}

FORCEINLINE FLOAT Matrix4::Trace( void ) const {
	return ( mRows[0][0] + mRows[1][1] + mRows[2][2] + mRows[3][3] );
}

FORCEINLINE Matrix4 Matrix4::Inverse( void ) const {
	Matrix4 invMat;

	invMat = *this;
#if 1
	invMat.InverseSelf();
#else
	AssertPtr( D3DXMatrixInverse( (D3DXMATRIX*) &invMat, NULL, (const D3DXMATRIX*) this->ToFloatPtr() ) );
#endif
	return invMat;
}

FORCEINLINE Matrix4 Matrix4::InverseFast( void ) const {
	Matrix4 invMat;

	invMat = *this;
	invMat.InverseFastSelf();
	return invMat;
}

FORCEINLINE Matrix4 Matrix3::ToMat4( void ) const {
	// NOTE: Matrix3 is transposed because it is column-major.
	return Matrix4(	mColumns[0][0],	mColumns[1][0],	mColumns[2][0],	0.0f,
					mColumns[0][1],	mColumns[1][1],	mColumns[2][1],	0.0f,
					mColumns[0][2],	mColumns[1][2],	mColumns[2][2],	0.0f,
					0.0f,			0.0f,			0.0f,			1.0f );
}

FORCEINLINE INT Matrix4::GetDimension( void ) const {
	return 16;
}

FORCEINLINE Matrix3 Matrix4::ToMat3() const
{
	return Matrix3(	mRows[0][0],	mRows[1][0],	mRows[2][0],
					mRows[0][1],	mRows[1][1],	mRows[2][1],
					mRows[0][2],	mRows[1][2],	mRows[2][2] );
}

FORCEINLINE const FLOAT *Matrix4::ToFloatPtr( void ) const {
	return mRows[0].ToFloatPtr();
}

FORCEINLINE FLOAT *Matrix4::ToFloatPtr( void ) {
	return mRows[0].ToFloatPtr();
}

FORCEINLINE void Matrix4::SetTranslation( const Vec3D& v ) {
	mRows[3].x = v.x;
	mRows[3].y = v.y;
	mRows[3].z = v.z;
}

FORCEINLINE const Vec3D & Matrix4::GetTranslation() const {
	return *(const Vec3D*) & mRows[3]; // the fourth row
}

FORCEINLINE void Matrix4::SetDiagonal( const Vec3D& v ) {
	mRows[0][0] = v.x;
	mRows[1][1] = v.y;
	mRows[2][2] = v.z;
}

FORCEINLINE Matrix4 Matrix4::CreateScale( FLOAT uniformScaleFactor ) {
	return Matrix4( 
		uniformScaleFactor,	0,					0,					0,
		0,					uniformScaleFactor,	0,					0,
		0,					0,					uniformScaleFactor,	0,
		0,					0,					0,					1 );
}

FORCEINLINE Matrix4 Matrix4::CreateScale( const Vec3D& scaleFactor ) {
	return Matrix4( 
		scaleFactor.x,		0,					0,					0,
		0,					scaleFactor.y,		0,					0,
		0,					0,					scaleFactor.z,		0,
		0,					0,					0,					1 );
}

FORCEINLINE Matrix4 Matrix4::CreateTranslation( const Vec3D& translation ) {
	return Matrix4( 
		1,				0,				0,				0,
		0,				1,				0,				0,
		0,				0,				1,				0,
		translation.x,	translation.y,	translation.z,	1 );
}

//
//	Matrix4::BuildView
//
FORCEINLINE void Matrix4::BuildView( const Vec3D& eyePosition, const Vec3D& lookAt, const Vec3D& upVector )
{
#ifdef USE_D3D_MATH_FUNC

	::D3DXMatrixLookAtLH
	(
		(D3DXMATRIX* ) mRows,
		(const D3DXVECTOR3* ) &eyePosition,
		(const D3DXVECTOR3* ) &lookAt,
		(const D3DXVECTOR3* ) &upVector
	);

#else

	Vec3D 	zaxis( lookAt - eyePosition );
	zaxis.Normalize();

	Vec3D 	xaxis( upVector.Cross( zaxis ) );
	zaxis.Normalize();

	Vec3D 	yaxis( zaxis.Cross( xaxis ) );

	mRows[0].Set(	xaxis.x,           			yaxis.x,           			zaxis.x,          			0 );
	mRows[1].Set(	xaxis.y,           			yaxis.y,           			zaxis.y,          			0 );
	mRows[2].Set(	xaxis.z,           			yaxis.z,           			zaxis.z,          			0 );
	mRows[3].Set(	-xaxis.Dot( eyePosition ),  -yaxis.Dot( eyePosition ),  -zaxis.Dot( eyePosition ),  1 );

#endif // ! USE_D3D_MATH_FUNC
}

//
//	Matrix4::BuildProjection
//
// FOVy - Vertical field of view ( in the y direction ), in radians.
// Aspect - Aspect ratio ( screen width divided by screen height ).
//
FORCEINLINE void Matrix4::BuildProjection( FLOAT FOVy, FLOAT Aspect, FLOAT NearZ, FLOAT FarZ )
{
#ifdef USE_D3D_MATH_FUNC

	::D3DXMatrixPerspectiveFovLH
	(
		(D3DXMATRIX* ) mRows,
		FOVy,
		Aspect,
		NearZ,
		FarZ
	);

#else

	const FLOAT yScale = 1 / mxMath::Tan( FOVy / 2 );
	const FLOAT xScale = yScale / Aspect;

	mRows[0].Set(	xScale,     0,          0,               			0 );
	mRows[1].Set(	0,        yScale,       0,               			0 );
	mRows[2].Set(	0,          0,       FarZ/(FarZ - NearZ),			1 );
	mRows[3].Set(	0,          0,       -NearZ * FarZ/(FarZ - NearZ),  0 );

#endif // ! USE_D3D_MATH_FUNC
}

//
//	Matrix4::BuildOrtho
//
FORCEINLINE void Matrix4::BuildOrtho( FLOAT Width, FLOAT Height, FLOAT NearZ, FLOAT FarZ )
{
#ifdef USE_D3D_MATH_FUNC

	::D3DXMatrixOrthoLH
	(
		(D3DXMATRIX* ) mRows,
		Width,
		Height,
		NearZ,
		FarZ
	);

#else

	mRows[0].Set(	2/Width,     0,			0,               	0 );
	mRows[1].Set(	0,        2/Height,		0,               	0 );
	mRows[2].Set(	0,          0,       1/(FarZ - NearZ),		0 );
	mRows[3].Set(	0,          0,       NearZ/(NearZ - FarZ),  1 );

#endif // ! USE_D3D_MATH_FUNC
}

//
//	Matrix4::TransformVector
//
FORCEINLINE void Matrix4::TransformVector( Vec3D & vect ) const
{
#ifdef USE_D3D_MATH_FUNC

	::D3DXVec3TransformCoord(
		(D3DXVECTOR3* ) &vect,
		(const D3DXVECTOR3* ) &vect,
		(const D3DXMATRIX* ) mRows
	);

#else

	FLOAT vector[3];
	FLOAT * M = (FLOAT*) mRows;

	vector[0] = vect.x*M[0] + vect.y*M[4] + vect.z*M[8] + M[12];
	vector[1] = vect.x*M[1] + vect.y*M[5] + vect.z*M[9] + M[13];
	vector[2] = vect.x*M[2] + vect.y*M[6] + vect.z*M[10] + M[14];

	vect.x = vector[0];
	vect.y = vector[1];
	vect.z = vector[2];

#endif  // ! USE_D3D_MATH_FUNC
}

//
//	Matrix4::TransformNormal
//
FORCEINLINE void Matrix4::TransformNormal( Vec3D & normal ) const
{
#ifdef USE_D3D_MATH_FUNC

	::D3DXVec3TransformNormal(
		(D3DXVECTOR3* ) &normal,
		(const D3DXVECTOR3* ) &normal,
		(const D3DXMATRIX* ) mRows
	);

#else

	Vec3D  vect( normal );
	FLOAT * M = (FLOAT*) mRows;

	normal.x = vect.x*M[0] + vect.y*M[4] + vect.z*M[8];
	normal.y = vect.x*M[1] + vect.y*M[5] + vect.z*M[9];
	normal.z = vect.x*M[2] + vect.y*M[6] + vect.z*M[10];

#endif  // ! USE_D3D_MATH_FUNC
}

}//End of namespace abc

#endif /* !__MATH_MATRIX_4x4_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
