/*
=============================================================================
	File:	EntitySystem.cpp
	Desc:	Scene entity manager implementation.
			(Component-based entity system using properties.)
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>

namespace abc {

/*================================
		mxEntityComponent
================================*/

DEFINE_ABSTRACT_CLASS( mxEntityComponent, 'COMT', mxMessagePort );

mxEntityComponent::mxEntityComponent()
{}

mxEntityComponent::mxEntityComponent( TPtr<mxEntity> theOwner )
	: owner( theOwner )
{
	Assert( this->owner.IsValid() );
}

mxEntityComponent::~mxEntityComponent()
{
	// Detach from the old parent.
	Detach();
}

void mxEntityComponent::SetOwner( TPtr<mxEntity> newOwner )
{
	Assert( newOwner.IsValid() );

	// Detach from the old parent.
	Detach();

	this->owner = newOwner;
}

bool mxEntityComponent::HasOwner() const
{
	return ( this->owner.IsValid() );
}

mxEntity * mxEntityComponent::GetOwner()
{
	Assert( this->owner.IsValid() );
	return this->owner;
}

void mxEntityComponent::Detach()
{
	if ( this->HasOwner() ) {
		this->GetOwner()->RemoveComponent( * this->GetType() );
		this->owner = null;
	}
}

/*================================
		mxEntity
================================*/

DEFINE_CLASS( mxEntity, 'ENTY', mxObject );

//
// Static members.
//
mxEntity::EntityID mxEntity::_entityIdCounter = 0;
mxEntity::EntityID mxEntity::_totalNumEntities = 0;

mxEntity::mxEntity()
	: uniqueID( _entityIdCounter++ )
	, graphics( & rxNullDrawEntity::dummy )
{
	spawnNode.SetOwner( this );

	mxEngine::get().GetEntitySystem().LinkEntity( this );

	++_totalNumEntities;
}

mxEntity::~mxEntity()
{
	// destroy built-in components
	this->graphics = null;

	//this->spatial->SetOwner( null );
	this->spatial = null;

	//----------------------------------------
	
	mxEngine::get().GetEntitySystem().UnlinkEntity( this );

	for ( IndexT iProperty = 0; iProperty < components.Num(); iProperty++ )
	{
		components[ iProperty ]->owner = null;
	}

	--_totalNumEntities;
}

mxEntity::EntityID mxEntity::GetUniqueID() const
{
	return this->uniqueID;
}

void mxEntity::AddComponent( TComponentPtr& newComponent )
{
	Assert( components.Find( newComponent ) == false );
	Assert( FindComponent( * newComponent->GetType() ) == null );

	//TODO: Detach the component from its previous owner if needed.
	Assert( ! newComponent->HasOwner() );

	newComponent->SetOwner( this );

	this->components.Append( newComponent );
}

void mxEntity::RemoveComponent( TComponentPtr& theComponent )
{
	if ( this->components.Remove( theComponent ) ) {
		theComponent->owner = null;
	}
}

void mxEntity::RemoveComponent( const mxTypeInfo& componentType )
{
	for ( IndexT iProperty = 0; iProperty < components.Num(); iProperty++ )
	{
		if ( componentType == * components[ iProperty ]->GetType() ) {
			components[ iProperty ]->owner = null;
			components.RemoveIndex( iProperty );
			break;
		}
	}
}

TComponentPtr mxEntity::FindComponent( const TComponentPtr& component ) const
{
	for ( IndexT iProperty = 0; iProperty < components.Num(); iProperty++ )
	{
		if ( component == components[ iProperty ] ) {
			return component;
		}
	}
	return TComponentPtr();
}

TComponentPtr mxEntity::FindComponent( const mxTypeInfo& componentType ) const
{
	for ( IndexT iProperty = 0; iProperty < components.Num(); iProperty++ )
	{
		TComponentPtr  pComponent( components[ iProperty ] );
		if ( pComponent->IsA( componentType ) ) {
			return pComponent;
		}
	}
	return TComponentPtr();
}

mxUInt mxEntity::NumComponents() const
{
	return components.Num();
}

void mxEntity::HandleMessage( const TMessagePtr& msg )
{
	MX_PROFILE( "Entity : HandleMessage" );
	for ( IndexT iProperty = 0; iProperty < components.Num(); iProperty++ )
	{
		TComponentPtr pComponent( components[ iProperty ] );
		pComponent->HandleMessage( msg );
	}
}

// [ static ]
mxUInt mxEntity::GetTotalCount()
{
	return _totalNumEntities;
}

void mxEntity::SetGraphics( rxDrawEntity* newDrawEntity )
{
	AssertPtr( newDrawEntity );
	this->graphics = newDrawEntity;
}

rxDrawEntity * mxEntity::GetGraphics() {
	return this->graphics;
}

void mxEntity::SetSpatialProxy( mxSpatialProxy* newSpatialProxy )
{
	AssertPtr( newSpatialProxy );
	this->spatial = newSpatialProxy;
	newSpatialProxy->SetOwner( this );
}

mxSpatialProxy* mxEntity::GetSpatialProxy() {
	return this->spatial.get();
}

/*================================
	mxEntitySystem
================================*/

mxEntitySystem::mxEntitySystem()
{
	// singleton
	ENSURE_ONE_CALL;
}

mxEntitySystem::~mxEntitySystem()
{
}

void mxEntitySystem::Initialize()
{}

void mxEntitySystem::Shutdown()
{
	// Collect garbage, delete unused entities.
	DeleteAllEntities();
}

void mxEntitySystem::Tick( const mxTime deltaTime )
{
	// Nothing here.
	// May iterate over all entities and call Tick() on them
}

void mxEntitySystem::LinkEntity( mxEntity* newEntity )
{
	AssertPtr( newEntity );
	newEntity->spawnNode.AddToEnd( this->spawnedEntities );
}

void mxEntitySystem::UnlinkEntity( mxEntity* theEntity )
{
	AssertPtr( theEntity );
	theEntity->spawnNode.Remove();
}

void mxEntitySystem::DeleteAllEntities()
{
	mxEntity * pEntity = this->spawnedEntities.Next();
	while ( pEntity != NULL )
	{
		mxEntity * pNextEntity = pEntity->spawnNode.Next();
		MX_FREE( pEntity );
		pEntity = pNextEntity;
	}
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
