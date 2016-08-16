/*
=============================================================================
	File:	Math.h
	Desc:	Common math constants, tables, global math class,
			floating-point arithmetic and so on.
	NOTE:	This file should be included before other math library headers.
			The math library must be initialized prior to its use.
	
	Originally written by Id Software and then modified by CodeGen.
	Copyright (C) 2004 Id Software, Inc. ( idMath )
=============================================================================
*/

#ifndef __MATH_MATH_H__
#define __MATH_MATH_H__

#ifdef MACOS_X
// for square root estimate instruction
#include <ppc_intrinsics.h>
// for FLT_MIN
#include <FLOAT.h>
#endif

#ifdef INFINITY
#undef INFINITY
#endif

#ifdef FLT_EPSILON
#undef FLT_EPSILON
#endif

//
//	IEEE 754 floating point type.
//

/*
		float IEEE-754 bit represenation

		0      0x00000000
		1.0    0x3f800000
		0.5    0x3f000000
		3      0x40400000
		+inf   0x7f800000
		-inf   0xff800000
		+NaN   0x7fc00000 or 0x7ff00000

		in general: number = (sign ? -1:1) * 2^(exponent) * 1.(mantissa bits)
*/

#define FLOATSIGNBITSET(f)		((*(const mxULong *)&(f)) >> 31)
#define FLOATSIGNBITNOTSET(f)	((~(*(const mxULong *)&(f))) >> 31)
#define FLOATNOTZERO(f)			((*(const mxULong *)&(f)) & ~(1<<31) )
#define INTSIGNBITSET(i)		(((const mxULong)(i)) >> 31)
#define INTSIGNBITNOTSET(i)		((~((const mxULong)(i))) >> 31)

#define	FLOAT_IS_NAN(x)			(((*(const mxULong *)&x) & 0x7f800000) == 0x7f800000)
#define FLOAT_IS_INF(x)			(((*(const mxULong *)&x) & 0x7fffffff) == 0x7f800000)
#define FLOAT_IS_IND(x)			((*(const mxULong *)&x) == 0xffc00000)
#define	FLOAT_IS_DENORMAL(x)	(((*(const mxULong *)&x) & 0x7f800000) == 0x00000000 && \
								 ((*(const mxULong *)&x) & 0x007fffff) != 0x00000000 )

#define	FLOAT_IS_INVALID(x)		(FLOAT_IS_NAN(x) || FLOAT_IS_DENORMAL(x))
#define FLOAT_SET_NAN( x )		(*(mxULong *)&x) |= 0x7f800000


namespace abc {

//
// 32-bit floating point data type properties.
//
enum E_IEEE_754_Float32_Standard
{
	IEEE_FLT_EXPONENT_BITS	= 8,
	IEEE_FLT_MANTISSA_BITS	= 23,
	IEEE_FLT_SIGN_BIT		= 31,
	IEEE_FLT_EXPONENT_BIAS	= 127,
};

//
// 64-bit floating point data type properties.
//
enum E_IEEE_754_Float64_Standard
{
	IEEE_DBL_EXPONENT_BITS	= 11,
	IEEE_DBL_MANTISSA_BITS	= 52,
	IEEE_DBL_SIGN_BIT		= 63,
	IEEE_DBL_EXPONENT_BIAS	= 1023,
};

//
// 80-bit floating point data type properties.
//
enum E_IEEE_754_Float80_Standard
{
	IEEE_DBLE_EXPONENT_BIAS	= 0,
	IEEE_DBLE_EXPONENT_BITS	= 15,
	IEEE_DBLE_MANTISSA_BITS	= 63,
	IEEE_DBLE_SIGN_BIT		= 79,
};


template< typename T > FORCEINLINE T	Max( T x, T y ) { return ( x > y ) ? x : y; }
template< typename T > FORCEINLINE T	Min( T x, T y ) { return ( x < y ) ? x : y; }
template< typename T > FORCEINLINE INT	MaxIndex( T x, T y ) { return  ( x > y ) ? 0 : 1; }
template< typename T > FORCEINLINE INT	MinIndex( T x, T y ) { return ( x < y ) ? 0 : 1; }

template< typename T > FORCEINLINE T	Max3( T x, T y, T z ) { return ( x > y ) ? ( ( x > z ) ? x : z ) : ( ( y > z ) ? y : z ); }
template< typename T > FORCEINLINE T	Min3( T x, T y, T z ) { return ( x < y ) ? ( ( x < z ) ? x : z ) : ( ( y < z ) ? y : z ); }
template< typename T > FORCEINLINE INT	Max3Index( T x, T y, T z ) { return ( x > y ) ? ( ( x > z ) ? 0 : 2 ) : ( ( y > z ) ? 1 : 2 ); }
template< typename T > FORCEINLINE INT	Min3Index( T x, T y, T z ) { return ( x < y ) ? ( ( x < z ) ? 0 : 2 ) : ( ( y < z ) ? 1 : 2 ); }

template< typename T > FORCEINLINE T	Abs( T f )		{ return ( f >= 0 ) ? f : -f; }
template< typename T > FORCEINLINE T	Sign( T f )		{ return ( f > 0 ) ? 1 : ( ( f < 0 ) ? -1 : 0 ); }
template< typename T > FORCEINLINE T	Square( T x )	{ return x * x; }
template< typename T > FORCEINLINE T	Cube( T x )		{ return x * x * x; }

template< typename T > FORCEINLINE T	Lerp( const T& a, const T& b, const T amount = 0.5f )	{ return a + (b - a) * amount; }
template< typename T > FORCEINLINE T	Clamp( const T value, const T Min, const T Max )	{ return (value < Min) ? Min : (value > Max) ? Max : value; }
template< typename T > FORCEINLINE T	Saturate( const T value )						{ return Clamp<T>( value, 0, 1 ); }

template< typename T > FORCEINLINE T	Mean( const T a, const T b )	{ return (a + b) * (T)0.5f; }

template< typename T > FORCEINLINE T	IsInRange( const T value, const T Min, const T Max )	{ return (value > Min) && (value < Max); }
template< typename T > FORCEINLINE T	IsInRangeInc( const T value, const T Min, const T Max )	{ return (value >= Min) && (value <= Max); }

//
//	ERoundingMethod
//
enum ERoundingMethod
{
	eRoundNearest,
	eRoundUp,        // Round up always in +ve direction
	eRoundDown,      // Round down always in -ve direction
	eRoundTowards0,  // Round values towards 0
	eRoundAwayFrom0, // Round values away from 0
};

//
//	mxMath
//
class mxMath {
public:
	static void		Init( void );
	static void		Shutdown( void );

