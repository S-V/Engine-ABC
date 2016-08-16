/*
=============================================================================
	File:	Renderer.h
	Desc:	Render system definitions.
=============================================================================
*/

#ifndef __RX_RENDERER_H__
#define __RX_RENDERER_H__

// Forward declarations.
struct IDirect3DDevice9;
struct ID3D10Device;
struct ID3D11Device;

namespace abc {

// Forward declarations.
class mxSceneView;
class mxScene;
class mxFilePath;

class rxMaterial;
class rxTextureDescription;
class rxTexture;
class rxIndexBufferDescription;
class rxIndices;
class rxVertexFormat;
class rxVertexLayout;
class rxVertexDeclaration;
class rxVertexBufferDescription;
class rxVertices;
class rxShader;
class rxMaterialDescription;
class rxModel;
class rxStatistics;
class rxRendererInfo;
class rxScene;
class rxResources;
class rxMesh;
class rxDebugDrawer;

/*
=============================================================================

	Main renderer interface.

=============================================================================
*/

//
//	rxRenderDeviceCreationInfo
//
struct rxRenderDeviceCreationInfo
{
	mxScreenSettings	screen;	// display mode

	mxUInt	refreshRateHz;	// desired refresh rate

	bool	bDebugRenderer;	// create debug renderer?

	void *	pWindowHandle;	// HWND on Windows.

public:
	rxRenderDeviceCreationInfo()
	{
		SetDefaultValues();
	}

	void SetDefaultValues()
	{
		screen.SetDefaultValues();

		refreshRateHz = 60;

		bDebugRenderer = g_bDebugMode;

		pWindowHandle = null;
	}

	bool IsValid() const;
};

//
//	rxRenderer
//
class rxRenderer : public mxMessagePort {
public:
	//------------------------------------------------------------------------------
	//	Common rxRenderer interface :
	//------------------------------------------------------------------------------

	// Initializes the device. Return 'true' upon success, 'false' if device initialization failed.
	//
	virtual bool	Create( const rxRenderDeviceCreationInfo& creationInfo ) = 0;

	// Destroys the device ( releases all allocated graphics resources, frees the memory occupied by the device, etc. )
	// Note: After calling this function the pointer to this device will become invalid !
	//
	virtual void	Destroy() = 0;

	//--- Rendering ----------------------------------------------------------------

	// Access to the graphics scene.
	virtual rxScene &	GetScene() = 0;

	// Generates draw calls and present the scene to the screen.
	virtual void	RenderScene( const mxSceneView& view, mxScene* scene ) = 0;

	//--- Graphics resource management. --------------------------------------------

	virtual class rxResources &	GetResources() = 0;

	//--- Testing and Debugging ----------------------------------------------------

	// Access to the interface for debug visualization.
	virtual rxDebugDrawer &	GetDebugDrawer() = 0;

	// Queries performance counters.
	virtual const rxStatistics &	GetStats() const = 0;

	// Gets the graphics device info, screen settings, etc.
	virtual const rxRendererInfo &	GetInfo() const = 0;

protected:
	rxRenderer()
	{}
	virtual ~rxRenderer()
	{}
};

/*
=======================================================================
	Graphics scene management.
=======================================================================
*/

//=========================================================
//	Render entity definition
//=========================================================

//
//	EDrawEntity - enumerates types of render entities.
//
enum EDrawEntity
{
	RE_Unknown = 0,
	RE_Model,
	RE_Light,

	RE_MAX
};

// Forward declarations.

struct rxView
{
protected:
	virtual ~rxView() {}
};

struct rxQueue
{
protected:
	virtual ~rxQueue() {}
};

//
//	rxDrawEntity - is an object in the graphics scene.
//
Interface( rxDrawEntity )
{
public:
	// Internal functions.

					// registers this entity in the render queue
	virtual void	Render( const rxView& view, rxQueue& queue ) = 0;

					// removes this object from the scene
	virtual void	Remove() = 0;

protected:
	virtual ~rxDrawEntity();
};

//
//	rxNullDrawEntity - used to avoid null pointer checks ( 'null object' idiom ).
//
struct rxNullDrawEntity : public rxDrawEntity
{
	rxNullDrawEntity();
	~rxNullDrawEntity();

