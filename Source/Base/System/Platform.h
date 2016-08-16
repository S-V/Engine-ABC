/*
=============================================================================
	File:	Platform.h
	Desc:	System-level unportable services,
			preprocessor definitions for platform, compiler, etc.
=============================================================================
*/

#ifndef __MX_PLATFORM_H__
#define __MX_PLATFORM_H__

/*
=======================================================================

	Platform-specific services.

=======================================================================
*/
namespace abc {

	// Functions used to initialize/shutdown platform layer.

	void	Platform_Init();
	void	Platform_Shutdown();

}//End of namespace abc

//
// Identify the current platform.
//

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN32_WCE)
//#	pragma message("Compiling on Win32 platform...")
#	define MX_PLATFORM_WIN_32	1

#elif defined(_WIN64) || defined(WIN64)
#	define MX_PLATFORM_WIN_64	1
#	error Unsupported platform !

#else
#	error Unknown platform !
#endif

//
// Find the current compiler.
//

#if defined(_MSC_VER)
#	define MX_COMPILER_MSVC		1
//#	pragma message("Compiling with MVC++...")
#elif defined(__INTEL_COMPILER)
#	define MX_COMPILER_INTEL	1
#else
#	error Unknown compiler !
#endif

#ifndef __cplusplus
#	error A C++ compiler is required!
#endif

//
// Find CPU architecture type.
//

#if defined(_MSC_VER) && defined(_M_IX86)
#	define MX_ARCHITECTURE_TYPE_IA32	1
#	define MX_CPU_X86					1
#	define MX_ENDIAN_LITTLE				1
#	define MX_ENDIAN_BIG				0
#	define MX_POINTER_SIZE				4

#elif defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) ||  defined(__s390__) || defined(__s390x__)
#	error "64-bit architecture is not supported!"

#else
#	error "Unknown processor architecture!"
#endif

//-------------------------------------------------------------------------------
// Include platform-specific headers.
//-------------------------------------------------------------------------------

#if MX_PLATFORM_WIN_32
	#include <System/Win32/Win32_MVCpp.h>
#else
#	error Unsupported platform !
#endif

/*
=======================================================================
	
	Non-portable system services.

=======================================================================
*/

namespace abc {
namespace sys {

//---------------------------------------------------------------------
//		Debug output and error handling
//---------------------------------------------------------------------

		// Prints a message to the text console.
void	Print( const mxChar* format, ... ) ATTRIBUTE( (format(printf,1,2)) );
 
		// The same as Print but works in debug mode only.
void	DbgOut( const mxChar* format, ... ) ATTRIBUTE( (format(printf,1,2)) );

		// Shows a message box.
void	ShowMessage( const mxChar* caption, const mxChar* format, ... ) ATTRIBUTE( (format(printf,2,3)) );

		// Shows a message box with a warning message.
void	Warning( const mxChar* format, ... ) ATTRIBUTE( (format(printf,1,2)) );

		// Shows a error message box; should be used for media or logic errors.
void	Error( const mxChar* format, ... ) ATTRIBUTE( (format(printf,1,2)) );

		// Fatal error - terminate execution immediately;
		// does not return;
		// may write something into the error log.
		// Note: causes a breakpoint exception to occur in debug builds.
void	FatalError( const mxChar* format, ... ) ATTRIBUTE( (format(printf,1,2)) );

		// Exit from the main loop.
void	Exit();

//---------------------------------------------------------------------
//		General system info
//---------------------------------------------------------------------

enum EPlatform
{
	Platform_Unknown = 0,
	Platform_Win32,
	Platform_Win64,
};

const mxChar* GetPlatformName( EPlatform platformType );

enum ECpuType
{
	CPU_Unknown = 0,
	CPU_x86_32,
	CPU_x86_64,
};

const mxChar* GetCpuTypeName( ECpuType CpuType );

//
//	EEndianness
//
enum EEndianness
{
	BigEndian = 0,
	LittleEndian,	// Lowest address contains the least significant byte.
};

EEndianness	GetEndianness();

inline EEndianness CheckMachineEndianness()
{
	INT32 a = 1;
	if ( *(UINT8*)&a == 1 ) {
		return EEndianness::LittleEndian;
	} else {
		return EEndianness::BigEndian;
	}
}

//
//	SystemInfo_s
//
struct SystemInfo_s
{
	EPlatform		Platform;
	ECpuType		Cpu;
	const char *	OSName;

