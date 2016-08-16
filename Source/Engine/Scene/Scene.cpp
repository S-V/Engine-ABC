/*
=============================================================================
	File:	Scene.cpp
	Desc:	Scene management.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>

namespace abc {

/*================================
		mxSceneDescription
================================*/

void mxSceneDescription::SetDefaults()
{
	Options.SetDefaults();
	Bounds = mxBounds::INFINITE_EXTENT;
	Root = NULL;
	Name = NULL;
}

bool mxSceneDescription::IsValid() const
{
	bool bResult = false;

	bResult &= Bounds.IsValid();

	return bResult;
}

/*================================
		mxSceneComponent
================================*/

mxSceneComponent::mxSceneComponent()
{}

mxSceneComponent::~mxSceneComponent()
{}

void mxSceneComponent::Setup( mxScene& theParentScene )
{
	this->parentScene = & theParentScene;
}

void mxSceneComponent::Close()
{
	this->parentScene = null;
}

void mxSceneComponent::Update( const mxTime deltaTime )
{}

void mxSceneComponent::Validate()
{
	Assert( this->parentScene != null );
}

/*================================
		mxSceneView
================================*/

mxSceneView::mxSceneView()
	: Position	( 0, 0, 0 )
	, LookAt	( 0, 0, 1 )
{
	const Vec3D	UpVector	( 0, 1, 0 );
	const Vec3D	RightVector( 1, 0, 0 );

	this->ViewMatrix.BuildView( this->Position, this->LookAt, UpVector );

	// Initialize the projection matrix.

	const FLOAT NearZ( 0.1f );
	const FLOAT FarZ( 1000.f );

	//const FLOAT FOVx( mxMath::PI / 2 );
	const FLOAT FOVy( mxMath::PI / 2 );

	const FLOAT AspectRatio = 1.333f;	// AspectRation = ScreenWidth / ScreenHeight

	MX_TODO( "NearZ should be pulled as far as possible (value of 1.0f is usually OK) !" )
	ProjMatrix.BuildProjection( FOVy / 2, AspectRatio, NearZ, FarZ );

	RecalculateViewFrustum();
}

mxSceneView::mxSceneView( const mxSceneView& other )
	: Position	( other.Position )
	, LookAt	( other.LookAt )
{
	this->ViewMatrix = other.ViewMatrix;
	this->ProjMatrix = other.ProjMatrix;

	RecalculateViewFrustum();
}

mxSceneView::mxSceneView( ENoInit )
{}

void mxSceneView::SetOrigin( const Vec3D& newOrigin )
{
	this->Position = newOrigin;

	ViewMatrix.BuildView( this->Position, this->LookAt );

	RecalculateViewFrustum();
}

void mxSceneView::SetLookAt( const Vec3D& newLookAt )
{
	Assert( newLookAt.IsNormalized() );
	this->LookAt = newLookAt;

	ViewMatrix.BuildView( this->Position, this->LookAt );

	RecalculateViewFrustum();
}

//
//	mxSceneView::SetView
//
void mxSceneView::SetView( const Vec3D& position, const Vec3D& lookAt,
		const Vec3D& upVector /* = Vec3D( 0, 1, 0 )*/ )
{
	Assert( lookAt.IsNormalized() && upVector.IsNormalized() );

#if 1
	Position = position;
	LookAt = lookAt;
	ViewMatrix.BuildView( Position, LookAt, upVector );
#else
	const Vec3D  vFrom( 0, 0, 1 );
	Matrix3 rot( Matrix3::CreateRotation( vFrom, this->LookAt ) );
	this->ViewMatrix = Matrix4( rot, this->Position ).Inverse();
#endif

	RecalculateViewFrustum();
}

//
//	mxSceneView::SetLens
//
void mxSceneView::SetLens( FLOAT fovY /* =3.14f/2 */, FLOAT aspect /* =1.333f */, FLOAT nearZ /* =1.0f */, FLOAT farZ /* =10000.0f */ )
{
	Assert( fovY > 0 && aspect > 0 );
	Assert( farZ > 0 );
	Assert( nearZ > 0 && nearZ < farZ );
	DEBUG_CODE(
		if ( nearZ <= 1.0f ) sys::Warning( "Near Z plane is too close to the eye!" );
	)

	ProjMatrix.BuildProjection( fovY/2, aspect, nearZ, farZ );

	RecalculateViewFrustum();
}

//
//	mxSceneView::RecalculateViewFrustum
//
void mxSceneView::RecalculateViewFrustum()
{
	Matrix4  viewProj( ViewMatrix * ProjMatrix );
	Frustum.ExtractFrustumPlanes( viewProj );
}