	void	Render( const rxView& view, rxQueue& queue );
	void	Remove();

	static rxNullDrawEntity		dummy;
};

/*
=============================================================

	Model.

	Models are render entities based on meshes.

=============================================================
*/

//
//	EModelFlags - defines properties of render models.
//
enum EModelFlags
{
	// True if model will have dynamic, modifiable geometry.
	MF_DynamicGeometry		= BIT(0),
};

//
//	EHWBufferAlloc -
//		enumerates ways of creating hardware geometry buffers
//		for render models with dynamic geometry.
//
enum EHWBufferAlloc
{
	HWA_Fixed			= 0,	// Create a non-growing buffer with the size enough to hold initial geometry; the buffer can still be written into, but not resized.
	HWA_Reserve			= 1,	// Create a fixed size buffer with some space reserved; the user must specify exactly how much space should be reserved.
	HWA_Discardable		= 2,	// Discardable buffers can be created and destroyed just as needed to hold new geometry.
};

//
//	EHWBufferUpdate - strategy of updating hardware geometry buffers.
//
enum EHWBufferUpdate
{
	HWU_NeverUpdate		= 0,	// Don't change the buffer.
	HWU_WriteExisting	= 1,	// Write new geometry into existing buffer (discard the new data if the buffer has no space left).
	HWU_GrowAsNeeded	= 2,	// Create a new buffer with a size enough to hold new geometry and discard the old buffer, if needed.
	HWU_GrowByChunks	= 3,	// Create a new buffer with a size enough to hold new geometry and the size must be a multiple of the specified granularity, discard the old buffer, if needed.
	HWU_GrowFromPool	= 4,	// Allocate space to hold new data from special pre-allocated dynamic buffers, if needed.
};

//
//	rxDynamicGeometry - settings for managing dynamic render geometry.
//		These are ignored if the model has been created with static geometry.
//
struct rxDynamicGeometry
{
	EHWBufferAlloc		allocationType;
	EHWBufferUpdate		updateType;

	// initial buffer size if HWA_Reserve flag is set
	SizeT				bufferSize;	// in bytes

public:
	rxDynamicGeometry()
		: allocationType( EHWBufferAlloc::HWA_Fixed )
		, updateType( EHWBufferUpdate::HWU_NeverUpdate )
		, bufferSize( 0 )
	{}

	bool IsOk() const { return true; }
};

//
//	rxDynMeshData - contains data for updating dynamic geometry.
//
struct rxDynMeshData
{
	UINT				numVertices;
	const rxVertex *	vertices;	// vertex data (readable by CPU)
	UINT				numIndices;
	const rxIndex *		indices;	// index data (readable by CPU)
	mxBounds			bounds;		// bounds in local space

	rxDynMeshData()
		: numVertices(0), vertices(null)
		, numIndices(0), indices(null)
		, bounds(mxBounds::INFINITE_EXTENT)
	{}
};

//
//	rxModelDescription - used for creating new render models.
//
struct rxModelDescription
{
	rxMeshDescription	meshDesc;

	UINT32				flags;	// bits from EModelFlags

	rxDynamicGeometry	dynamicGeom;

	// ...

public:
	rxModelDescription();

	bool	IsOk() const;
};

//
//	rxBatch
//
struct rxBatch
{
	rxIndex		StartIndex;
	rxIndex		IndexCount;
	UINT		StartVertex;	// Offset from the start of the vertex buffer to the first vertex.
	UINT		VertexCount;

public:
	rxBatch()
		: StartIndex(0), IndexCount(0), StartVertex(0), VertexCount(0)
	{}
};

//
//	rxSurface - a renderable surface with a single material.
//
struct rxSurface
{
	rxMaterial *	Material;
	rxBatch			Batch;
};

//
//	rxModel - this structure describes drawable geometry based on a mesh.
//
class rxModel : public rxDrawEntity {
public:

