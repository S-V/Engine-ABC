/*
=============================================================================
	File:	Files.h
	Desc:	Very simple file system.
=============================================================================
*/

#ifndef __MX_FILE_SYSTEM_H__
#define __MX_FILE_SYSTEM_H__

namespace abc {

/*
===========================================================

		File system.

===========================================================
*/

// Forward declarations.
class mxDir;
class mxFilePath;
class mxFile;

//
//	mxDirName - directory name.
//
typedef String	mxDirName;

//
//	mxFileName - file name.
//
typedef String	mxFileName;

//
//	mxDir - represents a directory, a folder containing zero ore more files.
//
class mxDir {
public:
			mxDir();	// empty ctor - initialized to the current directory
			mxDir( const mxChar* directoryName, ... );
			~mxDir();

	// may returns smth like e.g. "home/dir1/dir3"
	const mxChar * GetName() const;
	
	// returns true if such directory exists; otherwise returns false
	bool Exists() const;

	// returns true if this directory contains a file with the given name
	bool ContainsFile( const mxFilePath& fileName ) const;

	// returns true if this is a root directory
	bool IsRoot() const;

	bool IsValid() const;

private:
	mxDirName	dirName;
};

//
//	mxFilePath - path to a file on a local hard drive.
//
class mxFilePath {
public:
			mxFilePath();
			mxFilePath( const mxChar* pathName, ... );
			~mxFilePath();

	// returns the original file name (which can be either relative or absolute)
	const mxChar * GetName() const;

	const mxChar * GetBaseName() const;
	const mxChar * GetExtension() const;

	// returns true if such file exists; otherwise returns false
	bool Exists() const;

	// returns true if this is a valid file name
	bool IsValid() const;

private:
	mutable mxFileName	fileName;	// original filename (can be modified during queries)

	String		baseName;	// cached file base name
	String		extension;	// cached file extension
};

//
//	mxFileSystem - responsible for searching/creating/opening/etc files.
//
//	Notes: only one instance of mxFileSystem can be created!
//
class mxFileSystem {
public:
			// Add a new path where files will be searched.
	void	AddDir( const mxChar* path );

			// Searches registered folders for a file with the given name
			// and returns the absolute file name (including full path name) if such file exists;
			// returns the original name in case of failure.
	bool	FindFile( const mxFilePath& fileName, String &OutFileName );

			// Searches registered folders for a file with the given name;
			// returns 'true' if such file exists, 'false' otherwise.
	bool	FileExists( const mxFilePath& fileName ) const;

private:
	// Only mxEngine can create/destroy mxFileSystem.
	friend class mxEngine;

	mxFileSystem();
	~mxFileSystem();

private:
	TArray< mxDir >	folders;	// search files in these directories
};

//=====================================================================

//
//	ESeekMode - parameters for file operations.
//
enum ESeekMode
{
	FILE_SEEK_CURRENT,
	FILE_SEEK_END,
	FILE_SEEK_SET,
};

//
//	mxFile - base class for file classes.
//
class mxFile : public mxDataStream {
public:
	// Returns the current file offset.
	virtual SizeT		Tell() = 0;

	// Seek on the file.
	virtual SizeT		Seek( SizeT  offset, ESeekMode ) = 0;

	// Go back to the beginning of the file.
	virtual void		Rewind() = 0;

	virtual void		PrintF( const mxChar* format, ... ) ATTRIBUTE( (format(printf,1,2)) ) = 0;
	virtual void		VPrintF( const mxChar* format, va_list args ) = 0;

	// Returns the name of the file.
	virtual const mxChar *	GetName() const = 0;

	// Returns the name of the full path to this file.
	virtual const mxChar *	GetFullPath() const = 0;

	// Returns the last modification time of this file.
	//virtual time_t	GetTimestamp() const = 0;

protected:
	mxFile() {}
	virtual ~mxFile() {}
};

//
//	EFileMode - describes file access types.
//
enum EFileMode
{
	ReadOnly,		// Open a file for reading.
	WriteOnly,		// Truncate to zero length or create a file for writing.
	ReadAndWrite,	// Open a file for update (reading and writing).
};

//
//	mxFile_ReadOnly - represents a read-only file on the local hard drive.
//
class mxFile_ReadOnly : public mxDataStream {
public:
	mxFile_ReadOnly( const mxFilePath& filename );
	~mxFile_ReadOnly();	// automatically closes the file (if it's open)

	//
	//	Override ( mxDataStream ) :
	//
	SizeT	Read( void* pBuffer, SizeT numBytes );
	SizeT	Write( const void* pBuffer, SizeT numBytes );

	void	Seek( const SizeT offset );

	void	Skip( mxLong offset );

	SizeT	GetSize() const;
	SizeT	Tell() const;

	bool	IsOpen() const;

	bool	AtEnd() const;

	bool	IsOk() const;

	//
	//	Override ( mxFile ) :
	//
	const mxChar *	GetName() const;

private:
	mxFilePath	m_name;		// relative file path
	FILE *		m_pFILE;	// file handle
	SizeT		m_length;	// file size, in bytes
};

}//End of namespace abc

#endif // ! __MX_FILE_SYSTEM_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