/*================================
		mxCamera
================================*/

mxCamera::mxCamera( const Vec3D& position /* =Vec3D::vec3_origin */, const Vec3D& target /* =Vec3D( 0,0,1 ) */ )
	: m_vUp( 0, 1, 0 )
	, m_vRight( 1, 0, 0 )
{
	SetPosition( position );
	SetTarget( target );
}

//
//	mxCamera::SetLens
//
void mxCamera::SetLens( FLOAT fovY /* =3.14f/2 */, FLOAT aspect /* =1.333f */, FLOAT nearZ /* =0.1f */, FLOAT farZ /* =10000.0f */ )
{
	m_view.SetLens( fovY, aspect, nearZ, farZ );
	m_view.RecalculateViewFrustum();
}

//
//	mxCamera::SetPosition
//
void mxCamera::SetPosition( const Vec3D& newPosition )
{
	m_view.Position = newPosition;

	_UpdateOnTransformChanged();
}

//
//	mxCamera::SetTarget
//
void mxCamera::SetTarget( const Vec3D& newTarget )
{
	// Don't use GetView().Position because it may be incorrect
	// ( camera update events may have not been yet processed ).
	Vec3D  newLookDir( newTarget - GetView().GetOrigin() );
	if ( newLookDir.Length() == 0 ) {
		return;
	}
	newLookDir.Normalize();

	SetLookAt( newLookDir ); // <= this will raise CameraRotated event.
}

//
//	mxCamera::SetLookAt
//
//	FIXME: incorrect rotation.
//
void mxCamera::SetLookAt( const Vec3D& newLookAt )
{
	m_view.LookAt.Normalize();

	// Find the angle of rotation.
	const FLOAT  angle = mxMath::ACos( m_view.LookAt.Dot( newLookAt ) );

	// Find the axis of rotation.
	Vec3D  axis( m_view.LookAt.Cross( newLookAt ) );
	axis.Normalize();

	Matrix4  mat;

	const bool IsAircraftCamera = false;
	if ( IsAircraftCamera ) {
		mat.SetRotationAxis( angle, axis );
	} else {
		mat.SetRotationY( angle );
	}

	// Rotate our view, right, up vectors.
	mat.TransformNormal( m_view.LookAt );
	mat.TransformNormal( m_vRight );

	_UpdateOnTransformChanged();
}

//
//	mxCamera::Zoom
//
void mxCamera::Zoom( FLOAT factor )
{	Assert( factor > 0 );
	//FOV = 2 * atan( 1/zoom );
	Unimplemented;
}

//
//	mxCamera::Walk
//
void mxCamera::Walk( FLOAT fUnits )
{
	const bool IsAircraftCamera = true;
	if ( IsAircraftCamera ) {
		m_view.Position += GetView().LookAt * fUnits;
	}
	else {
		// Move only on XZ plane for land object.
		m_view.Position += Vec3D( GetView().LookAt.x, 0.0f, GetView().LookAt.z ) * fUnits;
	}

	_UpdateOnTransformChanged();
}

//
//	mxCamera::Strafe
//
void mxCamera::Strafe( FLOAT fUnits )
{
	const bool IsAircraftCamera = true;
	if ( IsAircraftCamera ) {
		m_view.Position += m_vRight * fUnits;
	}
	else {
		// Move only on XZ plane for land object.
		m_view.Position += Vec3D( m_vRight.x, 0.0f, m_vRight.z ) * fUnits;
	}

	_UpdateOnTransformChanged();
}

//
//	mxCamera::Fly ( Vertical movement ).
// 
void mxCamera::Fly( FLOAT fUnits )
{
	// Move only on Y-axis for land object.
	m_view.Position.y += fUnits;

	_UpdateOnTransformChanged();
}

//
//	mxCamera::Yaw
//
void mxCamera::Yaw( FLOAT fAngle )
{
	Matrix4  mat;
	mat.SetRotationY( fAngle );	// Rotate around world y (0, 1, 0) always for land objects.

	// Rotate Right and LookAt vectors around y-axis.
	mat.TransformVector( m_vRight );
	mat.TransformVector( m_view.LookAt );

	_UpdateOnTransformChanged();
}

//
//	mxCamera::Pitch
//
void mxCamera::Pitch( FLOAT fAngle )
{
	MX_TODO( "limit the angle" )
	// limit the value of pitch
//	const FLOAT  MAX_ANGLE = mxMath::ONEFOURTH_PI - 0.001f;
//	fPitch = Clamp( fPitch, -MAX_ANGLE, +MAX_ANGLE );

	Matrix4  mat;
	mat.SetRotationAxis( fAngle, m_vRight );

	// Rotate Up and LookAt vectors around Right vector.
	mat.TransformVector( m_vUp );
	mat.TransformVector( m_view.LookAt );

	_UpdateOnTransformChanged();
}