	UINT32			GetFlags() const;	// returns bits from EModelFlags

	virtual void	SetTransform( const Matrix4& newWorldTransform ) = 0;

					// sets all render surfaces to the given material
	virtual void	SetMaterial( rxMaterial* newMaterial ) = 0;

					// Modify geometry of this model.
					// NOTE: This only works if the model was created with EModelFlags::MF_DynamicGeometry.
	virtual void	SetGeometry( const rxDynMeshData* newMesh ) = 0;

	virtual bool	IsOk() const = 0;

protected:
	rxModel() : flags(0) {}
	virtual ~rxModel() {}

protected:
	UINT32		flags;	// EModelFlags
};

FORCEINLINE UINT32 rxModel::GetFlags() const { return flags; }

//=========================================================
//	Dynamic lights
//=========================================================

//
//	ELightType - enumerates all allowed light types.
//
//	NOTE: Do not change the order! Global lights should go first.
//
enum ELightType
{
	Light_Directional = 0,

	Light_Point,
	Light_Spot,
//	Light_Area,
//	Light_Volume,

	NumLightTypes	// invallid type
};

//
//	ELightFlags
//
enum ELightFlags
{
	LF_ALL		= -1,
	LF_ENABLED	= BIT(0),	// is the light active?
	LF_SHADOWS	= BIT(1),	// does the light cast shadows? (Set if the light causes the affected geometry to cast shadows.)
	LF_PROJTEX	= BIT(2),	// is projective texturing enabled?
	LF_VOLUME	= BIT(3),	// should we draw the light volume?
};

//
//	rxLightDescription - contains some common light parameters.
//
struct rxLightDescription
{
	ColorRGB	color;	// The diffuse color emitted by the light source.

	// Attenuation: constant,linear,quadratic (not used by directional lights).
	Vec3D	attenuation;

	UINT32	flags;

public:
	rxLightDescription()
	{
		rxLightDescription::SetDefaults();
	}

	virtual void SetDefaults()
	{
		color		= FColor::WHITE;

		// attenuation ( 0.1f, 0.05f, 0.025f ) gives good results too
		attenuation.Set( 0.07f, 0.03f, 0.01f );

		flags		= LF_ENABLED;
	}

	virtual bool IsOk() const
	{
		return 1
			;
	}
};

//
//	rxLight - is a base class for dynamic light classes.
//
//	(Lights can cast light onto other objects in a scene.)
//
class rxLight : public rxDrawEntity {
public:
	virtual void	SetColor( const ColorRGB& newDiffuseColor ) = 0;

	FASTBOOL		IsEnabled() const;
	void			SetEnabled( FASTBOOL bEnabled );

	UINT32			GetFlags() const;	// returns bits from ELightFlags
	void			SetFlags( UINT32 newFlags );

protected:
	UINT32		flags;// bits from ELightFlags

protected:
	rxLight()
		: flags( ELightFlags::LF_ENABLED )
	{}
	void Init( const rxLightDescription& desc )
	{
		Assert( desc.IsOk() );
		this->flags = desc.flags;
	}
	virtual ~rxLight()
	{}
};

FORCEINLINE FASTBOOL rxLight::IsEnabled() const {
	return (this->flags & ELightFlags::LF_ENABLED);
}

FORCEINLINE void rxLight::SetEnabled( FASTBOOL bEnabled ) {
	if( bEnabled )
		this->flags |= ELightFlags::LF_ENABLED;
	else
		this->flags &= ~ELightFlags::LF_ENABLED;
}

FORCEINLINE UINT32 rxLight::GetFlags() const {
	return this->flags;
}

FORCEINLINE void rxLight::SetFlags( UINT32 newFlags ) {
	this->flags = newFlags;
}

//
//	rxParallelLight - represents a directional (infinite) light.
//
class rxParallelLight : public rxLight {
public:
	virtual const Vec3D &	GetDirection() const = 0;
	virtual void			SetDirection( const Vec3D& newDir ) = 0;

protected:
	virtual ~rxParallelLight() {}
};

//
//	rxPointLight
//
class rxPointLight : public rxLight {
public:
	virtual const Vec3D &	GetOrigin() const = 0;
	virtual void			SetOrigin( const Vec3D& newOrigin ) = 0;

