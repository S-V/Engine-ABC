/*
=============================================================================
	File:	TStatic2DArray.h
	Desc:
=============================================================================
*/

#ifndef __MX_CONTAINTERS_STATIC_2D_ARRAY_H__
#define __MX_CONTAINTERS_STATIC_2D_ARRAY_H__

namespace abc {

//
//	TStatic2DArray
//

template< typename T, mxUInt ROW_COUNT, mxUInt COLUMN_COUNT >
class TStatic2DArray {
public:
			TStatic2DArray()
			{}

			~TStatic2DArray()
			{}

	mxUInt GetRowCount()
	{
		return ROW_COUNT;
	}
	mxUInt GetColumnCount()
	{
		return COLUMN_COUNT;
	}

	T & Get( mxUInt row, mxUInt column )
	{	Assert( row < GetRowCount() && column < GetColumnCount() );
		return m_data[ row ][ column ];
	}

	const T & Get( mxUInt row, mxUInt column ) const
	{	Assert( row < GetRowCount() && column < GetColumnCount() );
		return m_data[ row ][ column ];
	}

	void Set( mxUInt row, mxUInt column, const T& newValue )
	{	Assert( row < GetRowCount() && column < GetColumnCount() );
		m_data[ row ][ column ] = newValue;
	}

	void SetZero()
	{
		MemSet( m_data, 0, sizeof( m_data ) );
	}


	T * Ptr()
	{ return m_data; }

	const T * Ptr() const
	{ return m_data; }

private:
	T	m_data[ ROW_COUNT ][ COLUMN_COUNT ];
};

}//end of namespace abc

#endif // ! __MX_CONTAINTERS_STATIC_2D_ARRAY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