	mxUInt	NumProcessors;
	mxUInt	ProcessorPageSize;		// CPU page size, in bytes

	//...
};

void GetSystemInfo( SystemInfo_s &OutInfo );

//---------------------------------------------------------------------
//		System time
//---------------------------------------------------------------------

// Returns the number of milliseconds elapsed since the last reset.
// NOTE: Should only be used for profiling, not for game timing!
mxUInt	GetMilliseconds();

// These functions can be used for accurate performance testing,
// but not for in-game timing.
//
mxUInt	GetClockTicks();	// Retrieves the number of milliseconds that have elapsed since the system was started, up to 49.7 days.
mxUInt	GetClockTicksPerSecond();

mxReal	GetClockTicks_F();
mxReal	GetClockTicksPerSecond_F();

// Returns the exact system time.
//
void GetTime( mxUInt & year, mxUInt & month, mxUInt & dayOfWeek,
			 mxUInt & day, mxUInt & hour, mxUInt & minute,
			 mxUInt & second, mxUInt & milliseconds );

//---------------------------------------------------------------------
//		CPU information
//---------------------------------------------------------------------

//
//	CPU_Type_e
//
enum CPU_Type_e
{
	CPU_Generic,	// Any unrecognized processor.
	CPU_AMD,
	CPU_Intel,
};

//
//	CPUID_e
//
enum CPUID_e
{
	CPUID_NONE			= 0x00000,
	CPUID_UNSUPPORTED	= 0x00001,	// unsupported (386/486)
	CPUID_GENERIC		= 0x00002,	// unrecognized processor
	CPUID_INTEL			= 0x00004,	// Intel
	CPUID_AMD			= 0x00008,	// AMD
	CPUID_MMX			= 0x00010,	// Multi Media Extensions
	CPUID_3DNOW			= 0x00020,	// 3DNow!
	CPUID_SSE			= 0x00040,	// Streaming SIMD Extensions
	CPUID_SSE2			= 0x00080,	// Streaming SIMD Extensions 2
	CPUID_SSE3			= 0x00100,	// Streaming SIMD Extentions 3 aka Prescott's New Instructions
	CPUID_ALTIVEC		= 0x00200,	// AltiVec
	CPUID_HTT			= 0x01000,	// Hyper-Threading Technology
	CPUID_CMOV			= 0x02000,	// Conditional Move (CMOV) and fast floating point comparison (FCOMI) instructions
	CPUID_FTZ			= 0x04000,	// Flush-To-Zero mode (denormal results are flushed to zero)
	CPUID_DAZ			= 0x08000,	// Denormals-Are-Zero mode (denormal source operands are set to zero)
	CPUID_EM64T			= 0x10000,	// 64-bit Memory Extensions (64-bit registers, address spaces, RIP-relative addressing mode)
};

// Gets the CPU clock frequency, in megaherts.
//
void GetProcessorClockFrequency( mxUInt &MHz );

//---------------------------------------------------------------------
//		FPU
//---------------------------------------------------------------------

enum FPU_Exceptions_e
{
	FPU_EXCEPTION_INVALID_OPERATION		= 1,
	FPU_EXCEPTION_DENORMALIZED_OPERAND	= 2,
	FPU_EXCEPTION_DIVIDE_BY_ZERO		= 4,
	FPU_EXCEPTION_NUMERIC_OVERFLOW		= 8,
	FPU_EXCEPTION_NUMERIC_UNDERFLOW		= 16,
	FPU_EXCEPTION_INEXACT_RESULT		= 32
};

enum FPU_Precision_e
{
	FPU_PRECISION_SINGLE			= 0,
	FPU_PRECISION_DOUBLE			= 1,
	FPU_PRECISION_DOUBLE_EXTENDED	= 2
};

enum FPU_Rounding_e
{
	FPU_ROUNDING_TO_NEAREST		= 0,
	FPU_ROUNDING_DOWN			= 1,
	FPU_ROUNDING_UP				= 2,
	FPU_ROUNDING_TO_ZERO		= 3
};

//
// FP_Class - Floating-point class of a number.
//
enum FP_Class
{
	FP_SNAN,		// Signaling NaN.
	FP_QNAN,		// Quiet NaN.
	FP_NINF,		// Negative infinity ( –INF ).
	FP_NN,			// Negative normalized non-zero.
	FP_ND,			// Negative denormalized.
	FP_NZ,			// Negative zero ( –0 ).
	FP_PZ,			// Positive 0 ( +0 ).
	FP_PD,			// Positive denormalized.
	FP_PN,			// Positive normalized non-zero.
	FP_PINF,		// Positive infinity ( +INF ).
};

FP_Class	GetFPClass( const double x );
FASTBOOL	IsNaN( const double x );
FASTBOOL	IsFinite( const double x );

//---------------------------------------------------------------------
//		Memory statistics
//---------------------------------------------------------------------

// Returns the total amount of system (physical) RAM, in megabytes.
mxUInt GetSystemRam();

struct MemoryStats_s
{
	// Number between 1 and 100 that specifies
	// the approximate percentage of physical memory  that is in use.
	mxUInt 	memoryLoad;