	// Get/Set the spherical range of the light source.
	virtual FLOAT			GetRange() const = 0;
	virtual void			SetRange( FLOAT newRange ) = 0;

protected:
	virtual ~rxPointLight() {}
};

//
//	rxSpotLight
//
class rxSpotLight : public rxPointLight {
public:
	// Sets the apex angles for the spot light which determine the light's angles of illumination.
	virtual void	SetInnerConeAngle( FLOAT theta ) = 0;
	virtual void	SetOuterConeAngle( FLOAT phi ) = 0;

	virtual const Vec3D &	GetDirection() const = 0;
	virtual void			SetDirection( const Vec3D& newDir ) = 0;

	// Set projective image.
	virtual void	SetProjector( rxTexture* texture ) = 0;

protected:
	virtual ~rxSpotLight() {}
};

//=========================================================
//	Billboards
//=========================================================

//
//	rxBillboard - is a planar billboard.
//
Interface( rxBillboard ) : public rxDrawEntity
{
public:
	// sets the modulation color
	virtual void	SetColor( const ColorRGB& newColor ) = 0;

	// sets the texture for additive blending
	virtual void	SetTexture( rxTexture* newTexture ) = 0;

	virtual void	SetOrigin( const Vec3D& newOrigin ) = 0;
	virtual void	SetSize( const Vec2D& newSize ) = 0;

protected:
	virtual ~rxBillboard() {}
};

//=========================================================
//	Sky model
//=========================================================

Interface( rxSky ) : public rxDrawEntity
{
public:
	
	virtual void	SetOrientation( const Quat& newOrientation ) = 0;

	//virtual void	SetCubemapTexture( rxTexture* skyTexture ) = 0;
	virtual void	SetTextures( rxTexture* sides[AABB::NUM_SIDES] ) = 0;

protected:
	virtual ~rxSky() {}
};

//=========================================================
//	Transformative portals
//=========================================================

const FLOAT MAX_PORTAL_SIZE = 2500.0f;

//
//	rxPortal - is a rectangular transformative portal.
//
Interface( rxPortal ) : public rxDrawEntity
{
public:
	virtual void	SetLookAt( const Vec3D& lookAt ) = 0;

					// NOTE: the matrix should only contain translation and rotation.
	virtual void	SetWorldTransform( const Matrix4& localToWorld ) = 0;

	virtual void	SetSize( FLOAT width, FLOAT height ) = 0;

	virtual void SetBuddy( rxPortal* other ) = 0;
	virtual rxPortal* GetBuddy() = 0;

protected:
	virtual ~rxPortal() {}
};

//=========================================================
//	Post-processing
//=========================================================

//
//	EPostFx - enumerates types of supported post-processing effects.
//
enum EPostFx
{
	PostFx_Unknown	= 0,	// user-defined
	PostFx_Coloring,		// multiply image by specified color
	PostFx_GrayScale,		// monochrome ('Black and White')
	PostFx_BlackAndWhite,	// over-saturated 'Black and White'
	PostFx_InvertColor,		// inverted color (photo negative)
	PostFx_Emboss,
	PostFx_Sharpen,
	PostFx_Sepia,			// sepia tone (photographic print toning)
	PostFx_CellShading,		// cartoonish look
	PostFx_ModulatingWave,	// screen sin/cos distortion

