/*
=============================================================================
	File:	SceneGraph.cpp
	Desc:	Basic scene graph usable for sample applications.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <MiniSG.h>

namespace abc {

static const Vec3D  IDENTITY_DIRECTION( Vec3D::UNIT_Z );

/*================================
		Animator_Rotation
================================*/

void Animator_Rotation::Animate( Node* node, const mxTime dt )
{
	UnusedParameter( dt );
	Vec3D  pos( node->GetOrigin() );
	this->rotation.RotatePoint( pos );
	node->SetOrigin( pos );
}

/*================================
		Animator_Orientation
================================*/

void Animator_Orientation::Animate( Node* node, const mxTime dt )
{
	UnusedParameter( dt );

	Quat newOrientation( node->GetOrientation() * this->rotationQuaternion );
	newOrientation.Normalize();
	node->SetOrientation( newOrientation );
}

/*================================
		Animator_LookAt
================================*/

Animator_LookAt::Animator_LookAt( Node* theTargetNode )
	: targetNode( theTargetNode )
{
	AssertPtr( this->targetNode );
	this->targetNode->Grab();
}

Animator_LookAt::~Animator_LookAt()
{
	this->targetNode->Drop();
}

void Animator_LookAt::Set( Node* theTargetNode )
{
	AssertPtr( theTargetNode );
	this->targetNode = theTargetNode;
}

void Animator_LookAt::Animate( Node* node, const mxTime dt )
{
	UnusedParameter( dt );
Unimplemented;
	Vec3D  newDirection = (this->targetNode->GetOrigin() - node->GetOrigin()).GetNormalized();
	Matrix3 rot( Matrix3::CreateRotation( IDENTITY_DIRECTION, newDirection ) );

	Quat newOrientation( rot.ToQuat() );
	node->SetOrientation( newOrientation );

//	Quat newOrientation( node->GetOrientation() * rot.ToQuat() );
//	node->SetOrientation( newOrientation );

	//Quat newOrientation( node->GetOrientation() * this->rotationQuaternion );
	//newOrientation.Normalize();
	//node->SetOrientation( newOrientation. );
	
	//Vec3D  newDirection = newOrientation * IDENTITY_DIRECTION;
	//newDirection.Normalize();
	//checked_cast< rxSpotLight*, rxDrawEntity* >( this->GetGraphics() )->SetDirection( newDirection );

	//Vec3D  newDirection = (this->GetOrientation() * newOrientation) * IDENTITY_DIRECTION
	//newDirection.Normalize();
	//checked_cast< rxSpotLight*, rxDrawEntity* >( this->GetGraphics() )->SetDirection( newDirection );
}

/*================================
			Node
================================*/

DEFINE_CLASS( Node, 'SGND', mxEntity );

Node::Node()
	: position( 0, 0, 0, 1 )
	, orientation( 0, 0, 0, 1 )
	, localToWorld( _InitIdentity )

	, name( null )
{}

Node::~Node()
{
	this->RemoveAllKids();
	this->RemoveAllAnimators();
}

void Node::SetOrigin( const Vec3D& newOrigin )
{
	this->position.x = newOrigin.x;
	this->position.y = newOrigin.y;
	this->position.z = newOrigin.z;
}

Vec3D Node::GetOrigin() const
{
	return this->position.ToVec3();
}

void Node::SetOrientation( const Quat& newOrientation )
{
	//Assert( newOrientation.IsNormalized( VECTOR_EPSILON ) );
	this->orientation = newOrientation;
}

Quat Node::GetOrientation() const
{
	return this->orientation;
}

void Node::SetScaling( const mxReal newScaling )
{
	this->position.w = newScaling;
}

Matrix4 & Node::GetAbsoluteTransform()
{
	return this->localToWorld;
}

Matrix4 Node::GetRelativeTransform()
{
	Matrix4  m;
	m.BuildTransform( this->position.ToVec3(), this->orientation, Vec3D( position.w ) );
	return m;
}

void Node::Validate()
{
	mxEntity::Validate();

	AlwaysAssert( parentScene != null );
}

void Node::SetName( const char* newName )
{
	this->name = newName;
}

const char* Node::GetName() const
{
	return this->name;
}

