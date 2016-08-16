/*
=============================================================================
	File:	D3D10Renderer.cpp
	Desc:	Direct3D 10 render system.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>
#include <Renderer/D3D/D3DRenderSystem.h>

namespace abc {

//
//	Global variables
//
namespace d3d10 {

	TPtr< ID3D10Device >		device;
	TPtr< D3D10Renderer >		renderer;
	TPtr< D3D10Resources >		resources;
	
	D3D10Scene			theScene;
	TPtr< D3D10Scene >	scene( &theScene );

	D3D10Stats					stats;

}//End of namespace d3d10

/*================================
		D3D10Renderer
================================*/

D3D10Renderer::D3D10Renderer()
{
	ENSURE_ONE_CALL;
}

D3D10Renderer::~D3D10Renderer()
{}

//
//	D3D10Renderer::Create
//

bool D3D10Renderer::Create( const rxRenderDeviceCreationInfo& creationInfo )
{
	//	Initialize our device data for the first time.
	{
		this->rendererInfo.Clear();

		this->rendererInfo.DriverType = EDriverType::GAPI_DirectX_10;
		this->rendererInfo.pWindowHandle = creationInfo.pWindowHandle;	// Init the handle to the output window.
	}

	const DXGI_FORMAT  backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	const UINT windowWidth = creationInfo.screen.width;
	const UINT windowHeight = creationInfo.screen.height;

	// The swap chain is responsible for taking the buffer that the device renders to and displaying the content on the actual monitor screen.
	DXGI_SWAP_CHAIN_DESC  sd;
	ZeroMemory( &sd, sizeof(sd) );

	sd.BufferCount						= 1;
	sd.BufferDesc.Width					= windowWidth;
	sd.BufferDesc.Height 				= windowHeight;
	sd.BufferDesc.Format 				= backBufferFormat;
	sd.BufferDesc.RefreshRate.Numerator = creationInfo.refreshRateHz;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.ScanlineOrdering		= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling				= DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_SHADER_INPUT;
	
	sd.OutputWindow						= (HWND) this->rendererInfo.pWindowHandle;	// the window that the swap chain will use to present images on the screen

	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;	// the contents of the back buffer are discarded after calling IDXGISwapChain::Present()

	// No multisampling.
	sd.SampleDesc.Count					= 1;	// SampleDesc is used to enable multi-sampling
	sd.SampleDesc.Quality				= 0;	// multi-sampling disabled

	sd.Windowed							= ! creationInfo.screen.isFullscreen;
	sd.Flags							= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// flag for switching between windowed/fullscreen modes

	// This D3DX version of create device tries to create a DX10.1 interface if possible (needs Vista service pack 1)
	// Otherwise gets a DX10 interface.

	UINT  createDeviceFlags = 0;

	DEBUG_CODE
	(
		createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
	)

	// Create a Direct3D 10.0 device and a swap chain.
	ID3D10Device * pd3dDevice10 = NULL;
	{
		// Determine the implementation of Direct3D10 to use.
		
		const D3D10_DRIVER_TYPE driverTypes[] =
		{
			D3D10_DRIVER_TYPE_HARDWARE,
			D3D10_DRIVER_TYPE_REFERENCE,
		};

		const UINT numDriverTypes = sizeof( driverTypes ) / sizeof( driverTypes[0] );
		bool bCreatedOk = 0;

		for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
		{
			const D3D10_DRIVER_TYPE driverType = driverTypes[ driverTypeIndex ];

			const HRESULT hr = D3D10CreateDeviceAndSwapChain(
				NULL,					// default adapter (monitor)
				driverType,
				NULL,					// A handle to the DLL that implements a software rasterizer. Must be NULL if driverType is non-software.
				createDeviceFlags,
				D3D10_SDK_VERSION,		// should be D3D10_SDK_VERSION
				& sd,					// swap chain description
				& this->swapChain.Ptr,
				& pd3dDevice10
			);

			if( SUCCEEDED( hr ) ) {
				bCreatedOk = true;
				break;
			}
		}

		if( ! bCreatedOk )
		{
			sys::ShowMessage( "Error", "Failed to create device and swap chain" );
			return false;
		}

		AssertPtr( this->swapChain );
	}

	// Initialize global pointer variables.
	{
		AssertPtr( pd3dDevice10 );
		d3d10::device = pd3dDevice10;

		d3d10::renderer		= this;
		d3d10::resources	= & this->resources;

		d3d10::stats.Reset();
	}

	// Record system information.
	GetRendererInfo( this->rendererInfo );

	//-------
	d3d10::theScene.Initialize( windowWidth, windowHeight );

	this->resources.Initialize();
	this->debugDrawer.Initialize();

	// The remaining steps should be done whenever the output window is resized.
	this->OnWindowResized( windowWidth, windowHeight, backBufferFormat );

	// Return success.
	return true;
}