	PostFx_Blur,			// simple blur
	PostFx_GBlur,			// Gaussian blur

	PostFx_Bloom,
	PostFx_DoF,

	PostFx_MotionBlur,
	PostFx_RadialBlur,

	PostFx_HDR,
	PostFx_SSAO,

	NumPostFx
};

const mxChar* GetPostFxName( EPostFx postFxType );

//
//	rxPostEffect
//
class rxPostEffect {
public:
	EPostFx			GetType() const;

	mxBool			IsEnabled() const;
	void			SetEnabled( mxBool bEnabled );

	virtual void	Remove() = 0;

protected:
	mxBool				bEnabled;
	TEnum< EPostFx >	type;

protected:
	rxPostEffect( EPostFx theType = EPostFx::PostFx_Unknown )
		: type( theType )
		, bEnabled( True )
	{}
	virtual ~rxPostEffect()
	{}
};

FORCEINLINE EPostFx rxPostEffect::GetType() const {
	return this->type;
}

FORCEINLINE mxBool rxPostEffect::IsEnabled() const {
	return this->bEnabled;
}

FORCEINLINE void rxPostEffect::SetEnabled( mxBool bEnabled ) {
	this->bEnabled = bEnabled;
}

//
//	rxPostEffectManager
//
Interface( rxPostEffectManager )
{
public:

	// Switches all created post-processing effects on/off.
	virtual void	ToggleAllEffects( bool bEnable ) = 0;

	virtual rxPostEffect *	AddColoring() = 0;
	virtual rxPostEffect *	AddGrayScale() = 0;
	virtual rxPostEffect *	AddBlackAndWhite() = 0;
	virtual rxPostEffect *	AddInvertColor() = 0;
	virtual rxPostEffect *	AddEmboss() = 0;
	virtual rxPostEffect *	AddSharpen() = 0;
	virtual rxPostEffect *	AddSepia() = 0;
	virtual rxPostEffect *	AddCellShading() = 0;
	virtual rxPostEffect *	AddModulatingWave() = 0;

	virtual rxPostEffect *	AddBlur() = 0;
	virtual rxPostEffect *	AddGBlur() = 0;

	virtual rxPostEffect *	AddBloom() = 0;
	virtual rxPostEffect *	AddDepthOfField() = 0;

	virtual rxPostEffect *	AddMotionBlur() = 0;
	virtual rxPostEffect *	AddRadialBlur() = 0;

	virtual rxPostEffect *	AddHDR() = 0;
	virtual rxPostEffect *	AddSSAO() = 0;

protected:
	virtual ~rxPostEffectManager() {}
};

/*
Interface( rxPostEffect_Bloom ) : public rxPostEffect
{
public:

protected:
	virtual ~rxPostEffect_Bloom() {}
};
*/
//=========================================================
//
//	rxScene - represents the graphics scene which can be rendered.
//
//	The graphics scene is populated with render entities and refreshed upon each frame.
//
class rxScene {
public:
	//
	//	Render entity creation.
	//

	virtual rxModel *	CreateModel( const rxModelDescription& desc ) = 0;

	//-----------------------------------------------------------------------
	//	Dynamic lights.
	//-----------------------------------------------------------------------

	// These functions create new dynamic light sources.

	virtual rxParallelLight *	CreateParallelLight( const Vec3D& direction,
										const rxLightDescription& desc = rxLightDescription() ) = 0;

	virtual rxPointLight *		CreatePointLight( const Vec3D& center, FLOAT radius,
										const rxLightDescription& desc = rxLightDescription() ) = 0;

	virtual rxSpotLight *		CreateSpotLight(
										const Vec3D& center, const Vec3D& direction, FLOAT range,
										FLOAT innerAngle, FLOAT outerAngle,
										const rxLightDescription& desc = rxLightDescription() ) = 0;

	// Get/Set ambient light color.
	virtual void	SetAmbientLightColor( const FColor& ambientColor ) = 0;
	virtual const FColor &	GetAmbientLightColor() const = 0;

