/*
=============================================================================
	File:	ObjectFactory.h
	Desc:	mxObject factory - creates objects by a class name.
			mxObjectFactory is a singleton.
			Classes register themselves in the factory
			through the macros DECLARE_CLASS and DEFINE_CLASS.
=============================================================================
*/

#ifndef __MX_ENGINE_CORE_OBJECT_FACTORY_H__
#define __MX_ENGINE_CORE_OBJECT_FACTORY_H__

namespace abc {

// Forward declarations.
class mxTypeInfo;
class String;
class FourCC;

class mxObject;

//
//	mxObjectFactory
//
class mxObjectFactory {
public:
	// singleton access
	// (we cannot use the Singleton template because ctor shouldn't be accessible to Singleton).

	static mxObjectFactory *	GetInstance();

	// NOTE: this must be called at the end of the main function to delete the instance of mxObjectFactory.
	static void			Destroy();

	//------------------------------------------------

	void	RegisterClass( const mxTypeInfo* typeInfo, const String& className, const FourCC& typeCode );

	bool	ClassExists( const String& className ) const;
	bool	ClassExists( const FourCC& typeCode ) const;

	const mxTypeInfo *	GetTypeInfo( const String& className ) const;
	const mxTypeInfo *	GetTypeInfo( const FourCC& typeCode ) const;

				// Create an object by its class name.
	mxObject *	Create( const String& className ) const;

				// Create an object by its type code.
	mxObject *	Create( const FourCC typeCode ) const;

private:
	// constructor and destructor should be private!
	mxObjectFactory();
	~mxObjectFactory();

	TStringHash< const mxTypeInfo* >		typesByName;	// for fast lookup by class name
	RBTreeMap< FourCC, const mxTypeInfo* >	typesByID;		// for fast lookup by FourCC code

private:
	static mxObjectFactory * theFactory;	// pointer to the only instance of mxObjectFactory
};

}//End of namespace abc

#endif // !__MX_ENGINE_CORE_OBJECT_FACTORY_H__

