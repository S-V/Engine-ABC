/*
=============================================================================
	File:	MeshLoader.h
	Desc:	Mesh loader.
=============================================================================
*/

#ifndef __MINISG_MESH_LOADER_H__
#define __MINISG_MESH_LOADER_H__

namespace abc {

//
//	MeshDescription
//
struct MeshDescription
{
	Matrix4		initialTransform;
	FLOAT		texCoordScale;

	MeshDescription()
		: initialTransform( Matrix4::mat4_identity )
		, texCoordScale( 1.0f )
	{}
};

//
//	LoadMeshFromFile
//
mxMesh* LoadMeshFromFile( const mxChar* filename, const MeshDescription& desc = MeshDescription() );

}//End of namespace abc

#endif // ! __MINISG_MESH_LOADER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
