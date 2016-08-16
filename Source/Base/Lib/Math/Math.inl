
//
// The magical inverse square root function from Quake, id Software.
// Note: uses a Newton-Raphson iteration with a clever first approximation ( magical constant ).
// Note: don't use if very high accuracy is needed.
// Note: this routine can actually be slower on modern CPUs because
// it operates on the integer representation of floating point numbers
// and data transfers between integer and floating-point units are relatively slow.
//
FORCEINLINE FLOAT mxMath::RSqrt( FLOAT x )
{
	DWORD i;
	FLOAT y, r;
	const FLOAT threeHalfs = 1.5f;

	y = x * 0.5f;

#if 0 // Original code from Id Software

	i = *reinterpret_cast<DWORD *>( &x );
	i = 0x5f3759df - ( i >> 1 );
	r = *reinterpret_cast<FLOAT *>( &i );

#else
	// This version is said to be slightly better.
	const DWORD magicConstant = 0x5f375a86;	// The new constant proposed by Chris Lomont.
	i = *(DWORD* )( &x );
	i = magicConstant - ( i >> 1 );
	r = *(FLOAT* )( &i );
#endif

	r = r * ( threeHalfs - r * r * y );		// 1st iteration ( repeating increases accuracy ).
	//r = r * ( threeHalfs - r * r * y );	// 2nd iteration - this can be removed.
	return r;
}

FORCEINLINE FLOAT mxMath::InvSqrt16( FLOAT x )
{
	DWORD a = ((union _flint*)(&x))->i;
	union _flint seed;

	Assert( initialized );

	DOUBLE y = x * 0.5f;
	seed.i = (( ( (3*EXP_BIAS-1) - ( (a >> EXP_POS) & 0xFF) ) >> 1)<<EXP_POS) | iSqrt[(a >> (EXP_POS-LOOKUP_BITS)) & LOOKUP_MASK];
	DOUBLE r = seed.f;
	r = r * ( 1.5f - r * r * y );
	return (FLOAT) r;
}

FORCEINLINE FLOAT mxMath::InvSqrt( FLOAT x )
{
#ifdef MX_USE_SSE_ASM

	__asm
	{
		movss		xmm0, x
		rsqrtss		xmm0, xmm0
		movss		x, xmm0
	}
	return x;

#else

	// Use a look-up table and floating-point tricks.
	// This can actually be slower on modern CPUs because of LHS issues.
	DWORD a = ((union _flint*)(&x))->i;
	union _flint seed;

	Assert( initialized );

	DOUBLE y = x * 0.5f;
	seed.i = (( ( (3*EXP_BIAS-1) - ( (a >> EXP_POS) & 0xFF) ) >> 1)<<EXP_POS) | iSqrt[(a >> (EXP_POS-LOOKUP_BITS)) & LOOKUP_MASK];
	DOUBLE r = seed.f;
	r = r * ( 1.5f - r * r * y );
	r = r * ( 1.5f - r * r * y );
	return (FLOAT) r;

#endif
}

FORCEINLINE DOUBLE mxMath::InvSqrt64( FLOAT x ) {
	DWORD a = ((union _flint*)(&x))->i;
	union _flint seed;

	Assert( initialized );

	DOUBLE y = x * 0.5f;
	seed.i = (( ( (3*EXP_BIAS-1) - ( (a >> EXP_POS) & 0xFF) ) >> 1)<<EXP_POS) | iSqrt[(a >> (EXP_POS-LOOKUP_BITS)) & LOOKUP_MASK];
	DOUBLE r = seed.f;
	r = r * ( 1.5f - r * r * y );
	r = r * ( 1.5f - r * r * y );
	r = r * ( 1.5f - r * r * y );
	return r;
}

FORCEINLINE FLOAT mxMath::Sqrt16( FLOAT x ) {
	return x * InvSqrt16( x );
}

FORCEINLINE FLOAT mxMath::Sqrt( FLOAT x )
{	MX_OPTIMIZE( check alternative implementations ( SSE-optimized? ) )
	return x * InvSqrt( x );
}

FORCEINLINE DOUBLE mxMath::Sqrt64( FLOAT x ) {
	return x * InvSqrt64( x );
}

FORCEINLINE FLOAT mxMath::Sin( FLOAT a ) {
	return ::sinf( a );
}

