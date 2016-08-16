/*
=============================================================================
	File:	Win32_MVCpp.cpp
	Desc:	Win32-specific code for MVC++ compilers.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Base.h>

// For file path utils.
//#include <shlwapi.h>
//#pragma comment( lib, "shlwapi.lib" )

//==========================================================================

namespace abc {

	using namespace sys;

// Helper macro to avoid repetitive typing.
//
#define GETTER( ReturnType, MethodName, MemberVariable ); \
	ReturnType MethodName()								\
	{													\
		assert( bInitialized );							\
		return (MemberVariable);						\
	}

//
//	Win32_SystemLayer
//
class Win32_SystemLayer {
public:
	GETTER( const SystemInfo_s&,	GetSystemInfo,				m_sysInfo );
	GETTER( EEndianness,			GetMachineEndianness,		m_machineEndianness );
	GETTER( mxUInt,					GetCpuFrequencyHz,			m_iCpuFrequencyHz );
	GETTER( mxReal,					GetCpuFrequencyHz_F,		m_fCpuFrequencyHz );
	GETTER( const MEMORYSTATUS&,	GetExeLaunchMemoryStatus,	m_MemoryStatusOnExeLaunch );
	GETTER( const mxChar*,			GetExeDirectory,			m_exeLaunchDirectory );
	GETTER( HINSTANCE,				GetAppHInstance,			m_hInstance );

public:
	void Init()
	{
		if( bInitialized ) {
			Assert(false);
			return;
		}

		// Get machine endianness.
		m_machineEndianness = CheckMachineEndianness();

		//
		//	Initialize platform info.
		//
		m_sysInfo.Platform = EPlatform::Platform_Win32;
		m_sysInfo.Cpu = ECpuType::CPU_x86_32;
		{
			SYSTEM_INFO	sysInfo;
			::GetSystemInfo( & sysInfo );

			m_sysInfo.NumProcessors		= sysInfo.dwNumberOfProcessors;
			m_sysInfo.ProcessorPageSize	= sysInfo.dwPageSize;
		}

		GetProcessorClockFrequency( m_iCpuFrequencyHz );
		m_fCpuFrequencyHz = (mxReal) m_iCpuFrequencyHz;

		//
		// Detect the version of Windows we're running under.
		//

		// NOTE: the following code was swiped from Irrlicht.
		{
			OSVERSIONINFOEX osvi;
			BOOL bOsVersionInfoEx;

			ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

			bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);
			if (!bOsVersionInfoEx)
			{
				osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
				if (! GetVersionEx((OSVERSIONINFO *) &osvi))
					goto L_End;
			}

			switch (osvi.dwPlatformId)
			{
			case VER_PLATFORM_WIN32_NT:
				if (osvi.dwMajorVersion <= 4)
					strcat_s( m_versionString, sizeof(m_versionString), "Microsoft Windows NT " );
				else
					if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
						strcat_s( m_versionString, sizeof(m_versionString), "Microsoft Windows 2000 ");
					else
						if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
							strcat_s( m_versionString, sizeof(m_versionString), "Microsoft Windows XP ");
						else
							if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
								strcat_s( m_versionString, sizeof(m_versionString), "Microsoft Windows Vista ");
							else
								if (osvi.dwMajorVersion == 7 && osvi.dwMinorVersion == 0)
									strcat_s( m_versionString, sizeof(m_versionString), "Microsoft Windows 7 ");

				if (bOsVersionInfoEx)
				{
#ifdef VER_SUITE_ENTERPRISE
					if (osvi.wProductType == VER_NT_WORKSTATION)
					{
#ifndef __BORLANDC__
						if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
							strcat_s( m_versionString, sizeof(m_versionString), "Personal ");
						else
							strcat_s( m_versionString, sizeof(m_versionString), "Professional ");
#endif
					}
					else if (osvi.wProductType == VER_NT_SERVER)
					{
						if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
							strcat_s( m_versionString, sizeof(m_versionString), "DataCenter Server ");
						else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
							strcat_s( m_versionString, sizeof(m_versionString), "Advanced Server ");
						else
							strcat_s( m_versionString, sizeof(m_versionString), "Server ");
					}
#endif
				}
				else
				{
					HKEY hKey;
					mxChar szProductType[80];
					DWORD dwBufLen;

					RegOpenKeyEx( HKEY_LOCAL_MACHINE,
						TEXT("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
						0, KEY_QUERY_VALUE, &hKey );
					RegQueryValueEx( hKey, TEXT("ProductType"), NULL, NULL,
						(LPBYTE) szProductType, &dwBufLen);
					RegCloseKey( hKey );

					if (lstrcmpi( TEXT("WINNT"), szProductType) == 0 )
						strcat_s( m_versionString, sizeof(m_versionString), TEXT("Professional "));
					if (lstrcmpi( TEXT("LANMANNT"), szProductType) == 0)
						strcat_s( m_versionString, sizeof(m_versionString), TEXT("Server "));
					if (lstrcmpi( TEXT("SERVERNT"), szProductType) == 0)
						strcat_s( m_versionString, sizeof(m_versionString), TEXT("Advanced Server "));
				}

				// Display version, service pack (if any), and build number.

				mxChar tmp[256];

				if (osvi.dwMajorVersion <= 4 )
				{
					sprintf(tmp, "version %ld.%ld %s (Build %ld)",
						osvi.dwMajorVersion,
						osvi.dwMinorVersion,
						osvi.szCSDVersion,
						osvi.dwBuildNumber & 0xFFFF);
				}
				else
				{
					sprintf(tmp, "%s (Build %ld)", osvi.szCSDVersion,
						osvi.dwBuildNumber & 0xFFFF);
				}

				strcat_s( m_versionString, sizeof(m_versionString), tmp);
				break;

			case VER_PLATFORM_WIN32_WINDOWS:
				if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
				{
					strcat_s( m_versionString, sizeof(m_versionString), "Microsoft Windows 95 ");
					if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
						strcat_s( m_versionString, sizeof(m_versionString), "OSR2 " );
				}

				if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
				{
					strcat_s( m_versionString, sizeof(m_versionString), "Microsoft Windows 98 ");
					if ( osvi.szCSDVersion[1] == 'A' )
						strcat_s( m_versionString, sizeof(m_versionString),  "SE " );
				}

				if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
					strcat_s( m_versionString, sizeof(m_versionString), "Microsoft Windows Me ");

				break;

			case VER_PLATFORM_WIN32s:
				strcat_s( m_versionString, sizeof(m_versionString), "Microsoft Win32s ");
				break;

			}//switch
		}
L_End:
		m_sysInfo.OSName = m_versionString;

		m_hInstance = GetModuleHandle( NULL );

		//
		// Get application executable directory.
		//

/* these global vars point to the full path name of the executable file
		_pgmptr
		_get_wpgmptr
*/
		::GetModuleFileName( null/*GetApplicationHInstance()*/, m_exeLaunchDirectory, MAX_PATH );

		//
		// Record current memory status.
		//
		GlobalMemoryStatus( & m_MemoryStatusOnExeLaunch );

		bInitialized = true;
	}
	void Shutdown()
	{
		if( bInitialized )
		{
			// Record current memory status.
			GlobalMemoryStatus( & m_MemoryStatusOnExeShutdown );

			bInitialized = false;
		}
	}

	Win32_SystemLayer()
	{
		ENSURE_ONE_CALL;
	}

	~Win32_SystemLayer()
	{}

