/*
=============================================================================
	File:	RTTI.cpp
	Desc:	Classes for run-time type checking
			and run-time instancing of objects.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <Base.h>

namespace abc {

/*================================
		mxTypeInfo
================================*/

mxTypeInfo::mxTypeInfo( const char* theClassName, FourCC theTypeCode,
		const mxTypeInfo* theParent, mxSizeT theInstanceSize,
		CreateFunc theCreateFunction )
	
	: fourCC( theTypeCode )
	, name( theClassName )
	, parent( theParent )
	, instanceSize( theInstanceSize )
	, createFunc( theCreateFunction )
{
	Assert( theClassName != NULL );
	Assert( theTypeCode.IsValid() );
	Assert( theParent != this );	// but theParent can be NULL
	Assert( theInstanceSize > 0 );
	
	// theCreateFunction can be null

	// register class with factory
    if ( ! mxObjectFactory::GetInstance()->ClassExists( fourCC ) )
    {
        mxObjectFactory::GetInstance()->RegisterClass( this, this->name, fourCC );        
    }
    
    // make a debug check that no name/fourCC collission occured, but only in debug mode
#ifdef MX_DEBUG
    const mxTypeInfo* checkRtti = mxObjectFactory::GetInstance()->GetTypeInfo( fourCC );
    Assert( NULL != checkRtti );
    if ( checkRtti != this )
    {
		sys::Error( "Class registry collision: (%s, %s) collides with (%s, %s)!",
            this->name.c_str(), this->fourCC.AsString().c_str(),
            checkRtti->name.c_str(), checkRtti->fourCC.AsString().c_str() );
    }
#endif
}

bool mxTypeInfo::IsDerivedFrom( const mxTypeInfo& other ) const
{
	for ( const mxTypeInfo * current = this; current != 0; current = current->GetParent() )
	{
		if ( current == &other )
		{
			return true;
		}
	}
	return false;
}

bool  mxTypeInfo::IsDerivedFrom( const String& className ) const
{
	for ( const mxTypeInfo * current = this; current != 0; current = current->GetParent() )
	{
		if ( current->GetClassName() == className )
		{
			return true;
		}
	}
	return false;
}

bool  mxTypeInfo::IsDerivedFrom( const FourCC& typeCode ) const
{
	for ( const mxTypeInfo * current = this; current != 0; current = current->GetParent() )
	{
		if ( current->GetTypeCode() == typeCode )
		{
			return true;
		}
	}
	return false;
}

mxObject * mxTypeInfo::CreateInstance() const
{
	if ( null != this->createFunc )
	{
		return this->createFunc();
	}
	else {
		sys::Error( "Can't instantiate abstract class '%s'", this->name.ToChar() );
		return null;
	}
}

}//end of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
