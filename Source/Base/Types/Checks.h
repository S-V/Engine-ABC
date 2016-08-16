/*
=============================================================================
	File:	Checks.h
	Desc:	Sanity checks: compile time assertions on type sizes, etc.
	Note:	This file must be cross-platform.
			This file must know debug macros for compile-time assertions.
=============================================================================
*/

namespace debug {

//
// This code is never called, it just contains compile-time assertions.
//
class ZZChecks
{
	void CheckBasicTypes()
	{
		using namespace ::abc;

		STATIC_CHECK( sizeof( mxInt8 )	== 1 );
		STATIC_CHECK( sizeof( mxUInt8 )	== 1 );
		STATIC_CHECK( sizeof( mxInt16 )	== 2 );
		STATIC_CHECK( sizeof( mxUInt16 ) == 2 );
		STATIC_CHECK( sizeof( mxInt32 )	== 4 );
		STATIC_CHECK( sizeof( mxUInt32 ) == 4 );
		STATIC_CHECK( sizeof( mxInt64 )	== 8 );
		STATIC_CHECK( sizeof( mxUInt64 ) == 8 );

		STATIC_CHECK( sizeof( mxFloat32 )	== 4 );
		STATIC_CHECK( sizeof( mxFloat64 )	== 8 );
		//STATIC_CHECK( sizeof( mxFloat80 )	== 10 );

	}// CheckBasicTypes
};

}//End of namespace debug

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