private:
	SystemInfo_s	m_sysInfo;
	char			m_versionString[ 256 ];

	EEndianness		m_machineEndianness;
	mxUInt			m_iCpuFrequencyHz;
	mxReal			m_fCpuFrequencyHz;

	HINSTANCE		m_hInstance;	// Handle to the app instance.

	MEMORYSTATUS	m_MemoryStatusOnExeLaunch;	// Initial memory status.
	MEMORYSTATUS	m_MemoryStatusOnExeShutdown;

	TCHAR	m_exeLaunchDirectory[ MAX_PATH ];	// base directory including exe file name

	static bool		bInitialized;
};

//
//	Global variables.
//
static	Win32_SystemLayer	TheWin32Sys;
bool	Win32_SystemLayer::bInitialized = false;

//
//	Platform_Init()
//
void Platform_Init()
{
#ifdef MX_DEBUG

	// Enable run-time memory check for debug builds.
	int flag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG ); // Get current flag

	// Turn on leak-checking bits.
	flag |= _CRTDBG_LEAK_CHECK_DF;
	flag |= _CRTDBG_ALLOC_MEM_DF;

	// Set flag to the new value.
	// That will report any leaks to the output window when upon exiting the application.
	_CrtSetDbgFlag( flag );

#endif

	TheWin32Sys.Init();
}