void Node::AddKid( Node* newChild )
{
	AssertPtr( newChild );
	Assert( newChild != this );

	if( this->parentScene != newChild->GetParentSceneGraph() )
	{
		newChild->SetParentSceneGraph( this->parentScene );
	}

	newChild->Grab();
	newChild->RemoveFromParent();
	this->kids.Append( newChild );
	newChild->parent = this;
}

void Node::RemoveKid( Node* theChild )
{
	AssertPtr( theChild );
	NodeList::Iterator it( this->kids.Begin() );
	for( ; it != this->kids.End(); ++it )
	{
		if( (*it) == theChild ) {
			(*it)->parent = null;
MX_FIXME(:)
#if 0
			this->kids.Erase( it );
			(*it)->Drop();
#else
			(*it)->Drop();
			this->kids.Erase( it );
#endif
			return;
		}
	}
}

void Node::RemoveAllKids()
{
	NodeList::Iterator it( this->kids.Begin() );
	for( ; it != this->kids.End(); ++it )
	{
		(*it)->parent = null;
		(*it)->Drop();
	}
	this->kids.RemoveAll();
}

Node * Node::GetParent()
{
	return this->parent;
}

void Node::SetParent( Node* newParent )
{
	if ( this->parent == newParent ) {
		return;
	}

	this->Grab();
	this->RemoveFromParent();
	this->parent = newParent;
	if( this->parent != null ) {
		this->parent->AddKid( this );
	}
	this->Drop();
}

void Node::AddAnimator( Animator* newAnim )
{
	AssertPtr( newAnim );
	newAnim->Grab();
	this->animators.Append( newAnim );
}

void Node::RemoveAnimator( Animator* theAnim )
{
	AssertPtr( theAnim );
	AnimatorList::Iterator it( this->animators.Begin() );
	for( ; it != this->animators.End(); ++it )
	{
		if( (*it) == theAnim ) {
			this->animators.Erase( it );
			theAnim->Drop();
			return;
		}
	}
}

void Node::RemoveAllAnimators()
{
	AnimatorList::Iterator it( this->animators.Begin() );
	for( ; it != this->animators.End(); ++it )
	{
		(*it)->Drop();
	}
	this->animators.RemoveAll();
}

void Node::Update( const mxTime dt )
{
	this->_setLocalTransform();

	this->localToWorld = (this->parent != null) ?
		this->parent->GetAbsoluteTransform() * this->GetRelativeTransform()
		:
		this->GetRelativeTransform();

	this->_onWorldTransformChanged();

	// Animate this node.
	{
		AnimatorList::Iterator it( this->animators.Begin() );
		for( ; it != this->animators.End(); ++it )
		{
			(*it)->Animate( this, dt );
		}
	}

	// Let the children update themselves.
	{
		NodeList::Iterator it( this->kids.Begin() );
		for( ; it != this->kids.End(); ++it )
		{
			Node * child(*it);
			child->Update( dt );
		}
	}
}

void Node::RemoveFromParent()
{
	if( this->parent != null ) {
		this->parent->RemoveKid( this );
	}
}

SceneGraph * Node::GetParentSceneGraph()
{
	return this->parentScene;
}

void Node::SetParentSceneGraph( SceneGraph* newParentSceneMgr )
{
	this->parentScene = newParentSceneMgr;

	NodeList::Iterator it( this->kids.Begin() );
	for( ; it != this->kids.End(); ++it )
	{
		Node * child(*it);
		child->SetParentSceneGraph( newParentSceneMgr );
	}
}

void Node::Grab() const
{
	MX_UNDONE
}
bool Node::Drop() const
{
	return false;
}
SizeT Node::NumRefs() const
{
	return 1;
}

/*================================
			Model
================================*/

DEFINE_CLASS( Model, 'MODL', Node );

Model::Model()
	: bCsgModel( false )
{}

Model::~Model()
{}