	static bool		IsInitialized();

public:
	static FLOAT		RSqrt( FLOAT x );			// reciprocal square root, returns huge number when x == 0.0, assumes x > 0.

	static FLOAT		InvSqrt( FLOAT x );			// inverse square root with 32 bits precision, returns huge number when x == 0.0, assumes x > 0.
	static FLOAT		InvSqrt16( FLOAT x );		// inverse square root with 16 bits precision, returns huge number when x == 0.0, assumes x > 0.
	static DOUBLE		InvSqrt64( FLOAT x );		// inverse square root with 64 bits precision, returns huge number when x == 0.0, assumes x > 0.

	static FLOAT		Sqrt( FLOAT x );			// square root with 32 bits precision
	static FLOAT		Sqrt16( FLOAT x );			// square root with 16 bits precision
	static DOUBLE		Sqrt64( FLOAT x );			// square root with 64 bits precision

	static FLOAT		Sin( FLOAT a );				// sine with 32 bits precision
	static FLOAT		Sin16( FLOAT a );			// sine with 16 bits precision, maximum absolute error is 2.3082e-09
	static DOUBLE		Sin64( FLOAT a );			// sine with 64 bits precision

	static FLOAT		Cos( FLOAT a );				// cosine with 32 bits precision
	static FLOAT		Cos16( FLOAT a );			// cosine with 16 bits precision, maximum absolute error is 2.3082e-09
	static DOUBLE		Cos64( FLOAT a );			// cosine with 64 bits precision

	static void			SinCos( FLOAT a, FLOAT &s, FLOAT &c );		// sine and cosine with 32 bits precision
	static void			SinCos16( FLOAT a, FLOAT &s, FLOAT &c );	// sine and cosine with 16 bits precision
	static void			SinCos64( FLOAT a, DOUBLE &s, DOUBLE &c );	// sine and cosine with 64 bits precision

	static FLOAT		Tan( FLOAT a );				// tangent with 32 bits precision
	static FLOAT		Tan16( FLOAT a );			// tangent with 16 bits precision, maximum absolute error is 1.8897e-08
	static DOUBLE		Tan64( FLOAT a );			// tangent with 64 bits precision

	static FLOAT		ASin( FLOAT a );			// arc sine with 32 bits precision, input is clamped to [-1, 1] to avoid a silent NaN
	static FLOAT		ASin16( FLOAT a );			// arc sine with 16 bits precision, maximum absolute error is 6.7626e-05
	static DOUBLE		ASin64( FLOAT a );			// arc sine with 64 bits precision

