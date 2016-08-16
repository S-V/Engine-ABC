/*
=============================================================================
	File:	Math.cpp
	Desc:	.
			Copyright (C) 2004 Id Software, Inc. ( idMath )
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
//#include <Base.h>

namespace abc {

const FLOAT	mxMath::PI				= 3.14159265358979323846f;
const FLOAT	mxMath::TWO_PI			= 2.0f * PI;
const FLOAT	mxMath::HALF_PI			= 0.5f * PI;
const FLOAT	mxMath::ONEFOURTH_PI	= 0.25f * PI;
const FLOAT mxMath::E				= 2.71828182845904523536f;
const FLOAT mxMath::SQRT_TWO		= 1.41421356237309504880f;
const FLOAT mxMath::SQRT_THREE		= 1.73205080756887729352f;
const FLOAT	mxMath::SQRT_1OVER2		= 0.70710678118654752440f;
const FLOAT	mxMath::SQRT_1OVER3		= 0.57735026918962576450f;
const FLOAT	mxMath::M_DEG2RAD		= PI / 180.0f;
const FLOAT	mxMath::M_RAD2DEG		= 180.0f / PI;
const FLOAT	mxMath::M_SEC2MS		= 1000.0f;
const FLOAT	mxMath::M_MS2SEC		= 0.001f;
const FLOAT	mxMath::INFINITY		= 1e30f;
const FLOAT	mxMath::NEG_INFINITY	= -INFINITY;
const FLOAT mxMath::FLT_EPSILON		= 1.192092896e-07f;

const DOUBLE mxMath::PI_f64	= 3.1415926535897932384626433832795;

bool	mxMath::initialized		= false;
DWORD	mxMath::iSqrt[ SQRT_TABLE_SIZE ];			// inverse square root lookup table
FLOAT	mxMath::Uint8ToFloat32[ CHAR_TABLE_SIZE ];	// 'unsigned char' to 'float' conversion table

/*
===============
mxMath::Init
===============
*/
void mxMath::Init( void )
{
	Assert( ! IsInitialized() );

	// Check that floating-point types on this machine conform to the IEEE 754 standard.
	// Run unit-tests ( e.g. if memzero is valid for zeroing out floats, etc. ).
	MX_TODO( "floating-point data type validation" );

	//===========================================================================

	// TODO: check SSE support, create a SIMD processor, set FPU states, run unit-tests, etc.

	//===========================================================================
/*
	AlwaysAssert( sizeof( int ) == 4 );	// 4 bytes.
	AlwaysAssert( sizeof( DWORD ) == 4 );

	AlwaysAssert( sizeof( ::abc::mxInt ) == sizeof( int ) );
	AlwaysAssert( sizeof( ::abc::mxUInt ) == sizeof( ::abc::mxInt ) );
	AlwaysAssert( sizeof( ::abc::FLOAT ) == sizeof( ::abc::mxInt ) );
	AlwaysAssert( sizeof( ::abc::DOUBLE ) == 2 * sizeof( ::abc::FLOAT ) );
*/
	//===========================================================================

	//
	//	Initialize look-up tables.
	//

	//********************************************************
    union _flint fi, fo;

    for ( UINT i = 0; i < SQRT_TABLE_SIZE; i++ )
	{
        fi.i	 = ((EXP_BIAS-1) << EXP_POS) | (i << LOOKUP_POS);
        fo.f	 = (FLOAT)( 1.0 / sqrt( fi.f ) );
        iSqrt[i] = ((DWORD)(((fo.i + (1<<(SEED_POS-2))) >> SEED_POS) & 0xFF))<<SEED_POS;
    }
    
	iSqrt[ SQRT_TABLE_SIZE / 2 ] = ((DWORD)(0xFF))<<(SEED_POS); 

	//********************************************************
	for ( UINT i = 0; i < CHAR_TABLE_SIZE; i++ )
	{
		Uint8ToFloat32[ i ] = (FLOAT) ((unsigned char) i );
	}
	//********************************************************

	initialized = true;
}

/*
===============
mxMath::Shutdown
===============
*/
void mxMath::Shutdown( void )
{
	Assert( IsInitialized() );
}

/*
===============
mxMath::IsInitialized
===============
*/
bool mxMath::IsInitialized( void )
{
	return initialized;
}

