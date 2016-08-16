/*
=============================================================================
	File:	DXCommon.cpp
	Desc:	Common DirectX stuff.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>
#include <Renderer/D3D/D3DRenderSystem.h>

namespace abc {

/*================================
		d3d
================================*/

namespace d3d
{

//
//	Colours.
//
const D3DXCOLOR 	WHITE				(	1.0f, 1.0f, 1.0f, 1.0f	);
const D3DXCOLOR 	BLACK				(	0.0f, 0.0f, 0.0f, 1.0f	);
const D3DXCOLOR 	RED					(	1.0f, 0.0f, 0.0f, 1.0f	);
const D3DXCOLOR 	GREEN				(	0.0f, 1.0f, 0.0f, 1.0f	);
const D3DXCOLOR 	BLUE				(	0.0f, 0.0f, 1.0f, 1.0f	);
const D3DXCOLOR 	YELLOW				(	1.0f, 1.0f, 0.0f, 1.0f	);
const D3DXCOLOR 	CYAN				(	0.0f, 1.0f, 1.0f, 1.0f	);
const D3DXCOLOR 	MAGENTA				(	1.0f, 0.0f, 1.0f, 1.0f	);

const D3DXCOLOR 	BEACH_SAND			(	1.0f, 0.96f, 0.62f, 1.0f	);
const D3DXCOLOR 	LIGHT_YELLOW_GREEN	(	0.48f, 0.77f, 0.46f, 1.0f	);
const D3DXCOLOR 	DARK_YELLOW_GREEN	(	0.1f, 0.48f, 0.19f, 1.0f	);
const D3DXCOLOR 	DARKBROWN			(	0.45f, 0.39f, 0.34f, 1.0f	);

D3DXCOLOR ConvertColor( const SColor& color )
{
	return D3DXCOLOR( color.ToInt() );
}

D3DXCOLOR ConvertColor( const FColor& color )
{
	return D3DXCOLOR( (const FLOAT*) color );
}

}//End of namespace d3d

/*================================
		DXPtr
================================*/

#ifdef MX_DEBUG
namespace d3d
{
	mxLong	TotalRefCount = 0;
}
#endif //MX_DEBUG

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