	mxUInt 	totalPhysical;	// Total size of physical memory, in bytes.
	mxUInt 	availPhysical;	// Size of physical memory available, in bytes.
	mxUInt 	totalPageFile;	// Size of commited memory limit, in bytes.
	mxUInt 	availPageFile;

	// Total size of the user mode portion
	// of the virtual address space of the calling process, in bytes.
	mxUInt 	totalVirtual;
	
	// Size of unreserved and uncommitted memory in the user mode portion
	// of the virtual address space of the calling process, in bytes.
	mxUInt 	availVirtual;
};

// Returns memory stats.
void GetCurrentMemoryStatus( MemoryStats_s & OutStats );
void GetExeLaunchMemoryStatus( MemoryStats_s & OutStats );

// This function is provided for convenience.
void GetMemoryInfo( mxUInt &TotalBytes, mxUInt &AvailableBytes );

// Returns amount of free drive space in path.
mxUInt GetDriveFreeSpace( const mxChar *path );

//---------------------------------------------------------------------
//		File system
//---------------------------------------------------------------------

// Returns the absolute file path name of the application executable.
const mxChar* GetLauchDirectory();

bool FileExists( const mxChar* filename );

bool PathExists( const mxChar* pathname );

// Returns true if a file or path with the given name exists.
bool FileOrPathExists( const mxChar* path );

bool IsValidFileName( const mxChar* filename );
bool IsValidPathName( const mxChar* pathname );

const mxChar* GetFileExtension( const mxChar* filename );

bool IsValidFileExtension( const mxChar* extension );

//---------------------------------------------------------------------
//		Strings
//---------------------------------------------------------------------

SizeT	StrLen( const mxChar* pSrc );
int		StrCpy( mxChar* pDest, mxSizeT destSize, const mxChar* pSrc );
int		StrCmp( const mxChar* s1, const mxChar* s2 );
bool	StrEqual( const mxChar* s1, const mxChar* s2 );

//---------------------------------------------------------------------
//		Miscellaneous
//---------------------------------------------------------------------

// Allow the application to yield some CPU time.
void	Sleep( mxUInt milliseconds );

//
//	Emit a simple sound through the speaker.
//
//	Param[In]: delayMs - duration of the sound, in milliseconds.
//	Param[In]: frequencyInHertz - frequency of the sound, in hertz
//
void	Beep( mxUInt delayMs, mxUInt frequencyInHertz = 600 );

//
// Mouse cursor control.
//
void	SetMouseCursorVisible( bool bVisible );	// show/hide mouse cursor
void	SetMouseCursorPosition( mxUInt x, mxUInt y ); // coordinates are absolute, not relative!

//
//	Returns the dimensions of the bounding rectangle of the main application window
//	(relative to the upper-left corner of the screen).
//
//	Param[Out]: Left	- the x-coordinate of the upper-left corner of the window
//	Param[Out]: Top		- the y-coordinate of the upper-left corner of the window
//	Param[Out]: Right	- the x-coordinate of the lower-right corner of the window
//	Param[Out]: Bottom	- the y-coordinate of the lower-right corner of the window
//
//void	GetWindowPosition( mxUInt &Left, mxUInt &Top, mxUInt &Right, mxUInt &Bottom );

void	GetCurrentDeckstopResolution( mxUInt &ScreenWidth, mxUInt &ScreenHeight );

}//End of namespace sys
}//End of namespace abc

#endif // !__MX_PLATFORM_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