//
//	Platform_Shutdown()
//
void Platform_Shutdown()
{
	TheWin32Sys.Shutdown();
}

//=====================================================================

namespace sys {

//---------------------------------------------------------------------
//		Win32-specific
//---------------------------------------------------------------------

// [Windows only] Returns the handle to the application's instance.
HINSTANCE GetApplicationHInstance()
{
	return TheWin32Sys.GetAppHInstance();
}

//---------------------------------------------------------------------
//		Debug output and error handling
//---------------------------------------------------------------------

// Maximum number of characters a temporary buffer can hold.
enum { MAX_STRING_CHARS = 4096 };

void Print( const mxChar* format, ... )
{
	va_list	 argptr;
	va_start( argptr, format );

	mxChar	buffer[ MAX_STRING_CHARS ];
	vsprintf( buffer, format, argptr );
	va_end( argptr );

	printf( buffer );
//	putchar('\n');
}

void DbgOut( const mxChar* format, ... )
{
#ifdef MX_DEBUG
	
	va_list	 argptr;
	va_start( argptr, format );

	mxChar	buffer[ MAX_STRING_CHARS ];
	vsprintf( buffer, format, argptr );
	va_end( argptr );

	printf( buffer );
	putchar('\n');

#endif // MX_DEBUG
}

//--------------------------------------------------------------------------------

void ShowMessage( const mxChar* caption, const mxChar* format, ... )
{
	va_list	 argptr;
	va_start( argptr, format );

	mxChar	buffer[ MAX_STRING_CHARS ];
	vsprintf( buffer, format, argptr );
	va_end( argptr );

	printf( buffer );
	putchar('\n');

#ifdef UNICODE
	mbstowcs( wbuffer, buffer, BUFFER_SIZE );
	::MessageBox( null, wbuffer, TEXT(caption), MB_OK );
#else
	::MessageBox( null, buffer, caption, MB_OK );
#endif
}

//--------------------------------------------------------------------------------

void Warning( const mxChar* format, ... )
{
	va_list	 argptr;
	va_start( argptr, format );
	
	mxChar	buffer[ MAX_STRING_CHARS ];
	vsprintf( buffer, format, argptr );
	va_end( argptr );

	printf( buffer );
	putchar('\n');

	DEBUG_BREAK
/*
#ifdef UNICODE
	mbstowcs( wbuffer, buffer, BUFFER_SIZE );
	::MessageBox( GetWindowHandle(), wbuffer, TEXT("Warning!"), MB_OK );
#else
	::MessageBox( GetWindowHandle(), buffer, TEXT("Warning!"), MB_OK );
#endif
*/
}

//--------------------------------------------------------------------------------

void Error( const mxChar* format, ... )
{
	va_list	 argptr;
	va_start( argptr, format );

	mxChar	buffer[ MAX_STRING_CHARS ];
	vsprintf( buffer, format, argptr );
	va_end( argptr );

	fprintf( stderr, buffer, argptr );
	fputc( '\n', stderr );

	DEBUG_CODE( printf( buffer ) );
	DEBUG_BREAK;

#ifdef UNICODE
	mbstowcs( wbuffer, buffer, BUFFER_SIZE );
	::MessageBox( null, wbuffer, TEXT("Error!"), MB_OK );
#else
	::MessageBox( null, buffer, TEXT("Error!"), MB_OK );
#endif

	if ( g_bReleaseMode ) {
		exit( -1 );
	}
}

//--------------------------------------------------------------------------------

void FatalError( const mxChar* format, ... )
{
	va_list	 argptr;
	va_start( argptr, format );

	mxChar  buffer[ MAX_STRING_CHARS ];
	vsprintf( buffer, format, argptr );
	va_end( argptr );

	fprintf( stderr, buffer, argptr );
	fputc( '\n', stderr );

	DEBUG_CODE( printf( buffer ) );
	DEBUG_BREAK;

#ifdef UNICODE
	wchar_t  wchar_buffer[ MAX_STRING_CHARS ];
	mbstowcs( wchar_buffer, buffer, MAX_STRING_CHARS );
	::MessageBox( null, wchar_buffer, TEXT("Fatal error, application will exit!"), MB_OK|MB_TOPMOST );
#else
	::MessageBox( null, buffer, TEXT("Fatal error, application will exit!"), MB_OK|MB_TOPMOST );
#endif

	if ( g_bReleaseMode ) {
		exit( -1 );
	}
}

void Exit()
{
	::PostQuitMessage( 0 );
}

//---------------------------------------------------------------------
//		General system info
//---------------------------------------------------------------------

const mxChar* GetPlatformName( EPlatform platformType )
{
	switch ( platformType )
	{
	default:
	case EPlatform::Platform_Unknown :
		return "Unknown platform";

	case EPlatform::Platform_Win32 :
		return "Win32 platform";

	case EPlatform::Platform_Win64 :
		return "Win64 platform";
	}
}

const mxChar* GetCpuTypeName( ECpuType CpuType )
{
	switch ( CpuType )
	{
	default:
	case ECpuType::CPU_Unknown :
		return "Unknown CPU type";

	case ECpuType::CPU_x86_32 :
		return "x86-32";

	case ECpuType::CPU_x86_64 :
		return "x86-64";
	}
}

EEndianness	GetEndianness()
{
	return TheWin32Sys.GetMachineEndianness();
}

void GetSystemInfo( SystemInfo_s &OutInfo )
{
	OutInfo = TheWin32Sys.GetSystemInfo();
}

//---------------------------------------------------------------------
//		System time
//---------------------------------------------------------------------

mxUInt GetMilliseconds()
{
	return ::timeGetTime();
}

mxUInt GetClockTicks()
{
	return ::GetTickCount();
}

mxUInt GetClockTicksPerSecond()
{
	return TheWin32Sys.GetCpuFrequencyHz();
}

mxReal GetClockTicks_F()
{
	return (mxReal) GetClockTicks();
}

mxReal GetClockTicksPerSecond_F()
{
	return TheWin32Sys.GetCpuFrequencyHz();
}

void GetTime( mxUInt & year, mxUInt & month, mxUInt & dayOfWeek,
				mxUInt & day, mxUInt & hour, mxUInt & minute, mxUInt & second, mxUInt & milliseconds )
{
	SYSTEMTIME	sysTime;

	::GetLocalTime( & sysTime );

	year		= sysTime.wYear;
	month		= sysTime.wMonth;
	dayOfWeek	= sysTime.wDayOfWeek;
	day			= sysTime.wDay;
	hour		= sysTime.wHour;
	minute		= sysTime.wMinute;
	second		= sysTime.wSecond;
	milliseconds = sysTime.wMilliseconds;
}

//---------------------------------------------------------------------
//		CPU information
//---------------------------------------------------------------------

void GetProcessorClockFrequency( mxUInt &MHz )
{
	HKEY key;

	if ( FAILED( ::RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
		0, KEY_READ, &key ) ) )
	{
		sys::Error( "Failed to get processor info" );
		return;
	}

