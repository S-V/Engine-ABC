/*
=============================================================================
	File:	Complex.cpp
	Desc:
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
//#include <Base.h>

namespace abc {

//
//	Static members.
//
const mxComplex mxComplex::complex_origin( 0.0f, 0.0f );

/*
=============
mxComplex::ToString
=============
*/
const char *mxComplex::ToString( INT precision ) const {
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
