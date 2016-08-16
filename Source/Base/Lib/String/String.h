/*
=============================================================================
	File:	String.h
	Desc:	String class.
			Swiped from Id Software ( idStr ).
=============================================================================
*/

#ifndef __MX_STRING_H__
#define __MX_STRING_H__

namespace abc {

//
//	String
//
class String {
public:

#ifdef MX_USE_STRING_POOL
	MX_DECLARE_MEMORY_CLASS( MX_MEMORY_CLASS_STRING );
#endif

						String( void );
						String( const String & other );
						String( const String & other, INT start, INT end );
						String( const char *text );
						String( const char *text, INT start, INT end );
						explicit String( const bool b );
						explicit String( const char c );
						explicit String( const int i );
						explicit String( const unsigned u );
						explicit String( const float f );
						~String( void );

	size_t				Size( void ) const;
	const char *		c_str( void ) const;
	operator 			const char * ( void ) const;
	operator 			const char * ( void );

	char				operator [] ( INT index ) const;
	char &				operator [] ( INT index );

	void				operator = ( const String & other );
	void				operator = ( const char * text );

	friend String		operator + ( const String &a, const String &b );
	friend String		operator + ( const String &a, const char *b );
	friend String		operator + ( const char *a, const String &b );

	friend String		operator + ( const String &a, const float b );
	friend String		operator + ( const String &a, const INT b );
	friend String		operator + ( const String &a, const unsigned b );
	friend String		operator + ( const String &a, const bool b );
	friend String		operator + ( const String &a, const char b );

	String &			operator += ( const String &a );
	String &			operator += ( const char *a );
	String &			operator += ( const float a );
	String &			operator += ( const char a );
	String &			operator += ( const INT a );
	String &			operator += ( const unsigned a );
	String &			operator += ( const bool a );

						// case sensitive compare
	friend bool			operator == ( const String &a, const String &b );
	friend bool			operator == ( const String &a, const char *b );
	friend bool			operator == ( const char *a, const String &b );

						// case sensitive compare
	friend bool			operator != ( const String &a, const String &b );
	friend bool			operator != ( const String &a, const char *b );
	friend bool			operator != ( const char *a, const String &b );

						// case sensitive compare
	INT					Cmp( const char *text ) const;
	INT					Cmpn( const char *text, INT n ) const;
	INT					CmpPrefix( const char *text ) const;

						// case insensitive compare
	INT					Icmp( const char *text ) const;
	INT					Icmpn( const char *text, INT n ) const;
	INT					IcmpPrefix( const char *text ) const;

						// compares paths and makes sure folders come first
	INT					IcmpPath( const char *text ) const;
	INT					IcmpnPath( const char *text, INT n ) const;
	INT					IcmpPrefixPath( const char *text ) const;

	INT					Length( void ) const;		
	INT					Allocated( void ) const;
	void				Empty( void );
	bool				IsEmpty( void ) const;
	void				Clear( void );
	void				Append( const char a );
	void				Append( const String &text );
	void				Append( const char *text );
	void				Append( const char *text, INT len );
	void				Insert( const char a, INT index );
	void				Insert( const char *text, INT index );
	void				ToLower( void );
	void				ToUpper( void );
	bool				IsNumeric( void ) const;

	bool				HasLower( void ) const;
	bool				HasUpper( void ) const;
	INT					LengthWithoutColors( void ) const;
	void				CapLength( INT );
	void				Fill( const char ch, INT newlen );

	INT					Find( const char c, INT start = 0, INT end = -1 ) const;
	INT					Find( const char *text, bool casesensitive = true, INT start = 0, INT end = -1 ) const;
	bool				Filter( const char *filter, bool casesensitive ) const;
	INT					Last( const char c ) const;						// return the index to the last occurance of 'c', returns -1 if not found
	const char *		Left( INT len, String &result ) const;			// store the leftmost 'len' characters in the result
	const char *		Right( INT len, String &result ) const;			// store the rightmost 'len' characters in the result
	const char *		Mid( INT start, INT len, String &result ) const;	// store 'len' characters starting at 'start' in result
	String				Left( INT len ) const;							// return the leftmost 'len' characters
	String				Right( INT len ) const;							// return the rightmost 'len' characters
	String				Mid( INT start, INT len ) const;				// return 'len' characters starting at 'start'
	void				StripLeading( const char c );					// strip char from front as many times as the char occurs
	void				StripLeading( const char *str );				// strip String from front as many times as the String occurs
	bool				StripLeadingOnce( const char *str );			// strip String from front just once if it occurs
	void				StripTrailing( const char c );					// strip char from end as many times as the char occurs
	void				StripTrailing( const char *str );				// strip String from end as many times as the String occurs
	bool				StripTrailingOnce( const char *str );			// strip String from end just once if it occurs
	void				Strip( const char c );							// strip char from front and end as many times as the char occurs
	void				Strip( const char *str );						// strip String from front and end as many times as the String occurs
	void				StripTrailingWhitespace( void );				// strip trailing white space characters
	String &			StripQuotes( void );							// strip quotes around String
	void				Replace( const char *old, const char *nw );
	const char *		ToChar() const;