/*
================
mxMath::FloatToBits
================
*/
INT mxMath::FloatToBits( FLOAT f, INT exponentBits, INT mantissaBits ) {
	INT i, sign, exponent, mantissa, value;

	Assert( exponentBits >= 2 && exponentBits <= 8 );
	Assert( mantissaBits >= 2 && mantissaBits <= 23 );

	INT maxBits = ( ( ( 1 << ( exponentBits - 1 ) ) - 1 ) << mantissaBits ) | ( ( 1 << mantissaBits ) - 1 );
	INT minBits = ( ( ( 1 <<   exponentBits       ) - 2 ) << mantissaBits ) | 1;

	FLOAT max = BitsToFloat( maxBits, exponentBits, mantissaBits );
	FLOAT min = BitsToFloat( minBits, exponentBits, mantissaBits );

	if ( f >= 0.0f ) {
		if ( f >= max ) {
			return maxBits;
		} else if ( f <= min ) {
			return minBits;
		}
	} else {
		if ( f <= -max ) {
			return ( maxBits | ( 1 << ( exponentBits + mantissaBits ) ) );
		} else if ( f >= -min ) {
			return ( minBits | ( 1 << ( exponentBits + mantissaBits ) ) );
		}
	}

	exponentBits--;
	i = *reinterpret_cast<INT *>(&f);
	sign = ( i >> IEEE_FLT_SIGN_BIT ) & 1;
	exponent = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
	mantissa = i & ( ( 1 << IEEE_FLT_MANTISSA_BITS ) - 1 );
	value = sign << ( 1 + exponentBits + mantissaBits );
	value |= ( ( INTSIGNBITSET( exponent ) << exponentBits ) | ( abs( exponent ) & ( ( 1 << exponentBits ) - 1 ) ) ) << mantissaBits;
	value |= mantissa >> ( IEEE_FLT_MANTISSA_BITS - mantissaBits );
	return value;
}

/*
================
mxMath::BitsToFloat
================
*/
FLOAT mxMath::BitsToFloat( INT i, INT exponentBits, INT mantissaBits ) {
	static INT exponentSign[2] = { 1, -1 };
	INT sign, exponent, mantissa, value;

	Assert( exponentBits >= 2 && exponentBits <= 8 );
	Assert( mantissaBits >= 2 && mantissaBits <= 23 );

	exponentBits--;
	sign = i >> ( 1 + exponentBits + mantissaBits );
	exponent = ( ( i >> mantissaBits ) & ( ( 1 << exponentBits ) - 1 ) ) * exponentSign[( i >> ( exponentBits + mantissaBits ) ) & 1];
	mantissa = ( i & ( ( 1 << mantissaBits ) - 1 ) ) << ( IEEE_FLT_MANTISSA_BITS - mantissaBits );
	value = sign << IEEE_FLT_SIGN_BIT | ( exponent + IEEE_FLT_EXPONENT_BIAS ) << IEEE_FLT_MANTISSA_BITS | mantissa;
	return *reinterpret_cast<FLOAT *>(&value);
}

/*
================
mxMath::Round
================
*/
mxUInt mxMath::Round( INT iValue, ERoundingMethod eRounding, INT iMultipleOf )
{
	Assert2( iMultipleOf != 0, "Invalid parameter" );	// Get positive multiple, ensure it is not 0.
	const mxUInt iPosMultipleOf = Abs( iMultipleOf );

	// Select rounding method - often need different technique for +ve and -ve numbers
	switch ( eRounding )
	{
		case eRoundTowards0 :
			return iPosMultipleOf * ( iValue / iPosMultipleOf );

		case eRoundDown :
		{
			if ( iValue >= 0 ) {
				return iPosMultipleOf * ( iValue / iPosMultipleOf );
			} else {
				return iPosMultipleOf * ( (iValue - iPosMultipleOf + 1) / iPosMultipleOf );
			}
		}
		case eRoundAwayFrom0 :
		{
			if ( iValue >= 0 ) {
				return iPosMultipleOf * ( (iValue + iPosMultipleOf - 1) / iPosMultipleOf );
			} else {
				return iPosMultipleOf * ( (iValue - iPosMultipleOf + 1) / iPosMultipleOf );
			}
		}
		case eRoundUp :
		{
			if ( iValue >= 0 ) {
				return iPosMultipleOf * ( (iValue + iPosMultipleOf - 1) / iPosMultipleOf );
			} else {
				return iPosMultipleOf * ( iValue / iPosMultipleOf );
			}
		}
		case eRoundNearest :
		{
			if ( iValue >= 0 ) {
				return iPosMultipleOf * ( (iValue + (iPosMultipleOf >> 1)) / iMultipleOf );
			} else {
				return iMultipleOf * ( (iValue - (iMultipleOf >> 1)) / iMultipleOf );
			}
		}
	}//End Of Switch

	Unreachable;
	return 0;
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
