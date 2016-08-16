/*
=============================================================================
	File:	TMap.h
	Desc:	UNDONE
=============================================================================
*/

#ifndef __MX_CONTAINTERS_MAP_H__
#define __MX_CONTAINTERS_MAP_H__

namespace abc {

//
//	DefaultHashProvider
//
class DefaultHashProvider {
public:
	static FORCEINLINE mxUInt GetHash( INT key )
	{
		return key;
	}

	static FORCEINLINE mxUInt GetHash( const char* key )
	{
		// hashpjw
		enum { PRIME = 211 };

		mxUInt h = 0, g;
		for ( const char *p = key; *p; p++ ) {
			if ( g = (h = (h << 4) + *p) & 0xF0000000 ) {
				h ^= g >> 24 ^ g;
			}
		}
		return h % PRIME;
	}
};

//
//	TMap
//
//	Maps unique keys to values.
//	TODO: fix bugs!
//
template< typename TKey, typename TValue, class HashProvider = DefaultHashProvider >
class TMap {
public:
	void Set( TKey key, TValue value )
	{
		mxUInt  hash = HashProvider::GetHash( key ) & m_tableSizeMask;

		HashNode  **nextPtr = NULL, *node = NULL;

		for ( nextPtr = &(m_pHeads[hash]), node = *nextPtr; node != NULL; nextPtr = &(node->next), node = *nextPtr )
		{
			if ( node->key == key ) {
				node->value = value;
				return;
			}
		}

		++m_numEntries;

		*nextPtr = new HashNode( key, value, m_pHeads[ hash ] );
		(*nextPtr)->next = node;
	}

	// Returns NULL if failed to find.
	TValue * Find( const TKey& key )
	{
		mxUInt  hash = HashProvider::GetHash( key ) & m_tableSizeMask;

		for ( HashNode* node = m_pHeads[hash]; node != NULL; node = node->next )
		{
			if ( node->key == key ) {
				return & node->value;
			}
		}
		return NULL;
	}

	TValue * const Find( const TKey& key ) const
	{
		mxUInt  hash = HashProvider::GetHash( key ) % m_pHeads.Num();

		for ( HashNode* node = m_pHeads[hash]; node != NULL; node = node->next )
		{
			if ( node->key == key ) {
				return & node->value;
			}
		}
		return NULL;
	}

	TMap( mxUInt tablesize = 256 )
	{
		Assert( tablesize > 0 && IsPowerOfTwo( tablesize ) );

		m_tableSize = tablesize;
		m_pHeads = new HashNode* [ m_tableSize ];
		MemSet( m_pHeads, NULL, sizeof( *m_pHeads ) * m_tableSize );

		m_numEntries = 0;
		m_tableSizeMask = m_tableSize - 1;
	}

	~TMap()
	{
		Clear();
		delete [] m_pHeads;
	}

	void Clear()
	{
		for ( mxUInt i = 0; i < m_tableSize; i++ )
		{
			HashNode * next = m_pHeads[ i ];
			while ( next != NULL ) {
				HashNode * node = next;
				next = next->next;
				delete node;
			}
			m_pHeads[ i ] = NULL;
		}
		m_numEntries = 0;
	}

private:
	struct HashNode
	{
		TKey		key;
		TValue		value;
		HashNode *	next;

		HashNode()
		{}

		HashNode( TKey inKey, TValue inValue, HashNode* pNext )
			: key( inKey ), value( inValue )
			, next( pNext )
		{}
	};
	HashNode **				m_pHeads;
	mxUInt					m_tableSize;
	mxUInt					m_numEntries;
	INT						m_tableSizeMask;
};

}//End of namespace abc

#endif // ! __MX_CONTAINTERS_MAP_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