//
//	D3D10Renderer::GetRendererInfo
//
void D3D10Renderer::GetRendererInfo( D3D10RendererInfo &OutInfo )
{
	OutInfo.DriverType = EDriverType::GAPI_DirectX_10;
	OutInfo.pD3D10Device = d3d10::device;
	OutInfo.pWindowHandle = this->rendererInfo.pWindowHandle;

	IDXGIDevice * pDXGIDevice = null;
	check( d3d10::device->QueryInterface(
		__uuidof(IDXGIDevice),
		(void **) &pDXGIDevice )
	);
	AssertPtr( pDXGIDevice );

	DXPtr< IDXGIAdapter > pAdapter;
	check( pDXGIDevice->GetAdapter( &pAdapter.Ptr ) );

	DXGI_ADAPTER_DESC  adapterDesc;
	check( pAdapter->GetDesc( &adapterDesc) );

	wcscpy( OutInfo.DeviceInfo.Description, adapterDesc.Description );
	wcscpy( OutInfo.DeviceInfo.VendorName, GetGPUVendorName( GetGPUVendorEnum( adapterDesc.VendorId ) ) );

	MX_UNDONE
	//UINT DeviceId;
	//UINT SubSysId;
	//UINT Revision;
	MemZero( & OutInfo.DeviceInfo.Version, sizeof(OutInfo.DeviceInfo.Version) );
	MemZero( OutInfo.DeviceInfo.Driver, sizeof(OutInfo.DeviceInfo.Driver) );

	//
	//	Device capabilities.
	//
	OutInfo.DeviceInfo.Caps.TotalVideoRAM = adapterDesc.DedicatedVideoMemory / (1024*1024);

	//
	//	Get current display mode.
	//
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	check( this->swapChain->GetDesc( &swapChainDesc ) );

	OutInfo.DisplayMode.screen.isFullscreen = (FALSE == swapChainDesc.Windowed);
	OutInfo.DisplayMode.screen.width = swapChainDesc.BufferDesc.Width;
	OutInfo.DisplayMode.screen.height = swapChainDesc.BufferDesc.Height;

	OutInfo.DisplayMode.refreshRateHz = swapChainDesc.BufferDesc.RefreshRate.Numerator
										/ swapChainDesc.BufferDesc.RefreshRate.Numerator;
}

