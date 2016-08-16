/*
=============================================================================
	File:	Messaging.h
	Desc:	Definitions of messages targeted at render system.
=============================================================================
*/

#ifndef __RX_MESSAGING_H__
#define __RX_MESSAGING_H__

namespace abc {

//
//	rxMessage_OnWindowResized - sent when the output window is resized.
//
class rxMessage_OnWindowResized : public mxMessage
{
	DECLARE_CLASS( rxMessage_OnWindowResized );

public:
			rxMessage_OnWindowResized( mxUInt newWidth =0, mxUInt newHeight =0 );

	mxUInt	width, height;
};

}//End of namespace abc

#endif // !__RX_MESSAGING_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
