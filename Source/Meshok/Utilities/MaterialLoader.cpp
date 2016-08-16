/*
=============================================================================
	File:	MaterialLoader.cpp
	Desc:	Material loader.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <MiniSG.h>

namespace abc {

//
// Enum of all possible token types which can be encountered during parsing.
//
enum TOKEN_TYPE
{
	T_END_OF_FILE	= 0,

	/*
	-----------------------------------------
		Literals
	-----------------------------------------
	*/

	// numeric literals
	L_INTEGER,	L_FLOAT,

	// char constants
	L_CHAR,

	// leaf operators
	L_IDENTIFIER,
	L_STRING,	// string constant (enclosed in double quotes)

	/*
	-----------------------------------------
		Reserved keywords
	-----------------------------------------
	*/
	T_MATERIAL,

	T_DIFFUSE_MAP,
	T_DETAIL_MAP,
	T_NORMAL_MAP,
	T_GLOSS_MAP,

	T_TECHNIQUE,
	T_PASS,

	T_LIGHTING,
	T_NORMALIZE_NORMALS,

	T_SHADE_MODE,
	T_FILL_MODE,
	T_CULL_MODE,

	T_AMBIENT_COLOR,
	T_DIFFUSE_COLOR,
	T_EMISSIVE_COLOR,
	T_SPECULAR_COLOR,
	T_SPECULAR_POWER,
	T_ENABLE_SPECULAR,

	T_TEXTURE_UNIT,
	T_TEXTURE,
	T_TEXTURE_COORD_SET,
	T_TEXTURE_FILTERING,
	T_MIN_FILTER,
	T_MAG_FILTER,
	T_MIP_FILTER,

	//-----------------------------------------------------------------
	//	Texture filtering.
	//-----------------------------------------------------------------
	T_NONE,
	T_POINT,
	T_LINEAR,
	T_BILINEAR,
	T_TRILINEAR,
	T_ANISOTROPIC,

	//-----------------------------------------------------------------
	//	Texture blending operations.
	//-----------------------------------------------------------------

	T_BLENDOP_DISABLE,
	
	T_BLENDOP_SELECTARG1,
	T_BLENDOP_SELECTARG2,
	
	T_BLENDOP_MODULATE,
	T_BLENDOP_MODULATE2X,
	T_BLENDOP_MODULATE4X,
	
	T_BLENDOP_ADD,
	T_BLENDOP_ADDSIGNED,
	T_BLENDOP_ADDSIGNED2X,

    T_BLENDOP_SUBTRACT,

	//-----------------------------------------------------------------
	//	Texture blending value sources.
	//-----------------------------------------------------------------

	T_CURRENT,
//	T_TEXTURE is already used
//	T_DIFFUSE,
//	T_SPECULAR,

	T_COLOR_OP,
	T_COLOR_ARG1,
	T_COLOR_ARG2,

	T_DEPTH_TEST_ENABLE,
	T_DEPTH_BUFFER_WRITE_ENABLE,


	T_TRUE,
	T_FALSE,

	//-----------------------------------------------------------------
	//	Miscellaneous.
	//-----------------------------------------------------------------

	T_SCALE,

	/*
	-----------------------------------------
		Preprocessor directives
	-----------------------------------------
	*/
	PP_DEFINE,
	PP_INCLUDE,
	PP_IF,
	PP_IFDEF,
	PP_IFNDEF,
	PP_ELIF,
	PP_ELSE,
	PP_ENDIF,
	PP_UNDEF,

	/*
	-----------------------------------------
		Punctuation
	-----------------------------------------
	*/
	P_AMPERSAND,					//	"&" 
	P_AMPERSAND_AMPERSAND,			//	"&&" 
	P_AMPERSAND_EQUAL,				//	"&=" 
	P_ASTERISK,						//	"*" 
	P_ASTERISK_EQUAL,				//	"*=" 
	P_AT,							//	"@" 
	P_AT_EQUAL,						//	"@=" 
	P_BACKSLASH,					//	"\=" 
	P_CARET,						//	"^" 
	P_CARET_EQUAL,					//	"^=" 
	P_COLON,						//	":" 
	P_COLON_COLON,					//	"::" 
	P_COMMA,						//	"," 
	P_DOLLAR,						//  "$"
	P_DOT,							//	"." 
	P_DOT_DOT_DOT,					//	"..." 
	P_EQUAL,						//	"=" 
	P_EQUAL_EQUAL,					//	"==" 
	P_EXCLAMATION,					//	"!" 
	P_GREATER,						//	">" 
	P_GREATER_EQUAL,				//	">=" 
	P_GREATER_GREATER,				//	">>" 
	P_GREATER_GREATER_EQUAL,		//	">>=" 
	P_LEFT_BRACE,					//	"{" 
	P_LEFT_BRACKET,					//	"[" 
	P_LEFT_PARENTH,					//	"(" 
	P_LESS,							//	"<" 
	P_LESS_EQUAL,					//	"<=" 
	P_LESS_LESS,					//	"<<" 
	P_LESS_LESS_EQUAL,				//	"<<=" 
	P_MINUS,						//	"-" 
	P_MINUS_EQUAL,					//	"-=" 
	P_MINUS_GREATER,				//	"->" 
	P_MINUS_MINUS,					//	"--" 
	P_NON_EQUAL,					//	"!=" 
	P_PERCENT,						//	"%" 
	P_PERCENT_EQUAL,				//	"%=" 
	P_PLUS,							//	"+" 
	P_PLUS_EQUAL,					//	"+=" 
	P_PLUS_PLUS,					//	"++" 
	P_QUESTION,						//	"?" 
	P_RANK_SPECIFIER,				//	"[]"  // [,] [,,] ...
	P_RIGHT_BRACE,					//	"}" 
	P_RIGHT_BRACKET,				//	"]" 
	P_RIGHT_PARENTH,				//	"" 
	P_SEMICOLON,					//	";" 
	P_SHARP,						//	"#" 
	P_SHARP_SHARP,					//	"##" 
	P_SLASH,						//	"/" 
	P_SLASH_EQUAL,					//	"/=" 
	P_TILDE,						//	"~" 
	P_TILDE_EQUAL,					//	"~="
	P_UNSIGNED_RIGHT_SHIFT,			//  ">>>"
	P_UNSIGNED_RIGHT_SHIFT_EQUAL,	//  ">>>="
	P_VERTICAL,						//	"|" 
	P_VERTICAL_EQUAL,				//	"|=" 
	P_VERTICAL_VERTICAL,			//	"||" 

	NUM_OF_TOKENS
};