/*
================================
		MakeRenderModel
================================
*/
rxModel* MakeRenderModel( const mxMesh* mesh, const ModelDescription& desc )
{
	rxModelDescription	modelDesc;

	if ( desc.bCsgModel ) {
		modelDesc.flags |= EModelFlags::MF_DynamicGeometry;

		modelDesc.dynamicGeom.allocationType	= EHWBufferAlloc::HWA_Discardable;
		modelDesc.dynamicGeom.updateType		= EHWBufferUpdate::HWU_GrowAsNeeded;
	}

	rxMeshDescription &	meshDesc = modelDesc.meshDesc;

	rxVertexBufferDescription &  vbDesc = meshDesc.Vertices;
	vbDesc.VertexCount	= mesh->numVertices;
	vbDesc.VertexSize	= sizeof(rxVertex);
	vbDesc.Data			= mesh->vertices;

	meshDesc.PrimitiveType = EPrimitiveType::PT_TriangleList;

	rxIndexBufferDescription &  ibDesc = meshDesc.Indices;
	ibDesc.IndexCount	= mesh->numIndices;
	ibDesc.IndexSize	= sizeof(rxIndex);
	ibDesc.Data			= mesh->indices;

	rxScene & scene = mxEngine::get().GetRenderer().GetScene();

	return scene.CreateModel( modelDesc );
}

/*
================================
	MakeCSGModelFromMesh
================================
*/
CSGModel * MakeCSGModelFromMesh( const mxMesh* mesh, const Matrix4* pNodeTransform = null )
{
	AssertPtr( mesh );
	
	CSGInfo csgInfo;
	csgInfo.mesh = mesh;

	CSGModel * newCsgModel = NewCSGModel( csgInfo );
	newCsgModel->SetTransform( pNodeTransform );
	return newCsgModel;
}

/*
================================
		Model::SetupMesh
================================
*/
void Model::SetupMesh( const mxMesh* mesh, const ModelDescription& desc, SceneGraph* sceneMgr )
{
	this->parentScene = sceneMgr;

	AssertPtr( mesh );

	this->SetGraphics( MakeRenderModel( mesh, desc ) );

	if ( desc.bCsgModel ) {
		this->SetSpatialProxy( MakeCSGModelFromMesh( mesh, &this->localToWorld ) );
	} else {
		this->SetSpatialProxy( MX_NEW mxSpatialProxy_Box( mesh->bounds, this->GetAbsoluteTransform() ) );
		this->GetSpatialProxy()->hitFilterMask |= HM_Solid;
	}

	this->bCsgModel = desc.bCsgModel;
}

/*
================================
		Model::SetupQuad
================================
*/
void Model::SetupQuad( mxReal size, const Vec2D& texCoordScale, const ModelDescription& desc, SceneGraph* sceneMgr )
{
	this->parentScene = sceneMgr;

	// Build the quad's geometry.

	mxMeshPtr quadMesh = MakeMesh_Quad( size, texCoordScale );
	this->SetGraphics( MakeRenderModel( quadMesh, desc ) );

	if ( desc.bCsgModel ) {
		this->SetSpatialProxy( MakeCSGModelFromMesh( quadMesh, &this->localToWorld ) );
	} else {
		FLOAT a = size * 0.5f;
		AABB aabb( Vec3D(-a,0,-a), Vec3D(a,0,a) );
		this->SetSpatialProxy( MX_NEW mxSpatialProxy_Box( aabb, this->GetAbsoluteTransform() ) );
		this->GetSpatialProxy()->hitFilterMask |= HM_Solid;
	}

	this->bCsgModel = desc.bCsgModel;
}

void Model::SetupBox( mxReal length, mxReal height, mxReal depth, const ModelDescription& desc, SceneGraph* sceneMgr )
{
	this->parentScene = sceneMgr;

	// Create the box's geometry.

	mxMeshPtr boxMesh = MakeMesh_Box( length, height, depth );
	boxMesh->Transform( desc.initialTransform );
	this->SetGraphics( MakeRenderModel( boxMesh, desc ) );

	if ( desc.bCsgModel ) {
		this->SetSpatialProxy( MakeCSGModelFromMesh( boxMesh, &this->localToWorld ) );
	} else {
		const Vec3D vScale( length, height, depth );
		AABB aabb( -vScale*0.5f, vScale*0.5f );
		this->SetSpatialProxy( MX_NEW mxSpatialProxy_Box( aabb, this->GetAbsoluteTransform() ) );
		this->GetSpatialProxy()->hitFilterMask |= HM_Solid;
	}
}