	DWORD dwClockFreq = 0;
	DWORD dwSizeOf = sizeof( dwClockFreq );

	if ( FAILED( ::RegQueryValueEx( key, "~MHz", NULL, NULL,
		(LPBYTE) &dwClockFreq, (LPDWORD) &dwSizeOf ) ) )
	{
		sys::Error( "Failed to get processor info" );
		return;
	}

	::RegCloseKey( key );

	MHz = (mxUInt) dwClockFreq;
}

//---------------------------------------------------------------------
//		FPU
//---------------------------------------------------------------------

FP_Class GetFPClass( const double x )
{
	int fpClass = ::_fpclass( x );

	switch ( fpClass )
	{
	case _FPCLASS_SNAN : return FP_Class::FP_SNAN;
	case _FPCLASS_QNAN : return FP_Class::FP_QNAN;
	case _FPCLASS_NINF : return FP_Class::FP_NINF;
	case _FPCLASS_NN   : return FP_Class::FP_NN;
	case _FPCLASS_ND   : return FP_Class::FP_ND;
	case _FPCLASS_NZ   : return FP_Class::FP_NZ;
	case _FPCLASS_PZ   : return FP_Class::FP_PZ;
	case _FPCLASS_PD   : return FP_Class::FP_PD;
	case _FPCLASS_PN   : return FP_Class::FP_PN;
	case _FPCLASS_PINF : return FP_Class::FP_PINF;
	default:	Unreachable; return FP_Class::FP_SNAN;
	}
}

