/*
=============================================================================
	File:	SceneGraph.h
	Desc:	Basic scene graph usable for sample applications.
	Note:	Using a scene graph incurs some overhead
			( efficiency is traded for convenience ).
=============================================================================
*/

#ifndef __MINISG_SCENE_GRAPH_H__
#define __MINISG_SCENE_GRAPH_H__

namespace abc {

// Forward declarations.
class	Node;
class		Model;
class		Light;
class		Camera;
class		Billboard;
class		Skybox;
class	Animator;
class	SceneGraph;
class	ModelDescription;
class	LightCreationInfo;

/*
=======================================================================
	
					Scene node controllers.

=======================================================================
*/

//
//	Animator - is a scene node animator.
//
class Animator : public ReferenceCounted
{
public:
	virtual ~Animator() {}

	virtual void Animate( Node* node, const mxTime dt ) = 0;

protected:
	Animator() {}
};

typedef TLinkedList< Animator* >	AnimatorList;

//
//	Animator_Rotation - rotation about some fixed point and axis.
//
class Animator_Rotation : public Animator
{
public:
	// angle in radians
	Animator_Rotation( const Vec3D& origin, const Vec3D& axis, FLOAT anglePerFrame )
	{
		this->Set( origin, axis, RAD2DEG(anglePerFrame) );
	}
	~Animator_Rotation()
	{}

	void Set( const Vec3D& origin, const Vec3D& axis, FLOAT anglePerFrame )
	{
		this->rotation.Set( origin, axis.GetNormalized(), anglePerFrame );
	}

	//
	//	Override ( Animator ) :
	//
	void Animate( Node* node, const mxTime dt );

private:
	mxRotation	rotation;
};

//
//	Animator_Orientation - changes object's orientation (rotates about some fixed axis).
//
class Animator_Orientation : public Animator
{
public:
	Animator_Orientation( const Vec3D& axis, FLOAT anglePerFrame )
		: rotationQuaternion( axis.GetNormalized(), anglePerFrame )
	{}
	~Animator_Orientation()
	{}

	void Set( const Vec3D& axis, FLOAT anglePerFrame )
	{
		this->rotationQuaternion = Quat( axis.GetNormalized(), anglePerFrame );
	}

	//
	//	Override ( Animator ) :
	//
	void Animate( Node* node, const mxTime dt );

private:
	Quat	rotationQuaternion;
};

//
//	Animator_LookAt - rotates the subject node to always face the target node.
//
class Animator_LookAt : public Animator
{
public:
	Animator_LookAt( Node* theTargetNode );
	~Animator_LookAt();

	void Set( Node* theTargetNode );

	//
	//	Override ( Animator ) :
	//
	void Animate( Node* node, const mxTime dt );

private:
	TPtr< Node >	targetNode;
};

/*
=======================================================================
	
					Scene nodes.

=======================================================================
*/

//
//	Node - in the base class for node classes in a scene graph.
//
//	These are usually discrete (separate) dynamic (movable) objects.
//
//	NOTE: each node can have only one parent.
//
class Node : public mxEntity
{
	DECLARE_CLASS( Node );

	friend class SceneGraph;
public:
			Node();
	virtual	~Node();

	// Spatial position / orientation.

	virtual void	SetOrigin( const Vec3D& newOrigin );
	virtual Vec3D	GetOrigin() const;

			// these functions are provided for convenience
			void	SetPosition( const Vec3D& newPosition ) { SetOrigin( newPosition ); }
			Vec3D	GetPosition() const { return GetOrigin(); }

	virtual void	SetOrientation( const Quat& newOrientation );
	virtual Quat	GetOrientation() const;
	virtual void	SetScaling( const mxReal newScaling );

	virtual Matrix4 &	GetAbsoluteTransform();	// get world transform
	virtual Matrix4		GetRelativeTransform();	// get local-to-parent transform

	// Child nodes.
	typedef TLinkedList< Node* >	NodeList;

