/*
=============================================================================
	File:	Object.h
	Desc:	Base class for many objects, root of our class hierarchy.
=============================================================================
*/

#ifndef __MX_CORE_OBJECT_H__
#define __MX_CORE_OBJECT_H__

namespace abc {

//
//	mxObject - base class for most objects, provides RTTI.
//
class mxObject {
public:
	DECLARE_ABSTRACT_CLASS( mxObject );

public:

	//
	//	RTTI
	//

	bool	IsA( const mxTypeInfo& type ) const;
	bool	IsA( const FourCC& typeCode ) const;
	bool	IsA( const String& typeName ) const;

	bool	IsInstanceOf( const mxTypeInfo& type ) const;
	bool	IsInstanceOf( const FourCC& typeCode ) const;
	bool	IsInstanceOf( const String& typeName ) const;

	// These functions are provided for convenience.

	const String &	GetClassName() const;
	FourCC			GetClassTypeCode() const;

protected:
			mxObject();
	virtual	~mxObject();
};

//
//	DynamicCast< T >
//
template< class TypeTo >
TypeTo* DynamicCast( mxObject* pObject )
{
	AssertPtr( pObject );
	return pObject->IsA( TypeTo::Type ) ?
		static_cast< TypeTo* >( pObject ) : null;
}

//
//	ConstCast< T >
//
template< class TypeTo >
const TypeTo* ConstCast( const mxObject* pObject )
{
	AssertPtr( pObject );
	return pObject->IsA( TypeTo::Type ) ?
		static_cast< const TypeTo* >( pObject ) : null;
}

/*================================
			mxObject
================================*/

MX_INLINE
bool mxObject::IsA( const mxTypeInfo& type ) const
{
	return this->GetType()->IsDerivedFrom( type );
}

MX_INLINE
bool mxObject::IsA( const FourCC& typeCode ) const
{
	return this->GetType()->IsDerivedFrom( typeCode );
}

MX_INLINE
bool mxObject::IsA( const String& typeName ) const
{
	return this->GetType()->IsDerivedFrom( typeName );
}

MX_INLINE
bool mxObject::IsInstanceOf( const mxTypeInfo& type ) const
{
	return ( this->GetType() == & type );
}

MX_INLINE
bool mxObject::IsInstanceOf( const FourCC& typeCode ) const
{
	return ( this->GetType()->GetTypeCode() == typeCode );
}

MX_INLINE
bool mxObject::IsInstanceOf( const String& typeName ) const
{
	return ( this->GetType()->GetClassName() == typeName );
}

MX_INLINE
const String & mxObject::GetClassName() const
{
	return this->GetType()->GetClassName();
}

MX_INLINE
FourCC mxObject::GetClassTypeCode() const
{
	return this->GetType()->GetTypeCode();
}

}//End of namespace abc

#endif // !__MX_CORE_OBJECT_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
