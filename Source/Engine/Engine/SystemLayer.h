/*
=============================================================================
	File:	SystemLayer.h
	Desc:	System layer.
=============================================================================
*/

#ifndef __MX_SYSTEM_LAYER_H__
#define __MX_SYSTEM_LAYER_H__

namespace abc {

/*
//
//	mxSystemLayer
//
class mxSystemLayer {
protected:
	virtual ~mxSystemLayer() {}
public:
*/

namespace sys {

//------------------------------------------------------------
//	Windows control.
//------------------------------------------------------------

//
//	Returns the dimensions of the bounding rectangle of the main application window
//	(relative to the upper-left corner of the screen).
//
//	Param[Out]: Left	- the x-coordinate of the upper-left corner of the window
//	Param[Out]: Top		- the y-coordinate of the upper-left corner of the window
//	Param[Out]: Right	- the x-coordinate of the lower-right corner of the window
//	Param[Out]: Bottom	- the y-coordinate of the lower-right corner of the window
//
void	GetWindowPosition( mxUInt &Left, mxUInt &Top, mxUInt &Right, mxUInt &Bottom );

// sets the main window's caption
void	SetWindowCaption( const mxChar* newCaption );

//------------------------------------------------------------
//	Mouse cursor control.
//------------------------------------------------------------

void	SetMouseCursorPositionRel( mxUInt x, mxUInt y ); // coordinates are relative to the main window's client area

//------------------------------------------------------------
//	Miscellaneous.
//------------------------------------------------------------

mxUInt	GetFrameCount();		// Returns the total number of frames rendered.
mxUInt	GetLastFrameTimeMs();	// Returns the amount of time in which the last frame was rendered.

mxUInt	GetAverageFPS();
mxUInt	GetInstantFPS();

};

}//End of namespace abc

#endif // ! __MX_SYSTEM_LAYER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
