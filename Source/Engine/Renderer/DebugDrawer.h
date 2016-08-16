/*
=============================================================================
	File:	DebugDrawer.h
	Desc:	Interfaces for debug visualization.
=============================================================================
*/

#ifndef __RX_DEBUG_DRAWER_H__
#define __RX_DEBUG_DRAWER_H__

namespace abc {

class SColor;

class Vec3D;
class Matrix3;
class Sphere;
class AABB;
class OOBB;

class mxViewFrustum;

class rxMaterial;

//
//	rxDebugText
//
//	These are used for drawing a 2D text on the screen.
//	Note: using debug visualization slows down a lot.
//

struct rxDebugTextDesc
{
	mxUInt X, Y; // Screen coordinates of this text.

//	const Char*	FontName;
//	EFontSize	FontSize;

	FColor	Color;	// The colour to draw this text with.

	const char* Text;	// Initial text data.

public:
	rxDebugTextDesc( mxUInt x = 0, mxUInt y = 0 )
		: X( x ), Y( y )
	//	, FontName( 0 )
	//	, FontSize( EFontSize::Font_Medium )
		, Color( FColor::WHITE )
		, Text( null )
	{}
	rxDebugTextDesc( const rxDebugTextDesc& other )
	{
		*this = other;
	}
	void operator = ( const rxDebugTextDesc& other )
	{
		X = other.X;
		Y = other.Y;
		//FontName = other.FontName;
		//FontSize = other.FontSize;
		Color = other.Color;
		Text = other.Text;
	}
};

//
//	rxDebugText
//
class rxDebugText {
public:
	virtual void	SetText( const char* format, ... ) ATTRIBUTE( (format(printf,1,2)) ) = 0;
	virtual void	SetColor( const FColor& newColor ) = 0;
	virtual void	SetPosition( mxUInt x, mxUInt y ) = 0;

protected:
			rxDebugText() {}
	virtual	~rxDebugText() {}
};

//
//	rxDebugMesh - a 3D object for debug purposes.
//
class rxDebugMesh {
public:
	virtual void	SetMaterial( rxMaterial* material ) = 0;
	virtual void	SetWorldTransform( const Matrix4& transform ) = 0;

protected:
	virtual	~rxDebugMesh() {}
};

//
//	rxDebugAxes - used for debug visualization of orthonormal bases.
//
class rxDebugAxes {
public:
	virtual void	SetColor( const FColor& newColor ) = 0;

	virtual void	SetTransform( const Vec3D& translation, const Quat& orientation ) = 0;

protected:
	virtual	~rxDebugAxes()
	{}
};

/*
//
//	Built-in materials for testing and debugging.
//
enum EDebugMaterial
{
	Material_Fallback	= 0,
	Material_Wireframe,
	Material_Semitransparent,
	
	NumDebugMaterials
};
*/
//---------------------------------------------------------------------------

//
//	rxDebugDrawer - a class for debug visualization.
//
//	Note: Using a debug drawer seriously hurts performance.
//

struct rxDebugDrawOptions
{
	// The render material to draw the object with.
	rxMaterial *	material;

public:
	rxDebugDrawOptions()
		: material( NULL )	// <= TODO: initialize the material to default render material
	{}

	rxDebugDrawOptions( rxMaterial *const pMaterial )
		: material( pMaterial )
	{}
};

class rxDebugDrawer {
public:
	virtual void	DebugPoint( const Vec3D& position, const FLOAT radius = 1.0f,
									const rxDebugDrawOptions& drawOptions = rxDebugDrawOptions() ) = 0;

	virtual void	DebugLine( const Vec3D& start, const Vec3D& end,
									const rxDebugDrawOptions& drawOptions = rxDebugDrawOptions() ) = 0;

	virtual void	DebugArrow( const Vec3D& start, const Vec3D& end, const mxUInt size,
									const rxDebugDrawOptions& = rxDebugDrawOptions() ) = 0;

	virtual void	DebugCircle( const Vec3D& origin, const Vec3D& axis, const FLOAT radius, const mxUInt numSteps = 8,
									const rxDebugDrawOptions& = rxDebugDrawOptions() ) = 0;

	virtual void	DebugSphere( const Sphere &sphere,
									const rxDebugDrawOptions& = rxDebugDrawOptions() ) = 0;

	virtual void	DebugAABB( const AABB& aabb,
									const rxDebugDrawOptions& = rxDebugDrawOptions() ) = 0;

	virtual void	DebugOBB( const OOBB& obb,
									const rxDebugDrawOptions& = rxDebugDrawOptions() ) = 0;

	//virtual void	DebugCylinder( const Vec3D& center, const FLOAT halfHeight, const mxUInt numSides = 8, const Matrix3 &axis = Matrix3::mat3_axisY,
	//								const rxDebugDrawOptions& = rxDebugDrawOptions() ) = 0;

	virtual void	DebugCone( const Vec3D& apex, const Vec3D& dir, const FLOAT radius1, const FLOAT radius2,
									const rxDebugDrawOptions& = rxDebugDrawOptions() ) = 0;

	virtual void	DebugAxis( const Vec3D& origin, const Matrix3 &axis,
									const rxDebugDrawOptions& = rxDebugDrawOptions() ) = 0;

	virtual void	DebugFrustum( const mxViewFrustum &frustum,
									const rxDebugDrawOptions& = rxDebugDrawOptions() ) = 0;

	// Clear debug objects with lifeTime beyond the given threshold.
	virtual void	DebugClear( mxUInt lifetimeThreshMs ) = 0;	// A time of 0 will clear all debug objects.

	//
	// Text drawing for debug visualization.
	//

	// Draw a 3D text string.
	virtual void	DebugText( const char *text, const Vec3D& origin, const Matrix3& viewAxis, const FLOAT size = 1.0f,
									const rxDebugDrawOptions& = rxDebugDrawOptions() ) = 0;

	// Create a 2D on-screen text at the specified screen position.
	virtual rxDebugText *	CreateText( const rxDebugTextDesc& = rxDebugTextDesc() ) = 0;

	// Removes this text from the screen.
	//virtual void	RemoveText( rxDebugText* text ) = 0;


	//
	//	3D models for debug drawing.
	//

	virtual rxDebugMesh *	CreateTeapot() = 0;
	virtual rxDebugMesh *	CreateBox( FLOAT width, FLOAT height, FLOAT depth ) = 0;
	virtual rxDebugMesh *	CreateSphere( FLOAT radius, UINT slices, UINT stacks ) = 0;
	virtual rxDebugMesh *	CreateCylinder( FLOAT radius1, FLOAT radius2, FLOAT length, UINT slices, UINT stacks ) = 0;
	virtual rxDebugMesh *	CreateTorus( FLOAT innerRadius, FLOAT outerRadius, UINT sides, UINT rings ) = 0;
	virtual void			RemoveMesh( rxDebugMesh* pMesh ) = 0;

	virtual rxDebugAxes *	CreateDebugAxes() = 0;

protected:
			rxDebugDrawer() {}
	virtual	~rxDebugDrawer() {}
};

}//End of namespace abc

#endif // !__RX_DEBUG_DRAWER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
