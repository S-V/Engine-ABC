/*
=============================================================================
	File:	Timer.cpp
	Desc:	A very simple high resolution Win32 timer class.
=============================================================================
*/
#include <Include/PCH/precompiled.h>
#pragma hdrstop
//#include <Include/ABC.h>
#include "Timer.h"

namespace abc {

mxTimer::mxTimer()
{
	Assert( ::QueryPerformanceFrequency( & m_frequency ) == TRUE );
	m_frequencyInverse = 1.0f / (FLOAT)( m_frequency.QuadPart );

	m_timeStart.QuadPart = 0;
	m_timeStop.QuadPart  = 0;

	m_cachedElapsedTime = 0;
	m_cachedElapsedTimeMS = 0;
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