	static FLOAT		ACos( FLOAT a );			// arc cosine with 32 bits precision, input is clamped to [-1, 1] to avoid a silent NaN
	static FLOAT		ACos16( FLOAT a );			// arc cosine with 16 bits precision, maximum absolute error is 6.7626e-05
	static DOUBLE		ACos64( FLOAT a );			// arc cosine with 64 bits precision

	static FLOAT		ATan( FLOAT a );			// arc tangent with 32 bits precision
	static FLOAT		ATan16( FLOAT a );			// arc tangent with 16 bits precision, maximum absolute error is 1.3593e-08
	static DOUBLE		ATan64( FLOAT a );			// arc tangent with 64 bits precision

	static FLOAT		ATan( FLOAT y, FLOAT x );	// arc tangent with 32 bits precision
	static FLOAT		ATan16( FLOAT y, FLOAT x );	// arc tangent with 16 bits precision, maximum absolute error is 1.3593e-08
	static DOUBLE		ATan64( FLOAT y, FLOAT x );	// arc tangent with 64 bits precision

	static FLOAT		Pow( FLOAT x, FLOAT y );	// x raised to the power y with 32 bits precision
	static FLOAT		Pow16( FLOAT x, FLOAT y );	// x raised to the power y with 16 bits precision
	static DOUBLE		Pow64( FLOAT x, FLOAT y );	// x raised to the power y with 64 bits precision

	static FLOAT		Exp( FLOAT f );				// e raised to the power f with 32 bits precision
	static FLOAT		Exp16( FLOAT f );			// e raised to the power f with 16 bits precision
	static DOUBLE		Exp64( FLOAT f );			// e raised to the power f with 64 bits precision

	static FLOAT		Log( FLOAT f );				// natural logarithm with 32 bits precision
	static FLOAT		Log16( FLOAT f );			// natural logarithm with 16 bits precision
	static DOUBLE		Log64( FLOAT f );			// natural logarithm with 64 bits precision

	static INT			IPow( INT x, INT y );		// integral x raised to the power y
	static INT			ILog2( FLOAT f );			// integral base-2 logarithm of the floating point value
	static INT			ILog2( INT i );				// integral base-2 logarithm of the integer value

	static INT			BitsForFloat( FLOAT f );	// minumum number of bits required to represent ceil( f )
	static INT			BitsForInteger( INT i );	// minumum number of bits required to represent i
	static INT			MaskForFloatSign( FLOAT f );// returns 0x00000000 if x >= 0.0f and returns 0xFFFFFFFF if x <= -0.0f
	static INT			MaskForIntegerSign( INT i );// returns 0x00000000 if x >= 0 and returns 0xFFFFFFFF if x < 0
	static INT			FloorPowerOfTwo( INT x );	// round x down to the nearest power of 2
	static INT			CeilPowerOfTwo( INT x );	// round x up to the nearest power of 2
	static bool			IsPowerOfTwo( INT x );		// returns true if a signed integer x is a power of 2
	static bool			IsPowerOfTwoU( mxUInt x );	// returns true if an unsigned integer x is a power of 2
	static INT			BitCount( INT x );			// returns the number of 1 bits in x
	static INT			BitReverse( INT x );		// returns the bit reverse of x

	static INT			Abs( INT x );				// returns the absolute value of the integer value (for reference only)
	static FLOAT		Fabs( FLOAT f );			// returns the absolute value of the floating point value
	static FLOAT		Floor( FLOAT f );			// returns the largest integer that is less than or equal to the given value
	static FLOAT		Ceil( FLOAT f );			// returns the smallest integer that is greater than or equal to the given value
	static FLOAT		Rint( FLOAT f );			// returns the nearest integer
	static FLOAT		IntToF( INT i );			// INT to FLOAT conversion
	static INT			Ftoi( FLOAT f );			// FLOAT to INT conversion
	static INT			FtoiFast( FLOAT f );		// fast FLOAT to INT conversion but uses current FPU round mode (default round nearest)
	static mxULong		Ftol( FLOAT f );			// FLOAT to long conversion
	static mxULong		FtolFast( FLOAT );			// fast FLOAT to long conversion but uses current FPU round mode (default round nearest)

	static FLOAT		Reciprocal( FLOAT f );

	static CHAR			ClampChar( INT i );
	static INT16		ClampShort( INT i );
	static INT			ClampInt( INT min, INT max, INT value );
	static FLOAT		ClampFloat( FLOAT min, FLOAT max, FLOAT value );

	static FLOAT		AngleNormalize360( FLOAT angle );
	static FLOAT		AngleNormalize180( FLOAT angle );
	static FLOAT		AngleDelta( FLOAT angle1, FLOAT angle2 );

