/*
=============================================================================
	File:	Scene.h
	Desc:	Scene definition.
=============================================================================
*/

#ifndef __MX_SCENE_H__
#define __MX_SCENE_H__

/*
=============================================================================
		Scene graph considerations.
	
	  A basic scene graph should be nothing more than glue of the world,
	holding all scene elements together.
	  Ideally, the world should manage several specialized structures
	(render graphs, AI, visibility, BVHs and contact graphs, etc)
	which would hopefully simplify maintenance and will also enable us
	to perform many optimizations on the scene. And the user should be able
	to get access to these structures.	  
=============================================================================
*/

namespace abc {

// Forward declarations.
class mxEntity;
class mxSceneComponent;
class	mxSpatialDatabase;
class mxSceneNode;
class mxScene;
class mxEvent;
class mxCamera;

//------------------------------------------------------------------
//	Upper design limits
//------------------------------------------------------------------

// Maximum extent a scene can have, in engine units (usually 1 unit == 1 m).
//
const FLOAT	MAX_SCENE_SIZE = 5e5f;

//
//	ESceneType
//
enum ESceneType
{
	Scene_Generic,
	Scene_Indoors,
	Scene_Outdoors,
};

//
//	EDataVariance
//
enum EDataVariance
{
	Variance_Static,
	Variance_Dynamic,
};

//
//	EBuildPreferences
//
enum EBuildPreferences
{
	Build_For_Maximum_Speed,
	Build_For_Optimal_Memory_Usage,
	Build_For_Maximum_Quality,
};

//
//	ESceneBuildOptions
//
enum ESceneBuildOptions
{
	// Analyze static objects (e.g. static geometry, lightning, materials, etc),
	// precompute static transforms.
	Opt_Combine_Static_Objects		= BIT( 0 ),

	// Merge static entities having identical geometry to improve batching, spatial locality, etc.
	Opt_Merge_Geometry				= BIT( 1 ),

	// Precache visibility information (generate PVS, identify portals, etc).
	Opt_Precompute_PVS				= BIT( 2 ),

	// Identify entities having no effect on the scene,
	// find and eliminate unreachable areas that will never be seen
	// (e.g. remove entities that will never be seen by the player).
	Opt_Remove_Redundant_Entities	= BIT( 3 ),

	// Minimize render state changes.
	Opt_Minimize_State_Changes		= BIT( 4 ),
	Opt_Build_Texture_Atlas			= BIT( 5 ),	// for minimizing texture changes

	// Check if we can use instancing for rendering identical objects.
	Opt_Detect_Instanced_Geometry	= BIT( 6 ),

	// Select acceleration structures and algorithms best suited for this type of scene.
	Opt_Spatialize_World			= BIT( 7 ),

	// Generate levels of detail.
	Opt_Generate_LOD				= BIT( 8 ),

	// [aggressive] Use fake objects (try to use billboards, impostors, etc)
	Opt_Use_Fake_Objects			= BIT( 9 ),

	// [low-level, aggressive] Optimize geometry for rendering
	// (e.g. use triangle strips or fans instead of triangle lists,
	// optimize for minimizing overdraw or vertex cache coherency, etc).
	Opt_Optimize_Geometry			= BIT( 10 ),

	// [aggressive] Allow any modifications of original data set
	// that potentially lead to improvement
	// (arbitrary cutting of mesh polygons when building spatial structures,
	// lowering texture resolution and combining similar textures,
	// stripify geometry, etc).
	Opt_Aggressive_Data_Modification	= BIT( 11 ),
};

//
//	mxBuildOptions - settings for scene creation.
//
struct mxBuildOptions
{
	ESceneType		SceneType;

	EDataVariance	DataVariance;	// is the scene mostly static/dynamic?

	UDWORD	Flags;	// combination of ESceneBuildOptions

	//TODO: describe the intended usage of your scene,
	// it's data variance (is it static or dynamic?),
	// if it's more of the indoors type of scene than outdoors,
	// and so on and so forth...
	// ...

public:
	mxBuildOptions()
	{
		SetDefaults();
	}

	void SetDefaults()
	{
		SceneType		= ESceneType::Scene_Generic;
		DataVariance	= EDataVariance::Variance_Dynamic;
		Flags			= 0;
	}
};

//
//	mxSceneDescription - used for creating new scenes.
//
class mxSceneDescription {
public:

