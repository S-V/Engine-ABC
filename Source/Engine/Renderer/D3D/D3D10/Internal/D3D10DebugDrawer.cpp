/*
=============================================================================
	File:	D3D10DebugDrawer.cpp
	Desc:	Direct3D 10 debug visualizer.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>
#include <Renderer/D3D/D3DRenderSystem.h>

namespace abc {

/*================================
		D3D10DebugText
================================*/

D3D10DebugText::D3D10DebugText( const rxDebugTextDesc& desc )
{
	MemSet( & this->rect, 0, sizeof( this->rect ) );
	MemSet( this->buffer, 0, sizeof( this->buffer ) );
}

D3D10DebugText::~D3D10DebugText()
{}

void D3D10DebugText::SetText( const char* format, ... )
{
	va_list	 argptr;		

	va_start( argptr, format );
	vsprintf( this->buffer, format, argptr );
	va_end( argptr );
}

void D3D10DebugText::SetColor( const FColor& newColor )
{
	this->color = d3d::ConvertColor( newColor );
}

void D3D10DebugText::SetPosition( mxUInt x, mxUInt y )
{
	this->rect.left	= x;
	this->rect.top	= y;
}

void D3D10DebugText::Render( ID3DX10Sprite* sprite, ID3DX10Font* font )
{
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	font->DrawText(
		sprite,
		this->buffer,
		-1,
		& this->rect,
		DT_LEFT |DT_TOP | DT_NOCLIP,
		this->color
	);
}

/*================================
		D3D10DebugTextData
================================*/

/*================================
		D3D10DebugAxes
================================*/

D3D10DebugAxes::D3D10DebugAxes()
{
}

D3D10DebugAxes::~D3D10DebugAxes()
{
}

void D3D10DebugAxes::Render()
{
}

/*================================
		D3D10DebugDrawer
================================*/

D3D10DebugDrawer::D3D10DebugDrawer()
{
	ENSURE_ONE_CALL;
}

D3D10DebugDrawer::~D3D10DebugDrawer()
{}

void D3D10DebugDrawer::Initialize()
{
	debugTextData.Init();
	debugAxesData.Init();
}

void D3D10DebugDrawer::Shutdown()
{
	debugTextData.Destroy();
	debugAxesData.Destroy();
}

//
//	D3D10DebugDrawer::DebugPoint
//
void D3D10DebugDrawer::DebugPoint( const Vec3D& position, const FLOAT radius /*= 1.0f*/,
								const rxDebugDrawOptions& options )
{	Unimplemented;
}

//
//	D3D10DebugDrawer::DebugLine
//
void D3D10DebugDrawer::DebugLine( const Vec3D& start, const Vec3D& end,
								const rxDebugDrawOptions& options )
{	Unimplemented;
}

//
//	D3D10DebugDrawer::DebugArrow
//
void D3D10DebugDrawer::DebugArrow( const Vec3D& start, const Vec3D& end, const mxUInt size,
								const rxDebugDrawOptions& options )
{	Unimplemented;
}

//
//	D3D10DebugDrawer::DebugCircle
//
void D3D10DebugDrawer::DebugCircle( const Vec3D& origin, const Vec3D& axis, const FLOAT radius, const mxUInt numSteps /*= 8*/,
								const rxDebugDrawOptions& options )
{	Unimplemented;
}

//
//	D3D10DebugDrawer::DebugSphere
//
void D3D10DebugDrawer::DebugSphere( const Sphere &sphere,
								const rxDebugDrawOptions& options )
{	Unimplemented;
}

//
//	D3D10DebugDrawer::DebugAABB
//
void D3D10DebugDrawer::DebugAABB( const AABB& aabb,
								const rxDebugDrawOptions& options )
{
Unimplemented;
}

//
//	D3D10DebugDrawer::DebugOBB
//
void D3D10DebugDrawer::DebugOBB( const OOBB& obb,
								const rxDebugDrawOptions& options )
{
Unimplemented;
}

