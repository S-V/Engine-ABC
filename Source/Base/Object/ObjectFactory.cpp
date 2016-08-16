/*
=============================================================================
	File:	ObjectFactory.cpp
	Desc:	Object factory for run-time class instancing and type information.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <Base.h>

namespace abc {

//
//	Singleton.
//
mxObjectFactory * mxObjectFactory::theFactory = NULL;

//
//	mxObjectFactory::GetInstance
//
mxObjectFactory * mxObjectFactory::GetInstance()
{
	if ( 0 == theFactory )
	{
		theFactory = MX_NEW( mxObjectFactory );
		AssertPtr( theFactory );
	}
	return theFactory;
}

//
//	mxObjectFactory::Destroy
//
void mxObjectFactory::Destroy()
{
	if ( 0 != theFactory )
    {
        MX_FREE( theFactory );
        theFactory = 0;
    }
}

/*================================
		mxObjectFactory
================================*/

mxObjectFactory::mxObjectFactory()
{
}

mxObjectFactory::~mxObjectFactory()
{
	this->typesByName.Clear();
	this->typesByID.Clear();
}

//
//	mxObjectFactory::RegisterClass
//
void mxObjectFactory::RegisterClass( const mxTypeInfo* typeInfo, const String& className, const FourCC& typeCode )
{
	AssertPtr( typeInfo );

    // check if class name already exists
    if ( this->ClassExists( className ) )
    {
        String errorMsg;
        _sprintf( errorMsg, "Class with name '%s' has already been registered!", className.c_str() );
        sys::Error( errorMsg.c_str() );
        return;
    }

    // check if class fourcc already exists
    if ( this->ClassExists( typeCode ) )
    {
        // NOTE: can't use n_error() here, because Console is not setup yet
        // when this method Register() is called!
        String errorMsg;
        _sprintf( errorMsg, "Class with fourcc '%s' (name: %s) has already been registered!", 
            typeCode.AsString().c_str(),
            className.c_str() );
        sys::Error( errorMsg.c_str() );
        return;
    }

    // register with lookup tables
    this->typesByName.Set( className, typeInfo );
	this->typesByID.Insert( typeCode, typeInfo );
}

bool mxObjectFactory::ClassExists( const String& className ) const
{
	return this->typesByName.Contains( className.c_str() );
}

bool mxObjectFactory::ClassExists( const FourCC& typeCode ) const
{
	return ( this->typesByID.Find( typeCode ) != 0 );
}

const mxTypeInfo * mxObjectFactory::GetTypeInfo( const String& className ) const
{
	Assert( this->ClassExists( className ) );
    return this->typesByName[ className ];
}

const mxTypeInfo * mxObjectFactory::GetTypeInfo( const FourCC& typeCode ) const
{
	Assert( this->ClassExists( typeCode ) );
    return this->typesByID.Find( typeCode )->GetValue();
}

mxObject * mxObjectFactory::Create( const String& className ) const
{
	if ( ! this->ClassExists( className ) )
	{
		sys::Error( "Failed to create class '%s'. Did you forget to register it?", className.ToChar() );
		return null;
	}

	const mxTypeInfo* pRtti = this->typesByName[ className.ToChar() ];
	AssertPtr( pRtti );

	return pRtti->CreateInstance();
}

mxObject * mxObjectFactory::Create( const FourCC typeCode ) const
{
	if ( ! this->ClassExists( typeCode ) )
	{
		sys::Error( "Failed to create class with type code '%s'. Did you forget to register it?",
			typeCode.AsString().ToChar() );
		return null;
	}

	const mxTypeInfo* pRtti = this->typesByID.Find( typeCode )->GetValue();
	AssertPtr( pRtti );

	return pRtti->CreateInstance();
}

}//end of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
