/*
=============================================================================
	File:	Lib.h
	Desc:	Foundation Library includes.
=============================================================================
*/

#ifndef __MX_LIBRARY_H__
#define __MX_LIBRARY_H__

#include <Lib/String/String.h>		// String type.

//------ Containers ------------------------------------------------

#include <Lib/Templates/Templates.h>

#include <Lib/Hashing/HashFunctions.h>

// Hash tables and maps.
#include <Lib/Templates/Containers/HashMap/StringHash.h>
//#include <Lib/Containers/HashMap/BTree.h>
#include <Lib/Templates/Containers/HashMap/RBTreeMap.h>
//#include <Lib/Containers/HashMap/TMap.h>
//#include <Lib/Containers/HashMap/TKeyValue.h>
//#include <Lib/Containers/HashMap/Dictionary.h>

// Smart pointers.
#include <Lib/Templates/SmartPtr/TPtr.h>
#include <Lib/Templates/SmartPtr/AutoPtr.h>
#include <Lib/Templates/SmartPtr/TSharedPtr.h>
#include <Lib/Templates/SmartPtr/TSmartPtr.h>
#include <Lib/Templates/SmartPtr/ReferenceCounted.h>

// Arrays.
#include <Lib/Templates/Containers/Array/TArray.h>
#include <Lib/Templates/Containers/Array/TStaticList.h>
#include <Lib/Templates/Containers/Array/TFixedArray.h>
#include <Lib/Templates/Containers/Array/TStatic2DArray.h>

// Lists.
#include <Lib/Templates/Containers/List/TLinkedList.h>
#include <Lib/Templates/Containers/List/TCircularList.h>

// Singleton.
#include <Lib/Templates/Singleton.h>

//------ Math ------------------------------------------------------

#include <Lib/Math/Math.h>
#include <Lib/Math/Random.h>
#include <Lib/Math/Complex.h>

// Vector
#include <Lib/Math/Vector/Vector_Common.h>
#include <Lib/Math/Vector/Vec2D.h>
#include <Lib/Math/Vector/Vec3D.h>
#include <Lib/Math/Vector/Vec4D.h>

// Matrix
#include <Lib/Math/Matrix/Matrix_Common.h>
#include <Lib/Math/Matrix/Matrix2x2.h>
#include <Lib/Math/Matrix/Matrix3x3.h>
#include <Lib/Math/Matrix/Matrix4x4.h>


#include <Lib/Math/Angles.h>
#include <Lib/Math/Quat.h>
#include <Lib/Math/Rotation.h>

#include <Lib/Math/Line.h>
#include <Lib/Math/Plane.h>

#include <Lib/Math/Pluecker.h>

// Bounding volumes.
#include <Lib/Geometry/BoundingVolumes/BoundingVolumes.h>
#include <Lib/Geometry/BoundingVolumes/Sphere.h>
#include <Lib/Geometry/BoundingVolumes/AABB.h>
#include <Lib/Geometry/BoundingVolumes/OOBB.h>
#include <Lib/Geometry/BoundingVolumes/ViewFrustum.h>
#include <Lib/Geometry/BoundingVolumes/Bounds.h>

// Math utils
#include <Lib/Math/Utils.h>

// Utilities.
#include <Lib/Utilities/Utilities.h>

#endif // ! __MX_LIBRARY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
