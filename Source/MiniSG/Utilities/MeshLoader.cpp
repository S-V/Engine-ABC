/*
=============================================================================
	File:	MeshLoader.cpp
	Desc:	Mesh loader.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <MiniSG.h>

namespace abc {

mxMesh* LoadMeshFromFile( const mxChar* filename, const MeshDescription& desc )
{
	mxFilePath  filepath( filename );
	if( !filepath.Exists() ) {
		sys::Warning(TEXT("File %s not found"),filename);
		return null;
	}

	struct MyAlloc : public lol::IAlloc
	{
		void * Allocate( size_t numBytes )
		{
			return ::abc::Allocate( numBytes, EMemoryClass::MX_MEMORY_CLASS_GEOMETRY );
		}
		void Free( void* pMemory )
		{
			 ::abc::Free( pMemory, EMemoryClass::MX_MEMORY_CLASS_GEOMETRY );
		}
	} myAlloc;


	lol::Options options;
	options.uvScale		= desc.texCoordScale;

	if( desc.initialTransform != Matrix4::mat4_identity ) {
		options.transform	= (const float*) desc.initialTransform.ToFloatPtr();
	}

	options.pAlloc		= &myAlloc;
	options.flags		= lol::Options::ALL;

	lol::Mesh  temp;
	if( lol::LoadMeshFromFile( filepath.GetName(), &temp, options ) )
	{
		mxMesh * newMesh = MX_NEW mxMesh();
		newMesh->numVertices	= temp.numVertices;
		newMesh->vertices		= (rxVertex*) temp.vertices;
		newMesh->numIndices		= temp.numIndices;
		newMesh->indices		= temp.indices;
		newMesh->bounds			= *(mxBounds*)&temp.bounds;
		return newMesh;
	}
	else
	{
		return null;
	}
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
