/*
=============================================================================
	File:	D3D10DebugDrawer.h
	Desc:	Direct3D 10 debug visualizer.
=============================================================================
*/

#ifndef __MX_D3D10_DEBUG_DRAWER_H__
#define __MX_D3D10_DEBUG_DRAWER_H__

namespace abc {

//
//	D3D10DebugText - for drawing 2D text on the screen.
//
class D3D10DebugText : public rxDebugText {
public:
	//
	// Override ( rxDebugText ) :
	//
	void	SetText( const char* format, ... ) ATTRIBUTE( (format(printf,1,2)) );
	void	SetColor( const FColor& newColor );
	void	SetPosition( mxUInt x, mxUInt y );

public:
	D3D10DebugText( const rxDebugTextDesc& desc );
	~D3D10DebugText();

	void	Render( ID3DX10Sprite* sprite, ID3DX10Font* font );

public:
	// Temporary string storage.
	enum { MAX_BUFFER_LENGTH = 1024 };
	mxChar	buffer[ MAX_BUFFER_LENGTH ];

	RECT			rect;	// The position of this text on the screen.
	D3DXCOLOR		color;	// The color of this text.
};

struct D3D10DebugTextData
{
	TArray< D3D10DebugText* >	debugTexts;
	DXPtr< ID3DX10Font >		debugFont;		// used for outputting debug text
	DXPtr< ID3DX10Sprite >		debugSprite;	// Sprite for batching draw text calls.

public:
	D3D10DebugTextData()
	{}
	~D3D10DebugTextData()
	{}

	void Init()
	{
		//
		//	Create fonts.
		//
		{
			D3DX10_FONT_DESC  fontDesc;
			fontDesc.Height          = 16;
			fontDesc.Width           = 0;
			fontDesc.Weight          = 0;
			fontDesc.MipLevels       = 1;
			fontDesc.Italic          = false;
			fontDesc.CharSet         = DEFAULT_CHARSET;
			fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
			fontDesc.Quality         = DEFAULT_QUALITY;
			fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;

			sys::StrCpy( fontDesc.FaceName, sizeof(fontDesc.FaceName),
				//TEXT("Times New Roman")
				TEXT("Arial")
				);

			check( D3DX10CreateFontIndirect(
				d3d10::device,
				& fontDesc,
				& this->debugFont.Ptr ) );
		}

		// Create a sprite for more efficient text drawing.
		{
			check( D3DX10CreateSprite(
				d3d10::device,
				32,
				& this->debugSprite.Ptr ) );
		}
	}

	void Destroy()
	{
		this->debugTexts.DeleteContents( true );
		this->debugFont = NULL;
		this->debugSprite = NULL;
	}

	void Render_PostFrame()
	{
		// Set the rasterizer stage to NULL to improve efficiency (we don't use any shaders).
		d3d10::device->RSSetState( NULL );

		// Begin batched rendering.
		this->debugSprite->Begin( D3DX10_SPRITE_SORT_TEXTURE );
		for ( IndexT iText = 0; iText < this->debugTexts.Num(); iText++ )
		{
			this->debugTexts[ iText ]->Render( this->debugSprite, this->debugFont );
		}
		// Finish batched rendering.
		this->debugSprite->End();
	}
};

//
//	rxDebugAxes
//
class D3D10DebugAxes : public rxDebugAxes {
public:
	//
	// Override ( rxDebugAxes ) :
	//
	void SetColor( const FColor& newColor )
	{
	}

	void SetTransform( const Vec3D& translation, const Quat& orientation )
	{
	}

public:
	D3D10DebugAxes();
	~D3D10DebugAxes();

	void	Render();

public:
};

struct D3D10DebugAxesData
{
	TArray< D3D10DebugAxes* >	debugAxes;

public:
	D3D10DebugAxesData()
	{}
	~D3D10DebugAxesData()
	{}

	void Init()
	{
	}

	void Destroy()
	{
		this->debugAxes.DeleteContents( true );
	}

