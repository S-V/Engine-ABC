#include <Base.h>
#include <Engine.h>
#include <MiniSG.h>

#include <stdlib.h>

#pragma warning( push )
#pragma warning( disable: 4189 )	// local variable is initialized but not referenced
#pragma warning( disable: 4702 )	// unreachable code
#pragma warning( disable: 4800 )	// forcing value to bool 'true' or 'false' (performance warning)

using namespace ::abc;

#include "../Examples/Example_Basic.h"
#include "../Examples/Example_HelloCube.h"
#include "../Examples/Example_Meshes.h"
//#include "../Examples/Example_Lighting.h"
#include "../Examples/Example_Shadows.h"
#include "../Examples/Example_CSG.h"
#include "../Examples/Example_PostProcessing.h"
//#include "../Examples/Example_Shaders.h"
//#include "../Examples/Example_Portals.h"
#include "../Examples/Example_Test.h"

#pragma warning( pop )


//
//	SampleBrowser
//
class SampleBrowser
{
public:
	SampleBrowser()
	{}
	~SampleBrowser()
	{}

	Sample_Basic* SelectApp()
	{
#ifdef _WINDOWS

		// return the last
		return this->apps.GetLast();

#else // #if defined(_CONSOLE_)
		sys::Print(TEXT("\nEnter the number of application to run:\n"));
		for( unsigned i = 0; i < (unsigned) this->apps.Num(); i++ )
		{
			sys::Print(TEXT("\n%d - %s"), i, this->apps[i]->GetName() );
		}
		sys::Print(TEXT("\n\n: "));

		int n;
		if( 1 != scanf( "%d", &n ) ) {
			sys::Print(TEXT("\nThe number was entered incorrectly!\n"));
			return null;
		}
		if( n >= 0 && n < (int) this->apps.Num() )
		{
			return this->apps[ n ];
		}
		else {
			sys::Print(TEXT("\nInvalid number!\n"));
			return null;
		}
#endif
	}

	Sample_Basic* GetLast()
	{
		return this->apps.GetLast();
	}

	void RegisterApp( Sample_Basic* pApp )
	{
		if( pApp ) {
			this->apps.Append( pApp );
		}
	}

private:
	TArray< Sample_Basic* >	apps;
};

#ifdef _WINDOWS

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{

#else //if defined(_CONSOLE_)

int main( int argc, char *argv[] )
{
	struct SDummy
	{
		static void WaitForKey()
		{
			::printf("\nPress Enter...\n");
			::getchar();
		}
	};
	::atexit( SDummy::WaitForKey );

#endif //!_WINDOWS

	using namespace ::abc;

	struct MyApp : public mxApplication
	{
		MyApp()
		{}
	} emptyApp;

	mxApplication * userApp = &emptyApp;

	Sample_Basic		appBasic;
	Sample_HelloCube	helloCube;
	Sample_Meshes		appMeshes;
	Sample_Shadows		appShadows;
	Sample_CSG			appCsg;
	Sample_PostProcess	appPostProcess;
	//Sample_Portals		appPortals;
	AppTest				appTest;

	SampleBrowser	browser;
	browser.RegisterApp( &appBasic );
	browser.RegisterApp( &helloCube );
	browser.RegisterApp( &appMeshes );
	browser.RegisterApp( &appShadows );
	browser.RegisterApp( &appCsg );
	browser.RegisterApp( &appPostProcess );
	//browser.RegisterApp( &appPortals );
	browser.RegisterApp( &appTest );


	userApp = 1 ? browser.SelectApp() : browser.GetLast();
	if( ! userApp ) {
		return -1;
	}


	mxSystemCreationInfo  cInfo;
	cInfo.pUserApp = userApp;
	cInfo.screen.width = 800;
	cInfo.screen.height = 600;
	cInfo.screen.isFullscreen = false;
	cInfo.driverType = 
	//	EDriverType::GAPI_DirectX_9
		EDriverType::GAPI_DirectX_10
	//	EDriverType::GAPI_AutomaticSelection
		;

	mxEngine * engine = CreateEngine( cInfo );

	return engine->Run();
}