//
//	D3D10Renderer::OnWindowResized
//
void D3D10Renderer::OnWindowResized( UINT width, UINT height, DXGI_FORMAT surfaceFormat )
{
	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.
	//
	this->renderTargetView = null;
	this->shaderResourceView = null;
	this->depthStencilView = null;
	this->depthStencilTexture = null;

	// OnWindowResized the swap chain and recreate the render target view.

	check( this->swapChain->ResizeBuffers(
		1,							// BufferCount
		width,
		height,
		surfaceFormat,
		0							// SwapChainFlags
	));
	
	// First get the back buffer from the swap chain just created - it is treated as a 2D texture.
	{
		ID3D10Texture2D * pBackBuffer = null;

		check( this->swapChain->GetBuffer(
			0,
			__uuidof( ID3D10Texture2D ),
			reinterpret_cast< LPVOID* >( &pBackBuffer )
		));

		// Create a "render target view" object from the back buffer.
		//
		check( d3d10::device->CreateRenderTargetView(
			pBackBuffer,
			null,			// D3D10_RENDER_TARGET_VIEW_DESC
			& this->renderTargetView.Ptr
		));
		AssertPtr( this->renderTargetView );

		// Create a shader-resource view.
		//
		check( d3d10::device->CreateShaderResourceView(
			pBackBuffer,
			null,
			& shaderResourceView.Ptr )
		);
		AssertPtr( this->shaderResourceView );

		SAFE_RELEASE( pBackBuffer );
	}

	//
	// Create depth stencil texture from the texture we created above.
	//
	{
		bool bUseStencil = false;
		D3D10_TEXTURE2D_DESC  descDepth;
		descDepth.Width				= width;
		descDepth.Height			= height;
		descDepth.MipLevels 		= 1;
		descDepth.ArraySize 		= 1;
		descDepth.Format			= bUseStencil ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count	= 1;	// multisampling must match
		descDepth.SampleDesc.Quality = 0;	// swap chain values
		descDepth.Usage				= D3D10_USAGE_DEFAULT;
		descDepth.BindFlags 		= D3D10_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags	= 0;
		descDepth.MiscFlags			= 0;

		if ( FAILED( d3d10::device->CreateTexture2D(
			&descDepth,
			NULL,
			& this->depthStencilTexture.Ptr ) ) )
		{
			sys::ShowMessage( "Error", "Failed to create depth stencil texture" );
			return;
		}

		AssertPtr( this->depthStencilTexture );
	}

	// Create depth stencil view.
	{
		check( d3d10::device->CreateDepthStencilView(
			this->depthStencilTexture,
			0,
			& this->depthStencilView.Ptr
		));

		AssertPtr( this->depthStencilView );
	}

	// Bind the render target view and depth/stencil view to the pipeline.
	// Set the "views" needed for the "output-merger" stage. Effectively bind the back buffer and depth/stencil buffer
	// to the end of the rendering pipeline. We can use this function to bind normal textures as render targets as
	// needed for post-processing. This function also allows us to bind to multiple targets
	//
	d3d10::device->OMSetRenderTargets(
		1,
		& this->renderTargetView.Ptr,
		this->depthStencilView
	);

	// Set the viewport transform.
	// Indentify the viewport (window) dimensions used in the "rasterisation stage". This is the (non-programmable)
	// step just before the pixel shader, that converts projected 2D primitives into into individual pixels and pixel
	// shader calls. Possible to define multiple viewports, with the one to use selected by a geometry shader.
	{
		D3D10_VIEWPORT  vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width    = width;
		vp.Height   = height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		d3d10::device->RSSetViewports( 1, &vp );
	}

	this->rendererInfo.DisplayMode.screen.width = width;
	this->rendererInfo.DisplayMode.screen.height = height;
}

//
//	D3D10Renderer::Destroy
//
void D3D10Renderer::Destroy()
{
	Shutdown();

	MX_FREE( this );
}

//
//	D3D10Renderer::Shutdown
//
void D3D10Renderer::Shutdown()
{
	RestoreDefaultRenderStates();	// prevent D3D10 warnings

	d3d10::theScene.Shutdown();
	
	if ( d3d10::device.IsValid() )
	{
		d3d10::device->ClearState();
	}

	this->debugDrawer.Shutdown();

	this->resources.Shutdown();
	d3d10::resources = null;

	// Switch it to the windowed state before releasing swap chain if needed.
	if ( this->swapChain != null )
	{
		BOOL bFullscreen;
		DXPtr< IDXGIOutput > pTarget;
		check( this->swapChain->GetFullscreenState( &bFullscreen, &pTarget.Ptr ) );
		if ( bFullscreen )
		{
			AssertPtr( pTarget );
			check( this->swapChain->SetFullscreenState( false, pTarget ) );
		}
		this->swapChain			= null;
	}
	this->renderTargetView = null;
	this->shaderResourceView = null;
	this->depthStencilView = null;
	this->depthStencilTexture = null;

	this->rendererInfo.Clear();

	d3d10::stats.Reset();

	d3d10::renderer	= null;

	SAFE_RELEASE( d3d10::device );
}

