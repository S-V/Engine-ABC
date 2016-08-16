/*
=============================================================================
	File:	Renderer.cpp
	Desc:	Render system definitions.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>

namespace abc {

/*================================
	rxRenderDeviceCreationInfo
================================*/

bool rxRenderDeviceCreationInfo::IsValid() const
{
	return (pWindowHandle != null)
		&& screen.IsValid()
		&& (refreshRateHz < 1000)
		;
}

/*================================
			rxRenderer
================================*/

/*================================
		rxDrawEntity
================================*/

rxDrawEntity::~rxDrawEntity()
{}

/*================================
		rxNullDrawEntity
================================*/

rxNullDrawEntity::rxNullDrawEntity()
{}

rxNullDrawEntity::~rxNullDrawEntity()
{}

void rxNullDrawEntity::Render( const rxView& view, rxQueue& queue )
{
	UnusedParameter( view );
	UnusedParameter( queue );
}

void rxNullDrawEntity::Remove()
{}

//[static]
rxNullDrawEntity	rxNullDrawEntity::dummy;

/*================================
			rxModel
================================*/



/*================================
		rxModelDescription
================================*/

rxModelDescription::rxModelDescription()
	: flags( 0 )
{}

bool rxModelDescription::IsOk() const
{
	return meshDesc.IsOk()
		&& dynamicGeom.IsOk()
		;
}

/*================================
		rxDisplayMode
================================*/

rxDisplayMode::rxDisplayMode()
{
	SetDefaultValues();
}

void rxDisplayMode::SetDefaultValues()
{
	screen.SetDefaultValues();
	refreshRateHz = 60;
	//format = EPixelFormat::PF_DEFAULT;
}

bool rxDisplayMode::IsValid() const
{
	return screen.IsValid();
}

//===================================================================

/*
	This code is based on information gotten from the following site:
	http://www.pcidatabase.com/
*/

EDeviceVendor GetGPUVendorEnum( mxUInt vendorId )
{
	switch ( vendorId ) {
		case 0x3D3D	:
			return EDeviceVendor::Vendor_3DLABS;

		case 0x1002 :
			return EDeviceVendor::Vendor_ATI;

		case 0x8086 :
			return EDeviceVendor::Vendor_Intel;

		case 0x102B :
			return EDeviceVendor::Vendor_Matrox;

		case 0x10DE	:
			return EDeviceVendor::Vendor_NVidia;

		case 0x5333	:
			return EDeviceVendor::Vendor_S3;

		case 0x1039	:
			return EDeviceVendor::Vendor_SIS;

		default:
			// empty
			;
	}
	return EDeviceVendor::Vendor_Unknown;
}

/*
	This code is based on information gotten from the following site:
	http://www.pcidatabase.com/
*/

const WChar * GetGPUVendorName( EDeviceVendor vendor )
{
	switch ( vendor ) {
		case EDeviceVendor::Vendor_3DLABS :
			return L"3Dlabs, Inc. Ltd";

		case EDeviceVendor::Vendor_ATI :
			return L"ATI Technologies Inc. / Advanced Micro Devices, Inc.";

		case EDeviceVendor::Vendor_Intel :
			return L"Intel Corporation";

		case EDeviceVendor::Vendor_Matrox :
			return L"Matrox Electronic Systems Ltd.";

		case EDeviceVendor::Vendor_NVidia :
			return L"NVIDIA Corporation";

		case EDeviceVendor::Vendor_S3 :
			return L"S3 Graphics Co., Ltd";

		case EDeviceVendor::Vendor_SIS :
			return L"SIS";

		case EDeviceVendor::Vendor_Unknown :
		default:
			return L"Unknown vendor";
	}
	//Unreachable;
}

/*
================================
		GetPostFxName
================================
*/
const mxChar* GetPostFxName( EPostFx postFxType )
{
	switch( postFxType )
	{
		case EPostFx::PostFx_Unknown :			return TEXT("Unknown");
		case EPostFx::PostFx_Coloring :			return TEXT("Coloring");		
		case EPostFx::PostFx_GrayScale :		return TEXT("Grayscale");
		case EPostFx::PostFx_BlackAndWhite :	return TEXT("Black And White");
		case EPostFx::PostFx_InvertColor :		return TEXT("Invert Color");
		case EPostFx::PostFx_Emboss :			return TEXT("Emboss");
		case EPostFx::PostFx_Sharpen :			return TEXT("Sharpen");
		case EPostFx::PostFx_Sepia :			return TEXT("Sepia");
		case EPostFx::PostFx_CellShading :		return TEXT("Cell Shading");
		case EPostFx::PostFx_ModulatingWave :	return TEXT("Wave Distortion");

		case EPostFx::PostFx_Blur :				return TEXT("Simple Blur");
		case EPostFx::PostFx_GBlur :			return TEXT("Gaussian Blur");		
															 
		case EPostFx::PostFx_Bloom :			return TEXT("Bloom");
		case EPostFx::PostFx_DoF :				return TEXT("Depth Of Field");	
															 
		case EPostFx::PostFx_MotionBlur :		return TEXT("Motion Blur");	
		case EPostFx::PostFx_RadialBlur :		return TEXT("Radial Blur");	
															 
		case EPostFx::PostFx_HDR :				return TEXT("HDR");
		case EPostFx::PostFx_SSAO :				return TEXT("SSAO");			

		default:
			;
	}
	Unreachable;
	return null;
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