FASTBOOL IsNaN( const double x ) {
	return ::_isnan( x );
}

FASTBOOL IsFinite( const double x ) {
	return ::_finite( x );
}

//---------------------------------------------------------------------
//		Memory stats
//---------------------------------------------------------------------

mxUInt GetSystemRam()
{
	MEMORYSTATUS  memStatus;
	memStatus.dwLength = sizeof( memStatus );

	::GlobalMemoryStatus( &memStatus );

	return (mxUInt) memStatus.dwTotalPhys;
}

void FromWinMemoryStatus( const MEMORYSTATUS& memStatus, MemoryStats_s & OutStats )
{
	OutStats.memoryLoad				= memStatus.dwTotalPhys;
	OutStats.totalPhysical			= memStatus.dwAvailPhys;
	OutStats.availPhysical			= memStatus.dwMemoryLoad;
	OutStats.totalPageFile			= memStatus.dwTotalPageFile;
	OutStats.availPageFile			= memStatus.dwAvailPageFile;
	OutStats.totalVirtual			= memStatus.dwTotalVirtual;
	OutStats.availVirtual			= memStatus.dwAvailVirtual;
}

void GetCurrentMemoryStatus( MemoryStats_s & OutStats )
{
	MEMORYSTATUS  memStatus;
	memStatus.dwLength = sizeof( memStatus );

	::GlobalMemoryStatus( &memStatus );

	FromWinMemoryStatus( memStatus, OutStats );
}

void GetExeLaunchMemoryStatus( MemoryStats_s & OutStats )
{
	FromWinMemoryStatus( TheWin32Sys.GetExeLaunchMemoryStatus(), OutStats );
}

void GetMemoryInfo( mxUInt &TotalBytes, mxUInt &AvailableBytes )
{
	MEMORYSTATUS  memStatus;
	memStatus.dwLength = sizeof( memStatus );

	::GlobalMemoryStatus( &memStatus );

	TotalBytes = memStatus.dwTotalPhys;
	AvailableBytes = memStatus.dwAvailPhys;
}

//---------------------------------------------------------------------
//		File system
//---------------------------------------------------------------------

const mxChar* GetLauchDirectory()
{
	return TheWin32Sys.GetExeDirectory();
}

bool FileExists( const mxChar* filename )
{
	// simply check for file existence
	//return ( ::_access( filename, 0 ) != -1 );
	return ( ::_access( filename, 0 ) == 0 );
}