	virtual void	AddKid( Node* newChild );
	virtual void	RemoveKid( Node* theChild );
	virtual void	RemoveAllKids();

			Node *	GetParent();
	virtual void	SetParent( Node* newParent );

	// Animators.
	virtual void	AddAnimator( Animator* newAnim );
	virtual void	RemoveAnimator( Animator* theAnim );
	virtual void	RemoveAllAnimators();

	// Reference counting.
	void	Grab() const;
	bool	Drop() const;
	SizeT	NumRefs() const;

	//
	//	Override ( mxEntity ) :
	//
	virtual void	Validate();

	// Testing & Debugging.

	void SetName( const char* newName );
	const char* GetName() const;

protected:
			// changes local (relative to the parent) orientation of this node
	//		void _setLookDirection( const Vec3D& lookAt );

	// updates 'localToWorld' transform as needed
	virtual void _setLocalTransform() {}

	// updates internal data after 'localToWorld' transform has been recalculated
	virtual void _onWorldTransformChanged() {}

	void	Update( const mxTime dt );

	SceneGraph * GetParentSceneGraph();
	virtual void SetParentSceneGraph( SceneGraph* newParentSceneMgr );

	void	RemoveFromParent();

protected:
	Matrix4	localToWorld;	// world transform

	Vec4D	position;		// Position (float3) and uniform scaling factor (float).
	Quat	orientation;	// Orientation of this node relative to the parent.

	TPtr< Node >	parent;	// parent scene node

	NodeList		kids;	// children

	AnimatorList	animators;

	TPtr< SceneGraph >	parentScene;

	// optional name of this node
	// (allows us to identify nodes by their names, very useful for debugging)
	const char *	name;

	// more will come...
};

//
//	NodePtr< T > - a pointer to a scene node.
//
template< typename T >
class NodePtr {
public:
	NodePtr( T* pNode = null )
		: theNode( pNode )
	{
		if ( theNode ) {
			theNode->Grab();
		}
	}
	NodePtr( const NodePtr& other )
		: theNode( other.theNode )
	{
		if ( theNode ) {
			theNode->Grab();
		}
	}
	~NodePtr()
	{
		if ( theNode ) {
			theNode->Drop();
		}
	}

	// Implicit conversions.

			operator T*	() const	{ Assert( theNode );	return theNode; }
	T &		operator *	() const	{ Assert( theNode );	return *theNode; }
	T *		operator ->	() const	{ return theNode; }

	// Assignment.

	NodePtr &	operator = ( T* pObject )
	{
		if ( pObject == null )
		{
			if ( theNode ) {
				theNode->Drop();
				theNode = null;
			}
			return *this;
		}
		if ( theNode != pObject )
		{
			if ( theNode ) {
				theNode->Drop();
			}
			pObject->Grab();
			theNode = pObject;
			return *this;
		}
		return *this;
	}

	NodePtr &	operator = ( const NodePtr& other )
	{
		return ( *this = other.theNode );
	}

	// Comparisons.

	bool	operator == ( T* pObject ) const
	{
		return theNode == pObject;
	}
	bool	operator != ( T* pObject ) const
	{
		return theNode != pObject;
	}
	bool	operator == ( const NodePtr& other ) const
	{
		return theNode == other.theNode;
	}
	bool	operator != ( const NodePtr& other ) const
	{
		return theNode != other.theNode;
	}

	// Unsafe...
	T *		get()		{ return theNode; }
	T *&	get_ref()	{ return theNode; }

private:
	T *		theNode;	// The shared reference counted object.
};

/*
=======================================================================
	
			'Geometry node'.

=======================================================================
*/

//
//	ModelDescription
//
struct ModelDescription
{
	Matrix4		initialTransform;	// initial mesh transform
	FLOAT		texCoordScale;
	bool		bCsgModel;		// Can CSG operations be applied to this model?

public:
	ModelDescription()
	{
		Reset();
	}
	void Reset()
	{
		initialTransform.SetIdentity();
		texCoordScale = 1.0f;
		bCsgModel = false;
	}
	bool IsOk() const
	{
		return 1
			&& (texCoordScale != 0.f)
			;
	}
};

class CSGInput;

//
//	Model - is a scene node containing renderable geometry.
//
class Model : public Node {
public:
			// set the given material to all submeshes
	void	SetMaterial( rxMaterial* newMaterial );