FORCEINLINE FLOAT mxMath::Sin16( FLOAT a ) {
	FLOAT s;

	if ( ( a < 0.0f ) || ( a >= TWO_PI ) ) {
		a -= ::floorf( a / TWO_PI ) * TWO_PI;
	}
#if 1
	if ( a < PI ) {
		if ( a > HALF_PI ) {
			a = PI - a;
		}
	} else {
		if ( a > PI + HALF_PI ) {
			a = a - TWO_PI;
		} else {
			a = PI - a;
		}
	}
#else
	a = PI - a;
	if ( fabs( a ) >= HALF_PI ) {
		a = ( ( a < 0.0f ) ? -PI : PI ) - a;
	}
#endif
	s = a * a;
	return a * ( ( ( ( ( -2.39e-08f * s + 2.7526e-06f ) * s - 1.98409e-04f ) * s + 8.3333315e-03f ) * s - 1.666666664e-01f ) * s + 1.0f );
}

FORCEINLINE DOUBLE mxMath::Sin64( FLOAT a ) {
	return ::sin( a );
}

FORCEINLINE FLOAT mxMath::Cos( FLOAT a ) {
	return ::cosf( a );
}

FORCEINLINE FLOAT mxMath::Cos16( FLOAT a ) {
	FLOAT s, d;

	if ( ( a < 0.0f ) || ( a >= TWO_PI ) ) {
		a -= ::floorf( a / TWO_PI ) * TWO_PI;
	}
#if 1
	if ( a < PI ) {
		if ( a > HALF_PI ) {
			a = PI - a;
			d = -1.0f;
		} else {
			d = 1.0f;
		}
	} else {
		if ( a > PI + HALF_PI ) {
			a = a - TWO_PI;
			d = 1.0f;
		} else {
			a = PI - a;
			d = -1.0f;
		}
	}
#else
	a = PI - a;
	if ( ::fabs( a ) >= HALF_PI ) {
		a = ( ( a < 0.0f ) ? -PI : PI ) - a;
		d = 1.0f;
	} else {
		d = -1.0f;
	}
#endif
	s = a * a;
	return d * ( ( ( ( ( -2.605e-07f * s + 2.47609e-05f ) * s - 1.3888397e-03f ) * s + 4.16666418e-02f ) * s - 4.999999963e-01f ) * s + 1.0f );
}

FORCEINLINE DOUBLE mxMath::Cos64( FLOAT a ) {
	return ::cos( a );
}

FORCEINLINE void mxMath::SinCos( FLOAT a, FLOAT &s, FLOAT &c ) {
#ifdef _WIN32
	_asm {
		fld		a
		fsincos
		mov		ecx, c
		mov		edx, s
		fstp	dword ptr [ecx]
		fstp	dword ptr [edx]
	}
#else
	s = ::sinf( a );
	c = ::cosf( a );
#endif
}

FORCEINLINE void mxMath::SinCos16( FLOAT a, FLOAT &s, FLOAT &c ) {
	FLOAT t, d;

	if ( ( a < 0.0f ) || ( a >= mxMath::TWO_PI ) ) {
		a -= ::floorf( a / mxMath::TWO_PI ) * mxMath::TWO_PI;
	}
#if 1
	if ( a < PI ) {
		if ( a > HALF_PI ) {
			a = PI - a;
			d = -1.0f;
		} else {
			d = 1.0f;
		}
	} else {
		if ( a > PI + HALF_PI ) {
			a = a - TWO_PI;
			d = 1.0f;
		} else {
			a = PI - a;
			d = -1.0f;
		}
	}
#else
	a = PI - a;
	if ( fabs( a ) >= HALF_PI ) {
		a = ( ( a < 0.0f ) ? -PI : PI ) - a;
		d = 1.0f;
	} else {
		d = -1.0f;
	}
#endif
	t = a * a;
	s = a * ( ( ( ( ( -2.39e-08f * t + 2.7526e-06f ) * t - 1.98409e-04f ) * t + 8.3333315e-03f ) * t - 1.666666664e-01f ) * t + 1.0f );
	c = d * ( ( ( ( ( -2.605e-07f * t + 2.47609e-05f ) * t - 1.3888397e-03f ) * t + 4.16666418e-02f ) * t - 4.999999963e-01f ) * t + 1.0f );
}

FORCEINLINE void mxMath::SinCos64( FLOAT a, DOUBLE &s, DOUBLE &c ) {
#ifdef _WIN32
	_asm {
		fld		a
		fsincos
		mov		ecx, c
		mov		edx, s
		fstp	qword ptr [ecx]
		fstp	qword ptr [edx]
	}
#else
	s = sin( a );
	c = cos( a );
#endif
}

