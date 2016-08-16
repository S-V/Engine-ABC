/*
=============================================================================
	File:	Application.cpp
	Desc:	Application framework.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>

namespace abc {

const mxChar* GetDriverName( EDriverType driver )
{
	switch( driver ) {
		case EDriverType::GAPI_DirectX_9 :
			return "D3D9";

		case EDriverType::GAPI_DirectX_10 :
			return "D3D10";

		case EDriverType::GAPI_AutomaticSelection :
			return "unknown";

		case EDriverType::GAPI_None :	// fallthru
		default:						// fallthru
			;
	}
	return "unknown";
}

/*================================
		mxScreenSettings
================================*/

mxScreenSettings::mxScreenSettings()
{
	SetDefaultValues();
}

void mxScreenSettings::SetDefaultValues()
{
	width = 1024;
	height = 768;
	isFullscreen = true;
}

bool mxScreenSettings::IsValid() const
{
	return true;
}

/*================================
		mxGraphicsSettings
================================*/

mxGraphicsSettings::mxGraphicsSettings()
{
	SetDefaultValues();
}

void mxGraphicsSettings::SetDefaultValues()
{
	driverType = EDriverType::GAPI_DirectX_9;

	screen.SetDefaultValues();
}

bool mxGraphicsSettings::IsValid() const
{
	return screen.IsValid()
		&& (driverType != EDriverType::GAPI_None);
}

/*================================
		mxSystemSettings
================================*/

mxSystemSettings::mxSystemSettings()
{
	SetDefaultValues();
}

void mxSystemSettings::SetDefaultValues()
{
	bCreateConsole = g_bDebugMode;
}

bool mxSystemSettings::IsValid() const
{
	return true;
}

/*================================
		mxSystemCreationInfo
================================*/

mxSystemCreationInfo::mxSystemCreationInfo()
{
	SetDefaultValues();
}

void mxSystemCreationInfo::SetDefaultValues()
{
	mxGraphicsSettings::SetDefaultValues();
	mxSystemSettings::SetDefaultValues();

	pUserApp = null;
}

bool mxSystemCreationInfo::IsValid() const
{
	return mxGraphicsSettings::IsValid()
		&& mxSystemSettings::IsValid()
		&& (pUserApp != null);
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