	void	ApplyCSG( const CSGInput& csgInput );

	//
	//	Override ( Node ) :
	//

protected:
	void _onWorldTransformChanged();

private:
	friend class SceneGraph;

	DECLARE_CLASS( Model );

			Model();
			~Model();

	void	SetupMesh( const mxMesh* mesh, const ModelDescription& desc, SceneGraph* sceneMgr );
	void	SetupQuad( mxReal size, const Vec2D& texCoordScale, const ModelDescription& desc, SceneGraph* sceneMgr );
	void	SetupBox( mxReal length, mxReal height, mxReal depth, const ModelDescription& desc, SceneGraph* sceneMgr );
	void	SetupSphere( mxReal radius, mxUInt slices, mxUInt stacks, const ModelDescription& desc, SceneGraph* sceneMgr );

	void	Close();

private:
	bool	bCsgModel;
};

//
//	LightCreationInfo - contains generic light info.
//
struct LightCreationInfo
{
	//-- common light parameters --
	ELightType	Type;

	bool		bCastShadows;	// true if the light does cast shadows

	//-- directional light parameters begin here --
	ColorRGB	Diffuse;		// the diffuse color emitted by the light source
	FLOAT		Intensity;		// light brighness ( must be in the range [0 .. 1.0f] )
	Vec3D		Direction;		// normalized direction
	
	//-- point light parameters begin here --
	FLOAT		Range;			// light's radius of influence
	Vec3D		Position;		
	Vec3D		Attenuation;	// attenuation parameters: constant,linear,quadratic
	
	//-- spot light parameters begin here --
	FLOAT		SpotInnerAngle;	// inner spot cone angle (in radians, must be in range [0..PI/2] (60 degrees by default))
	FLOAT		SpotOuterAngle;	// outer spot cone angle (in radians, must be in range [0..PI/2] (60 degrees by default))

	// misc settings

	rxTexture *	flare;		// != null if light flares should be rendered (as billboards)
	rxTexture *	projector;	// projective texture (2D for spot lights and cube texture for point lights)

public:
	LightCreationInfo()
	{
		SetDefaults();
	}

	void SetDefaults()
	{
		Type =  ELightType::Light_Directional;

		Diffuse		= FColor::WHITE;
		Intensity	= 1.0f;
		Direction	.Set( 0,0,1 );
		Range		= 10.0f;
		Position	.Set( 0,0,0 );
		Attenuation	.Set( 0.07f, 0.03f, 0.01f );
		
		SpotOuterAngle	= DEG2RAD( 60.0f );
		SpotInnerAngle	= DEG2RAD( 60.0f - 10.0f );

		bCastShadows = false;

		flare		= null;
		projector	= null;
	}

	bool IsOk() const
	{
		return (IsInRange<FLOAT>(SpotInnerAngle, 0, mxMath::HALF_PI))
			&& (IsInRange<FLOAT>(SpotOuterAngle, 0, mxMath::HALF_PI))
			&& 1
			;
	}
};

//
//	Light
//
class Light : public Node {
public:
	FLOAT			GetIntensity() const;
	void			SetIntensity( FLOAT newBrightness );

	// this is only valid for directional and spot lights
	Vec3D	GetDirection() const;

	// this is only valid for directional and spot lights
	void	SetDirection( const Vec3D& newDirection );

	mxBool	IsEnabled() const;
	void	SetEnabled( bool bEnabled );

	//
	//	Override ( Node ) :
	//
//	void	SetOrigin( const Vec3D& newOrigin );
//	void	SetOrientation( const Quat& newOrientation );

protected:
	void	_onWorldTransformChanged();

private:
	friend class SceneGraph;

	DECLARE_CLASS( Light );

			Light();
			~Light();

