/*
=============================================================================
	File:	QSort.h
	Desc:	Quick sort.
=============================================================================
*/

#ifndef __MX_QSORT_H__
#define __MX_QSORT_H__

namespace abc {

typedef int (CDECL* QSortComparisonFunction)( const void* A, const void* B );

void QSort( void* pBase, INT numOfElements, INT sizeOfElements, QSortComparisonFunction pCompareFunc );

#ifdef MX_PLATFORM_WIN_32
inline void QSort( void* pBase, INT count, INT width, QSortComparisonFunction pCompareFunc );
{
	::qsort( pBase, count, width, pCompareFunc );
}
#endif

}//End of namespace abc

#endif // ! __MX_QSORT_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
