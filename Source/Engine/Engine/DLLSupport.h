/*
=============================================================================
	File:	DLLSupport.h
	Desc:	DLL-related stuff.
=============================================================================
*/

#ifndef __MX_DLL_SUPPORT_H__
#define __MX_DLL_SUPPORT_H__

namespace abc {

//
//	mxDLL
//
class mxDLL {
public:
	mxDLL();
	~mxDLL();

	void	LoadLibrary( const mxChar* filename );
	void	Unload();
};

}//End of namespace abc

#endif // ! __MX_DLL_SUPPORT_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
