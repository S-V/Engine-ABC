/*
=============================================================================
	File:	Resource.h
	Desc:	Resource management.
=============================================================================
*/

#ifndef __MX_RESOURCE_H__
#define __MX_RESOURCE_H__

namespace abc {

/*
===========================================================
		Name system.
===========================================================
*/

// Maximum allowed length of a single resource name.
enum { MAX_NAME_CHARS = 32 };

//
//	TName
//
template
<
	typename TChar = mxChar,
	mxUInt MaxChars = MAX_NAME_CHARS
>
class TName {
public:
	TName()
	{
		Empty();
	}
	TName( const TChar* name )
	{
		Set( name );
	}

	operator const TChar* () const
	{
		return m_data.Ptr();
	}
	mxUInt	GetSize() const
	{
		return m_data.Num()-1; // last mxChar is null
	}
	void Set( const TChar* pSrc )
	{
		Assert( pSrc );
		const SizeT size = sys::StrLen( pSrc ) * sizeof(TChar);
		Assert( size <= sizeof(m_data) );

		m_data.SetNum( size + 1 );
		MemCopy( m_data.Ptr(), pSrc, size );
		m_data[ size ] = null;
	}
	void Empty()
	{
		m_data.SetNum( 0 );
		m_data.Ptr()[0] = '\0';
	}

	mxUInt GetCapacity() const
	{
		return MaxChars;
	}

private:

	// actual string data is kept here;
	// static array is used to minimize memory fragmentation
	//
	TStaticList< TChar, MaxChars >	m_data;
};

//
//	TNamedElement
//
template
<
	typename TChar = mxChar,
	mxUInt MaxChars = MAX_NAME_CHARS
>
class TNamedElement {
public:
	TNamedElement()
	{}
	TNamedElement( const TChar* name )
		: m_name( name )
	{}

	void SetName( const TChar* name )
	{
		m_name.Set( name );
	}
	const TChar * GetName() const
	{
		return m_name;
	}

private:
	TName< TChar, MaxChars >	m_name;	// unique name of this object
};

/*
===========================================================

	Resource management.

===========================================================
*/

typedef	TNamedElement< mxChar, MAX_NAME_CHARS >	NamedResource;

//
//	mxResourceLoader
//
class mxResourceLoader : public ReferenceCounted {
public:
	virtual	~mxResourceLoader() {}

	// examples of file extensions: "3DS", "OBJ", "BSP".
	//
	virtual bool IsSupportedFileExtension( const mxChar* extension )
	{
		return false;
	}
};

//
//	TResourceSet< T >
//
//	Used for hashing resources by name for quick access.
//
template< class TResourceType >
class TResourceSet {
public:
	// Insert the given resource pointer into the hash table.
	void Insert( const mxChar* name, TResourceType* pResource )
	{	AssertPtr( name );
		if( name )
		{
			this->resourceHash.Set( name, pResource );
		}
	}

	TResourceType* InsertUnique( const mxChar* name, TResourceType* pResource )
	{
		if ( TResourceType * pResource = Find( name ) ) {
			return pResource;
		} else {
			Insert( name, pResource );
			return pResource;
		}
	}

	// Get a pointer to the resource with the given name (returns null on failure).
	TResourceType * Find( const mxChar* name )
	{
		AssertPtr( name );
		TResourceType ** ppResource = null;
		if ( this->resourceHash.Get( name, &ppResource ) )
		{
			return *ppResource;
		}
		return null;
	}

	// Remove the element with the given name from the hashtable.
	void Remove( const mxChar* name )
	{
		AssertPtr( name );
		this->resourceHash.Remove( name );
	}

	// Removes all entries from the hash table.
	void Clear()
	{
		this->resourceHash.Clear();
	}

	// Deletes all entries (assuming that pointers are stored).
	void Delete()
	{
		this->resourceHash.DeleteContents();
	}

	TResourceSet()
	{}

	~TResourceSet()
	{}

private:
	TStringHash< TResourceType* >	resourceHash;	// maps resources to their names
};

//
//	THashedResourcesBase< TResourceType >
//
//	NOTE: derived classes must have methods with the following signatures:
//
//	// invoked if failed to find a resource in the database
//	void OnResourceNotFound(const mxChar* name, TResourceType*& pResource);
//

template< class TResourceType >
class THashedResourcesBase : protected TResourceSet< TResourceType > {
public:
	void Add( const mxChar* name, TResourceType* pointer )
	{	Assert( name );
		if( name )
		{
			this->resourceHash.Set( name, pointer );
		}
	}

	TResourceType * GetByName( const mxChar* name )
	{
		Assert( name );
		TResourceType ** ppResource = null;
		if ( this->resourceHash.Get( name, &ppResource ) )
		{
			return *ppResource;
		}
		TResourceType * result = null;
		OnResourceNotFound( name, result );
		return result;
	}

protected:
	virtual void OnResourceNotFound( const mxChar* name, TResourceType*& pResource ) = 0;
};

}//End of namespace abc

#endif // ! __MX_RESOURCE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
