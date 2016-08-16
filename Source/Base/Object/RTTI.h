/*
=============================================================================
	File:	RTTI.h
	Desc:	Classes for run-time type checking
			and run-time instancing of objects.
=============================================================================
*/

#ifndef __MX_RTTI_H__
#define __MX_RTTI_H__

/*
=============================================================================

	Run-time type information.

=============================================================================
*/

//---------------------------------------------------------------------------
//	DECLARE_CLASS( classname ) must be included in the definition of any subclass of mxObject (usually in header files).
//	This must be used on single inheritance concrete classes only!
//---------------------------------------------------------------------------
//
#define DECLARE_CLASS( className )				\
	public:										\
	static mxTypeInfo		Type;				\
	virtual mxTypeInfo *	GetType() const;	\
	/* creates a new instance of this class */	\
	static mxObject *		CreateFunc();		\
	static className *		New();				\
	static bool		RegisterWithFactory();		\
	private:

//---------------------------------------------------------------------------
//	DECLARE_ABSTRACT_CLASS( classname ) must be included in the definition of any abstract subclass of mxObject (usually in header files).
//	This must be used on single inheritance abstract classes only!
//---------------------------------------------------------------------------
//
#define DECLARE_ABSTRACT_CLASS( className )		\
	public:										\
	static mxTypeInfo		Type;				\
	virtual mxTypeInfo *	GetType() const;	\
	private:

//---------------------------------------------------------------------------
//	DEFINE_CLASS must be included in the implementation of any subclass of mxObject (usually in source files).
//---------------------------------------------------------------------------
//
#define DEFINE_CLASS( className, fourCC, superClassName )	\
	mxTypeInfo	className::Type( #className, fourCC, &superClassName::Type, sizeof(className), className::CreateFunc ); \
    mxTypeInfo*	className::GetType() const { return & this->Type; } \
	mxObject * className::CreateFunc() \
	{ \
		return className::New(); \
	} \
    className*	className::New() \
    { \
        className * newObject = MX_NEW( className ); \
        return newObject; \
    } \
    bool className::RegisterWithFactory() \
    { \
        if ( ! mxObjectFactory::GetInstance()->ClassExists( #className ) ) \
        { \
            mxObjectFactory::GetInstance()->RegisterClass( &className::Type, #className, fourCC ); \
        } \
        return true; \
    }

//---------------------------------------------------------------------------
//	DEFINE_ABSTRACT_CLASS
//---------------------------------------------------------------------------
//
#define DEFINE_ABSTRACT_CLASS( className, fourCC, superClassName )	\
	mxTypeInfo	className::Type( #className, fourCC, &superClassName::Type, sizeof(className), null ); \
    mxTypeInfo*	className::GetType() const { return & this->Type; }

//---------------------------------------------------------------------------

namespace abc {

// Forward declarations.
class mxObject;

//
//	mxTypeInfo - class for dynamic type information.
//
class mxTypeInfo {
public:
	bool	operator == ( const mxTypeInfo& other ) const;
	bool	operator != ( const mxTypeInfo& other ) const;

	const String &	GetClassName() const;
	FourCC			GetTypeCode() const;

	const mxTypeInfo *	GetParent() const;

				// Returns the size of a single instance of the class, in bytes.
	mxSizeT		GetInstanceSize() const;

	bool	IsDerivedFrom( const mxTypeInfo& other ) const;
	bool	IsDerivedFrom( const String& className ) const;
	bool	IsDerivedFrom( const FourCC& typeCode ) const;

public:
	typedef mxObject* (*CreateFunc) (void);	// callback for creating mxObject instances

	mxTypeInfo( const char* theClassName, FourCC theTypeCode,
		const mxTypeInfo* theParent, mxSizeT theInstanceSize,
		CreateFunc theCreateFunction );

	mxObject *	CreateInstance() const;

private:
	const FourCC	fourCC;		// 32-bit type code
	const String	name;		// name of the class
	const mxTypeInfo *	parent;		// parent class of this class
	const mxSizeT	instanceSize;	// size of a single instance of the class
	const CreateFunc createFunc;

private:
	NO_COPY_CONSTRUCTOR( mxTypeInfo );
	NO_ASSIGNMENT( mxTypeInfo );
};

FORCEINLINE
bool mxTypeInfo::operator == ( const mxTypeInfo& other ) const
{
	return ( this == &other );
}

FORCEINLINE
bool mxTypeInfo::operator != ( const mxTypeInfo& other ) const
{
	return ( this != &other );
}

FORCEINLINE
const String & mxTypeInfo::GetClassName() const
{
	return this->name;
}

FORCEINLINE
FourCC mxTypeInfo::GetTypeCode() const
{
	return this->fourCC;
}

FORCEINLINE
const mxTypeInfo * mxTypeInfo::GetParent() const
{
	return this->parent;
}

FORCEINLINE
mxSizeT mxTypeInfo::GetInstanceSize() const
{
	return this->instanceSize;
}

}//End of namespace abc

#endif // !__MX_RTTI_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
