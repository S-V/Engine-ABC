/*
=============================================================================
	File:	Win32_Asm.h
	Desc:	Win32-specific routines coded in assembly.
=============================================================================
*/

#ifndef __MX_WIN32_ASM_H__
#define __MX_WIN32_ASM_H__

#if MX_SSE_VERSION < 1
#	error Minimum SSE 1.0 are required!
#endif

namespace abc {
namespace sys {

//
//	Returns true if the CPUID command is available on the processor.
//
inline bool Is_CPUID_available()
{
	mxUInt32 bitChanged = 0;

    // We have to check if we can toggle the flag register bit 21.
    // If we can't the processor does not support the CPUID command.
    
    __asm
	{
        push	eax
        push	ebx
        pushfd
        pushfd
        pop		eax
        mov		ebx, eax
        xor		eax, 0x00200000 
        push	eax
        popfd
        pushfd
        pop		eax
        popfd
        xor		eax, ebx 
        mov		bitChanged, eax
        pop		ebx
        pop		eax
    }

    return ( bitChanged != 0 );
}


/*
=======================================================================
	
		Math functions.

=======================================================================
*/

//
//  Round - rounds a floating point number to the nearest integer.
//
__forceinline INT Round( FLOAT x )
{
	__asm cvtss2si eax, [x]
}

//
//	Truncate
//
__forceinline INT Truncate( FLOAT x )
{
	__asm cvttss2si eax, [x]
}


//
// Reciprocal - calculates 1 / x
//
__forceinline FLOAT Reciprocal( FLOAT f )
{
	// SSE Newton-Raphson reciprocal estimate, accurate to 23 significant
	// bits of the mantissa
	// One Newtown-Raphson Iteration:
	// f(i+1) = 2 * rcpss(f) - f * rcpss(f) * rcpss(f)
	FLOAT rec;
	__asm rcpss xmm0, f               // xmm0 = rcpss(f)
	__asm movss xmm1, f               // xmm1 = f
	__asm mulss xmm1, xmm0            // xmm1 = f * rcpss(f)
	__asm mulss xmm1, xmm0            // xmm2 = f * rcpss(f) * rcpss(f)
	__asm addss xmm0, xmm0            // xmm0 = 2 * rcpss(f)
	__asm subss xmm0, xmm1            // xmm0 = 2 * rcpss(f)
									  //        - f * rcpss(f) * rcpss(f)
	__asm movss rec, xmm0             // return xmm0
	return rec;
}

//
//	Rsq function calculates 1 / sqrtf ( x )
//
//	@cite Nick nicolas@capens.net
//
__forceinline FLOAT Rsq( FLOAT x )
{
	// SSE reciprocal square root estimate, accurate to 12 significant
	// bits of the mantissa
#if 0
    __asm
	{
       movss	xmm0, x
       rsqrtss	xmm0, xmm0
       movss	x, xmm0
    }
    return x;
#else
	FLOAT result;
	__asm rsqrtss	xmm0, x			// xmm0 = rsqrtss(f)
	__asm movss		result, xmm0	// return xmm0
	return result;
#endif
}

}//End of namespace sys
}//End of namespace abc

#endif // ! __MX_WIN32_ASM_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