	enum { FILE_HASH_SIZE =	1024 };

	// file name methods
	INT					FileNameHash( void ) const;						// hash key for the filename (skips extension)
	String &			BackSlashesToSlashes( void );					// convert slashes
	String &			SetFileExtension( const char *extension );		// set the given file extension
	String &			StripFileExtension( void );						// remove any file extension
	String &			StripAbsoluteFileExtension( void );				// remove any file extension looking from front (useful if there are multiple .'s)
	String &			DefaultFileExtension( const char *extension );	// if there's no file extension use the default
	String &			DefaultPath( const char *basepath );			// if there's no path use the default
	void				AppendPath( const char *text );					// append a partial path
	String &			StripFilename( void );							// remove the filename from a path
	String &			StripPath( void );								// remove the path from the filename
	void				ExtractFilePath( String &dest ) const;			// copy the file path to another String
	void				ExtractFileName( String &dest ) const;			// copy the filename to another String
	void				ExtractFileBase( String &dest ) const;			// copy the filename minus the extension to another String
	void				ExtractFileExtension( String &dest ) const;		// copy the file extension to another String
	bool				CheckExtension( const char *ext );

	// char * methods to replace library functions
	static INT			Length( const char *s );
	static char *		ToLower( char *s );
	static char *		ToUpper( char *s );
	static bool			IsNumeric( const char *s );
	static bool			HasLower( const char *s );
	static bool			HasUpper( const char *s );
	static INT			Cmp( const char *s1, const char *s2 );
	static INT			Cmpn( const char *s1, const char *s2, INT n );
	static INT			Icmp( const char *s1, const char *s2 );
	static INT			Icmpn( const char *s1, const char *s2, INT n );
	static INT			IcmpNoColor( const char *s1, const char *s2 );
	static INT			IcmpPath( const char *s1, const char *s2 );			// compares paths and makes sure folders come first
	static INT			IcmpnPath( const char *s1, const char *s2, INT n );	// compares paths and makes sure folders come first
	static void			Append( char *dest, INT size, const char *src );
	static void			Copynz( char *dest, const char *src, INT destsize );
	static INT			snPrintf( char *dest, INT size, const char *fmt, ... );
	static INT			vsnPrintf( char *dest, INT size, const char *fmt, va_list argptr );
	static INT			FindChar( const char *str, const char c, INT start = 0, INT end = -1 );
	static INT			FindText( const char *str, const char *text, bool casesensitive = true, INT start = 0, INT end = -1 );
	static bool			Filter( const char *filter, const char *name, bool casesensitive );
	static void			StripMediaName( const char *name, String &mediaName );
	static bool			CheckExtension( const char *name, const char *ext );
	static const char *	FloatArrayToString( const float *array, const INT length, const INT precision );

	// hash keys
	static INT			Hash( const char *str );
	static INT			Hash( const char *str, INT length );
	static INT			IHash( const char *str );					// case insensitive
	static INT			IHash( const char *str, INT length );		// case insensitive

	// character methods
	static char			ToLower( char c );
	static char			ToUpper( char c );
	static bool			CharIsPrintable( INT c );
	static bool			CharIsLower( INT c );
	static bool			CharIsUpper( INT c );
	static bool			CharIsAlpha( INT c );
	static bool			CharIsNumeric( INT c );
	static bool			CharIsNewLine( char c );
	static bool			CharIsTab( char c );
	static INT			ColorIndex( INT c );

	friend INT			_sprintf( String &dest, const char *fmt, ... );
	friend INT			_vsprintf( String &dest, const char *fmt, va_list ap );

	void				ReAllocate( INT amount, bool keepold );				// reallocate String data buffer
	void				FreeData( void );									// free allocated String memory

	static void			InitMemory( void );
	static void			ShutdownMemory( void );
	static void			PurgeMemory( void );