bool PathExists( const mxChar* pathname )
{
	// check for existence
	if ( ::_access( pathname, 0 ) != -1 )
	{
		// path exists

		// check for write permission
		if ( ::_access( pathname, 2 ) == -1 )
		{
			// file/path is read-only
			sys::Warning( TEXT("%s is read only"), pathname );
			return true;
		}

		return true;
	}
	return false;
}

bool FileOrPathExists( const mxChar* path )
{
	// GetFileAttributes() works with both files and directories.
	return ( GetFileAttributes( path ) != INVALID_FILE_ATTRIBUTES );
}

bool IsValidFileName( const mxChar* filename )
{
	// TODO: more checks.
	return (filename != NULL)
		&& (strlen(filename) < MAX_PATH);
}

bool IsValidPathName( const mxChar* pathname )
{
	if ( ! pathname ) {
		return false;
	}

	Assert( strlen( pathname ) < MAX_PATH_CHARS );

	//static mxChar szPathName[ sys::MAX_PATH_CHARS ];
	//strcpy_s( szPathName, sizeof(szPathName), pathname );

	//if ( szPathName[ length - 1 ] != '/' ) {
	//	return false;
	//}

	return true;
}

const mxChar* GetFileExtension( const mxChar* filename )
{
	//return ::PathFindExtension( filename );

	static mxChar  baseFileName[ MAX_FILENAME_CHARS ];
	static mxChar  fileExtension[ MAX_FILENAME_CHARS ];

	::_splitpath_s( filename,
		NULL,	// drive (e.g. C:)
		0,		// drive buffer size
		NULL,	// dir
		0,
		baseFileName,
		MAX_FILENAME_CHARS,
		fileExtension,
		MAX_FILENAME_CHARS
	);

	return fileExtension;
}

bool IsValidFileExtension( const mxChar* extension )
{
	MX_TODO("some checks")
	return true;
}

//---------------------------------------------------------------------
//		Strings
//---------------------------------------------------------------------

SizeT StrLen( const mxChar* pSrc )
{
	return ::strlen( pSrc );
}

int StrCpy( mxChar* pDest, mxSizeT destSize, const mxChar* pSrc )
{
	Assert( pDest != pSrc );
	return ::strcpy_s( pDest, destSize, pSrc );
}

int StrCmp( const mxChar* s1, const mxChar* s2 )
{
	return ::strcmp( s1, s2 );
}

bool StrEqual( const mxChar* s1, const mxChar* s2 )
{
	return ( ::strcmp( s1, s2 ) == 0 );
}

//---------------------------------------------------------------------
//		Miscellaneous
//---------------------------------------------------------------------

void Sleep( mxUInt milliseconds )
{
	::Sleep( milliseconds );
}

void Beep( mxUInt delayMs, mxUInt frequencyInHertz )
{
	::Beep( frequencyInHertz, delayMs );
}

void SetMouseCursorVisible( bool bVisible )
{
#if 0
	static HCURSOR hCursor = NULL;	// application cursor
	if ( !hCursor ) {
		hCursor = ::GetCursor();
	}

	if ( bVisible ) {
		::SetCursor( hCursor );
	} else {
		::SetCursor( NULL );
	}
#else
	::ShowCursor( bVisible );
#endif
}

void SetMouseCursorPosition( mxUInt x, mxUInt y )
{
	POINT point;
	point.x = x;
	point.y = y;

	::SetCursorPos( point.x, point.y );
}
/*
void GetWindowPosition( mxUInt &Left, mxUInt &Top, mxUInt &Right, mxUInt &Bottom )
{
	RECT  rect;
	::GetWindowRect( GetWindowHandle(), &rect );

	Left	= rect.left;
	Top		= rect.top;
	Right	= rect.right;
	Bottom	= rect.bottom;
}
*/
void GetCurrentDeckstopResolution( mxUInt &ScreenWidth, mxUInt &ScreenHeight )
{
	ScreenWidth  = (mxUInt) GetSystemMetrics( SM_CXSCREEN );
	ScreenHeight = (mxUInt) GetSystemMetrics( SM_CYSCREEN );
}

}//End of namespace sys
}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