void Model::SetupSphere( mxReal radius, mxUInt slices, mxUInt stacks, const ModelDescription& desc, SceneGraph* sceneMgr )
{
	Unimplemented;
}

void Model::Close()
{
	this->GetGraphics()->Remove();
}

void Model::SetMaterial( rxMaterial* newMaterial )
{
	AssertPtr( newMaterial );
	rxModel* model = checked_cast< rxModel*, rxDrawEntity* >( this->GetGraphics() );
	model->SetMaterial( TPtr<rxMaterial>( newMaterial ) );
}

void Model::ApplyCSG( const CSGInput& csgInput )
{
	if( CSGModel * csgModel = DynamicCast<CSGModel>(this->GetSpatialProxy()) )
	{
		rxModel* renderModel = checked_cast< rxModel*, rxDrawEntity* >( this->GetGraphics() );
		if( renderModel->GetFlags() & EModelFlags::MF_DynamicGeometry )
		{
			CSGOutput csgOutput;
			csgModel->Apply( csgInput, csgOutput );
			if( csgOutput.flags & CSGOutput::MeshChanged )
			{
				renderModel->SetGeometry( &csgOutput.meshData );
			}
		}
	}
}

void Model::_onWorldTransformChanged()
{
	rxModel* model = checked_cast< rxModel*, rxDrawEntity* >( this->GetGraphics() );
	model->SetTransform( this->GetAbsoluteTransform() );
}

/*================================
			Light
================================*/

DEFINE_CLASS( Light, 'LGHT', Node );

Light::Light()
	: type( ELightType::NumLightTypes )
{}

Light::~Light()
{}

void Light::Setup( const LightCreationInfo& cInfo, SceneGraph* sceneMgr )
{
	Assert( cInfo.IsOk() );

	this->parentScene = sceneMgr;

	this->type = cInfo.Type;

	rxScene & scene = mxEngine::get().GetRenderer().GetScene();

	rxLightDescription  renderLightDesc;
	renderLightDesc.color		= cInfo.Diffuse * cInfo.Intensity;
	renderLightDesc.attenuation	= cInfo.Attenuation;
	
	if ( cInfo.bCastShadows ) {
		renderLightDesc.flags |= ELightFlags::LF_SHADOWS;
	}
	if ( cInfo.projector ) {
		renderLightDesc.flags |= ELightFlags::LF_PROJTEX;
	}

	const FLOAT MAX_FLARE_SIZE = 1.0f;
	const FLOAT flareRadius = Clamp<FLOAT>( 0.05f*cInfo.Range, 0, MAX_FLARE_SIZE );
	const Vec2D flareSize( flareRadius );

	switch ( cInfo.Type )
	{
	case ELightType::Light_Directional :
		{
			this->SetGraphics( scene.CreateParallelLight(
				cInfo.Direction,
				renderLightDesc
			));

			// directional lights are infinite
			this->SetSpatialProxy( mxSpatialProxy_Infinite::New() );
		}
		break;

	case ELightType::Light_Point :
		{
			this->SetGraphics( scene.CreatePointLight(
				cInfo.Position,
				cInfo.Range,
				renderLightDesc
			));
			this->SetSpatialProxy( MX_NEW mxSpatialProxy_Sphere( cInfo.Range, GetAbsoluteTransform() ) );

			if ( cInfo.flare )
			{
				this->AddKid( sceneMgr->AddBillboard(
					cInfo.flare,
					flareSize,
					renderLightDesc.color
				) );
			}
		}
		break;

	case ELightType::Light_Spot :
		{
			rxSpotLight * pSpotLight = scene.CreateSpotLight(
				cInfo.Position,
				cInfo.Direction,
				cInfo.Range,
				cInfo.SpotInnerAngle, cInfo.SpotOuterAngle,
				renderLightDesc
			);
			if ( cInfo.projector != null ) {
				pSpotLight->SetProjector( cInfo.projector );
			}
			this->SetGraphics( pSpotLight );

			this->SetSpatialProxy( MX_NEW mxSpatialProxy_Sphere( cInfo.Range, GetAbsoluteTransform() ) );

			if ( cInfo.flare )
			{
				this->AddKid( sceneMgr->AddBillboard(
					cInfo.flare,
					flareSize,
					renderLightDesc.color
				) );
			}
		}
		break;

	default:
		Unimplemented;
	}

	this->SetPosition( cInfo.Position );
	this->SetDirection( cInfo.Direction );
}

