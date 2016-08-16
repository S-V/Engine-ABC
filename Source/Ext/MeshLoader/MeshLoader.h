#ifndef MX_RESOURCE_LOADER_H
#define MX_RESOURCE_LOADER_H

#ifdef LOL_BUILD_DLL
	#define LOL_API __declspec(dllexport)
#else
	#define LOL_API __declspec(dllimport)
#endif

#define LOL_CALL __cdecl

//===============================================================

namespace lol
{

extern "C" LOL_API void LOL_CALL SetupResourceLoader();
extern "C" LOL_API void LOL_CALL CloseResourceLoader();

struct Vertex
{
	float	XYZ[3];
	float	Normal[3];
	float	Tangent[3];
	float	UV[2];
};

typedef unsigned int Index; // 32-bit index type

class IAlloc {
public:
	virtual		~IAlloc() {}

	virtual void *	Allocate( size_t numBytes ) = 0;
	virtual void	Free( void* pMemory ) = 0;
};

struct Mesh
{
	unsigned	numVertices;
	Vertex *	vertices;		// vertex data (readable by CPU)
	unsigned	numIndices;
	Index *		indices;		// index data (readable by CPU)

	float		bounds[2][3];	// mesh bounds in local space, AABB, min & max
};

struct Options
{
	float			uvScale;
	const float *	transform;	// [optiona] initial transform, [4]x[4] floats
	IAlloc *		pAlloc;		// [optiona] custom memory allocator

	enum
	{
		PRINT_INFO = (1<<0),	// verbose
		DEBUG_INFO = (1<<1),

		ALL = -1
	};
	unsigned flags;

public:
	Options()
		: uvScale( 0.003f )
		, transform(0)
		, pAlloc( 0 )
		, flags(0)
	{}
};

// returns 1 if success, otherwise, returns 0
//
extern "C" LOL_API int LOL_CALL LoadMeshFromFile( const char* filename, Mesh *OutMesh, const Options& options = Options() );

}//end of namespace lol

#endif //MX_RESOURCE_LOADER_H
