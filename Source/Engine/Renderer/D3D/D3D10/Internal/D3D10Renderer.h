/*
=============================================================================
	File:	D3D10Renderer.h
	Desc:	Direct3D 10 render system.
=============================================================================
*/

#ifndef __MX_D3D10_RENDERER_H__
#define __MX_D3D10_RENDERER_H__

namespace abc {

//
//	D3D10Renderer
//
class D3D10Renderer : public rxRenderer {
public:
			D3D10Renderer();
			~D3D10Renderer();

	//
	// Override ( rxRenderer ) :
	//

	bool	Create( const rxRenderDeviceCreationInfo& creationInfo );
	void	Destroy();

	//--- Rendering ----------------------------------------------------------------

	rxScene &	GetScene();

	void	RenderScene( const mxSceneView& view, mxScene* scene );

	//--- Graphics resource management. --------------------------------------------

	rxResources &		GetResources();

	//--- Testing and Debugging ----------------------------------------------------

	rxDebugDrawer &		GetDebugDrawer();

	const rxStatistics &	GetStats() const;

	const rxRendererInfo &	GetInfo() const;

	//
	// Override ( mxMessagePort ) :
	//
	void HandleMessage( const TMessagePtr& msg );

public:

	//--- Rendering ----------------------------------------------------

			// Render the mesh with the current shader.
	void	DrawMesh( rxMesh& mesh );

			// Render the mesh with the specified shader.
	//void	DrawMesh( rxMesh& mesh, D3D10Shader& shader );

	void	SetMesh( rxMesh& mesh );
	void	DrawBatch( const rxBatch& batch );

	//--- Other functions ----------------------------------------------------

	ID3D10ShaderResourceView *	GetBackbufferSRV();
	ID3D10RenderTargetView *	GetBackbufferRTV();
	ID3D10DepthStencilView *	GetDepthStencilView();
	ID3D10Texture2D *			GetDepthStencilTexture();

	void	RestoreMainRenderTarget();

private:	// Internal functions.

	//--- Initialization/Destruction ------------------------------------------

	void	OnWindowResized( UINT width, UINT height, DXGI_FORMAT surfaceFormat );
	void	Shutdown();

	//--- Rendering ----------------------------------------------------

	void	SetVertices( const D3D10VertexBuffer* pVB );
	void	SetIndices( const D3D10IndexBuffer* pIB );

	void	RestoreDefaultRenderStates();

	//--- Other functions ----------------------------------------------------

	void	GetRendererInfo( D3D10RendererInfo &OutInfo );
	void	ResetStats();	// reset graphics stats (performance counters,etc)

	//----------------------------------------
	//	Testing & Debugging.
	//----------------------------------------

private:

	//-----------------------------------------------
	// This is needed for lazy state updating.
	//
	struct D3D10RendererState
	{
		const rxMesh *				pCurrentMesh;
		const D3D10VertexFormat *	pVertexLayout;
		const D3D10VertexBuffer *	pVertexBuffer;
		const D3D10IndexBuffer *	pIndexBuffer;

	public:
		D3D10RendererState()
		{
			Reset();
		}
		FORCEINLINE void Reset()
		{
			pCurrentMesh = null;
			pVertexLayout = null;
			pVertexBuffer = null;
			pIndexBuffer = null;
		}
	}								currentState;

	//-----------------------------------------------

	DXPtr< IDXGISwapChain >    		swapChain;

	DXPtr< ID3D10RenderTargetView >		renderTargetView;
	DXPtr< ID3D10ShaderResourceView >	shaderResourceView;
	DXPtr< ID3D10DepthStencilView >		depthStencilView;
	DXPtr< ID3D10Texture2D >			depthStencilTexture;

	D3D10Resources		resources;
	D3D10DebugDrawer	debugDrawer;

private:
	D3D10RendererInfo	rendererInfo;	// Device-specific data.
};

FORCEINLINE
ID3D10ShaderResourceView * D3D10Renderer::GetBackbufferSRV()
{
	return this->shaderResourceView;
}

FORCEINLINE
ID3D10RenderTargetView * D3D10Renderer::GetBackbufferRTV()
{
	return this->renderTargetView;
}

FORCEINLINE
ID3D10DepthStencilView * D3D10Renderer::GetDepthStencilView()
{
	return this->depthStencilView;
}

FORCEINLINE
ID3D10Texture2D * D3D10Renderer::GetDepthStencilTexture()
{
	return this->depthStencilTexture;
}

}//End of namespace abc

#endif // !__MX_D3D10_RENDERER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