	//-----------------------------------------------------------------------
	//	Billboards.
	//-----------------------------------------------------------------------

	virtual rxBillboard *		CreateBillboard( rxTexture* texture, const Vec3D& origin,
									const Vec2D& size, const ColorRGB& color = FColor::WHITE ) = 0;

	//-----------------------------------------------------------------------
	//	Sky model.
	//-----------------------------------------------------------------------

	//virtual rxSky *	CreateSkybox( rxTexture* pCubeTexture ) = 0;
	virtual rxSky *	CreateSkybox( rxTexture* sides[AABB::NUM_SIDES] ) = 0;

	//-----------------------------------------------------------------------
	//	Transformative portals.
	//-----------------------------------------------------------------------

	virtual rxPortal *	CreatePortal( FLOAT width, FLOAT height ) = 0;

	//-----------------------------------------------------------------------
	//	Post-processing.
	//-----------------------------------------------------------------------

	virtual rxPostEffectManager* GetPostEffectManager() = 0;

protected:
	virtual ~rxScene() {}
};

/*
=======================================================================
	Graphics resources management.
=======================================================================
*/

class rxResourceStats;

//
//	rxResources - graphics resource manager (manages shaders, geometry, textures, etc).
//
class rxResources {
public:
	//--------------------------------------------------------
	//	Geometry management.
	//--------------------------------------------------------

						// Creates a vertex declaration and returns a handle to it.
	virtual rxVertexFormat*	CreateVertexFormat( const rxVertexDeclaration& decl, rxMaterial* material ) = 0;

						// Creates a new vertex buffer.
	virtual rxVertices*	AllocateVertexBuffer( const rxVertexBufferDescription& desc ) = 0;

						// Creates a new index buffer.
	virtual rxIndices*	AllocateIndexBuffer( const rxIndexBufferDescription& desc ) = 0;

	//--------------------------------------------------------
	// Texture resources.
	//--------------------------------------------------------

	virtual rxTexture*	LoadTexture( const mxFilePath& filename ) = 0;
	virtual rxTexture*	CreateTexture( const void* pData, mxUInt numBytes, const rxTextureDescription& desc ) = 0;

						// Returns the texture given its name (returns null if not found).
	virtual rxTexture*	GetTexture( const mxChar* name ) = 0;

	//--------------------------------------------------------
	//	Render materials.
	//--------------------------------------------------------

	// Parses a file with material description(s) and hashes materials by name for quicker access.
	// Returns the last parsed material (returns NULL upon failure). 'path' can be either relative or absolute.
	//
	virtual rxMaterial*	LoadMaterial( const mxFilePath& filename ) = 0;
	
	virtual rxMaterial*	CreateMaterial( const rxMaterialDescription& desc ) = 0;

	virtual rxMaterial*	GetMaterial( const mxChar* name ) = 0;

	//--------------------------------------------------------
	//	Resource usage statistics.
	//--------------------------------------------------------

	virtual const rxResourceStats &	GetStats() const = 0;	// gets resource usage info
	
	//--------------------------------------------------------
	// Testing & Debugging.
	//--------------------------------------------------------

	// ...

protected:
	virtual ~rxResources() {}
};

/*
=======================================================================
	Performance measurement.
=======================================================================
*/

//
//	rxStatistics - for performance monitoring and statistics, testing & debugging.
//
class rxStatistics
{
public:

	mxUInt	numBatches;		// number of draw calls issued in the last frame
	mxUInt	numEntities;	// number of objects rendered in the last frame

//	mxUInt	numVisibleLights;

	mxUInt	Cpu_cull_time_milliseconds;	// total time spent on high-level culling
//	mxUInt	scene_geometry_culled_percents;	// approx. number in range [0..100]

	mxUInt	renderTime;	// total time spent on rendering the last frame, in milliseconds

	// Work in progress...

public:
	rxStatistics()
	{
		Reset();
	}

