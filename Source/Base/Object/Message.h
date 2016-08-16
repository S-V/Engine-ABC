/*
=============================================================================
	File:	Message.h
	Desc:	Messaging system.
			Messages are used for communication between subsystems.
=============================================================================
*/

#ifndef __MX_MESSAGE_H__
#define __MX_MESSAGE_H__

namespace abc {

//
//	mxMessage - base class for messages.
//
class mxMessage : public mxObject
{
	DECLARE_ABSTRACT_CLASS( mxMessage );

protected:
			mxMessage();
	virtual	~mxMessage();
};

typedef TPtr< mxMessage >	TMessagePtr;

//
//	mxMessagePort - receives and processes messages.
//
//	A message port is blocked until the received message is processed.
//
class mxMessagePort : public mxObject
{
	DECLARE_CLASS( mxMessagePort );

public:

	// Process the given message.
	//
	virtual void HandleMessage( const TMessagePtr& msg );

protected:
			mxMessagePort();
	virtual	~mxMessagePort();
};

}//End of namespace abc

#endif // !__MX_MESSAGE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