void Light::Close()
{
	Unimplemented;
}

void Light::_setRenderLightPosition( const Vec3D& newOrigin )
{
	switch ( this->type )
	{
	case ELightType::Light_Directional :
		// nothing
		DEBUG_BREAK
		break;

	case ELightType::Light_Point :
		checked_cast< rxPointLight*, rxDrawEntity* >( this->GetGraphics() )->SetOrigin( newOrigin );
		break;

	case ELightType::Light_Spot :
		checked_cast< rxSpotLight*, rxDrawEntity* >( this->GetGraphics() )->SetOrigin( newOrigin );
		break;

	default:
		Unimplemented;
	}
}

void Light::_setRenderLightDirection( const Vec3D& newLookAt )
{
	switch ( this->type )
	{
	case ELightType::Light_Directional :
		checked_cast< rxParallelLight*, rxDrawEntity* >( this->GetGraphics() )->SetDirection( newLookAt );
		break;

	case ELightType::Light_Point :
		// nothing
		DEBUG_BREAK
		break;

	case ELightType::Light_Spot :
		checked_cast< rxSpotLight*, rxDrawEntity* >( this->GetGraphics() )->SetDirection( newLookAt );
		break;

	default:
		Unimplemented;
	}
}

Vec3D Light::GetDirection() const
{
	Light * pThisLight = const_cast< Light* >( this );
	switch ( this->type )
	{
	case ELightType::Light_Directional :
		return checked_cast< rxParallelLight*, rxDrawEntity* >( pThisLight->GetGraphics() )->GetDirection();
		break;

	case ELightType::Light_Point :
		// nothing
		DEBUG_BREAK
		break;

	case ELightType::Light_Spot :
		return checked_cast< rxSpotLight*, rxDrawEntity* >( pThisLight->GetGraphics() )->GetDirection();
		break;

	default:
		Unimplemented;
	}
	return IDENTITY_DIRECTION;
}

void Light::SetDirection( const Vec3D& newDirection )
{
	StaticAssert( ELightType::NumLightTypes == 3 );
	if( this->type != ELightType::Light_Point )
	{
		_setRenderLightDirection( newDirection );
	}
}

mxBool Light::IsEnabled() const
{
	Light * pThisLight = const_cast< Light* >( this );
	return checked_cast< rxLight*, rxDrawEntity* >(pThisLight->GetGraphics())->IsEnabled();
}

void Light::SetEnabled( bool bEnabled )
{
	Light * pThisLight = const_cast< Light* >( this );
	checked_cast< rxLight*, rxDrawEntity* >(pThisLight->GetGraphics())->SetEnabled( bEnabled );
}

/*
void Light::SetOrigin( const Vec3D& newOrigin )
{
	Node::SetOrigin( newOrigin );

	_setRenderLightPosition( newOrigin );
}

void Light::SetOrientation( const Quat& newOrientation )
{
	Node::SetOrientation( newOrientation );

	Vec3D  newDirection = newOrientation * IDENTITY_DIRECTION;
	newDirection.Normalize();

	_setRenderLightDirection( newDirection );
}
*/
void Light::_onWorldTransformChanged()
{
	if( this->type != ELightType::Light_Directional )
	{
		_setRenderLightPosition( this->localToWorld.GetTranslation() );
	}

	// Get the direction in world from my absolute transformation matrix.

	StaticAssert( ELightType::NumLightTypes == 3 );
	if( this->type != ELightType::Light_Point )
	{
		MX_HACK("the light should have the same direction as the camera")
		if( this->parent && this->parent->IsA( Camera::Type ) )
		{
			Camera * parentCamera = checked_cast< Camera*, Node* >( this->parent );
			_setRenderLightDirection( parentCamera->GetView().GetLookAt() );
		}
		else
		{
			MX_FIXME(:)
			/*
			Vec3D  newDirection( this->GetDirection() );
			this->localToWorld.TransformNormal( newDirection );
			newDirection.Normalize();
			_setRenderLightDirection( newDirection );
			*/
		}
	}
}