/*
======================================================
	class Location
		for tracking locations in a source file.
======================================================
*/

class Location {
public:
					Location();
					Location( const Location & other );

	UINT			GetLine() const;
	UINT			GetEndLine() const;
	UINT			GetColumn() const;
	UINT			GetEndColumn() const;

	const char *	Filename() const;

	Location &		operator = ( const Location &other );

private:
	friend class Lexer;

	UINT		line, endLine;
	UINT		column, endColumn;
	String		filename;
};

INLINE Location::Location()
	: line( 1 )
	, endLine( 1 )
	, column( 1 )
	, endColumn( 1 )
{
}

INLINE Location::Location( const Location & other )
	: line( other.line )
	, endLine( other.endLine )
	, column( other.column )
	, endColumn( other.endColumn )
	, filename( other.filename )
{
}

INLINE UINT Location::GetLine() const {
	return line;
}

INLINE UINT Location::GetEndLine() const {
	return endLine;
}

INLINE UINT Location::GetColumn() const {
	return column;
}

INLINE UINT Location::GetEndColumn() const {
	return endColumn;
}

INLINE const char * Location::Filename() const {
	return filename.c_str();
}

INLINE Location & Location::operator = ( const Location & other ) {
	line	= other.line;
	endLine = other.endLine;
	column	= other.column;
	endColumn = other.endColumn;
	filename = other.filename;

	return *this;
}

/*
==========================================================================
	class Token
		
	Token is a single lexeme from a source file.
	These are produced by the lexer and are linked into the token list.
==========================================================================
*/
class Token {
public:

	int				Value() const;		// returns the type value of this token
	const char *	Text() const;		// returns the text of this token
	const Location &GetLocation() const;	// returns the textual location of this token

	Token *			NextToken();
	Token *			PrevToken();

public:
	// returns the token name corresponding to the given token type
	static const char *	ToChars( int type );

private:
	friend class Lexer;

	MX_DECLARE_MEMORY_CLASS( MX_MEMORY_CLASS_SCRIPT );

			Token();
			Token( const Token & other );
			Token( const char *text, int type );
			~Token();

private:
	int				m_type;	    // value of this token ( enum TOKEN_TYPE )
	String			m_text;	    // raw text of this token
	Location		m_location; // position of this token in a source file

	friend class TokenList;

	Token *		m_next;
	Token *		m_prev;

private:
	NO_ASSIGNMENT( Token );
};

//
//	TokenList - is a list of tokens (usually produced by a lexer).
//
class TokenList {
public:
	MX_DECLARE_MEMORY_CLASS( MX_MEMORY_CLASS_SCRIPT );

				TokenList();
				~TokenList();

	void		Append( Token * token );
	int			Num() const;
	void		DeleteAll();

	Token *		Head();
	Token *		Tail();

private:
	int			m_num;
	Token *		m_head;
	Token *		m_tail;

	friend class Lexer;
};

/*
===============================================
	class StringBuffer

	used by the lexer to store string data.
===============================================
*/
class StringBuffer : public String {
public:
				StringBuffer( UINT initialAlloc = 1024 );
				~StringBuffer();

	void		Reset();
	char *		Ptr();
};

INLINE StringBuffer::StringBuffer( UINT initialAlloc /* =1024 */ )
{
	String::EnsureAlloced( initialAlloc, false );
}

INLINE StringBuffer::~StringBuffer() {
}

INLINE void StringBuffer::Reset() {
	Empty();
}

INLINE char * StringBuffer::Ptr() {
	return String::GetBufferPtr();
}

/*
==========================================================================
	class Lexer - scanner ( or tokenizer, lexicographical parser ).

	Builds a list of tokens; scans and appends new tokens as needed.
==========================================================================
*/
class Lexer {
public:
	MX_DECLARE_MEMORY_CLASS( MX_MEMORY_CLASS_SCRIPT );

		Lexer();
		Lexer( const char* source );	// 'source' is a pointer to the buffer containing the script
		~Lexer();

						// Set the script pointer to the given memory.
	void				Reset( const char* source );

						// Insert the given keyword into the internal hash table.
	void				SetKeyword( const char* name, int type );

						// Read a token from the source and return its type.
	int					ReadToken();

						// Read a token from the source and return a pointer to it.
	Token *				ReadNextToken();

						// Unread one token, move back to the previous token.
	void				UnreadToken();

						// Returns the next character of the script without reading it.
	char				PeekChar() const;

	int					PeekTokenType();
	Token *				PeekToken();

						// useful for doing lookaheads, for example, PeekNextToken( PeekNextToken() )->Type() ...
	Token *				PeekNextToken( Token* prevToken = 0 );
	const char *		PeekTokenString();

						// Read a token and return true if its type equals the given token type.
	bool				ExpectToken( int expectedType );
	bool				ExpectTokenString( const char* expectedText );
	bool				ExpectAnyToken();

						// Returns the current line number.
	UINT				GetLineNum() const;
	UINT				CurrentColumn() const;
	
						// Returns the current token type.
	int					TokenType() const;
						// Returns the last parsed token.
	const Token &		CurrentToken() const;

						// Returns the current location in a source file.
	const Location &	GetLocation() const;

						// Skip the rest of the current line.
	bool				SkipRestOfLine();

						// Skip tokens until the given token string is read.
	bool				SkipUntilString( const char* str );

						// Skip the braced section.
	bool				SkipBracedSection( bool parseFirstBrace = true );

						// Parse a braced section into a string.
	const char *		ParseBracedSection();

						// Returns true if at the end of the file.
	BOOL				EndOfFile() const;

public:
	static bool		IsWhitespace( char c );
	static bool		IsDigit( char c );
	static bool		IsHexDigit( char c );
	static bool		IsBinDigit( char c );
	static bool		IsOctDigit( char c );
	static bool		IsFirstIdChar( char c );	// returns true if this can be the first character of an identifier
	static bool		IsIdChar( char c );
	static int		AsHexDigit( char c );

	bool	IsKeyword( const char * text );

	bool	IsValidIdentifier( const char * text );

	enum LEXER_SETTINGS
	{
		ID_LENGTH				= 32,		// number of significant letters in identifiers
		MAX_TOKEN_LENGTH		= 256,
		MAX_PARENTHESES_LEVEL   = 64,	// maximum number of syntactical nested parentheses
		MAX_PARSE_LEVEL			= 128,	// maximum number of syntactical nested non-terminals
		TAB_WIDTH				= 4,
	};
	enum {
		ALL_LOWERCASE	= true	// treat all identifiers as if they were in lowercase
	};

private:
	// Internal functions.