//
//	D3D10DebugDrawer::DebugCylinder
//
//void D3D10DebugDrawer::DebugCylinder( const Vec3D& center, const FLOAT halfHeight, const mxUInt numSides = 8, const Matrix3 &axis,
//								const rxDebugDrawOptions& )
//{
//}

//
//	D3D10DebugDrawer::DebugCone
//
void D3D10DebugDrawer::DebugCone( const Vec3D& apex, const Vec3D& dir, const FLOAT radius1, const FLOAT radius2,
								const rxDebugDrawOptions& options )
{	Unimplemented;
}

//
//	D3D10DebugDrawer::DebugAxis
//
void D3D10DebugDrawer::DebugAxis( const Vec3D& origin, const Matrix3 &axis,
								const rxDebugDrawOptions& options )
{	Unimplemented;
}

//
//	D3D10DebugDrawer::DebugFrustum
//
void D3D10DebugDrawer::DebugFrustum( const mxViewFrustum &frustum,
								const rxDebugDrawOptions& options )
{	Unimplemented;
}

//
//	D3D10DebugDrawer::DebugClear
//
void D3D10DebugDrawer::DebugClear( mxUInt lifetimeThreshMs )

{	Unimplemented;
}

//
//	D3D10DebugDrawer::DebugText
//
void D3D10DebugDrawer::DebugText( const char *text, const Vec3D& origin, const Matrix3& viewAxis, const FLOAT size /*= 1.0f*/,
								const rxDebugDrawOptions& options )
{	Unimplemented;
}

//
//	D3D10DebugDrawer::CreateText
//
rxDebugText * D3D10DebugDrawer::CreateText( const rxDebugTextDesc& desc )
{
	D3D10DebugText * pNewText( MX_NEW D3D10DebugText( desc ) );
	pNewText->SetColor( desc.Color );
	pNewText->SetPosition( desc.X, desc.Y );
	if( desc.Text ) {
		pNewText->SetText( desc.Text );
	}

	this->debugTextData.debugTexts.Append( pNewText );
	return pNewText;
}

//
//	D3D10DebugDrawer::CreateTeapot
//
rxDebugMesh * D3D10DebugDrawer::CreateTeapot()
{
Unimplemented;
	return 0;
}

//
//	D3D10DebugDrawer::CreateBox
//
rxDebugMesh * D3D10DebugDrawer::CreateBox( FLOAT width, FLOAT height, FLOAT depth )
{
	Unimplemented;
	return 0;
}

//
//	D3D10DebugDrawer::CreateSphere
//
rxDebugMesh * D3D10DebugDrawer::CreateSphere( FLOAT radius, UINT slices, UINT stacks )
{
	Unimplemented;
	return 0;
}

//
//	D3D10DebugDrawer::CreateCylinder
//
rxDebugMesh * D3D10DebugDrawer::CreateCylinder( FLOAT radius1, FLOAT radius2, FLOAT length, UINT slices, UINT stacks )
{
	Unimplemented;
	return 0;
}

//
//	D3D10DebugDrawer::CreateTorus
//
rxDebugMesh * D3D10DebugDrawer::CreateTorus( FLOAT innerRadius, FLOAT outerRadius, UINT sides, UINT rings )
{
	Unimplemented;
	return 0;
}

//
//	D3D10DebugDrawer::RemoveMesh
//
void D3D10DebugDrawer::RemoveMesh( rxDebugMesh* pMesh )
{
	Unimplemented;
}

rxDebugAxes * D3D10DebugDrawer::CreateDebugAxes()
{
	D3D10DebugAxes * pNewObj( MX_NEW D3D10DebugAxes() );
	this->debugAxesData.debugAxes.Append( pNewObj );
	return pNewObj;
}

void D3D10DebugDrawer::Draw_PostFrame()
{
	// Draw debug axes.
	debugAxesData.Render_PostFrame();

	// Draw debug texts.
	debugTextData.Render_PostFrame();
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
