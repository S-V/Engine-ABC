/*
=============================================================================
	File:	Win32_Threading.h
	Desc:	Win32 threading utils.
	Note:	This module is far from finished.
=============================================================================
*/

// Thread local storage.
#define MX_THREAD_LOCAL		__declspec( thread )

//===========================================================================

namespace abc {
namespace sys {

	enum EThreadPriority
	{
		Thread_Low,
		Thread_Normal,
		Thread_High
	};

	typedef ::DWORD	ThreadID;	// Thread identifier.

	//
	//	Main thread function.
	//
	typedef unsigned int (*ThreadFunc_t)( void * );

	//
	//	Thread - is a thread of execution.
	//
	class Thread {
	public:
		virtual ~Thread() {}

		// Sets the priority of a thread.

		virtual void	SetThreadPriority( EThreadPriority newPriority ) = 0;

		// Indicates whether a thread has completed execution.

		virtual bool	IsComplete() const = 0;

		// Suspends the current thread for a period of time. Accepts the length of time to sleep, in milliseconds.

		virtual void	Suspend( unsigned long Ms ) = 0;	// Sleep is already defined.

		// Yields the time slice for the current thread.
		// Notes: The Yield function causes the calling thread to give up the remainder of its time slice.
		// The thread will continue running normally once the operating system schedules it for execution again.

		virtual void	YieldTime( void ) = 0;	// Yield is already defined.
	};

}//End of namespace sys
}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
