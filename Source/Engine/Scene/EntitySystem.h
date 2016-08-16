/*
=============================================================================
	File:	EntitySystem.h
	Desc:	Scene entity management.
			(Component-based entity system using properties.)
=============================================================================
*/

#ifndef __MX_ENTITY_SYSTEM_H__
#define __MX_ENTITY_SYSTEM_H__

namespace abc {

// Forward declarations.
class mxTypeInfo;
class mxEntity;
class mxEntityComponent;
class mxSpatialProxy;

/*
=============================================================================

	Components (properties) are basic building blocks of scene entities.
	They implement specific functionality and they can be attached to scene entities.
	Components are derived from message port so they can receive messages.

=============================================================================
*/

//
//	mxEntityComponent - is a base class for scene entity components.
//
class mxEntityComponent : public mxMessagePort, public ReferenceCounted
{
	DECLARE_ABSTRACT_CLASS( mxEntityComponent );

public:
				mxEntityComponent();
	explicit	mxEntityComponent( TPtr<mxEntity> theOwner );
	virtual		~mxEntityComponent();

	void		SetOwner( TPtr<mxEntity> newOwner );

				// Is this component attached to an entity?
	bool		HasOwner() const;

				// Note: this will trigger 'Assertion' if this component is not attached to an entity.
	mxEntity *	GetOwner();

			// Detach this component from the current owner.
	virtual void Detach();

private:
	friend class mxEntity;

private:
	TPtr< mxEntity >	owner;
};

//
//	TComponentPtr
//
class TComponentPtr : public RefPtr<mxEntityComponent>
{
public:
	TComponentPtr()
	{}
	TComponentPtr( const TComponentPtr& other )
	{
		*this = other;
	}
	TComponentPtr( mxEntityComponent* p )
		: RefPtr( p )
	{}
	~TComponentPtr()
	{}

	void operator = ( mxEntityComponent* p )
	{
		RefPtr::operator = ( p );
	}
	void operator = ( const TComponentPtr& other )
	{
		RefPtr::operator = ( other );
	}

	bool IsValid() const
	{
		return ( *this != NULL );
	}
	bool IsNull() const
	{
		return ( *this == NULL );
	}
};

/*
=============================================================================
		Some random notes on scene entities.

	Each entity has its own unique properties, i.e. components
	are not shared among multiple instances of game entities.
=============================================================================
*/

//
//	mxEntity - base class for scene entities.
//
//	Entities usually represent discrete dynamic (movable) scene objects.
//	Entities are containers for storing components.
//	mxEntity takes full ownership over its components
//	(i.e. is responsible for deleting them).
//
class mxEntity : public mxObject
{
	DECLARE_CLASS( mxEntity );

	friend class mxEntitySystem;

public:
				mxEntity();
	virtual		~mxEntity();

	//-----------------------------------------------------------
	//	Properties.
	//-----------------------------------------------------------

	void	AddComponent( TComponentPtr& newComponent );

	void	RemoveComponent( TComponentPtr& theComponent );	// slow function!
	void	RemoveComponent( const mxTypeInfo& componentType );	// slow function!

	TComponentPtr	FindComponent( const TComponentPtr& component ) const;	// slow function
	TComponentPtr	FindComponent( const mxTypeInfo& componentType ) const;	// slow function

	mxUInt	NumComponents() const;

	//-----------------------------------------------------------
	//	Message/Event handling.
	//-----------------------------------------------------------

	// Passes the given message to all properties of this entity.
	//
	void HandleMessage( const TMessagePtr& msg );

public:

	//-----------------------------------------------------------
	//	Thinking / Updating.
	//-----------------------------------------------------------

	virtual void	Think( const mxTime deltaTime ) {};

	//-----------------------------------------------------------
	//	Testing and Debugging.
	//-----------------------------------------------------------

	virtual void	Validate() {};

	//-----------------------------------------------------------
	//	Other functions and attributes.
	//-----------------------------------------------------------

	typedef UINT32	EntityID;	// Type used to uniquely identify entities.

	EntityID	GetUniqueID() const;

	static mxUInt	GetTotalCount();	// returns the total number of spawned entities

public:

	// Built-in components.

	void			SetGraphics( rxDrawEntity* newDrawEntity );
	rxDrawEntity *	GetGraphics();

	void			SetSpatialProxy( mxSpatialProxy* newSpatialProxy );
	mxSpatialProxy*	GetSpatialProxy();

private:
	// 'Renderable' interface, visuals of this entity. This pointer can NOT be null.
	TPtr< rxDrawEntity >	graphics;	// it's a hack, but we're building a graphics engine, right?

	// Spatial proxy of this entity (for being inserted into spatial databases). This pointer can be null.
	RefPtr< mxSpatialProxy >	spatial;

private:

	// References to all components.

	TArray< TComponentPtr >		components;	// all properties of this entity

	// Unique id of this entity (can we use smth like "(EntityID)this"? ).
	const EntityID				uniqueID;

	TCircularList< mxEntity >	spawnNode;	// for being linked into the global entity list

private:
	static EntityID	_entityIdCounter;	// for generating unique IDs
	static mxUInt	_totalNumEntities;

private:
	// prevent assignment and copying
	NO_COPY_CONSTRUCTOR( mxEntity );
	NO_ASSIGNMENT( mxEntity );
};

//
//	mxEntityList
//
typedef TArray< mxEntity* >	mxEntityList;

//
//	mxEntityCache - temporary storage of references to entities collected during ray casts, etc.
//
typedef TArray< mxEntity* >	mxEntityCache;

//
//	mxEntitySystem - entity manager.
//
//	Creates/loads/deletes entities and provides quick access to entities by their names, etc.
//
class mxEntitySystem {
public:

	//--------------------------------------------------------------------
	//	Utilities.
	//--------------------------------------------------------------------

public:
	//--------------------------------------------------------------------
	//	Internal functions. Don't use them!
	//--------------------------------------------------------------------

			// Register a new entity for garbage collection, etc.
	void	LinkEntity( mxEntity* newEntity );

			// Unregisters the given entity.
	void	UnlinkEntity( mxEntity* theEntity );

private:
	// all created entities for garbage collection
	TCircularList< mxEntity >	spawnedEntities;

private:
	//--------------------------------------------------------------------
	//	Private internal functions.
	//--------------------------------------------------------------------

	void	Tick( const mxTime deltaTime );

			// Delete all spawned entities in the list.
	void	DeleteAllEntities();

private:
	// mxEntitySystem is singleton, its instance can only be created by the main engine.
	friend class mxEngine;

	mxEntitySystem();
	~mxEntitySystem();

	void	Initialize();
	void	Shutdown();
};

}//End of namespace abc

#endif // ! __MX_ENTITY_SYSTEM_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