	mxBuildOptions		Options;	// Preferred build settings (most generic by default).
	mxBounds			Bounds;		// Maximum size of the world (infinity by default).
	mxSceneNode *		Root;		// The root node of the scene description graph. This can be null.
	const mxChar *		Name;		// Optional name of the scene. This can be null.

public:
	mxSceneDescription()
	{	SetDefaults();	}

	void SetDefaults();

	bool IsValid() const;
};

//
//	ESceneComponentType - enumerates most common types of scene subsystems.
//
enum ESceneComponentType
{
	SCT_UnknownComponent = 0,
	SCT_SpatialDatabase,
	SCT_RenderGraph,
	SCT_SceneGraph,
	SCT_PhysicsComponent,
	SCT_AudioComponent,
};

//
//	mxSceneComponent - is the base class for most scene subsystems.
//
//	Each component should abide by the Single Responsibility Principle.
//
class mxSceneComponent {
protected:
			mxSceneComponent();
	virtual	~mxSceneComponent();

public:

	// Initialization / Destruction.

	virtual void	Setup( mxScene& theParentScene );
	virtual void	Close();

	virtual void	Add( mxEntity* entity ) {};
	virtual void	Remove( mxEntity* entity ) {};

	// Thinking.
					// Advances the subsystem by the given time.
	virtual void	Update( const mxTime deltaTime );

	// Miscellaneous.

					// returns the enclosing scene
	mxScene *		GetParentScene();

	// Sanity checks, testing & debugging.

	virtual void	Validate();

private:
	TPtr< mxScene >		parentScene;
};

FORCEINLINE
mxScene * mxSceneComponent::GetParentScene() {
	return this->parentScene;
}

//
//	mxSceneView - represents view from which a scene is rendered.
//
//	This structure can be modified during rendering if portals are used:
//	transformative, warping portals, sectors for culling and such.
//	NOTE: must be very light-weight ( mxSceneView are passed very often thru the stack ).
//
ALIGN_16 class mxSceneView {
public:
		mxSceneView();
		mxSceneView( const mxSceneView& other );
		mxSceneView( ENoInit );

	const Matrix4 & GetViewMatrix() const;
	const Matrix4 & GetProjectionMatrix() const;

	const Vec3D &	GetOrigin() const;
	const Vec3D &	GetLookAt() const;

			void	SetOrigin( const Vec3D& newOrigin );		// Rebuilds the view frustum.
			void	SetLookAt( const Vec3D& newLookAt );	// Rebuilds the view frustum.

	// Rebuilds view matrix and frustum.
	void SetView( const Vec3D& position, const Vec3D& lookAt,
				const Vec3D& upVector = Vec3D( 0, 1, 0 ) );

	// Rebuilds projection matrix and frustum.
	void SetLens( FLOAT fovY = 3.14f/2, FLOAT aspect = 1.333f,
				FLOAT nearZ = 1.0f, FLOAT farZ = 10000.0f );

	const mxViewFrustum &	GetFrustum() const;

	// Rebuilds the view frustum.
	void RecalculateViewFrustum();

public:
	Matrix4		ViewMatrix;
	Matrix4		ProjMatrix;

	mxViewFrustum	Frustum;	// for view frustum culling

	Vec3D	Position;	// Absolute position.
	Vec3D	LookAt;		// Look direction vector.

	//mxUInt32	FrameStamp;
};

FORCEINLINE const Matrix4 & mxSceneView::GetViewMatrix() const			{	return ViewMatrix;	}
FORCEINLINE const Matrix4 & mxSceneView::GetProjectionMatrix() const	{	return ProjMatrix;	}
FORCEINLINE const Vec3D & mxSceneView::GetOrigin() const				{	return Position;	}
FORCEINLINE const Vec3D & mxSceneView::GetLookAt() const				{	return LookAt;		}
FORCEINLINE const mxViewFrustum & mxSceneView::GetFrustum() const		{	return Frustum;		}

//
//	mxCamera - is a simple camera class.
//
//	This is a simple vector-based FPS camera.
//
class mxCamera {
public:
			mxCamera( const Vec3D& position = Vec3D::vec3_origin, const Vec3D& target = Vec3D( 0,0,1 ) );

	// Returns the view from this camera.
	const mxSceneView &	GetView() const;

	//
	// These methods modify the camera's projection matrix.
	//

	void	SetLens( FLOAT fovY = 3.14f/2, FLOAT aspect = 1.333f, FLOAT nearZ = 0.1f, FLOAT farZ = 10000.0f );