	INT					DynamicMemoryUsed() const;

	char *				GetBufferPtr();

protected:
	// ensure string data buffer is large anough
	void			EnsureAlloced( INT amount, bool keepold = true );

private:
	enum {
		STR_ALLOC_BASE = 20,
		STR_ALLOC_GRANULARITY = 32
	};

	INT				len;
	char *			data;
	INT				alloced;
	char			baseBuffer[ STR_ALLOC_BASE ];

private:
	void	Init( void );	// initialize string using base buffer
};

FORCEINLINE void String::EnsureAlloced( INT amount, bool keepold ) {
	if ( amount > alloced ) {
		ReAllocate( amount, keepold );
	}
}

FORCEINLINE void String::Init( void ) {
	len = 0;
	alloced = STR_ALLOC_BASE;
	data = baseBuffer;
	data[ 0 ] = '\0';
}

FORCEINLINE String::String( void ) {
	Init();
}

FORCEINLINE String::String( const String &text ) {
	INT l;

	Init();
	l = text.Length();
	EnsureAlloced( l + 1 );
	strcpy( data, text.data );
	len = l;
}

FORCEINLINE String::String( const String &text, INT start, INT end ) {
	INT i;
	INT l;

	Init();
	if ( end > text.Length() ) {
		end = text.Length();
	}
	if ( start > text.Length() ) {
		start = text.Length();
	} else if ( start < 0 ) {
		start = 0;
	}

	l = end - start;
	if ( l < 0 ) {
		l = 0;
	}

	EnsureAlloced( l + 1 );

	for ( i = 0; i < l; i++ ) {
		data[ i ] = text[ start + i ];
	}

	data[ l ] = '\0';
	len = l;
}

FORCEINLINE String::String( const char *text ) {
	INT l;

	Init();
	if ( text ) {
		l = strlen( text );
		EnsureAlloced( l + 1 );
		strcpy( data, text );
		len = l;
	}
}

FORCEINLINE String::String( const char *text, INT start, INT end ) {
	INT i;
	INT l = strlen( text );

	Init();
	if ( end > l ) {
		end = l;
	}
	if ( start > l ) {
		start = l;
	} else if ( start < 0 ) {
		start = 0;
	}

	l = end - start;
	if ( l < 0 ) {
		l = 0;
	}

	EnsureAlloced( l + 1 );

	for ( i = 0; i < l; i++ ) {
		data[ i ] = text[ start + i ];
	}

	data[ l ] = '\0';
	len = l;
}

FORCEINLINE String::String( const bool b ) {
	Init();
	EnsureAlloced( 2 );
	data[ 0 ] = b ? '1' : '0';
	data[ 1 ] = '\0';
	len = 1;
}

FORCEINLINE String::String( const char c ) {
	Init();
	EnsureAlloced( 2 );
	data[ 0 ] = c;
	data[ 1 ] = '\0';
	len = 1;
}

FORCEINLINE String::String( const int i ) {
	char text[ 64 ];
	INT l;

	Init();
	l = sprintf( text, "%d", i );
	EnsureAlloced( l + 1 );
	strcpy( data, text );
	len = l;
}

FORCEINLINE String::String( const unsigned u ) {
	char text[ 64 ];
	INT l;

	Init();
	l = sprintf( text, "%u", u );
	EnsureAlloced( l + 1 );
	strcpy( data, text );
	len = l;
}

FORCEINLINE String::String( const float f ) {
	char text[ 64 ];
	INT l;

	Init();
	l = String::snPrintf( text, sizeof( text ), "%f", f );
	while( l > 0 && text[l-1] == '0' ) text[--l] = '\0';
	while( l > 0 && text[l-1] == '.' ) text[--l] = '\0';
	EnsureAlloced( l + 1 );
	strcpy( data, text );
	len = l;
}

FORCEINLINE String::~String( void ) {
	FreeData();
}

FORCEINLINE size_t String::Size( void ) const {
	return sizeof( *this ) + Allocated();
}

FORCEINLINE const char *String::c_str( void ) const {
	return data;
}

FORCEINLINE String::operator  const char * ( void ) {
	return c_str();
}

FORCEINLINE String::operator  const char * ( void ) const {
	return c_str();
}

FORCEINLINE char String::operator [] ( INT index ) const {
	Assert( ( index >= 0 ) && ( index <= len ) );
	return data[ index ];
}

FORCEINLINE char &String::operator [] ( INT index ) {
	Assert( ( index >= 0 ) && ( index <= len ) );
	return data[ index ];
}

FORCEINLINE void String::operator = ( const String &text ) {
	INT l;

	l = text.Length();
	EnsureAlloced( l + 1, false );
	memcpy( data, text.data, l );
	data[l] = '\0';
	len = l;
}

FORCEINLINE String operator + ( const String &a, const String &b ) {
	String result( a );
	result.Append( b );
	return result;
}

FORCEINLINE String operator + ( const String &a, const char *b ) {
	String result( a );
	result.Append( b );
	return result;
}

inline String operator + ( const char *a, const String &b ) {
	String result( a );
	result.Append( b );
	return result;
}

FORCEINLINE String operator + ( const String &a, const bool b ) {
	String result( a );
	result.Append( b ? "true" : "false" );
	return result;
}

FORCEINLINE String operator + ( const String &a, const char b ) {
	String result( a );
	result.Append( b );
	return result;
}

FORCEINLINE String operator + ( const String &a, const float b ) {
	char	text[ 64 ];
	String	result( a );

	sprintf( text, "%f", b );
	result.Append( text );

	return result;
}

FORCEINLINE String operator + ( const String &a, const INT b ) {
	char	text[ 64 ];
	String	result( a );

	sprintf( text, "%d", b );
	result.Append( text );

	return result;
}

FORCEINLINE String operator + ( const String &a, const unsigned b ) {
	char	text[ 64 ];
	String	result( a );

	sprintf( text, "%u", b );
	result.Append( text );

	return result;
}

FORCEINLINE String &String::operator += ( const float a ) {
	char text[ 64 ];

	sprintf( text, "%f", a );
	Append( text );

	return *this;
}

FORCEINLINE String &String::operator += ( const INT a ) {
	char text[ 64 ];

	sprintf( text, "%d", a );
	Append( text );

	return *this;
}

FORCEINLINE String &String::operator += ( const unsigned a ) {
	char text[ 64 ];

	sprintf_s( text, 64, "%u", a );
	Append( text );

	return *this;
}

FORCEINLINE String &String::operator += ( const String &a ) {
	Append( a );
	return *this;
}

FORCEINLINE String &String::operator += ( const char *a ) {
	Append( a );
	return *this;
}

FORCEINLINE String &String::operator += ( const char a ) {
	Append( a );
	return *this;
}

FORCEINLINE String &String::operator += ( const bool a ) {
	Append( a ? "true" : "false" );
	return *this;
}

FORCEINLINE bool operator == ( const String &a, const String &b ) {
	return ( !String::Cmp( a.data, b.data ) );
}

FORCEINLINE bool operator == ( const String &a, const char *b ) {
	Assert( b );
	return ( !String::Cmp( a.data, b ) );
}

FORCEINLINE bool operator == ( const char *a, const String &b ) {
	Assert( a );
	return ( !String::Cmp( a, b.data ) );
}

FORCEINLINE bool operator != ( const String &a, const String &b ) {
	return !( a == b );
}

FORCEINLINE bool operator != ( const String &a, const char *b ) {
	return !( a == b );
}

FORCEINLINE bool operator != ( const char *a, const String &b ) {
	return !( a == b );
}

FORCEINLINE INT String::Cmp( const char *text ) const {
	Assert( text );
	return String::Cmp( data, text );
}

FORCEINLINE INT String::Cmpn( const char *text, INT n ) const {
	Assert( text );
	return String::Cmpn( data, text, n );
}

FORCEINLINE INT String::CmpPrefix( const char *text ) const {
	Assert( text );
	return String::Cmpn( data, text, strlen( text ) );
}

FORCEINLINE INT String::Icmp( const char *text ) const {
	Assert( text );
	return String::Icmp( data, text );
}

FORCEINLINE INT String::Icmpn( const char *text, INT n ) const {
	Assert( text );
	return String::Icmpn( data, text, n );
}

FORCEINLINE INT String::IcmpPrefix( const char *text ) const {
	Assert( text );
	return String::Icmpn( data, text, strlen( text ) );
}

FORCEINLINE INT String::Length( void ) const {
	return len;
}

FORCEINLINE INT String::Allocated( void ) const {
	if ( data != baseBuffer ) {
		return alloced;
	} else {
		return 0;
	}
}

FORCEINLINE void String::Empty( void ) {
	EnsureAlloced( 1 );
	data[ 0 ] = '\0';
	len = 0;
}

FORCEINLINE bool String::IsEmpty( void ) const {
	return ( String::Cmp( data, "" ) == 0 );
}

FORCEINLINE void String::Clear( void ) {
	FreeData();
	Init();
}

FORCEINLINE void String::Append( const char a ) {
	EnsureAlloced( len + 2 );
	data[ len ] = a;
	len++;
	data[ len ] = '\0';
}

FORCEINLINE void String::Append( const String &text ) {
	INT newLen;
	INT i;

	newLen = len + text.Length();
	EnsureAlloced( newLen + 1 );
	for ( i = 0; i < text.len; i++ ) {
		data[ len + i ] = text[ i ];
	}
	len = newLen;
	data[ len ] = '\0';
}

FORCEINLINE void String::Append( const char *text ) {
	INT newLen;
	INT i;

	if ( text ) {
		newLen = len + strlen( text );
		EnsureAlloced( newLen + 1 );
		for ( i = 0; text[ i ]; i++ ) {
			data[ len + i ] = text[ i ];
		}
		len = newLen;
		data[ len ] = '\0';
	}
}

FORCEINLINE void String::Append( const char *text, INT l ) {
	INT newLen;
	INT i;

	if ( text && l ) {
		newLen = len + l;
		EnsureAlloced( newLen + 1 );
		for ( i = 0; text[ i ] && i < l; i++ ) {
			data[ len + i ] = text[ i ];
		}
		len = newLen;
		data[ len ] = '\0';
	}
}

FORCEINLINE void String::Insert( const char a, INT index ) {
	INT i, l;

	if ( index < 0 ) {
		index = 0;
	} else if ( index > len ) {
		index = len;
	}

	l = 1;
	EnsureAlloced( len + l + 1 );
	for ( i = len; i >= index; i-- ) {
		data[i+l] = data[i];
	}
	data[index] = a;
	len++;
}

FORCEINLINE void String::Insert( const char *text, INT index ) {
	INT i, l;

	if ( index < 0 ) {
		index = 0;
	} else if ( index > len ) {
		index = len;
	}

	l = strlen( text );
	EnsureAlloced( len + l + 1 );
	for ( i = len; i >= index; i-- ) {
		data[i+l] = data[i];
	}
	for ( i = 0; i < l; i++ ) {
		data[index+i] = text[i];
	}
	len += l;
}

FORCEINLINE void String::ToLower( void ) {
	for (INT i = 0; data[i]; i++ ) {
		// HUMANHEAD pdm: cast to unsigned for the sake of western european characters, which use the sign bit
		if ( CharIsUpper( (unsigned char)data[i] ) ) {
			data[i] += ( 'a' - 'A' );
		}
	}
}

FORCEINLINE void String::ToUpper( void ) {
	for (INT i = 0; data[i]; i++ ) {
		// HUMANHEAD pdm: cast to unsigned for the sake of western european characters, which use the sign bit
		if ( CharIsLower( (unsigned char)data[i] ) ) {
			data[i] -= ( 'a' - 'A' );
		}
	}
}

FORCEINLINE bool String::IsNumeric( void ) const {
	return String::IsNumeric( data );
}

FORCEINLINE bool String::HasLower( void ) const {
	return String::HasLower( data );
}

FORCEINLINE bool String::HasUpper( void ) const {
	return String::HasUpper( data );
}

FORCEINLINE void String::CapLength( INT newlen ) {
	if ( len <= newlen ) {
		return;
	}
	data[ newlen ] = 0;
	len = newlen;
}

FORCEINLINE void String::Fill( const char ch, INT newlen ) {
	EnsureAlloced( newlen + 1 );
	len = newlen;
	memset( data, ch, len );
	data[ len ] = 0;
}

FORCEINLINE INT String::Find( const char c, INT start, INT end ) const {
	if ( end == -1 ) {
		end = len;
	}
	return String::FindChar( data, c, start, end );
}

FORCEINLINE INT String::Find( const char *text, bool casesensitive, INT start, INT end ) const {
	if ( end == -1 ) {
		end = len;
	}
	return String::FindText( data, text, casesensitive, start, end );
}

FORCEINLINE bool String::Filter( const char *filter, bool casesensitive ) const {
	return String::Filter( filter, data, casesensitive );
}

FORCEINLINE const char *String::Left( INT len, String &result ) const {
	return Mid( 0, len, result );
}

FORCEINLINE const char *String::Right( INT len, String &result ) const {
	if ( len >= Length() ) {
		result = *this;
		return result;
	}
	return Mid( Length() - len, len, result );
}

FORCEINLINE String String::Left( INT len ) const {
	return Mid( 0, len );
}

inline String String::Right( INT len ) const {
	if ( len >= Length() ) {
		return *this;
	}
	return Mid( Length() - len, len );
}

FORCEINLINE void String::Strip( const char c ) {
	StripLeading( c );
	StripTrailing( c );
}

FORCEINLINE void String::Strip( const char *str ) {
	StripLeading( str );
	StripTrailing( str );
}

FORCEINLINE bool String::CheckExtension( const char *ext ) {
	return String::CheckExtension( data, ext );
}

FORCEINLINE INT String::Length( const char *s ) {
	INT i;
	for ( i = 0; s[i]; i++ ) {}
	return i;
}

FORCEINLINE char *String::ToLower( char *s ) {
	for ( INT i = 0; s[i]; i++ ) {
		// HUMANHEAD pdm: cast to unsigned for the sake of western european characters, which use the sign bit
		if ( CharIsUpper( (unsigned char)s[i] ) ) {
			s[i] += ( 'a' - 'A' );
		}
	}
	return s;
}

FORCEINLINE char *String::ToUpper( char *s ) {
	for ( INT i = 0; s[i]; i++ ) {
		// HUMANHEAD pdm: cast to unsigned for the sake of western european characters, which use the sign bit
		if ( CharIsLower( (unsigned char)s[i] ) ) {
			s[i] -= ( 'a' - 'A' );
		}
	}
	return s;
}

FORCEINLINE INT String::Hash( const char *str ) {
	INT i, hash = 0;
	for ( i = 0; *str != '\0'; i++ ) {
		hash += ( *str++ ) * ( i + 119 );
	}
	return hash;
}

FORCEINLINE INT String::Hash( const char *str, INT length ) {
	INT i, hash = 0;
	for ( i = 0; i < length; i++ ) {
		hash += ( *str++ ) * ( i + 119 );
	}
	return hash;
}

FORCEINLINE INT String::IHash( const char *str ) {
	INT i, hash = 0;
	for( i = 0; *str != '\0'; i++ ) {
		hash += ToLower( *str++ ) * ( i + 119 );
	}
	return hash;
}

FORCEINLINE INT String::IHash( const char *str, INT length ) {
	INT i, hash = 0;
	for ( i = 0; i < length; i++ ) {
		hash += ToLower( *str++ ) * ( i + 119 );
	}
	return hash;
}

FORCEINLINE char String::ToLower( char c ) {
	if ( c <= 'Z' && c >= 'A' ) {
		return ( c + ( 'a' - 'A' ) );
	}
	return c;
}

FORCEINLINE char String::ToUpper( char c ) {
	if ( c >= 'a' && c <= 'z' ) {
		return ( c - ( 'a' - 'A' ) );
	}
	return c;
}

FORCEINLINE bool String::CharIsPrintable( INT c ) {
	// test for regular ascii and western European high-ascii chars
	return ( c >= 0x20 && c <= 0x7E ) || ( c >= 0xA1 && c <= 0xFF );
}

FORCEINLINE bool String::CharIsLower( INT c ) {
	// test for regular ascii and western European high-ascii chars
	return ( c >= 'a' && c <= 'z' ) || ( c >= 0xE0 && c <= 0xFF );
}

FORCEINLINE bool String::CharIsUpper( INT c ) {
	// test for regular ascii and western European high-ascii chars
	return ( c <= 'Z' && c >= 'A' ) || ( c >= 0xC0 && c <= 0xDF );
}

FORCEINLINE bool String::CharIsAlpha( INT c ) {
	// test for regular ascii and western European high-ascii chars
	return ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ||
			 ( c >= 0xC0 && c <= 0xFF ) );
}

FORCEINLINE bool String::CharIsNumeric( INT c ) {
	return ( c <= '9' && c >= '0' );
}

FORCEINLINE bool String::CharIsNewLine( char c ) {
	return ( c == '\n' || c == '\r' || c == '\v' );
}

FORCEINLINE bool String::CharIsTab( char c ) {
	return ( c == '\t' );
}

FORCEINLINE INT String::ColorIndex( INT c ) {
	return ( c & 15 );
}

FORCEINLINE INT String::DynamicMemoryUsed() const {
	return ( data == baseBuffer ) ? 0 : alloced;
}

FORCEINLINE const char* String::ToChar() const {
	return data;
}

FORCEINLINE char * String::GetBufferPtr() {
	return data;
}

}// end namespace abc

#endif // !__MX_STRING_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
