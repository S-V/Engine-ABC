/*
=============================================================================
	File:	SpatialQuery.h
	Desc:	Classes for performing spatial queries.
=============================================================================
*/

#ifndef __MX_SPATIAL_QUERY_H__
#define __MX_SPATIAL_QUERY_H__

namespace abc {

//
//	EHitMask - used for collision filtering during hit testing.
//
enum EHitMask
{
	HM_Solid	= BIT(0),

	// NOTE: only 16 bits are available.
};

class mxSceneView;
class mxEntity;

class mxTraceInput;
class mxTraceResult;

//
//	mxTraceInput
//
class mxTraceInput {
public:
	mxTraceInput( const Vec3D& startPosition, const Vec3D& rayDirection )
		: m_start( startPosition )
		, m_rayDir( rayDirection )
	{}

	const Vec3D & GetStart() const
	{
		return m_start;
	}
	const Vec3D & GetDirection() const
	{
		return m_rayDir;
	}

protected:
	Vec3D	m_start;
	Vec3D	m_rayDir;
};

//
//	mxLocalRayResult
//
class mxLocalRayResult {
public:
	mxEntity *	m_entityHit;	// the entity that was hit (null if didn't hit anything)
	Vec3D		m_position;		// hit point position in coordinate system of the hit entity
	Vec3D		m_normal;		// collision plane normal in coordinate system of the hit entity
	mxReal		m_hitFraction;	// fraction of movement completed ([0..1])
};

//
//	mxTraceResult
//
class mxTraceResult {
public:
	virtual ~mxTraceResult() {}

	// returns false if we should ignore the given entity
	virtual bool NeedsChecking( const mxEntity& entity ) { return true; }

	virtual void AddSingleResult( mxLocalRayResult& newResult ) = 0;

	// returns true if the trace did hit anything
	virtual bool HasHit() const = 0;
};

//
//	mxTraceResult_ClosestHit
//
class mxTraceResult_ClosestHit : public mxTraceResult {
public:
	mxTraceResult_ClosestHit();

	mxLocalRayResult &	GetResult();

	//
	//	Override ( mxTraceResult ) :
	//
	virtual bool HasHit() const;
	virtual void AddSingleResult( mxLocalRayResult& newResult );

private:
	mxLocalRayResult	m_result;
};

} //end of namespace abc

#endif // ! __MX_SPATIAL_QUERY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
