/*
=============================================================================
	File:	Message.cpp
	Desc:	Messaging system.
			Messages are used for communication between subsystems.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <Base.h>

namespace abc {

/*================================
		mxMessage
================================*/

DEFINE_ABSTRACT_CLASS( mxMessage, 'MSG_', mxObject );

mxMessage::mxMessage()
{
}

mxMessage::~mxMessage()
{
}

/*================================
		mxMessagePort
================================*/

DEFINE_CLASS( mxMessagePort, 'MSGP', mxObject );

mxMessagePort::mxMessagePort()
{
}

mxMessagePort::~mxMessagePort()
{
}

void mxMessagePort::HandleMessage( const TMessagePtr& msg )
{
	// Empty...
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
