/*
=============================================================================
	File:	Timer.h
	Desc:	A very simple high resolution Win32 timer class.
=============================================================================
*/

#ifndef __MX_LIBRARY_WIN32_TIMER_H__
#define __MX_LIBRARY_WIN32_TIMER_H__

namespace abc {

//
//	mxTimer
//
class mxTimer {
public:
			mxTimer();

	void	Start();

			// Measures the time elapsed since the last call of Start().
	void	Stop();

				// Returns the elapsed time in microseconds.
				// Note: don't forget to first call Stop() to get correct values.
	mxFloat32	GetElapsedTimeMs() const;

				// Returns the elapsed time ( as an integer ) in milliseconds.
				// Note: don't forget to first call Stop() to get correct values.
	mxUInt32	GetElapsedTimeMS() const;

private:
	LARGE_INTEGER	m_frequency;
	mxFloat32		m_frequencyInverse;

	LARGE_INTEGER	m_timeStart;
	LARGE_INTEGER	m_timeStop;

	mxFloat32		m_cachedElapsedTime;	// in microseconds
	mxUInt32		m_cachedElapsedTimeMS;	// in milliseconds
};

FORCEINLINE void mxTimer::Start()
{
	::QueryPerformanceCounter( & m_timeStart );
}

FORCEINLINE void mxTimer::Stop()
{
	::QueryPerformanceCounter( & m_timeStop );

	MX_TODO( "fix possible bugs - unexpected leaps forward in the perfomance counter" );

	m_cachedElapsedTime = (m_timeStop.QuadPart - m_timeStart.QuadPart) * m_frequencyInverse;
	m_cachedElapsedTimeMS = mxUInt32((m_timeStop.QuadPart - m_timeStart.QuadPart) / m_frequency.QuadPart)  * 1024;
}

FORCEINLINE mxFloat32 mxTimer::GetElapsedTimeMs() const
{
	return m_cachedElapsedTime;
}

FORCEINLINE mxUInt32 mxTimer::GetElapsedTimeMS() const
{
	return m_cachedElapsedTimeMS;
}

}//End of namespace abc

#endif // ! __MX_LIBRARY_WIN32_TIMER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