	void	Setup( const LightCreationInfo& cInfo, SceneGraph* sceneMgr );
	void	Close();

private:
	void	_setRenderLightPosition( const Vec3D& newOrigin );
	void	_setRenderLightDirection( const Vec3D& newLookAt );

private:
	TEnum< ELightType >		type;
};

//
//	Skybox
//
class Skybox : public Node {
public:

	//
	//	Override ( Node ) :
	//
	void	SetOrientation( const Quat& newOrientation );

private:
	friend class SceneGraph;

	DECLARE_CLASS( Skybox );

			Skybox();
			~Skybox();

	void	Setup( rxTexture* sides[AABB::NUM_SIDES], SceneGraph* sceneMgr );
	void	Close();
};

//
//	Billboard
//
class Billboard : public Node {
public:
	//
	//	Override ( Node ) :
	//
	void	SetOrigin( const Vec3D& newOrigin );
	void	SetOrientation( const Quat& newOrientation );

protected:
	void _onWorldTransformChanged();

private:
	friend class SceneGraph;

	DECLARE_CLASS( Billboard );

			Billboard();
			~Billboard();

	void	Setup( rxTexture* texture, const Vec2D& size, const ColorRGB& color, SceneGraph* sceneMgr );
	void	Close();
};

//
//	Camera
//
class Camera : public Node {
public:
	
	const mxSceneView &	GetView() const;

	void	SetLookAt( const Vec3D& lookAt );

	//
	//	Override ( Node ) :
	//
	void	SetOrigin( const Vec3D& newOrigin );	// sets transform to the camera
	void	SetOrientation( const Quat& newOrientation );	// sets transform to the camera

protected:
	void _setLocalTransform();	// sets node's relative transform from the camera

private:
	friend class SceneGraph;

	DECLARE_CLASS( Camera );

			Camera();
			~Camera();

	void	Setup( mxCamera& theCamera, SceneGraph* sceneMgr );
	void	Close();

private:
	TPtr< mxCamera >	camera;
};


/*
=======================================================================
	
					Scene manager.

=======================================================================
*/

//
//	SceneGraph - is a basic scene manager.
//
class SceneGraph : public ReferenceCounted {
public:
	SceneGraph( mxScene* theScene );
	~SceneGraph();

	void Add( Node* newNode );
	void Remove( Node* node );

	void Update( const mxTime dt );

	Node *	FindNodeByName( const char* name );

public:

	//	These function create entities and make them immediately renderable (by uploading created geometry onto GPU).

	Model * AddMesh( const mxMesh* mesh,
					const char* name = null, const ModelDescription& desc = ModelDescription() );

	Model * AddMesh( const mxChar* filename,
					const char* name = null, const ModelDescription& desc = ModelDescription() );

	// Creates a quad lying in XoZ plane with the given side length.
	//
	Model * AddQuad( mxReal size, const Vec2D& texCoordScale = Vec2D::vec2_unit,
					const char* name = null, const ModelDescription& desc = ModelDescription() );

	Model *	AddBox( mxReal length, mxReal height, mxReal depth,
					const char* name = null, const ModelDescription& desc = ModelDescription() );

	Model *	AddSphere( mxReal radius, mxUInt slices, mxUInt stacks,
					const char* name = null, const ModelDescription& desc = ModelDescription() );

	Light *	AddLight( const LightCreationInfo& desc,
					const char* name = null );

	Billboard *	AddBillboard( rxTexture* texture, const Vec2D& size, const ColorRGB& color = FColor::WHITE,
					const char* name = null );

	Skybox*	AddSkybox( rxTexture* sides[AABB::NUM_SIDES],
					const char* name = null );

	Camera* AddCamera( mxCamera& theCamera, const char* name = null );

private:
	TPtr< Node >	rootNode;
	TPtr< mxScene >	parentScene;
//	TArray< Node* >	allNodes;

	TStringHash< Node* >	nodesByName;
};

}//End of namespace abc

#endif // !__MINISG_SCENE_GRAPH_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