				// reads a keyword or an identifier into the internal buffer
	bool		ReadName();

				// reads a numerical literal and returns its type
	int			ReadNumber();

				// reads "string literals", "etc", ...
	int			ReadStringConstant();

				// skip comments, whitespaces, etc..
	bool		ReadWhiteSpace();
	void		SkipComments();

				// Save current linenumber info and prepare lexer for reading new portions of script.
	void		UpdateLexerState();

				// The main worker function which actually reads a token.
	void		Scan( Token * token );

				// Update the token's linenumber info.
	void		Update( Token * token );

				// Empties the internal string buffer
				// and prepares it for writing a new string.
	void		ResetStringBuffer();

	char		CurrentChar() const;
	char		PreviousChar() const;	// NOTE: the lexer must read one or more chars before calling this function
	char		ReadChar();
	char		SkipChar();

private:
	const char *	m_source;	// pointer to the buffer containing the script

	const char *	m_p;	// Current position in the source file.
	const char *	m_prev;	// Previous position in the source file.

	UINT			m_line;			// line number in script before reading token
	UINT			m_lastLine;		// line after reading token
	UINT			m_column;		// column before reading token
	UINT			m_lastColumn;	// column after reading token

	StringBuffer	m_buffer;	// for temporary string data

	Token *			m_token;		// current token in the token list
	TokenList		m_tokenList;	// parsed tokens

	TStringHash< int >	m_keywords;	// keyword IDs hashed by their names
};

INLINE bool Lexer::IsWhitespace( char c ) {
	return 0x20 == c || '\t' == c || '\n' == c || '\r' == c;
}

INLINE bool Lexer::IsDigit( char c ) {
	return c >= '0' && c <= '9';
}

