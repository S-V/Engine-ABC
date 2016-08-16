/*
=============================================================================
	File:	D3D10RenderSystem.h
	Desc:	Direct3D 10 render system includes.
=============================================================================
*/

#ifndef __MX_D3D10_RENDER_SYSTEM_H__
#define __MX_D3D10_RENDER_SYSTEM_H__

//------------------------------------------------------------------------
//	Defines
//------------------------------------------------------------------------

#define MX_D3D10_DEBUG

//
// Enables debug error checking ( check for every HRESULT returned by a function call, etc. ).
//
#define MX_D3DX10_PARANOID

//
// Paths to files with shader resources
//
MX_TODO("read path to shaders from config file")

// this will probably get replaced by strings,
// but for the sake of efficiency it's left as plain chars
#define PATH_TO_SHADERS_	"../../Shaders/HLSL/D3D10/"

//------------------------------------------------------------------------
//	Declarations
//------------------------------------------------------------------------

namespace abc {

// Forward declarations.
class D3D10Renderer;
class D3D10Resources;
class D3D10Scene;

//
//	D3D10StatCounter
//
struct D3D10StatCounter
{
	mxUInt &	timeCounter;
	mxUInt		startTime;

	D3D10StatCounter( mxUInt& theCounter )
		: timeCounter( theCounter )
		, startTime( sys::GetClockTicks() )
	{}

	~D3D10StatCounter()
	{
		mxUInt currTime = sys::GetClockTicks();
		timeCounter += currTime - startTime;
	}
};

#define D3D10_STAT_COUNTER( timeCounter )		D3D10StatCounter((timeCounter))

//
//	D3D10Stats - performance counters.
//
struct D3D10Stats : public rxStatistics
{
	mxUInt	numTriangles;	// number of triangles rendered in the last frame

	mxUInt	numShaderChanges;	// number of SetShader() calls

	mxUInt	shadowMapRenderTime;

public:
	D3D10Stats()
	{
		// Initialize performance counters for the first time.
		Reset();
	}

	// This function should be called at the beginning (or end) of each frame.
	void Reset()
	{
		MemZero( this, sizeof(*this) );
		rxStatistics::Reset();
	}
};

//
//	D3D10RendererInfo
//
struct D3D10RendererInfo : public rxRendererInfo
{
	// add here any d3d10-specific info...

public:
	D3D10RendererInfo()
	{
		DriverType = EDriverType::GAPI_DirectX_10;
	}
	void Clear()
	{
		MemZero( this, sizeof(*this) );
	}
};

}//End of namespace abc

//------------------------------------------------------------------------
//	Global variables
//------------------------------------------------------------------------

namespace abc {

namespace d3d10
{
	//
	// These are initialized by the render system upon start up.
	//

	extern TPtr< ID3D10Device >			device;		// [ read-only ]
	
	extern TPtr< D3D10Renderer >		renderer;	// [ read-only ]
	extern TPtr< D3D10Resources >		resources;	// [ read-only ]

	extern TPtr< D3D10Scene >			scene;		// [ read-only ]

	extern D3D10Stats					stats;	// [ read-write ]

}//End of namespace d3d10

}//End of namespace abc

//------------------------------------------------------------------------
//	Helper macros
//------------------------------------------------------------------------

//
//	Debugging aids.
//

#define ensure( expr )	\
{																							\
	const HRESULT hr = (expr);																\
	const int line = __LINE__;																\
	if ( FAILED( hr ) ) {																	\
		const TCHAR * pRetCodeDesc = d3d10::GetErrorCodeString( hr );						\
		const TCHAR * pErrorStr = DXGetErrorDescription( hr );								\
		sys::FatalError( "'%s',\n'%s' '%s'\n in %s, in '%s', line %d",						\
						#expr, pRetCodeDesc, pErrorStr, __FUNCTION__, __FILE__, line );		\
	}																						\
}


#ifdef MX_DEBUG

#define check( expr )	ensure(expr)

#else //if !MX_DEBUG

#define check( expr )	(expr)

#endif //MX_DEBUG

//------------------------------------------------------------------------
//	Includes
//------------------------------------------------------------------------

//#include "Internal/D3D10Mappings.h"
#include "Internal/D3D10Utilities.h"
//#include "Internal/D3D10MaterialCompiler.h"
#include "Internal/D3D10Shaders.h"
#include "Internal/D3D10Resources.h"
//#include "Internal/D3D10MeshRenderer.h"
//#include "Internal/D3D10SceneRenderer.h"
#include "Internal/D3D10DebugDrawer.h"
//#include "Internal/D3D10RenderQueue.h"
#include "Internal/D3D10Renderer.h"

//#include "Internal/D3D10FullScreenQuad.h"
//#include "Internal/D3D10RenderTargetChain.h"
#include "Internal/D3D10Scene.h"

#endif // !__MX_D3D10_RENDER_SYSTEM_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
