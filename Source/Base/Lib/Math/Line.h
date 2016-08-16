/*
=============================================================================
	File:	Line.h
	Desc:	Lines.
=============================================================================
*/

#ifndef __MATH_LINE_H__
#define __MATH_LINE_H__

namespace abc {

//
//	Line3D - limited 3D line (finite line segment).
//
class Line3D {
public:
	Vec3D	start, end;

public:
	Line3D()
	{}
	Line3D( const Vec3D& a, const Vec3D& b )
		: start( a ), end( b )
	{
		Assert( this->IsOk() );
	}

	Vec3D GetMiddle() const {
		return ( start + end ) * 0.5f;
	}

	bool IsOk() const {
		return ( start != end );
	}
};

//
//	Ray3D - infinite 3D line.
//
class Ray3D {
public:
	Vec3D	origin;
	Vec3D	direction;	// must be normalized

public:
	Ray3D()
	{}
	Ray3D( const Vec3D& p, const Vec3D& d )
		: origin( p ), direction( d )
	{
		Assert( this->IsOk() );
	}

	// Returns the closest point on the line to the given point.
	//
	Vec3D ClosestPoint( const Vec3D& point ) const {
		FLOAT t = direction.Dot( point - origin );
		return origin + direction * t;
	}

	// Returns the distance between the given point and the line.
	//
	FLOAT Distance( const Vec3D& point ) const {
		return ( ClosestPoint( point ) - point ).Length();
	}

	bool IsOk() const {
		return ( direction.IsNormalized() );
	}
};

}//End of namespace abc

#endif /* !__MATH_LINE_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
