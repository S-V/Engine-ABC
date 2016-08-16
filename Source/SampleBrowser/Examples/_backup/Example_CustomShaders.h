/*
=============================================================================
	File:	Example_CustomShaders.h
	Desc:	Shows how to integrate your own shaders.
=============================================================================
*/

#ifndef __EXAMPLE_CUSTOM_SHADERS_H__
#define __EXAMPLE_CUSTOM_SHADERS_H__

#include <ExtLib/ExtLib.h>

namespace examples {

using namespace ::abc;

class Sample_CustomShaders : public Sample_Basic {
public:
	Sample_CustomShaders()
	{}
	//----------------------------------------------------------------------------------------------------
	override(Sample_Basic) bool Create()
	{
		if ( ! Sample_Basic::Create() ) {
			return false;
		}

		rxResources & resources = mxEngine::get().GetRenderer().GetResources();

		return true;
	}
};

}//End of namespace examples

#endif // ! __EXAMPLE_CUSTOM_SHADERS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
