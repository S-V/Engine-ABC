/*
=============================================================================
	File:	HashFunctions.h
	Desc:	Useful hash functions.
=============================================================================
*/

#ifndef __MX_HASH_FUNCTIONS_H__
#define __MX_HASH_FUNCTIONS_H__

namespace abc {

//
//	Utilities.
//
INLINE char ToLower( char c ) {
	if ( c <= 'Z' && c >= 'A' ) {
		return ( c + ( 'a' - 'A' ) );
	}
	return c;
}

INLINE char ToUpper( char c ) {
	if ( c >= 'a' && c <= 'z' ) {
		return ( c - ( 'a' - 'A' ) );
	}
	return c;
}

//
//	GetHash
//
INLINE INT GetHash( const char *str ) {
	INT hash = 0;
	for ( INT i = 0; *str != '\0'; i++ ) {
		hash += ( *str++ ) * ( i + 119 );
	}
	return hash;
}

INLINE mxUInt GetHash( mxUInt key ) {
	return mxUInt( ( key >> ( 8 * sizeof( mxUInt ) - 1 ) ) ^ key );
	//return mxUInt( key );
}

//
//	NameHash
//

enum ENameHashSettings
{
	DEFINEHASHSIZE = 2048,
	FILE_HASH_SIZE = 1024,
};

INLINE INT NameHash( const char *name ) {
	INT hash, i;

	hash = 0;
	for ( i = 0; name[i]; i++ ) {
		hash += name[i] * (119 + i);
	}
	hash = (hash ^ (hash >> 10) ^ (hash >> 20)) & (DEFINEHASHSIZE - 1);
	return hash;
}

INLINE INT FileNameHash( const char *name ) {
	char letter;
	long hash = 0;
	INT i = 0;
	while( name[i] != '\0' ) 
	{
		letter = ToLower( name[i] );
		if ( letter == '.' ) {
			break;				// don't include extension
		}
		if ( letter =='\\' ) {
			letter = '/';
		}

		hash += (long)( letter ) * ( i + 119 );
		i++;
	}
	hash &= (FILE_HASH_SIZE-1);
	return hash;
}

//
//	HashPjw
//
INLINE INT HashPjw( const char *s )
{
	enum { PRIME = 211 };

	unsigned h = 0, g;
	for ( const char *p = s; *p; p++ ) {
		if ( g = (h = (h << 4) + *p) & 0xF0000000 ) {
			h ^= g >> 24 ^ g;
		}
	}
	return h % PRIME;
}

// Thomas Wang's hash
INLINE INT Hash32Bits_1( INT key )
{
	key += ~(key << 15);
	key ^=  (key >> 10);
	key +=  (key << 3);
	key ^=  (key >> 6);
	key += ~(key << 11);
	key ^=  (key >> 16);
	return key;
}

//
//	Hash traits.
//
//	Note: this code comes from G3D.
//

// Must be specialized for custom types.
//
template < typename T >
struct HashTrait
{};

template < typename T >
struct HashTrait< T* >
{
	static size_t	HashCode( const void* k )
	{
		return reinterpret_cast< size_t >( k );
	}
};

template <>
struct HashTrait< INT >
{
	static size_t	HashCode( INT k )
	{
		return static_cast< size_t >( k );
	}
};

template <>
struct HashTrait< mxUInt >
{
	static size_t	HashCode( mxUInt k )
	{
		return static_cast< size_t >( k );
	}
};

template <>
struct HashTrait< UINT64 >
{
	static size_t	HashCode( UINT64 k )
	{
		return static_cast< size_t >( k );
	}
};

template <>
struct HashTrait< const char* >
{
	static size_t	HashCode( const char* k )
	{
		return static_cast<size_t>( NameHash(k) );
	}
};

}//end namespace abc

#endif // ! __MX_HASH_FUNCTIONS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