	void Reset()
	{
		numBatches = 0;
		numEntities = 0;
//		numVisibleLights = 0;
		Cpu_cull_time_milliseconds = 0;
//		scene_geometry_culled_percents = 0;
		renderTime = 0;
	}
};

//
//	rxResourceStats - graphics resource usage statistics.
//
class rxResourceStats {
public:
	mxUInt	availableTextureMemory;	// approx. amount of free texture memory on GPU, in MBytes.

	mxUInt	numVertexFormats;	// Total number of created vertex types.
	mxUInt	numVertexBuffers;	// Total number of created vertex buffers.
	mxUInt	numIndexBuffers;	// Total number of created index buffers.

	// Work in progress...
};

/*
=======================================================================

	Graphics device info.

=======================================================================
*/

//
//	rxDeviceCapabilities - describes GPU capabilities.
//
struct rxDeviceCapabilities
{
	mxUInt	TotalVideoRAM;	// approx. amount of video RAM, in Mbytes.

	// More to follow...
};

//
// EDeviceVendor - enumerates video card vendors.
//
enum EDeviceVendor
{
	Vendor_Unknown	= 0,
	Vendor_3DLABS	= 1,
	Vendor_ATI		= 2,
	Vendor_Intel	= 3,
	Vendor_Matrox	= 4,
	Vendor_NVidia	= 5,
	Vendor_S3		= 6,
	Vendor_SIS		= 7,
};

EDeviceVendor	GetGPUVendorEnum( mxUInt vendorId );
const WChar *	GetGPUVendorName( EDeviceVendor vendor );

//
//	rxDriverVersion
//
struct rxDriverVersion
{
	UWORD	Product;
	UWORD	Major;
	UWORD	Minor;	// subversion
	UWORD	Build;
};

//
//	rxDeviceInfo - videocard info.
//
//	Contains information about a given display adapter.
//	This info can be used to identify a specific piece of hardware or driver version.
//
struct rxDeviceInfo
{
	enum { MAX_NAME_CHARS = 32 };
	enum { MAX_INFO_CHARS = 512 };

	WChar	Description[ MAX_INFO_CHARS ];	// human readable device description

	// GPU vendor information.
	EDeviceVendor	Vendor;
	WChar	VendorName[ MAX_NAME_CHARS ];

	// Driver info.
	rxDriverVersion		Version;
	WChar	Driver[ MAX_INFO_CHARS ];	// human readable driver name

	// GPU capabilities.
	rxDeviceCapabilities	Caps;
};

//
//	rxDisplayMode - screen settings.
//
struct rxDisplayMode
{
	mxScreenSettings	screen;

	mxUInt			refreshRateHz;

	//EPixelFormat		format;	// display format
	//pixel format
	//window position
	//antialiasing
	//etc
	// ...

public:
			rxDisplayMode();

	void	SetDefaultValues();
	bool	IsValid() const;
};

//
//	rxRendererInfo
//
class rxRendererInfo {
public:
	rxDeviceInfo	DeviceInfo;	// GPU
	
	rxDisplayMode	DisplayMode;	// current screen settings
	
	EDriverType		DriverType;	// the graphics driver (D3D, OpenGL, etc).
	
	union
	{
		// Bare pointers to device objects - usage depends on DriverType.
		// (This is a hacky and unsafe way to get direct access to device context without recompiling the engine.)

		IDirect3DDevice9 *	pD3D9Device;
		ID3D10Device *		pD3D10Device;
		ID3D11Device *		pD3D11Device;

		void *				pDevice;	// This may depend on the device type.
	};

	// Handle to the main window to which all rendering is done.
	void *	pWindowHandle;	// HWND on Windows OS.

public:
	rxRendererInfo()
	{
		// Initialize data for the first time.
		MemZero( this, sizeof(*this) );
		DisplayMode.SetDefaultValues();
	}
};

}//End of namespace abc

#endif // !__RX_RENDERER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
