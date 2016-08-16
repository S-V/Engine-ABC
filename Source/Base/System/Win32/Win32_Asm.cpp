/*
=============================================================================
	File:	Win32_Asm.cpp
	Desc:	Win32-specific routines coded in assembly.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
//#include <Base.h>

namespace abc {
namespace sys {

//
// Michael Herf's fast memcpy
//
void memcpyMMX( void* dst, const void* src, int nbytes )
{
    int remainingBytes = nbytes;

    if ( nbytes > 64 )
	{
		_asm
		{
			mov		 esi, src 
			mov		 edi, dst 
			mov		 ecx, nbytes 
			shr		 ecx, 6 // 64 bytes per iteration 

loop1: 
			movq	 mm1,  0[ESI] // Read in source data 
			movq	 mm2,  8[ESI]
			movq	 mm3, 16[ESI]
			movq	 mm4, 24[ESI] 
			movq	 mm5, 32[ESI]
			movq	 mm6, 40[ESI]
			movq	 mm7, 48[ESI]
			movq	 mm0, 56[ESI]

			movntq  0[EDI], mm1 // Non-temporal stores 
			movntq  8[EDI], mm2 
			movntq 16[EDI], mm3 
			movntq 24[EDI], mm4 
			movntq 32[EDI], mm5 
			movntq 40[EDI], mm6 
			movntq 48[EDI], mm7 
			movntq 56[EDI], mm0 

			add		 esi, 64 
			add		 edi, 64 
			dec		 ecx 
			jnz		 loop1 

			emms
		}
		remainingBytes -= ( ( nbytes >> 6 ) << 6 ); 
	}

    if ( remainingBytes > 0 )
	{
        // Memcpy the rest
		::memcpy((UINT8*)dst + (nbytes - remainingBytes),
               (const UINT8*)src + (nbytes - remainingBytes), remainingBytes); 
    }
}

}//End of namespace sys
}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