//
//	mxCamera::RecalculateViewMatrix
//
void mxCamera::RecalculateViewMatrix()
{
	// Keep camera's axes orthogonal to each other.
	m_view.LookAt.Normalize();

	m_vUp = GetView().LookAt.Cross( m_vRight );
	m_vUp.Normalize();

	m_vRight = m_vUp.Cross( GetView().LookAt );
	m_vRight.Normalize();

	// Fill in the view matrix entries.
	FLOAT x = -GetView().Position.Dot( m_vRight );
	FLOAT y = -GetView().Position.Dot( m_vUp );
	FLOAT z = -GetView().Position.Dot( GetView().LookAt );

	m_view.ViewMatrix[0][0] = m_vRight.x; 
	m_view.ViewMatrix[1][0] = m_vRight.y; 
	m_view.ViewMatrix[2][0] = m_vRight.z; 
	m_view.ViewMatrix[3][0] = x;   

	m_view.ViewMatrix[0][1] = m_vUp.x;
	m_view.ViewMatrix[1][1] = m_vUp.y;
	m_view.ViewMatrix[2][1] = m_vUp.z;
	m_view.ViewMatrix[3][1] = y;  

	m_view.ViewMatrix[0][2] = m_view.LookAt.x; 
	m_view.ViewMatrix[1][2] = m_view.LookAt.y; 
	m_view.ViewMatrix[2][2] = m_view.LookAt.z; 
	m_view.ViewMatrix[3][2] = z;   

	m_view.ViewMatrix[0][3] = 0.0f;
	m_view.ViewMatrix[1][3] = 0.0f;
	m_view.ViewMatrix[2][3] = 0.0f;
	m_view.ViewMatrix[3][3] = 1.0f;
}

//
//	mxCamera::_UpdateOnTransformChanged
//
void mxCamera::_UpdateOnTransformChanged()
{
	RecalculateViewMatrix();
	m_view.RecalculateViewFrustum();
}

/*================================
		mxVisibleSet
================================*/

mxVisibleSet::mxVisibleSet( SizeT initialCount /* = 256 */ )
	: m_visibleObjects( initialCount )
{}

mxVisibleSet::~mxVisibleSet()
{}

//
//	mxVisibleSet::Add
//
void mxVisibleSet::Add( mxEntity* pObject )
{	Assert( pObject );
	m_visibleObjects.Append( pObject );
}

//
//	mxVisibleSet::GetNum
//
SizeT mxVisibleSet::GetNum() const
{
	return m_visibleObjects.Num();
}

//
//	mxVisibleSet::Get
//
mxEntity * mxVisibleSet::Get( IndexT index )
{
	return m_visibleObjects[ index ];
}

//
//	mxVisibleSet::Empty
//
void mxVisibleSet::Empty()
{
	m_visibleObjects.SetNum( 0, false /* don't resize */ );
}

/*================================
			mxScene
================================*/

mxScene::mxScene()
{}

//
//	mxScene::~mxScene
//
mxScene::~mxScene()
{}

//
//	mxScene::Initialize
//
void mxScene::Initialize( const mxSceneDescription& creationInfo )
{
	(void) creationInfo;

	if( null == activeCamera )
	{
		activeCamera = MX_NEW mxCamera();
	}

	if( null == spatialHash )
	{
		spatialHash = MX_NEW mxSpatialDatabase_Simple();
		spatialHash->Setup( *this );
	}
}

void mxScene::Shutdown()
{
	entities.Clear();
	spatialHash = null;

	activeCamera = null;
}

//
//	mxScene::Add
//
void mxScene::Add( mxEntity* entity )
{
	AssertPtr( entity );

	entities.Append( entity );

	GetSpatialDatabase().Add( entity );
}

//
//	mxScene::Remove
//
void mxScene::Remove( mxEntity* entity )
{
	AssertPtr( entity );
	Unimplemented;
}

//
//	mxScene::Tick
//
void mxScene::Tick( const mxTime deltaTime )
{
	// Let each entity think.
	for ( IndexT iEntity = 0; iEntity < entities.Num(); iEntity++ )
	{
		mxEntity * pEntity = entities[ iEntity ];
		pEntity->Think( deltaTime );
	}

	// Tick subsystems.
	{
		GetSpatialDatabase().Update( deltaTime );
	}
}

//
//	mxScene::Present
//
void mxScene::Present()
{
	MX_PROFILE("Scene: Present");
	mxEngine::get().GetRenderer().RenderScene( GetActiveCamera().GetView(), this );
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