FORCEINLINE FLOAT mxMath::Tan( FLOAT a ) {
	return ::tanf( a );
}

FORCEINLINE FLOAT mxMath::Tan16( FLOAT a ) {
	FLOAT s;
	bool reciprocal;

	if ( ( a < 0.0f ) || ( a >= PI ) ) {
		a -= ::floorf( a / PI ) * PI;
	}
#if 1
	if ( a < HALF_PI ) {
		if ( a > ONEFOURTH_PI ) {
			a = HALF_PI - a;
			reciprocal = true;
		} else {
			reciprocal = false;
		}
	} else {
		if ( a > HALF_PI + ONEFOURTH_PI ) {
			a = a - PI;
			reciprocal = false;
		} else {
			a = HALF_PI - a;
			reciprocal = true;
		}
	}
#else
	a = HALF_PI - a;
	if ( fabs( a ) >= ONEFOURTH_PI ) {
		a = ( ( a < 0.0f ) ? -HALF_PI : HALF_PI ) - a;
		reciprocal = false;
	} else {
		reciprocal = true;
	}
#endif
	s = a * a;
	s = a * ( ( ( ( ( ( 9.5168091e-03f * s + 2.900525e-03f ) * s + 2.45650893e-02f ) * s + 5.33740603e-02f ) * s + 1.333923995e-01f ) * s + 3.333314036e-01f ) * s + 1.0f );
	if ( reciprocal ) {
		return 1.0f / s;
	} else {
		return s;
	}
}

FORCEINLINE DOUBLE mxMath::Tan64( FLOAT a ) {
	return ::tan( a );
}

FORCEINLINE FLOAT mxMath::ASin( FLOAT a ) {
	if ( a <= -1.0f ) {
		return -HALF_PI;
	}
	if ( a >= 1.0f ) {
		return HALF_PI;
	}
	return ::asinf( a );
}

FORCEINLINE FLOAT mxMath::ASin16( FLOAT a ) {
	if ( FLOATSIGNBITSET( a ) ) {
		if ( a <= -1.0f ) {
			return -HALF_PI;
		}
		a = ::fabs( a );
		return ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a ) - HALF_PI;
	} else {
		if ( a >= 1.0f ) {
			return HALF_PI;
		}
		return HALF_PI - ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a );
	}
}

FORCEINLINE DOUBLE mxMath::ASin64( FLOAT a ) {
	if ( a <= -1.0f ) {
		return -HALF_PI;
	}
	if ( a >= 1.0f ) {
		return HALF_PI;
	}
	return ::asin( a );
}

FORCEINLINE FLOAT mxMath::ACos( FLOAT a ) {
	if ( a <= -1.0f ) {
		return PI;
	}
	if ( a >= 1.0f ) {
		return 0.0f;
	}
	return ::acosf( a );
}

FORCEINLINE FLOAT mxMath::ACos16( FLOAT a ) {
	if ( FLOATSIGNBITSET( a ) ) {
		if ( a <= -1.0f ) {
			return PI;
		}
		a = fabs( a );
		return PI - ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a );
	} else {
		if ( a >= 1.0f ) {
			return 0.0f;
		}
		return ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a );
	}
}

FORCEINLINE DOUBLE mxMath::ACos64( FLOAT a ) {
	if ( a <= -1.0f ) {
		return PI;
	}
	if ( a >= 1.0f ) {
		return 0.0f;
	}
	return ::acos( a );
}

FORCEINLINE FLOAT mxMath::ATan( FLOAT a ) {
	return atanf( a );
}

FORCEINLINE FLOAT mxMath::ATan16( FLOAT a ) {
	FLOAT s;

	if ( ::fabs( a ) > 1.0f ) {
		a = 1.0f / a;
		s = a * a;
		s = - ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
				* s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
		if ( FLOATSIGNBITSET( a ) ) {
			return s - HALF_PI;
		} else {
			return s + HALF_PI;
		}
	} else {
		s = a * a;
		return ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
			* s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
	}
}

FORCEINLINE DOUBLE mxMath::ATan64( FLOAT a ) {
	return ::atan( a );
}

FORCEINLINE FLOAT mxMath::ATan( FLOAT y, FLOAT x ) {
	return ::atan2f( y, x );
}