/*================================
			Skybox
================================*/

DEFINE_CLASS( Skybox, 'SKYB', Node );

Skybox::Skybox()
{}

Skybox::~Skybox()
{}

void Skybox::Setup( rxTexture* sides[AABB::NUM_SIDES], SceneGraph* sceneMgr )
{
	this->parentScene = sceneMgr;
	rxScene & scene = mxEngine::get().GetRenderer().GetScene();

	this->SetGraphics( scene.CreateSkybox( sides ) );
	this->SetSpatialProxy( mxSpatialProxy_Infinite::New() );
}

void Skybox::Close()
{
	Unimplemented;
}

void Skybox::SetOrientation( const Quat& newOrientation )
{
	Node::SetOrientation( newOrientation );
	rxSky * sky = checked_cast< rxSky*, rxDrawEntity* >( this->GetGraphics() );
	sky->SetOrientation( newOrientation );
}

/*================================
			Billboard
================================*/

DEFINE_CLASS( Billboard, 'BLDT', Node );

Billboard::Billboard()
{}

Billboard::~Billboard()
{}

void Billboard::Setup( rxTexture* texture, const Vec2D& size, const ColorRGB& color, SceneGraph* sceneMgr )
{
	this->parentScene = sceneMgr;
	rxScene & scene = mxEngine::get().GetRenderer().GetScene();
	this->SetGraphics( scene.CreateBillboard( texture, Vec3D(0), size, color ) );
	
	this->SetSpatialProxy( MX_NEW mxSpatialProxy_Sphere( size.Length(), GetAbsoluteTransform() ) );
	//this->SetSpatialProxy( MX_NEW mxSpatialProxy_Box( Vec2D(size).Length(), GetAbsoluteTransform() ) );
}

void Billboard::Close()
{
	Unimplemented;
}

void Billboard::SetOrigin( const Vec3D& newOrigin )
{
	Node::SetOrigin( newOrigin );
	rxBillboard * billboard = checked_cast< rxBillboard*, rxDrawEntity* >( this->GetGraphics() );
	billboard->SetOrigin( newOrigin );
}

void Billboard::SetOrientation( const Quat& newOrientation )
{
	Node::SetOrientation( newOrientation );
}

void Billboard::_onWorldTransformChanged()
{
	rxBillboard * billboard = checked_cast< rxBillboard*, rxDrawEntity* >( this->GetGraphics() );
	billboard->SetOrigin( this->GetAbsoluteTransform().GetTranslation() );
}

/*================================
			Camera
================================*/

DEFINE_CLASS( Camera, 'CAMR', Node );

Camera::Camera()
{}

Camera::~Camera()
{}

void Camera::Setup( mxCamera& theCamera, SceneGraph* sceneMgr )
{
	this->parentScene = sceneMgr;
	this->camera = & theCamera;
}

void Camera::Close()
{
	Unimplemented
}

void Camera::SetLookAt( const Vec3D& lookAt )
{
	this->camera->SetLookAt( lookAt );
	Node::SetOrientation( Matrix3::CreateRotation( IDENTITY_DIRECTION, lookAt ).ToQuat() );
}

const mxSceneView & Camera::GetView() const
{
	return this->camera->GetView();
}

void Camera::SetOrigin( const Vec3D& newOrigin )
{
	this->camera->SetPosition( newOrigin );

	Node::SetOrigin( newOrigin );
}

void Camera::SetOrientation( const Quat& newOrientation )
{
	Vec3D  newDirection = newOrientation * IDENTITY_DIRECTION;
	newDirection.Normalize();
	this->camera->SetLookAt( newDirection );

	Node::SetOrientation( newOrientation );
}

void Camera::_setLocalTransform()
{
	Node::SetOrigin( this->camera->GetView().GetOrigin() );
	Node::SetOrientation( Matrix3::CreateRotation( IDENTITY_DIRECTION, this->camera->GetView().GetLookAt() ).ToQuat() );
}

/*================================
			SceneGraph
================================*/

SceneGraph::SceneGraph( mxScene* theScene )
	: parentScene( theScene )
{
	this->rootNode = Node::New();
	this->rootNode->SetName( "root" );

	lol::SetupResourceLoader();
}