//
//	D3D10Renderer::GetDebugDrawer
//
rxDebugDrawer & D3D10Renderer::GetDebugDrawer()
{
	return this->debugDrawer;
}

//
//	D3D10Renderer::GetStats
//
const rxStatistics & D3D10Renderer::GetStats() const
{
	return d3d10::stats;
}

//
//	D3D10Renderer::GetInfo
//
const rxRendererInfo & D3D10Renderer::GetInfo() const
{
	return this->rendererInfo;
}

//
//	D3D10Renderer::GetResources
//
rxResources & D3D10Renderer::GetResources()
{
	return this->resources;
}

//
//	D3D10Renderer::RenderScene
//
void D3D10Renderer::RenderScene( const mxSceneView& view, mxScene* scene )
{
	// reset performance counters
	d3d10::stats.Reset();

	// reset current state
	this->currentState.Reset();

	D3D10_STAT_COUNTER( d3d10::stats.renderTime );
	{
		// Clear the main render target.
		//
		d3d10::device->ClearRenderTargetView(
			this->renderTargetView,
			(const FLOAT*) FColor::BLACK
		);

		// Clear the depth and stencil buffers.
		//
		d3d10::device->ClearDepthStencilView(
			this->depthStencilView,
			D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL,
			1.0f,	// clear depth buffer to this value
			0		// stencil value
		);
	}

	//
	//	Do actual rendering.
	//
	{
		d3d10::theScene.DrawScene( view, scene );
	}

	// Render overlays, HUDs, debug texts, etc.
	{
		this->debugDrawer.Draw_PostFrame();
	}

	// Finish rendering the current frame and present the contents of the backbuffer to the screen.

	// Present the information rendered to the back buffer to the front buffer (the screen).

	check( this->swapChain->Present(
		0,	// SyncInterval, first parameter = 0 means present back buffer immediately (no vsync), first parameter = 1 means wait for next vsync
		0	// Flags
	));

	//RestoreDefaultRenderStates();
}

void D3D10Renderer::RestoreDefaultRenderStates()
{
	const FLOAT * blendFactor = (const FLOAT*) d3d::BLACK;
	const UINT sampleMask   = 0xFFFFFFFF;
	d3d10::device->OMSetBlendState( NULL, blendFactor, sampleMask );

	d3d10::device->OMSetDepthStencilState( NULL, 0 );	// Restore default state, because ID3DX10Font::DrawText changes them.

	d3d10::device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	d3d10::device->IASetInputLayout( NULL );
	d3d10::device->IASetIndexBuffer( NULL, DXGI_FORMAT_R16_UINT, 0 );
	
	//ID3D10Buffer * vertexBuffers[] = { null };
	//d3d10::device->IASetVertexBuffers(
	//	0,				// StartSlot
	//	1,				// NumBuffers
	//	vertexBuffers,	// ppVertexBuffers
	//	0,				// VertexSize
	//	0				// Offset
	//);

	// Unbind all shader resources.

	ID3D10ShaderResourceView * pSRV[ D3D10RenderTargetChain::NUM_RENDER_TARGETS ] = { 0 };
	d3d10::device->VSSetShaderResources(
		0,
		D3D10RenderTargetChain::NUM_RENDER_TARGETS,
		pSRV
	);
	d3d10::device->GSSetShaderResources(
		0,
		D3D10RenderTargetChain::NUM_RENDER_TARGETS,
		pSRV
	);
	d3d10::device->PSSetShaderResources(
		0,
		D3D10RenderTargetChain::NUM_RENDER_TARGETS,
		pSRV
	);
}

