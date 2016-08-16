/*
=============================================================================
	File:	DataStream.h
	Desc:	Input/Output system interfaces.
=============================================================================
*/

#ifndef __MX_DATA_STREAM_H__
#define __MX_DATA_STREAM_H__

namespace abc {

/*
===========================================================

	Input/Output.

===========================================================
*/

//
//	mxDataStream - abstract interface for writing/reading bytes of data.
//
class mxDataStream {
public:

	// Read the given number of bytes into the buffer and return the number of bytes actually read.
	//
	virtual SizeT	Read( void* pBuffer, SizeT numBytes ) = 0;

	// Write to the stream and return the number of bytes written.
	//
	virtual SizeT	Write( const void* pBuffer, SizeT numBytes ) = 0;

	// Set the file pointer to the given offset from the beginning.
	//
	virtual void	Seek( const SizeT offset ) = 0;

	// NOTE: 'offset' can be negative.
	//
	virtual void	Skip( mxLong offset ) = 0;

	// Return the size of data (length of the file if this is a file), in bytes.
	//
	virtual SizeT	GetSize() const = 0;

	// Returns the current byte offset from beginning.
	//
	virtual SizeT	Tell() const = 0;

	virtual bool	IsOpen() const = 0;

	// Returns true if EOF has been reached.
	virtual bool	AtEnd() const = 0;

	// Checks for testing & debugging.
	virtual bool	IsOk() const { return true; }

public:
	// These functions are provided for convenience.

	mxInt16 ReadShort()
	{
		mxInt16 result;
		this->Read( &result, sizeof(result) );
		return result;
	}
	mxInt ReadInt()
	{
		mxInt result;
		this->Read( &result, sizeof(result) );
		return result;
	}
	mxReal ReadFloat()
	{
		mxReal result;
		this->Read( &result, sizeof(result) );
		return result;
	}

public:
	virtual	~mxDataStream() {}
};

}//End of namespace abc

#endif // ! __MX_DATA_STREAM_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
