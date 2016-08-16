/*
=============================================================================
	File:	Files.cpp
	Desc:	Very simple file system.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <Base.h>

namespace abc {

/*================================
		Global variables
================================*/

namespace {

static mxFileSystem * GFileSys = null;	// singleton access

}//end of anonymous namespace

/*================================
		mxDir
================================*/

mxDir::mxDir()
{
	_sprintf( this->dirName, sys::GetLauchDirectory() );
}

mxDir::mxDir( const mxChar* directoryName, ... )
{
	_sprintf( this->dirName, directoryName );
}

mxDir::~mxDir()
{}

const mxChar * mxDir::GetName() const
{
	return this->dirName.c_str();
}

bool mxDir::Exists() const
{
	return ( sys::PathExists( this->dirName.ToChar() ) );
}

bool mxDir::ContainsFile( const mxFilePath& fileName ) const
{
	mxChar  absPath[ sys::MAX_PATH_CHARS ];
	String::snPrintf( absPath, sizeof(absPath),
		"%s%s", this->GetName(), fileName.GetName() );

	return ( sys::FileExists( absPath ) );
}

bool mxDir::IsValid() const
{
	return ( sys::IsValidPathName( this->dirName.ToChar() ) );
}

/*================================
		mxFilePath
================================*/

mxFilePath::mxFilePath()
{}

mxFilePath::mxFilePath( const mxChar* pathName, ... )
{
	_sprintf( this->fileName, pathName );
	this->fileName.ExtractFileBase( this->baseName );
	this->fileName.ExtractFileExtension( this->extension );
}

mxFilePath::~mxFilePath()
{}

const mxChar * mxFilePath::GetName() const
{
	return this->fileName.ToChar();
}

const mxChar * mxFilePath::GetBaseName() const
{
	return this->baseName.ToChar();
}

const mxChar * mxFilePath::GetExtension() const
{
	return this->extension.ToChar();
}

bool mxFilePath::Exists() const
{
	MX_OPTIMIZE( "too many FileExists() calls, many redundant ones" )

	if ( sys::FileExists( this->fileName.ToChar() ) )
	{
		return true;
	}

	String  tmp;
	if( GFileSys && GFileSys->FindFile( *this, tmp ) )
	{
		this->fileName = tmp;
		return true;
	}
	return false;
}

bool mxFilePath::IsValid() const
{
	return ( sys::IsValidFileName( this->fileName.ToChar() ) )
		&& ( sys::IsValidFileExtension( this->extension.ToChar() ) );
}

/*================================
		mxFileSystem
================================*/

mxFileSystem::mxFileSystem()
{
	// singleton
	ENSURE_ONE_CALL;
	GFileSys = this;

	this->AddDir( "" );
}

mxFileSystem::~mxFileSystem()
{
	GFileSys = null;
}

//
//	mxFileSystem::AddDir
//
void mxFileSystem::AddDir( const mxChar* path )
{
#if 0
	if ( sys::IsValidPathName( path )
		&& sys::PathExists( path ) )
	{
		this->folders.Append( path );
	}
	else {
		sys::Print( "Path '%s' is invalid\n", path );
	}
#else
	this->folders.Append( path );
#endif
}

//
//	mxFileSystem::FindFile
//
bool mxFileSystem::FindFile( const mxFilePath& fileName, String &OutFileName )
{
	Assert( fileName.IsValid() );

	for ( IndexT iDir = 0; iDir < folders.Num(); iDir++ )
	{
		const mxDir & dir = this->folders[ iDir ];
		
		OutFileName.Empty();
		_sprintf( OutFileName, "%s%s", dir.GetName(), fileName.GetName() );
		if ( sys::FileExists( OutFileName.ToChar() ) )
		{
			return true;
		}
	}
	return false;
}

//
//	mxFileSystem::FileExists
//
bool mxFileSystem::FileExists( const mxFilePath& fileName ) const
{
	Assert( fileName.IsValid() );

	for ( IndexT iDir = 0; iDir < folders.Num(); iDir++ )
	{
		const mxDir & dir = this->folders[ iDir ];
		
		if ( dir.ContainsFile( fileName ) )
		{
			return true;
		}
	}
	return false;
}

/*================================
		mxFile_ReadOnly
================================*/

mxFile_ReadOnly::mxFile_ReadOnly( const mxFilePath& filename )
	: m_pFILE( null )
	, m_length( 0 )
	, m_name( filename.GetName() )
{
	Assert( filename.IsValid() );
	if ( ! filename.Exists() ) {
		sys::Warning( "File '%s' not found\n", filename.GetName() );
		return;
	}

	m_pFILE = fopen( filename.GetName(), "r" );
	AssertPtr( m_pFILE );
	if ( ! m_pFILE ) {
		sys::Warning( "Failed to open file '%s' for reading", filename );
	}

	// Get the length of the file.
	fseek( m_pFILE, 0, SEEK_END );
	m_length = ftell( m_pFILE );
	fseek( m_pFILE, 0, SEEK_SET );
}

mxFile_ReadOnly::~mxFile_ReadOnly()
{
	if ( m_pFILE ) {
		fclose( m_pFILE );
	}
}

SizeT mxFile_ReadOnly::Read( void* pBuffer, SizeT numBytes )
{
	AssertPtr( m_pFILE );
	return fread( pBuffer, sizeof(BYTE), numBytes, m_pFILE );
}

SizeT mxFile_ReadOnly::Write( const void* pBuffer, SizeT numBytes )
{
	InvalidCall;
	return 0;
}

void mxFile_ReadOnly::Seek( const SizeT offset )
{
	AssertPtr( m_pFILE );
	fseek( m_pFILE, static_cast<mxLong>( offset ), SEEK_SET );
}

void mxFile_ReadOnly::Skip( mxLong offset )
{
	AssertPtr( m_pFILE );
	fseek( m_pFILE, offset, SEEK_CUR );
}

const mxChar * mxFile_ReadOnly::GetName() const
{
	return m_name.GetName();
}

SizeT mxFile_ReadOnly::GetSize() const
{
	AssertPtr( m_pFILE );
	return m_length;
}

SizeT mxFile_ReadOnly::Tell() const
{
	return static_cast< SizeT >( ftell( m_pFILE ) );
}

bool mxFile_ReadOnly::IsOpen() const
{
	return ( null != this->m_pFILE );
}

bool mxFile_ReadOnly::AtEnd() const
{
	if ( ! m_pFILE ) {
		return true;
	}

	const SizeT reachedPos = ftell( m_pFILE );

	// NOTE: '>=' is not a bug !
	return ( reachedPos >= this->m_length );
}

bool mxFile_ReadOnly::IsOk() const
{
	return ( null != this->m_pFILE );
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