rxScene & D3D10Renderer::GetScene() {
	return d3d10::theScene;
}

void D3D10Renderer::HandleMessage( const TMessagePtr& aMsg )
{
/*	if ( aMsg->IsA( rxMessage_OnWindowResized::Type ) )
	{
		const rxMessage_OnWindowResized & theMsg = (const rxMessage_OnWindowResized& )( aMsg );
		this->OnWindowResized( theMsg.width, theMsg.height, this->rendererInfo.backBufferFormat10 );
	}
*/
}

void D3D10Renderer::RestoreMainRenderTarget()
{
	d3d10::device->OMSetRenderTargets(
		1,
		& this->renderTargetView.Ptr,
		this->depthStencilView
	);

	D3D10_VIEWPORT vp =
	{
		0,
		0,
		this->rendererInfo.DisplayMode.screen.width,
		this->rendererInfo.DisplayMode.screen.height,
		0.0f,
		1.0f
	};
	d3d10::device->RSSetViewports( 1, &vp );
}

void D3D10Renderer::DrawMesh( rxMesh& mesh )
{
	D3D10VertexBuffer * pVB10 = checked_cast< D3D10VertexBuffer*, rxVertices* >( mesh.Vertices );
	D3D10IndexBuffer *  pIB10 = checked_cast< D3D10IndexBuffer*, rxIndices* >( mesh.Indices );

	// Bind vertex buffer to the pipeline.
	SetVertices( pVB10 );

	Assert2( mesh.PrimitiveType == EPrimitiveType::PT_TriangleList, "Only triangle lists are impl" );
	{
		d3d10::device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		SetIndices( pIB10 );

		d3d10::device->DrawIndexed(
			pIB10->NumIndices,
			0,	// StartIndexLocation
			0	// BaseVertexLocation
			);

		// Update performance counters.

		const mxUInt triangleCount = (pIB10->NumIndices) / 3;

		d3d10::stats.numBatches++;
		d3d10::stats.numTriangles += triangleCount;
	}
}

void D3D10Renderer::SetMesh( rxMesh& mesh )
{
//	if ( this->currentState.pCurrentMesh != & mesh )
	{
		D3D10VertexBuffer * pVB10 = checked_cast< D3D10VertexBuffer*, rxVertices* >( mesh.Vertices );
		D3D10IndexBuffer *  pIB10 = checked_cast< D3D10IndexBuffer*, rxIndices* >( mesh.Indices );

		SetVertices( pVB10 );
		SetIndices( pIB10 );

		Assert2( mesh.PrimitiveType == EPrimitiveType::PT_TriangleList, "Only triangle lists are impl" );
		d3d10::device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		this->currentState.pCurrentMesh = & mesh;
	}
}

void D3D10Renderer::DrawBatch( const rxBatch& batch )
{
	d3d10::device->DrawIndexed(
		batch.IndexCount,
		batch.StartIndex,
		batch.StartVertex
	);
}

void D3D10Renderer::SetVertices( const D3D10VertexBuffer* pNewVB )
{
//	if ( this->currentState.pVertexBuffer != pNewVB )
	{
		const UINT offset = 0;

		d3d10::device->IASetVertexBuffers(
			0,				// StartSlot
			1,				// NumBuffers
			& pNewVB->pVB.Ptr,
			& pNewVB->VertexSize,
			& offset
		);

		this->currentState.pVertexBuffer = pNewVB;
	}
}

void D3D10Renderer::SetIndices( const D3D10IndexBuffer* pNewIB )
{
//	if ( this->currentState.pIndexBuffer != pNewIB )
	{
		d3d10::device->IASetIndexBuffer(
			pNewIB->pIB,
			pNewIB->Format,
			0	// Offset
		);

		this->currentState.pIndexBuffer = pNewIB;
	}
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
