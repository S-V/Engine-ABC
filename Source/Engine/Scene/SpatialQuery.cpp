/*
=============================================================================
	File:	SpatialQuery.cpp
	Desc:	Classes for performing spatial queries.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>

namespace abc {

/*================================
	mxTraceResult_ClosestHit
================================*/

mxTraceResult_ClosestHit::mxTraceResult_ClosestHit()
{
}

mxLocalRayResult & mxTraceResult_ClosestHit::GetResult()
{
	return m_result;
}

bool mxTraceResult_ClosestHit::HasHit() const
{
	return false;
}

void mxTraceResult_ClosestHit::AddSingleResult( mxLocalRayResult& newResult )
{
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