FORCEINLINE FLOAT mxMath::ATan16( FLOAT y, FLOAT x ) {
	FLOAT a, s;

	if ( ::fabs( y ) > ::fabs( x ) ) {
		a = x / y;
		s = a * a;
		s = - ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
				* s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
		if ( FLOATSIGNBITSET( a ) ) {
			return s - HALF_PI;
		} else {
			return s + HALF_PI;
		}
	} else {
		a = y / x;
		s = a * a;
		return ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
			* s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
	}
}

FORCEINLINE DOUBLE mxMath::ATan64( FLOAT y, FLOAT x ) {
	return ::atan2( y, x );
}

FORCEINLINE FLOAT mxMath::Pow( FLOAT x, FLOAT y ) {
	return ::powf( x, y );
}

FORCEINLINE FLOAT mxMath::Pow16( FLOAT x, FLOAT y ) {
	return Exp16( y * Log16( x ) );
}

FORCEINLINE DOUBLE mxMath::Pow64( FLOAT x, FLOAT y ) {
	return ::pow( x, y );
}

FORCEINLINE FLOAT mxMath::Exp( FLOAT f ) {
	return ::expf( f );
}

FORCEINLINE FLOAT mxMath::Exp16( FLOAT f ) {
	INT i, s, e, m, exponent;
	FLOAT x, x2, y, p, q;

	x = f * 1.44269504088896340f;		// multiply with ( 1 / log( 2 ) )
#if 1
	i = *reinterpret_cast<INT *>(&x);
	s = ( i >> IEEE_FLT_SIGN_BIT );
	e = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
	m = ( i & ( ( 1 << IEEE_FLT_MANTISSA_BITS ) - 1 ) ) | ( 1 << IEEE_FLT_MANTISSA_BITS );
	i = ( ( m >> ( IEEE_FLT_MANTISSA_BITS - e ) ) & ~( e >> 31 ) ) ^ s;
#else
	i = (INT) x;
	if ( x < 0.0f ) {
		i--;
	}
#endif
	exponent = ( i + IEEE_FLT_EXPONENT_BIAS ) << IEEE_FLT_MANTISSA_BITS;
	y = *reinterpret_cast<FLOAT *>(&exponent);
	x -= (FLOAT) i;
	if ( x >= 0.5f ) {
		x -= 0.5f;
		y *= 1.4142135623730950488f;	// multiply with sqrt( 2 )
	}
	x2 = x * x;
	p = x * ( 7.2152891511493f + x2 * 0.0576900723731f );
	q = 20.8189237930062f + x2;
	x = y * ( q + p ) / ( q - p );
	return x;
}

FORCEINLINE DOUBLE mxMath::Exp64( FLOAT f ) {
	return ::exp( f );
}

FORCEINLINE FLOAT mxMath::Log( FLOAT f ) {
	return ::logf( f );
}

FORCEINLINE FLOAT mxMath::Log16( FLOAT f ) {
	INT i, exponent;
	FLOAT y, y2;

	i = *reinterpret_cast<INT *>(&f);
	exponent = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
	i -= ( exponent + 1 ) << IEEE_FLT_MANTISSA_BITS;	// get value in the range [.5, 1>
	y = *reinterpret_cast<FLOAT *>(&i);
	y *= 1.4142135623730950488f;						// multiply with sqrt( 2 )
	y = ( y - 1.0f ) / ( y + 1.0f );
	y2 = y * y;
	y = y * ( 2.000000000046727f + y2 * ( 0.666666635059382f + y2 * ( 0.4000059794795f + y2 * ( 0.28525381498f + y2 * 0.2376245609f ) ) ) );
	y += 0.693147180559945f * ( (FLOAT)exponent + 0.5f );
	return y;
}

FORCEINLINE DOUBLE mxMath::Log64( FLOAT f ) {
	return log( f );
}

FORCEINLINE INT mxMath::IPow( INT x, INT y ) {
	INT r; for( r = x; y > 1; y-- ) { r *= x; } return r;
}

