/*
=============================================================================
	File:	Utilities.h
	Desc:	Miscellaneous helpers.
=============================================================================
*/

#ifndef __MX_LIBRARY_UTILITIES_H__
#define __MX_LIBRARY_UTILITIES_H__

// Platform-specific utility classes.
//#include <Lib/Utilities/Platform/PlatformUtils.h>

#include <Lib/Utilities/FourCC.h>

//============================================================

namespace abc {
	
//
// 'Drop' each element of the given array
// of pointers to reference-counted objects.
//
template< class T >
void DropArray( const TArray< T* > & rArray )
{
	for ( UINT i = 0; i < rArray.Num(); i++ )
	{
		const ReferenceCounted * pRefCountedObj =
			checked_cast< const ReferenceCounted*, const T* >( rArray[ i ] );
		pRefCountedObj->Drop();
	}
}

}//End of namespace abc

#endif // ! __MX_LIBRARY_UTILITIES_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