	void Render_PostFrame()
	{
	//{
	//	d3d10::device->IASetInputLayout( mVertexLayout );
	//	d3d10::device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINELIST );

	//	for ( IndexT iObj = 0; iObj < this->debugAxes.Num(); iObj++ )
	//	{
	//		this->debugAxes[ iObj ]->Render();

	//		UINT stride = sizeof(Vertex);
	//		UINT offset = 0;
	//		d3d10::device->IASetVertexBuffers(0, 1, &this->pVB, &stride, &offset);
	//		d3d10::device->IASetIndexBuffer(this->pIB, DXGI_FORMAT_R32_UINT, 0);
	//		d3d10::device->DrawIndexed(this->numTriangles*3, 0, 0);
	//	}
	//}
	}
};

//
//	D3D10DebugDrawer - used for debug visualization.
//
class D3D10DebugDrawer : public rxDebugDrawer {
public:

	//
	// Override ( rxDebugDrawer ) :
	//
	void	DebugPoint( const Vec3D& position, const FLOAT radius = 1.0f,
					const rxDebugDrawOptions& drawOptions = rxDebugDrawOptions() );

	void	DebugLine( const Vec3D& start, const Vec3D& end,
					const rxDebugDrawOptions& drawOptions = rxDebugDrawOptions() );

	void	DebugArrow( const Vec3D& start, const Vec3D& end, const mxUInt size,
					const rxDebugDrawOptions& = rxDebugDrawOptions() );

	void	DebugCircle( const Vec3D& origin, const Vec3D& axis, const FLOAT radius, const mxUInt numSteps = 8,
					const rxDebugDrawOptions& = rxDebugDrawOptions() );

	void	DebugSphere( const Sphere &sphere, const rxDebugDrawOptions& = rxDebugDrawOptions() );

	void	DebugAABB( const AABB& aabb, const rxDebugDrawOptions& = rxDebugDrawOptions() );

	void	DebugOBB( const OOBB& obb, const rxDebugDrawOptions& = rxDebugDrawOptions() );

	//void	DebugCylinder( const Vec3D& center, const FLOAT halfHeight, const mxUInt numSides = 8, const Matrix3 &axis,
	//						const rxDebugDrawOptions& = rxDebugDrawOptions() );

	void	DebugCone( const Vec3D& apex, const Vec3D& dir, const FLOAT radius1, const FLOAT radius2,
					const rxDebugDrawOptions& = rxDebugDrawOptions() );

	void	DebugAxis( const Vec3D& origin, const Matrix3 &axis, const rxDebugDrawOptions& = rxDebugDrawOptions() );

	void	DebugFrustum( const mxViewFrustum &frustum, const rxDebugDrawOptions& = rxDebugDrawOptions() );

			// Clear debug objects with lifeTime beyond the given threshold.
	void	DebugClear( mxUInt lifetimeThreshMs );	// A time of 0 will clear all debug objects.

	//
	// Text drawing for debug visualization.
	//

	// Draw a 3D text string.
	void	DebugText( const char *text, const Vec3D& origin, const Matrix3& viewAxis, const FLOAT size = 1.0f,
									const rxDebugDrawOptions& = rxDebugDrawOptions() );

	// Create a 2D on-screen text at the specified screen position.
	rxDebugText *	CreateText( const rxDebugTextDesc& = rxDebugTextDesc() );

	//
	//	3D models for debug drawing.
	//
	rxDebugMesh *	CreateTeapot();
	rxDebugMesh *	CreateBox( FLOAT width, FLOAT height, FLOAT depth );
	rxDebugMesh *	CreateSphere( FLOAT radius, UINT slices, UINT stacks );
	rxDebugMesh *	CreateCylinder( FLOAT radius1, FLOAT radius2, FLOAT length, UINT slices, UINT stacks );
	rxDebugMesh *	CreateTorus( FLOAT innerRadius, FLOAT outerRadius, UINT sides, UINT rings );
	void			RemoveMesh( rxDebugMesh* pMesh );

	rxDebugAxes *	CreateDebugAxes();

private:
	friend class D3D10Renderer;

	//
	//	Internal functions called by the renderer.
	//

			D3D10DebugDrawer();
			~D3D10DebugDrawer();

	void	Initialize();
	void	Shutdown();

	void	Draw_PostFrame();

private:
	D3D10DebugTextData		debugTextData;
	D3D10DebugAxesData		debugAxesData;
};

}//End of namespace abc

#endif // !__MX_D3D10_DEBUG_DRAWER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