SceneGraph::~SceneGraph()
{
	lol::CloseResourceLoader();
}

void SceneGraph::Add( Node* newNode )
{
	newNode->Validate();

	this->rootNode->AddKid( newNode );
	this->parentScene->Add( newNode );

	if ( newNode->name != null ) {
		if ( FindNodeByName( newNode->name ) ) {
			sys::Error("a node with name '%s' already exist", newNode->name );
		} else {
			this->nodesByName.Insert( newNode->name, newNode );
		}
	}
}

void SceneGraph::Remove( Node* node )
{
	Unimplemented;

	if ( node->name != null ) {
		this->nodesByName.Remove( node->name );
	}
}

void SceneGraph::Update( const mxTime dt )
{
	this->rootNode->Update( dt );

//	rxDebugDrawer & debugDrawer = mxEngine::get().GetRenderer().GetDebugDrawer();
}

Node * SceneGraph::FindNodeByName( const char* name )
{
	AssertPtr( name );
	Node ** ppNode = null;
	if ( this->nodesByName.Get( name, &ppNode ) )
	{
		return *ppNode;
	}
	return null;
}

Model * SceneGraph::AddMesh( const mxMesh* mesh,
				const char* name, const ModelDescription& desc )
{
	AssertPtr( mesh );
	mesh->Grab();

	Model * newModel = Model::New();

	newModel->SetName( name );
	newModel->SetupMesh( mesh, desc, this );

	mesh->Drop();

	this->Add( newModel );
	return newModel;
}

Model * SceneGraph::AddMesh( const mxChar* filename,
				const char* name, const ModelDescription& desc )
{
	MeshDescription  meshDesc;
	meshDesc.initialTransform = desc.initialTransform;
	meshDesc.texCoordScale = desc.texCoordScale;

	mxMeshPtr mesh( LoadMeshFromFile( filename, meshDesc ) );
	return this->AddMesh( mesh, name, desc );
}

//
//	AddQuad
//
Model * SceneGraph::AddQuad( mxReal size, const Vec2D& texCoordScale,
							const char* name, const ModelDescription& desc )
{
	Model * newModel = Model::New();
	newModel->SetName( name );
	newModel->SetupQuad( size, texCoordScale, desc, this );

	this->Add( newModel );
	return newModel;
}

//
//	AddBox
//
Model *	SceneGraph::AddBox( mxReal length, mxReal height, mxReal depth,
						   const char* name, const ModelDescription& desc )
{
	Model * newModel = Model::New();
	newModel->SetName( name );
	newModel->SetupBox( length, height, depth, desc, this );

	this->Add( newModel );
	return newModel;
}

Model * SceneGraph::AddSphere( mxReal radius, mxUInt slices, mxUInt stacks,
							  const char* name, const ModelDescription& desc )
{
	Model * newModel = Model::New();
	newModel->SetName( name );
	newModel->SetupSphere( radius, slices, stacks, desc, this );

	this->Add( newModel );
	return newModel;
}

Light * SceneGraph::AddLight( const LightCreationInfo& desc, const char* name )
{
	TPtr< Light > newLight( Light::New() );
	newLight->SetName( name );
	newLight->Setup( desc, this );

	this->Add( newLight );
	return newLight;
}

Billboard * SceneGraph::AddBillboard( rxTexture* texture, const Vec2D& size, const ColorRGB& color, const char* name )
{
	TPtr< Billboard > newBillboard( Billboard::New() );
	newBillboard->SetName( name );
	newBillboard->Setup( texture, size, color, this );

	this->Add( newBillboard );
	return newBillboard;
}

Skybox*	SceneGraph::AddSkybox( rxTexture* sides[AABB::NUM_SIDES], const char* name )
{
	TPtr< Skybox > newSkybox( Skybox::New() );
	newSkybox->SetName( name );
	newSkybox->Setup( sides, this );

	this->Add( newSkybox );
	return newSkybox;
}

Camera* SceneGraph::AddCamera( mxCamera& theCamera, const char* name ) 
{
	TPtr< Camera > newCamera( Camera::New() );
	newCamera->SetName( name );
	newCamera->Setup( theCamera, this );

	this->Add( newCamera );
	return newCamera;
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