INLINE bool Lexer::IsHexDigit( char c ) {
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

INLINE bool	Lexer::IsBinDigit( char c ) {
	return c == '0' || c == '1';
}

INLINE bool	Lexer::IsOctDigit( char c ) {
	return c >= '0' && c <= '7';
}

INLINE bool Lexer::IsFirstIdChar( char c ) {
	return ( c == '_' ) || ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' );
}

INLINE bool Lexer::IsIdChar( char c ) {
	return IsFirstIdChar( c ) || IsDigit( c );	
}

INLINE int Lexer::AsHexDigit( char c ) {
	if (c >= '0' && c <= '9') {
		return  c - '0';
	}
	if (c >= 'A' && c <= 'F') {
		return  c - 'A' + 10;
	}
	if (c >= 'a' && c <= 'f') {
		return  c - 'a' + 10;   
	}
	return  -1;
}












/*================================
		Token
================================*/

Token::Token()
	: m_type( 0 )
	, m_next( 0 )
	, m_prev( 0 )
{
}

Token::Token( const Token & other )
	: m_type( other.m_type )
	, m_text( other.m_text )
	, m_next( 0 )
	, m_prev( 0 )
{
}

Token::Token( const char *text, int type )
	: m_text( text )
	, m_type( type )
	, m_next( 0 )
	, m_prev( 0 )
{
}

Token::~Token() {
}

Token * Token::NextToken() {
	return m_next;
}

Token * Token::PrevToken() {
	return m_prev;
}

int Token::Value() const {
	return m_type;
}

const char * Token::Text() const {
	return m_text.c_str();
}

const Location & Token::GetLocation() const {
	return m_location;
}

/*
=========================
	Token : static members
=========================
*/
const char * Token::ToChars( int type )
{
	static const char* tokenNames[ NUM_OF_TOKENS ] =
	{
		"<EOF>",

		/*
		-----------------------------------------
			Literals
		-----------------------------------------
		*/
		"integer number",
		"floating-point number",
		"char literal",
		"identifier",
		"string constant",

		/*
		-----------------------------------------
			Reserved keywords
		-----------------------------------------
		*/
		"keyword 'material'",

		"keyword 'Diffuse_Map'",
		"keyword 'Detail_Map'",
		"keyword 'Normal_Map'",
		"keyword 'Gloss_Map'",

		"keyword 'technique'",
		"keyword 'pass'",

		"keyword 'lighting'",
		"keyword 'normalize_normals'",

		"keyword 'Shading'",
		"keyword 'Fill_Mode'",
		"keyword 'cull_mode'",

		"keyword 'ambient_color'",
		"keyword 'diffuse_color'",
		"keyword 'emissive_color'",
		"keyword 'specular_color'",
		"keyword 'specular_power'",
		"keyword 'enable_specular'",

		"keyword 'texture_unit'",
		"keyword 'texture'",
		"keyword 'tex_coord_set'",
		"keyword 'texture_filtering'",
		"keyword 'min_filter'",
		"keyword 'mag_filter'",
		"keyword 'mip_filter'",

	//-----------------------------------------------------------------
	//	Texture filtering.
	//-----------------------------------------------------------------
		"keyword 'none'",
		"keyword 'point'",
		"keyword 'linear'",
		"keyword 'bilinear'",
		"keyword 'trilinear'",
		"keyword 'anisotropic'",

	//-----------------------------------------------------------------
	//	Texture blending operations.
	//-----------------------------------------------------------------

		"keyword 'blendop_disable'",
		
		"keyword 'blendop_selectarg1'",
		"keyword 'blendop_selectarg2'",
		
		"keyword 'blendop_modulate'",
		"keyword 'blendop_modulate2x'",
		"keyword 'blendop_modulate4x'",
		
		"keyword 'blendop_add'",
		"keyword 'blendop_addsigned'",
		"keyword 'blendop_addsigned2x'",

		"keyword 'blendop_subtract'",

	//-----------------------------------------------------------------
	//	Texture blending value sources.
	//-----------------------------------------------------------------
		"keyword 'current'",
	//	"keyword 'diffuse'",
	//	"keyword 'specular'",

		"keyword 'color_op'",
		"keyword 'color_arg1'",
		"keyword 'color_arg2'",

		
		"keyword 'depth_test'",
		"keyword 'depth_write'",

		"keyword 'true'",
		"keyword 'false'",

	//-----------------------------------------------------------------
	//	Miscellaneous.
	//-----------------------------------------------------------------

		"keyword 'scale'",

		/*
		-----------------------------------------
			Preprocessor directives
		-----------------------------------------
		*/
		"preprocessor: #define"					,
		"preprocessor: #include"				,
		"preprocessor: #if"						,
		"preprocessor: #ifdef"					,
		"preprocessor: #ifndef"					,
		"preprocessor: #elif"					,
		"preprocessor: #else"					,
		"preprocessor: #endif"					,
		"preprocessor: #undef"					,

		/*
		-----------------------------------------
			Punctuation
		-----------------------------------------
		*/
		"punctuation: '&'"			,
		"punctuation: '&&'"			,
		"punctuation: '&='"			,
		"punctuation: '*'"			,
		"punctuation: '*='"			,
		"punctuation: '@'" 			,
		"punctuation: '@='" 		,
		"punctuation: '\\'" 		,
		"punctuation: '^'" 			,
		"punctuation: '^='" 		,
		"punctuation: ':'" 			,
		"punctuation: '::'"			,
		"punctuation: ','"			,
		"punctuation: '$'" 			,
		"punctuation: '.'" 			,
		"punctuation: '...'" 		,
		"punctuation: '='" 			,
		"punctuation: '=='" 		,
		"punctuation: '!'" 			,
		"punctuation: '>'"			,
		"punctuation: '>='" 		,
		"punctuation: '>>'" 		,
		"punctuation: '>>='" 		,
		"punctuation: '{'" 			,
		"punctuation: '['" 			,
		"punctuation: '('" 			,
		"punctuation: '<'" 			,
		"punctuation: '<='" 		,
		"punctuation: '<<'" 		,
		"punctuation: '<<='" 		,
		"punctuation: '-'" 			,
		"punctuation: '-='" 		,
		"punctuation: '->'" 		,
		"punctuation: '--'" 		,
		"punctuation: '!='" 		,
		"punctuation: '%'" 			,
		"punctuation: '%='" 		,
		"punctuation: '+'" 			,
		"punctuation: '+='" 		,
		"punctuation: '++'" 		,
		"punctuation: '?'" 			,
		"punctuation: '[]'" 		,
		"punctuation: '}'" 			,
		"punctuation: ']'" 			,
		"punctuation: ')'"			,
		"punctuation: ';'" 			,
		"punctuation: '#'" 			,
		"punctuation: '##'" 		,
		"punctuation: '/'"			,
		"punctuation: '/='"			,
		"punctuation: '~'"			,
		"punctuation: '~='"			,
		"punctuation: '>>>'"		,
		"punctuation: '>>>='"		,
		"punctuation: '|'"			,
		"punctuation: '|='"			,
		"punctuation: '||'"			,
	};

	Assert( type >= 0 && type < NUM_OF_TOKENS );
	if ( type >= 0 && type < NUM_OF_TOKENS ) {
		return tokenNames[ type ];
	}
	return null;
}

/*=========================
	TokenList
=========================*/

TokenList::TokenList()
	: m_num( 0 )
	, m_head( 0 )
	, m_tail( 0 )
{
}

TokenList::~TokenList() {
	DeleteAll();
}

void TokenList::Append( Token * token )
{
	if ( ! m_head ) {
		m_head = token;
	}
	
	token->m_prev = m_tail;

	if ( m_tail ) {
		m_tail->m_next = token;
	}
	m_tail = token;

	++m_num;
}

int TokenList::Num() const {
	return m_num;
}

void TokenList::DeleteAll()
{
	Token * token = m_head;
	while ( token ) {
		Token * next = token->m_next;
		MX_FREE token;
		token = next;
	}

	m_head = null;
	m_tail = null;
	m_num = 0;
}

Token * TokenList::Head() {
	return m_head;
}

Token * TokenList::Tail() {
	return m_tail;
}

/*================================
		Lexer
================================*/

Lexer::Lexer()
	: m_source	( null )
	, m_p		( m_source )
	, m_prev	( null )
	, m_token	( null )

	, m_line		( 1 )
	, m_lastLine	( 1 )
	, m_column		( 1 )
	, m_lastColumn	( 1 )
{}

Lexer::Lexer( const char * source ) 
{
	Reset( source );
}

Lexer::~Lexer() {
}

void Lexer::Reset( const char * source )
{
	Assert( source );

	m_tokenList.DeleteAll();
	m_token = MX_NEW Token(); // dummy token to be linked into the token list
	m_tokenList.Append( m_token );

	m_source = source;

	m_p		= m_source;
	m_prev	= m_source;

	m_line		= 1;
	m_lastLine	= 1;
	m_column	= 1;
	m_lastColumn = 1;

	ResetStringBuffer();
}

void Lexer::SetKeyword( const char* name, int type )
{
	Assert( IsValidIdentifier( name ) );

#ifdef MX_DEBUG
	if ( m_keywords.Get( name ) ) {
		sys::Warning( "keyword '%s' already exists", name );
	}
#endif // MX_DEBUG

	m_keywords.Set( name, type );
}

int Lexer::ReadToken() {
	return ReadNextToken()->Value();
}

Token * Lexer::ReadNextToken()
{
	if ( m_token->m_next ) {
		 m_token = m_token->m_next;
	}
	else {
		Token * token = MX_NEW Token();
		Scan( token );
		Update( token );
		m_tokenList.Append( token );

		m_token = token;
	}

	return m_token;
}

void Lexer::UnreadToken()
{
	m_p = m_prev;
	m_line = m_lastLine;
	m_column = m_lastColumn;

	m_token = m_token->m_prev;
}

char Lexer::PeekChar() const {
	//Assert( m_p[1] != null );
	return *( m_p + 1 );
}

int Lexer::PeekTokenType() {
	return PeekToken()->Value();
}

Token * Lexer::PeekToken()
{
	if ( m_token->m_next ) {
		return m_token->m_next;
	}
	else {
		Token * token = MX_NEW Token();
		Scan( token );
		Update( token );
		m_tokenList.Append( token );

		return token;
	}
}

Token * Lexer::PeekNextToken( Token* prevToken /* =0 */ )
{
	if ( prevToken )
	{
		// peek the token following the given token
		if ( prevToken->m_next ) {
			return prevToken->m_next;
		}
		else {
			Token * token = MX_NEW Token();
			Scan( token );
			Update( token );
			m_tokenList.Append( token );
			return token;
		}
	}
	else {
		// peek the token following the current token
		return PeekToken();
	}
}

const char * Lexer::PeekTokenString()
{
	return PeekToken()->Text();
}

bool Lexer::ExpectToken( int expectedType )
{
	if ( expectedType != ReadToken() )
	{
		sys::Warning( "expected %s, but got '%s'",
			Token::ToChars( expectedType ), CurrentToken().Text() );
		return false;
	}
	return true;
}

bool Lexer::ExpectTokenString( const char* expectedText )
{
	const char* pCurrText = ReadNextToken()->Text();
	if ( strcmp( pCurrText, expectedText ) != 0 )
	{
		sys::Warning( "expected '%s', but got '%s'", expectedText, pCurrText );
		return false;
	}
	return true;
}

bool Lexer::ExpectAnyToken()
{
	return ( ReadToken() != T_END_OF_FILE );
}

UINT Lexer::GetLineNum() const {
	return CurrentToken().GetLocation().GetLine();
}

UINT Lexer::CurrentColumn() const {
	return CurrentToken().GetLocation().GetColumn();
}

int Lexer::TokenType() const
{
	return m_token->Value();
}

const Token & Lexer::CurrentToken() const
{
	return *m_token;
}

bool Lexer::SkipRestOfLine()
{
	Unimplemented;
	return 0;
}
bool Lexer::SkipUntilString( const char* str )
{
	Unimplemented;
	return 0;
}
bool Lexer::SkipBracedSection( bool parseFirstBrace /* = true */ )
{
	Unimplemented;
	return 0;
}
const char * Lexer::ParseBracedSection()
{
	Unimplemented;
	return 0;
}

BOOL Lexer::EndOfFile() const
{
	return ( *m_p == null );
}

bool Lexer::IsKeyword( const char * text )
{
	return m_keywords.Get( text );
}

bool Lexer::IsValidIdentifier( const char * text )
{
	return ( text != null )
		&& ( sys::StrLen(text) <= MAX_TOKEN_LENGTH );
}

/*
================================================
	Lexer::ReadName()

	Reads an identifier or a keyword into the internal buffer
	and return false if reached the end of file.

	TODO:
		Checks for buffer overrun,
		skip redundant chars in long identifiers,
		etc.
================================================
*/
bool Lexer::ReadName()
{
	unsigned numChars = 0;

	while ( IsIdChar( *m_p ) )
	{
		char  c = *m_p;
	
		if ( ALL_LOWERCASE )
		{
			c = String::ToLower( c );
		}

		m_buffer.Ptr()[ numChars++ ] = c;
		++m_p;

		++m_column;

		if ( numChars >= ID_LENGTH ) {
			break;
		}
	}

	m_buffer.Ptr()[ numChars ] = '\0';
	//m_buffer.SetLength( numChars );

	return true;
}

/*
================================================
	Lexer::ReadNumber

	Reads a number and returns its type ( L_INTEGER, L_UINT32, ... ).
================================================
*/
int Lexer::ReadNumber()
{
	MX_OPTIMIZE(:)

	while ( IsDigit( *m_p ) )
	{
		m_buffer += *m_p++;
		++m_column;
	}

	String  intPart = m_buffer;

	// check if this is not a floating-point number
	if ( '.' != *m_p ) {
		return L_INTEGER;
	}

	// this is a floating-point number

	// skip '.'
	m_buffer += *m_p++;
	++m_column;	

	// read the fractional part
	while ( IsDigit( *m_p ) )
	{
		m_buffer += *m_p++;
		++m_column;
	}

	return L_FLOAT;
}

int Lexer::ReadStringConstant()
{
	ReadChar();	  // read '\"'

	m_buffer.Reset();

	while ( '\"' != CurrentChar()
		&& !EndOfFile() )
	{
		ReadChar();
	}

	SkipChar();		// read the closing '\"'

	return L_STRING;
}

//--------------------------------------------------------------------------
//	Reads spaces, tabs, C-like comments, etc.
//	When a newline character is found the line counter is increased.
//--------------------------------------------------------------------------
bool Lexer::ReadWhiteSpace()
{
	while ( true )
	{
		// skip white space
		while ( CurrentChar() <= ' ' )
		{
			if ( EndOfFile() ) {
				return false;
			}
			if ( '\n' == CurrentChar() ) {
				++m_line;
			}
			if ( '\t' == CurrentChar() ) {
				++m_p;
				m_column += TAB_WIDTH;
			}

			SkipChar();
		}
		// skip comments
		if ( '/' == CurrentChar() )
		{
			// comments //
			if ( '/' == PeekChar() )
			{
				SkipChar();
				do {
					SkipChar();
					if ( EndOfFile() ) {
						return false;
					}
				} while ( '\n' != CurrentChar() );
				
				++m_line;
				SkipChar();

				if ( EndOfFile() ) {
					return false;
				}
				continue;
			}
			// comments /* */
			else if ( '*' == PeekChar() )
			{
				SkipChar();
				while ( true )
				{
					SkipChar();
					if ( EndOfFile() ) {
						return false;
					}
					if ( '\n' == CurrentChar() ) {
						++m_line;
					}
					else if ( '/' == CurrentChar() )
					{
						if ( '*' == PreviousChar() ) {
							break;
						}
						if ( '*' == PeekChar() ) {
							sys::Warning( "nested comment" );
						}
					}
				}
				
				SkipChar();

				if ( EndOfFile() ) {
					return false;
				}
				continue;
			}
		}
		break;

	}//while

	return true;
}

/*
=====================================
	Lexer::SkipComments()

	Notes:
	When a newline character is found the line counter is increased.
=====================================
*/
void Lexer::SkipComments()
{
	unsigned nestLevel = 0;

	if ( '/' == CurrentChar() )
	{
		// single-line comments //
		// line comments terminate at the end of the line
		if ( '/' == PeekChar() )
		{
			SkipChar();
			do {
				SkipChar();
				if ( EndOfFile() ) {
					return;
				}
			} while ( '\n' != CurrentChar() );

			++m_line;
		}
		// multi-line comments /* ... */
		// these can span multiple lines, but do not nest
		else if ( '*' == PeekChar() )
		{
			SkipChar();
			while ( true )
			{
				SkipChar();
				if ( EndOfFile() ) {
					return;
				}
				if ( '\n' == CurrentChar() ) {
					++m_line;
				}
				else if ( '/' == CurrentChar() )
				{
					if ( '*' == PreviousChar() ) {
						break;
					}
//					if ( '*' == PeekChar() ) {
//						Warning( "nested comment" );
//					}
				}
			}

			SkipChar();
		}
		// nesting comments /+ ... +/
		// these can span multiple lines and can nest
		else if ( '+' == PeekChar() ) 
		{
			SkipChar();
			++nestLevel;		
			
			while ( nestLevel )
			{
				SkipChar();
				if ( EndOfFile() ) {
					if ( nestLevel ) {
						sys::Error( "unterminated /+ +/ comment; end of file reached." );
					}
					return;
				}
				if ( '\n' == CurrentChar() ) {
					++m_line;
				}
				else if ( '/' == CurrentChar() )
				{
					if ( '+' == PreviousChar() ) {
						--nestLevel;
					}
				}
				else if( '+' == CurrentChar() )
				{
					if ( '/' == PreviousChar() ) {
						++nestLevel;
					}
				}
			}

			SkipChar();
		}
	}
}

void Lexer::UpdateLexerState()
{
	m_prev		= m_p;
	m_lastLine	= m_line;
	m_lastColumn = m_column;
}

/*
======================================================
	Lexer::Scan( Token * token )

	The main worker function. Reads the next lexeme
	from the source file and copies it into the given token.
======================================================
*/
void Lexer::Scan( Token * token )
{
	UpdateLexerState();

	while ( true )
	{
		ResetStringBuffer();

#pragma region "Comments and whitespaces"
		
		SkipComments();

#pragma endregion

#pragma region Identifiers and keywords

		if ( IsFirstIdChar( CurrentChar() ) )	// TODO: re-work this: return ReadKeywordOrIdentifier();
		{
			ReadName();
			token->m_text = m_buffer.ToChar();

			int		value;
			int *	p = & value;

			// if we got a keyword
			if ( m_keywords.Get( m_buffer, &p ) ) {
				token->m_type = *p;
			}
			else {
				token->m_type = L_IDENTIFIER;
			}

			return;
		}
#pragma endregion

		else

#pragma region	Numbers
		
		if ( IsDigit( CurrentChar() ) ) {
			token->m_type = ReadNumber();
			token->m_text = m_buffer.Ptr();
			return;
		}

#pragma endregion

		else

#pragma region String literals

		if ( '\"' == CurrentChar() ) {
			token->m_type = ReadStringConstant();
			token->m_text = m_buffer.Ptr();
			return;
		}

#pragma endregion

		else

		switch ( CurrentChar() )
		{
			case 0:
			case 0x1A:
				token->m_text = "<end of file>";
				token->m_type = T_END_OF_FILE;
				return;

			case ' ':
			case '\v':
			case '\f':
				++m_p;
				++m_column;
				continue;	// skip white space

			case '\t':
				++m_p;
				m_column += TAB_WIDTH;
				continue;

			case '\r':
				++m_p;
				m_column = 0;
				if ( '\n' != CurrentChar() ) {	// if CR stands by itself	
					++m_line;
				}
				continue;	// skip white space

			case '\n':
				++m_p;
				++m_line;
				m_column = 0;
				continue;	// skip white space


#pragma region Punctuation & Operators

			// Punctuation

#define GET_TOKEN( ch, the_type )		case ch:						\
											m_buffer.Reset();			\
											ReadChar();					\
											token->m_type = the_type;	\
											token->m_text = m_buffer;	\
											return;

		GET_TOKEN( '(',		P_LEFT_PARENTH 		);
		GET_TOKEN( ')',		P_RIGHT_PARENTH	 	);
		GET_TOKEN( '{',		P_LEFT_BRACE 		);
		GET_TOKEN( '}',		P_RIGHT_BRACE 		);
		GET_TOKEN( '[',		P_LEFT_BRACKET 		);
		GET_TOKEN( ']',		P_RIGHT_BRACKET	 	);
		GET_TOKEN( ';',		P_SEMICOLON 		);
		GET_TOKEN( ',',		P_COMMA 			);
		GET_TOKEN( '.',		P_DOT				);
		GET_TOKEN( '?',		P_QUESTION			);
		GET_TOKEN( '$',		P_DOLLAR			);
		GET_TOKEN( '\\',	P_BACKSLASH			);

#undef GET_TOKEN

		// Arithmetic operators

#define GET_TOKEN2( ch1, ch2, type1, type2 )	case ch1:							\
													m_buffer.Reset();				\
													ReadChar();						\
													if ( ch2 == *m_p ) {			\
														ReadChar();					\
														token->m_type = type2;		\
														token->m_text = m_buffer;	\
														return;						\
													}								\
													token->m_type = type1;			\
													token->m_text = m_buffer;		\
													return;


		GET_TOKEN2( '@', '=',	P_AT,			P_AT_EQUAL			);
		GET_TOKEN2( '^', '=',	P_CARET,		P_CARET_EQUAL		);
		GET_TOKEN2( '*', '=',	P_ASTERISK,		P_ASTERISK_EQUAL	);
		GET_TOKEN2( '/', '=',	P_SLASH,		P_SLASH_EQUAL		);
		GET_TOKEN2( '%', '=',	P_PERCENT,		P_PERCENT_EQUAL		);

		GET_TOKEN2( '=', '=',	P_EQUAL,		P_EQUAL_EQUAL		);
		GET_TOKEN2( '!', '=',	P_EXCLAMATION,	P_NON_EQUAL			);

		GET_TOKEN2( '~', '=',	P_TILDE,		P_TILDE_EQUAL		);

		GET_TOKEN2( ':', ':',	P_COLON,		P_COLON_COLON		);
		GET_TOKEN2( '#', '#',	P_SHARP,		P_SHARP_SHARP		);

#undef GET_TOKEN2



// Ex.:
// +
// +=
// ++

#define GET_TOKEN3( ch1, ch2, ch3, type1, type2, type3 )	case ch1:									\
																m_buffer.Reset();						\
																ReadChar();								\
																if ( ch2 == *m_p ) {					\
																	ReadChar();							\
																	token->m_type = type2;				\
																}										\
																else if ( ch3 == *m_p ) {				\
																	ReadChar();							\
																	token->m_type = type3;				\
																} else {								\
																	token->m_type = type1;				\
																}										\
																token->m_text = m_buffer;				\
																return;


	GET_TOKEN3( '+', '=', '+',	P_PLUS,			P_PLUS_EQUAL,			P_PLUS_PLUS			);

//------------------------------------------------------------
case '-':						
	m_buffer.Reset();			
	ReadChar();					
	if ( '>' == *m_p ) {		
		ReadChar();				
		token->m_type = P_MINUS_GREATER;	
	}							
	else if ( '-' == *m_p ) {	
		ReadChar();				
		token->m_type = P_MINUS_MINUS;	
	}
	else if ( '=' == *m_p ) {	
		ReadChar();				
		token->m_type = P_MINUS_EQUAL;	
	} 
	else {					
		token->m_type = P_MINUS;	
	}							
	token->m_text = m_buffer;	
	return;
//------------------------------------------------------------


	GET_TOKEN3( '&', '&', '=',	P_AMPERSAND,	P_AMPERSAND_AMPERSAND,	P_AMPERSAND_EQUAL	);
	GET_TOKEN3( '|', '=', '|',	P_VERTICAL,		P_VERTICAL_EQUAL,		P_VERTICAL_VERTICAL );

#undef GET_TOKEN3


// Shifts & Relational operators
// Example:
// <
// <=
// <<
// <<=

#define GET_TOKEN4( ch1, ch2, ch3, ch4, type1, type2, type3, type4 )	\
	case ch1:															\
		m_buffer.Reset();												\
		ReadChar();														\
		if ( ch2 == *m_p ) {											\
			ReadChar();													\
			token->m_type = type2;										\
		}																\
		else if ( ch3 == *m_p ) {										\
			ReadChar();													\
			if ( ch4 == *m_p ) {										\
				ReadChar();												\
				token->m_type = type4;									\
			} else {													\
				token->m_type = type3;									\
			}															\
			token->m_text = m_buffer;									\
			return;														\
		} else {														\
			token->m_type = type1;										\
		}																\
		token->m_text = m_buffer;										\
		return;															


	GET_TOKEN4( '<', '=', '<', '=', 
		P_LESS,		P_LESS_EQUAL,		P_LESS_LESS,		P_LESS_LESS_EQUAL		
	);

	//-- Note: cannot use this because we need to parse >>> and >>>=.
	//GET_TOKEN4( '>', '=', '>', '=', 
	//	P_GREATER,	P_GREATER_EQUAL,	P_GREATER_GREATER,	P_GREATER_GREATER_EQUAL	
	//);

#undef GET_TOKEN4


// Unsigned right shift: >>>, >>>=
//
	case '>':
		ReadChar();
		if ( '>' == *m_p ) {
			ReadChar();
			if ( '>' == *m_p ) {
				ReadChar();
				if ( '=' == *m_p ) {
					ReadChar();
					token->m_type = P_UNSIGNED_RIGHT_SHIFT_EQUAL;
				}
				else {
					token->m_type = P_UNSIGNED_RIGHT_SHIFT;
				}
				token->m_text = m_buffer;
				return;
			} 
			else if ( '=' == *m_p ) {
				ReadChar();
				token->m_type = P_GREATER_GREATER_EQUAL;
				token->m_text = m_buffer;
				return;
			}
			token->m_type = P_GREATER_GREATER;
			token->m_text = m_buffer;
			return;
		}
		else if ( '=' == *m_p ) {
			ReadChar();
			token->m_type = P_GREATER_EQUAL;
			token->m_text = m_buffer;
			return;
		}
		token->m_type = P_GREATER;
		token->m_text = m_buffer;
		return;

#pragma endregion

	default:
		//sys::Warning( "unknown token: %s; stopping parsing...", m_buffer );
		token->m_text = "EOF";
		token->m_type = T_END_OF_FILE;
		return;

		}//switch ( curr. char )

	}//while ( true )
}

//
//	Lexer::Update
//
void Lexer::Update( Token * token )
{
	token->m_location.line		= m_line;
	token->m_location.endLine	= m_lastLine;
	token->m_location.column	= m_column;
	token->m_location.endColumn = m_lastColumn;
}

void Lexer::ResetStringBuffer()
{
	m_buffer.Reset();
}

char Lexer::CurrentChar() const {
	return *m_p;
}

char Lexer::PreviousChar() const {
	return *( m_p - 1 );
}

char Lexer::ReadChar() {
	++m_column;
	m_buffer += *m_p;
	return *( ++m_p );
}

char Lexer::SkipChar() {
	++m_column;
	return *( ++m_p );
}

//----------------------------------------------------------------------------------------------------

//
//	MaterialParser
//
class MaterialParser : public Lexer {
public:
	MaterialParser();
	~MaterialParser();

	rxMaterial* ParseMaterialScript( const mxChar* filename );

private:
	bool ParseMaterial( rxMaterialDescription &OutDesc );

	void ReadOptional( int tokenType );
	void Expect( int tokenType );

private:
	UINT	numMaterials;
};

/*================================
		MaterialParser
================================*/

MaterialParser::MaterialParser()
	: numMaterials( 0 )
{

//
	// Initialize the keyword table.
	//

//#define	SET_KEYWORD( keyword, keywordID )		Lexer::SetKeyword( #keyword, keywordID );

#define	SET_KEYWORD( keyword, keywordID ) \
	{													\
		String  temp( #keyword );						\
		temp.ToLower();									\
		Lexer::SetKeyword( temp.c_str(), keywordID );	\
	}


	SET_KEYWORD( Material,				T_MATERIAL );
	
	SET_KEYWORD( Diffuse_Map,			T_DIFFUSE_MAP );
	SET_KEYWORD( Detail_Map,			T_DETAIL_MAP );
	SET_KEYWORD( Normal_Map,			T_NORMAL_MAP );
	SET_KEYWORD( Gloss_Map,				T_GLOSS_MAP );

	SET_KEYWORD( Technique,				T_TECHNIQUE );
	SET_KEYWORD( Pass,					T_PASS );

	SET_KEYWORD( Lighting,				T_LIGHTING );
	SET_KEYWORD( Normalize_Normals,		T_NORMALIZE_NORMALS );

	SET_KEYWORD( Shading,				T_SHADE_MODE );
	SET_KEYWORD( Fill_Mode,				T_FILL_MODE );
	SET_KEYWORD( Cull_Mode,				T_CULL_MODE );

	SET_KEYWORD( Ambient,				T_AMBIENT_COLOR );
	SET_KEYWORD( Diffuse,				T_DIFFUSE_COLOR );
	SET_KEYWORD( Emissive,				T_EMISSIVE_COLOR );
	SET_KEYWORD( Specular,				T_SPECULAR_COLOR );
	SET_KEYWORD( Power,					T_SPECULAR_POWER );
	SET_KEYWORD( Enable_Specular,		T_ENABLE_SPECULAR );

	SET_KEYWORD( Texture_Unit,			T_TEXTURE_UNIT );
	SET_KEYWORD( Texture,				T_TEXTURE );
	SET_KEYWORD( Tex_Coord_Set,			T_TEXTURE_COORD_SET );
	SET_KEYWORD( Filtering,				T_TEXTURE_FILTERING );
	SET_KEYWORD( MinFilter,				T_MIN_FILTER );
	SET_KEYWORD( MagFilter,				T_MAG_FILTER );
	SET_KEYWORD( MipFilter,				T_MIP_FILTER );

	SET_KEYWORD( None,					T_NONE );

	SET_KEYWORD( Point,					T_POINT );
	SET_KEYWORD( Linear,				T_LINEAR );
	SET_KEYWORD( Bilinear,				T_BILINEAR );
	SET_KEYWORD( Trilinear,				T_TRILINEAR );
	SET_KEYWORD( Anisotropic,			T_ANISOTROPIC );

	//-----------------------------------------------------------------
	//	Texture blending operations.
	//-----------------------------------------------------------------

	SET_KEYWORD( BlendOp_Disable,		T_BLENDOP_DISABLE );
										
	SET_KEYWORD( BlendOp_SelectArg1,	T_BLENDOP_SELECTARG1 );
	SET_KEYWORD( BlendOp_SelectArg2,	T_BLENDOP_SELECTARG2 );
										
	SET_KEYWORD( BlendOp_Modulate,		T_BLENDOP_MODULATE );
	SET_KEYWORD( BlendOp_Modulate2X,	T_BLENDOP_MODULATE2X );
	SET_KEYWORD( BlendOp_Modulate4X,	T_BLENDOP_MODULATE4X );
										
	SET_KEYWORD( BlendOp_Add,			T_BLENDOP_ADD );
	SET_KEYWORD( BlendOp_AddSigned,		T_BLENDOP_ADDSIGNED );
	SET_KEYWORD( BlendOp_AddSigned2X,	T_BLENDOP_ADDSIGNED2X );
										
    SET_KEYWORD( BlendOp_Subtract,		T_BLENDOP_SUBTRACT );

	//-----------------------------------------------------------------
	//	Texture blending value sources.
	//-----------------------------------------------------------------

	SET_KEYWORD( Current,			T_CURRENT );
	// T_TEXTURE is already used
//	SET_KEYWORD( Diffuse,			T_DIFFUSE );
//	SET_KEYWORD( Specular,			T_SPECULAR );


	SET_KEYWORD( Color_Op,				T_COLOR_OP );
	SET_KEYWORD( Color_Arg1,			T_COLOR_ARG1 );
	SET_KEYWORD( Color_Arg2,			T_COLOR_ARG2 );


	SET_KEYWORD( Depth_Test,			T_DEPTH_TEST_ENABLE );
	SET_KEYWORD( Depth_Write,			T_DEPTH_BUFFER_WRITE_ENABLE );

	SET_KEYWORD( True,					T_TRUE );
	SET_KEYWORD( False,					T_FALSE );

	//-----------------------------------------------------------------
	//	Miscellaneous.
	//-----------------------------------------------------------------

	SET_KEYWORD( Scale,					T_SCALE );

#undef SET_KEYWORD


}

MaterialParser::~MaterialParser()
{
}

rxMaterial* MaterialParser::ParseMaterialScript( const mxChar* filename )
{
	mxFile_ReadOnly  file( filename );
	if( ! file.IsOk() ) {
		return null;
	}

	const mxSizeT  numBytes = file.GetSize();
	BYTE * pScript = (BYTE*) Allocate( numBytes+1, EMemoryClass::MX_MEMORY_CLASS_SCRIPT );
	file.Read( pScript, numBytes );
	pScript[ numBytes ] = '\0';

	Lexer::Reset( (const char*) pScript );

	rxResources & resources = mxEngine::get().GetRenderer().GetResources();
	rxMaterial * lastMaterial = null;

	this->numMaterials = 0;

	while( PeekTokenType() != T_END_OF_FILE )
	{
		rxMaterialDescription matDesc;

		if( ParseMaterial( matDesc ) ) {
			lastMaterial = resources.CreateMaterial( matDesc );
			this->numMaterials++;
		}
	}

	Free( pScript, EMemoryClass::MX_MEMORY_CLASS_SCRIPT );

	Assert( this->numMaterials > 0 );
	return lastMaterial;
}

bool MaterialParser::ParseMaterial( rxMaterialDescription &OutDesc )
{
	Expect( T_MATERIAL );
	
	B_RET( ExpectToken( L_IDENTIFIER ) );
	OutDesc.name = CurrentToken().Text();
	
	Expect( P_LEFT_BRACE );

	rxResources & resources = mxEngine::get().GetRenderer().GetResources();

	while ( true )
	{
		int token = PeekTokenType();
		if ( T_END_OF_FILE == token || P_RIGHT_BRACE == token ) {
			break;
		}

		switch( token )
		{
			case T_DIFFUSE_MAP :
				{
					if( null != OutDesc.layers[TL_Diffuse] ) {
						sys::Warning("material already has diffuse layer defined");
						return false;
					}
					ReadToken();
					B_RET( ExpectToken( L_STRING ) );
					OutDesc.layers[TL_Diffuse] = resources.LoadTexture( CurrentToken().Text() );
				}
				continue;

			case T_NORMAL_MAP :
				{
					if( null != OutDesc.layers[TL_Normals] ) {
						sys::Warning("material already has normal map defined");
						return false;
					}
					ReadToken();
					B_RET( ExpectToken( L_STRING ) );
					OutDesc.layers[TL_Normals] = resources.LoadTexture( CurrentToken().Text() );
				}
				continue;

			default:
				Unimplemented;
		}
	}

	Expect( P_RIGHT_BRACE );

	return true;
}

void MaterialParser::ReadOptional( int tokenType )
{
	if ( PeekTokenType() == tokenType ) {
		ReadToken();
	}
}

void MaterialParser::Expect( int tokenType )
{
	const Token * token = PeekToken();

	if ( tokenType != token->Value() ) {
		sys::Error( "expected %s after '%s'", Token::ToChars( tokenType ), token->Text() );
	} else {
		ReadToken();
	}
}

//----------------------------------------------------------------------------------------------------

static MaterialParser  theParser;

//
//	LoadMaterialFromFile
//
rxMaterial* LoadMaterialFromFile( const mxChar* filename )
{
	if( rxMaterial * pLastMaterial = theParser.ParseMaterialScript( filename ) )
	{
		return pLastMaterial;
	} else {
		sys::Warning( TEXT("Failed to load material script '%s'"), filename );
		return null;
	}
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
