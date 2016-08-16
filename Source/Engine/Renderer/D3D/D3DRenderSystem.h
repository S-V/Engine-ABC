/*
=============================================================================
	File:	D3DRenderSystem.h.h
	Desc:	Direct3D render system includes.
=============================================================================
*/

#ifndef __MX_D3D_RENDER_SYS_H__
#define __MX_D3D_RENDER_SYS_H__

//===========================================================================

#undef MX_D3DX9

// Direct3D render system.

#if defined (MX_D3DX9) || defined(MX_D3DX10)
	#include <Renderer/D3D/DXCommon/DXCommon.h>
#endif

#if defined (MX_D3DX9)
	#include <Renderer/D3D/D3D9/D3D9RenderSystem.h>	// D3DX9 render system.
#endif

#if defined (MX_D3DX10)
	#include <Renderer/D3D/D3D10/D3D10RenderSystem.h>
#endif


#endif // ! __MX_D3D_RENDER_SYS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