	static INT			FloatToBits( FLOAT f, INT exponentBits, INT mantissaBits );
	static FLOAT		BitsToFloat( INT i, INT exponentBits, INT mantissaBits );

	static INT			FloatHash( const FLOAT *array, const INT numFloats );

						// Round integer value to a multiple of another value. Supply the rounding method to use.
						// NOTE: very unoptimized.
	static mxUInt		Round( INT iValue, ERoundingMethod eRounding, INT iMultipleOf );

	static const FLOAT		PI;							// pi
	static const FLOAT		TWO_PI;						// pi * 2
	static const FLOAT		HALF_PI;					// pi / 2
	static const FLOAT		ONEFOURTH_PI;				// pi / 4
	static const FLOAT		E;							// e
	static const FLOAT		SQRT_TWO;					// sqrt( 2 )
	static const FLOAT		SQRT_THREE;					// sqrt( 3 )
	static const FLOAT		SQRT_1OVER2;				// sqrt( 1 / 2 )
	static const FLOAT		SQRT_1OVER3;				// sqrt( 1 / 3 )
	static const FLOAT		M_DEG2RAD;					// degrees to radians multiplier
	static const FLOAT		M_RAD2DEG;					// radians to degrees multiplier
	static const FLOAT		M_SEC2MS;					// seconds to milliseconds multiplier
	static const FLOAT		M_MS2SEC;					// milliseconds to seconds multiplier
	static const FLOAT		INFINITY;					// huge number which should be larger than any valid number used
	static const FLOAT		NEG_INFINITY;

	static const FLOAT		FLT_EPSILON;				// smallest positive number such that 1.0+FLT_EPSILON != 1.0

	static const DOUBLE		PI_f64;						// pi

private:
	enum {
		LOOKUP_BITS			= 8,							
		EXP_POS				= 23,							
		EXP_BIAS			= 127,							
		LOOKUP_POS			= (EXP_POS-LOOKUP_BITS),
		SEED_POS			= (EXP_POS-8),
		SQRT_TABLE_SIZE		= (2<<LOOKUP_BITS),
		LOOKUP_MASK			= (SQRT_TABLE_SIZE-1)
	};

	union _flint {
		DWORD	i;
		FLOAT	f;
	};


	static DWORD	iSqrt[ SQRT_TABLE_SIZE ];	// Inverse square root lookup table.

	// A lookup table for converting unsigned chars to floats.
	// Useful for avoiding LHS.
	enum { CHAR_TABLE_SIZE = 256 };
	static FLOAT	Uint8ToFloat32[ CHAR_TABLE_SIZE ];

	static bool		initialized;	// True if global Math has been successfully initialized.

private:
	// Disallow instantiating.
	mxMath() {}
	mxMath( const mxMath& ) {}
	~mxMath() {}
	void operator = ( const mxMath& ) {}

	void ZZStaticChecks()
	{
		StaticAssert( 1
					&& sizeof(INT) == sizeof(FLOAT)
					&& sizeof(mxInt) == sizeof(mxUInt)
					&& sizeof(DWORD) == sizeof(mxReal)
					&& sizeof(DOUBLE) == sizeof(double)
		);
	}
};

#include "Math.inl"

//---------------------------------------------------------------

//
//	Conversions.
//

FORCEINLINE FLOAT DEG2RAD( FLOAT a )	{ return ( (a) * mxMath::M_DEG2RAD ); }
FORCEINLINE FLOAT RAD2DEG( FLOAT a )	{ return ( (a) * mxMath::M_RAD2DEG ); }

FORCEINLINE INT		SEC2MS( FLOAT t )	{ return ( mxMath::FtoiFast( (t) * mxMath::M_SEC2MS ) ); }
FORCEINLINE FLOAT	MS2SEC( INT t )		{ return ( mxMath::IntToF( t ) * mxMath::M_MS2SEC ); }

FORCEINLINE UINT16	ANGLE2SHORT( FLOAT x )	{ return ( mxMath::FtoiFast( (x) * 65536.0f / 360.0f ) & 65535 ); }
FORCEINLINE FLOAT	SHORT2ANGLE( UINT16 x )	{ return ( (x) * ( 360.0f / 65536.0f ) ); }

FORCEINLINE BYTE	ANGLE2BYTE( FLOAT x )	{ return ( mxMath::FtoiFast( (x) * 256.0f / 360.0f ) & 255 ); }
FORCEINLINE FLOAT	BYTE2ANGLE( BYTE x )	{ return ( (x) * ( 360.0f / 256.0f ) ); }

}//End of namespace abc

#endif /* !__MATH_MATH_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