FORCEINLINE INT mxMath::ILog2( FLOAT f ) {
	return ( ( (*reinterpret_cast<INT *>(&f)) >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
}

FORCEINLINE INT mxMath::ILog2( INT i ) {
	return ILog2( (FLOAT)i );
}

FORCEINLINE INT mxMath::BitsForFloat( FLOAT f ) {
	return ILog2( f ) + 1;
}

FORCEINLINE INT mxMath::BitsForInteger( INT i ) {
	return ILog2( (FLOAT)i ) + 1;
}

FORCEINLINE INT mxMath::MaskForFloatSign( FLOAT f ) {
	return ( (*reinterpret_cast<INT *>(&f)) >> 31 );
}

FORCEINLINE INT mxMath::MaskForIntegerSign( INT i ) {
	return ( i >> 31 );
}

FORCEINLINE INT mxMath::FloorPowerOfTwo( INT x ) {
	return CeilPowerOfTwo( x ) >> 1;
}

// "Next Largest Power of 2
// Given a binary integer value x, the next largest power of 2 can be computed by a SWAR algorithm
// that recursively "folds" the upper bits into the lower bits. This process yields a bit vector with
// the same most significant 1 as x, but all 1's below it. Adding 1 to that value yields the next
// largest power of 2. For a 32-bit value:"
//
FORCEINLINE INT mxMath::CeilPowerOfTwo( INT x ) {
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	return x;
}
/*
FORCEINLINE INT mxMath::CeilPowerOfTwo( mxUInt x ) {
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}
*/

FORCEINLINE bool mxMath::IsPowerOfTwo( INT x ) {
	return ( x & ( x - 1 ) ) == 0 && x > 0;
}

FORCEINLINE bool mxMath::IsPowerOfTwoU( mxUInt x ) {
	return ((x) & ((x) - 1)) == 0;
}

FORCEINLINE INT mxMath::BitCount( INT x ) {
	x -= ( ( x >> 1 ) & 0x55555555 );
	x = ( ( ( x >> 2 ) & 0x33333333 ) + ( x & 0x33333333 ) );
	x = ( ( ( x >> 4 ) + x ) & 0x0f0f0f0f );
	x += ( x >> 8 );
	return ( ( x + ( x >> 16 ) ) & 0x0000003f );
}

FORCEINLINE INT mxMath::BitReverse( INT x ) {
	x = ( ( ( x >> 1 ) & 0x55555555 ) | ( ( x & 0x55555555 ) << 1 ) );
	x = ( ( ( x >> 2 ) & 0x33333333 ) | ( ( x & 0x33333333 ) << 2 ) );
	x = ( ( ( x >> 4 ) & 0x0f0f0f0f ) | ( ( x & 0x0f0f0f0f ) << 4 ) );
	x = ( ( ( x >> 8 ) & 0x00ff00ff ) | ( ( x & 0x00ff00ff ) << 8 ) );
	return ( ( x >> 16 ) | ( x << 16 ) );
}

FORCEINLINE INT mxMath::Abs( INT x ) {
   INT y = x >> 31;
   return ( ( x ^ y ) - y );
}

FORCEINLINE FLOAT mxMath::Fabs( FLOAT f )
{
	INT tmp = *reinterpret_cast<INT *>( &f );
	tmp &= 0x7FFFFFFF;	// Clears the sign bit.
	return *reinterpret_cast<FLOAT *>( &tmp );
}

FORCEINLINE FLOAT mxMath::Floor( FLOAT f ) {
	return ::floorf( f );
}

FORCEINLINE FLOAT mxMath::Ceil( FLOAT f ) {
	return ::ceilf( f );
}

FORCEINLINE FLOAT mxMath::Rint( FLOAT f ) {
	return ::floorf( f + 0.5f );
}

FORCEINLINE FLOAT mxMath::IntToF( INT i ) {
	MX_OPTIMIZE("the conversion is slow, LHS issues, not recommended");
	return (FLOAT) i;
}

FORCEINLINE INT mxMath::Ftoi( FLOAT f ) {
#if 1
	return (INT) f;	// float to int conversion using truncation
#else
	// Fast round ( banker's round ) using Sree Kotay's method.
	f += 6755399441055744.0f;	// The magic number is ( 2^52 * 1.5 ).

	MX_BUG( "is [0] needed here? (It's surely needed for doubles.)" );
	return ((INT*)&f)[0];	// The index must be [1] on big-endian systems.

#endif
}

FORCEINLINE INT mxMath::FtoiFast( FLOAT f ) {
#ifdef _WIN32
	INT i;
	__asm fld		f
	__asm fistp		i		// use default rouding mode (round nearest)
	return i;
#elif 0						// round chop (C/C++ standard)
	INT i, s, e, m, shift;
	i = *reinterpret_cast<INT *>(&f);
	s = i >> IEEE_FLT_SIGN_BIT;
	e = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
	m = ( i & ( ( 1 << IEEE_FLT_MANTISSA_BITS ) - 1 ) ) | ( 1 << IEEE_FLT_MANTISSA_BITS );
	shift = e - IEEE_FLT_MANTISSA_BITS;
	return ( ( ( ( m >> -shift ) | ( m << shift ) ) & ~( e >> 31 ) ) ^ s ) - s;
//#elif defined( __i386__ )
#elif 0
	INT i = 0;
	__asm__ __volatile__ (
						  "fld %1\n" \
						  "fistp %0\n" \
						  : "=m" (i) \
						  : "m" (f) );
	return i;
#else
	return (INT) f;
#endif
}

FORCEINLINE mxULong mxMath::Ftol( FLOAT f ) {
	MX_OPTIMIZE( "implement Sree Kotay's method; look at mxMath::Ftoi()" );
	return (mxULong) f;
}

FORCEINLINE mxULong mxMath::FtolFast( FLOAT f ) {
#ifdef _WIN32
	// FIXME: this overflows on 31bits still .. same as FtoiFast
	mxULong i;
	__asm fld		f
	__asm fistp		i		// use default rouding mode (round nearest)
	return i;
#elif 0						// round chop (C/C++ standard)
	INT i, s, e, m, shift;
	i = *reinterpret_cast<INT *>(&f);
	s = i >> IEEE_FLT_SIGN_BIT;
	e = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
	m = ( i & ( ( 1 << IEEE_FLT_MANTISSA_BITS ) - 1 ) ) | ( 1 << IEEE_FLT_MANTISSA_BITS );
	shift = e - IEEE_FLT_MANTISSA_BITS;
	return ( ( ( ( m >> -shift ) | ( m << shift ) ) & ~( e >> 31 ) ) ^ s ) - s;
//#elif defined( __i386__ )
#elif 0
	// for some reason, on gcc I need to make sure i == 0 before performing a fistp
	INT i = 0;
	__asm__ __volatile__ (
						  "fld %1\n" \
						  "fistp %0\n" \
						  : "=m" (i) \
						  : "m" (f) );
	return i;
#else
	return (mxULong) f;
#endif
}

FORCEINLINE CHAR mxMath::ClampChar( INT i ) {
	if ( i < -128 ) {
		return -128;
	}
	if ( i > 127 ) {
		return 127;
	}
	return i;
}

FORCEINLINE INT16 mxMath::ClampShort( INT i ) {
	if ( i < -32768 ) {
		return -32768;
	}
	if ( i > 32767 ) {
		return 32767;
	}
	return i;
}

FORCEINLINE INT mxMath::ClampInt( INT min, INT max, INT value ) {
	if ( value < min ) {
		return min;
	}
	if ( value > max ) {
		return max;
	}
	return value;
}

FORCEINLINE FLOAT mxMath::ClampFloat( FLOAT min, FLOAT max, FLOAT value ) {
	if ( value < min ) {
		return min;
	}
	if ( value > max ) {
		return max;
	}
	return value;
}

FORCEINLINE FLOAT mxMath::AngleNormalize360( FLOAT angle ) {
	if ( ( angle >= 360.0f ) || ( angle < 0.0f ) ) {
		angle -= ::floor( angle / 360.0f ) * 360.0f;
	}
	return angle;
}

FORCEINLINE FLOAT mxMath::AngleNormalize180( FLOAT angle ) {
	angle = AngleNormalize360( angle );
	if ( angle > 180.0f ) {
		angle -= 360.0f;
	}
	return angle;
}

FORCEINLINE FLOAT mxMath::AngleDelta( FLOAT angle1, FLOAT angle2 ) {
	return AngleNormalize180( angle1 - angle2 );
}

FORCEINLINE INT mxMath::FloatHash( const FLOAT *array, const INT numFloats ) {
	INT i, hash = 0;
	const INT *ptr;

	ptr = reinterpret_cast<const INT *>( array );
	for ( i = 0; i < numFloats; i++ ) {
		hash ^= ptr[i];
	}
	return hash;
}

FORCEINLINE FLOAT mxMath::Reciprocal( FLOAT x ) {
	//Assert( x != 0.0f );
#ifdef MX_USE_SSE_ASM
	__asm
	{
		movss		xmm0, x
		rcpss		xmm0, xmm0
		movss		x, xmm0
	}
	return x;
#else
	return ( 1.0f / x );
#endif
}