	// Zoom by a positive factor.
	// Zoom measures the ratio of the apparent size of the object relative to a 90 degree field of view.
	// e.g. a zoom of 2.0 means that the object will appear twice as big on screen as it would be 
	// if we were using a 90 degree field of view.
	void	Zoom( FLOAT factor );

	//
	// These methods modify the camera's view matrix.
	//

	void	SetPosition( const Vec3D& newPosition );
	void	SetTarget( const Vec3D& newTarget );
	void	SetLookAt( const Vec3D& newLookAt );	// Set the view direction.

	const Vec3D &	GetPosition() const;
	const Vec3D &	GetLookAt() const;	// Returns a normalized look direction vector.


	void	Walk( FLOAT fUnits );		// Convenience function to move backward or forward in Camera space.
	void	Strafe( FLOAT fUnits );		// Convenience function to move to the right or left in Camera space.
	void	Fly( FLOAT fUnits );		// Convenience function to move up or down in Camera space.

	void	Yaw( FLOAT fAngle );		// Convenience function to rotate around the y-axis of the Camera. NOTE: Angle in radians.
	void	Pitch( FLOAT fAngle );		// Convenience function to rotate around the x-axis of the Camera. NOTE: Angle in radians.

	// Recalculate a view matrix from Position, LookAt, and Up vectors.
	void 	RecalculateViewMatrix();

	// Utilities.
	const Vec3D & GetUpVector() const		{ return m_vUp; }
	const Vec3D & GetRightVector() const	{ return m_vRight; }

public:
	// Recalculate all transform-dependant data (view matrix, frustum planes, etc).
	void	_UpdateOnTransformChanged();

private:
	mxSceneView		m_view;

	Vec3D	m_vUp, m_vRight;
};

//
//	mxCamera::GetView
//
FORCEINLINE const mxSceneView & mxCamera::GetView() const {
	return m_view;
}
FORCEINLINE const Vec3D & mxCamera::GetPosition() const {
	return m_view.GetOrigin();
}
FORCEINLINE const Vec3D & mxCamera::GetLookAt() const {
	return m_view.GetLookAt();
}

//
//	mxVisibleSet - used for collecting visible objects.
//
// For now it's a simple array, but in the future
// it will be a data structure which will probably not require
// complete rebuilding in each frame.

class mxVisibleSet {
public:
			mxVisibleSet( SizeT initialCount = 256 );
			~mxVisibleSet();

	void		Add( mxEntity* pObject );
	SizeT		GetNum() const;		// Returns the total number of objects in this set.
	mxEntity *	Get( IndexT index );

	void	Empty();	// Empties this set.

private:
	TArray< mxEntity* >	m_visibleObjects;
};

//
//	mxScene
//
class mxScene {
public:

	//
	//	mxScene interface :
	//

	// Spatial database.
	mxSpatialDatabase &	GetSpatialDatabase();

	//--- Scene management --------------------------------------------------------

	void	Add( mxEntity* entity );
	void	Remove( mxEntity* entity );

	mxCamera &		GetActiveCamera();

	//--- Scene event handling ----------------------------------------------------


	//--- Utilities ----------------------------------------------------------

			// Returns the total number of entities in this scene.
	mxUInt	GetEntityCount() const;

	//--- Validation / Verification / Checks ---------------------------------

	//--- Testing & Debugging ------------------------------------------------

private:
	//
	//	Internal functions.
	//

	friend class mxEngine;

			mxScene();
			~mxScene();

			// Note: this function must be called after creating the scene.
	void	Initialize( const mxSceneDescription& creationInfo );

	void	Shutdown();

			// Advances the scene by the given time.
	void	Tick( const mxTime deltaTime );

	//--- Scene rendering ----------------------------------------------------

			// Renders the frame.
	void	Present();

private:
	mxEntityList	entities;		// for quickly iterating over all entities

	AutoPtr< mxSpatialDatabase >	spatialHash;	// for accelerating spatial queries on the scene

	AutoPtr< mxCamera >		activeCamera;	// Current camera.

private:
	NO_COPY_CONSTRUCTOR( mxScene );
	NO_ASSIGNMENT( mxScene );
};

FORCEINLINE mxSpatialDatabase & mxScene::GetSpatialDatabase() {
	return *spatialHash;
}

FORCEINLINE mxCamera & mxScene::GetActiveCamera() {
	return *activeCamera;
}

FORCEINLINE mxUInt mxScene::GetEntityCount() const {
	return entities.Num();
}

}//End of namespace abc

#endif // ! __MX_SCENE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
