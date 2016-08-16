/*
=============================================================================
	File:	CameraController.cpp
	Desc:	A simple camera which can be controlled with mouse and keys.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <MiniSG.h>

namespace abc {

/*================================
	mxCameraController
================================*/

mxCameraController::mxCameraController( mxCamera& camera )
	: m_camera( camera )
{
	// These values are taken out of thin air.
	const FLOAT  MOVE_SPEED = 0.03f;
	const FLOAT  ROT_SPEED = 0.003f;

	m_fMoveSpeed = MOVE_SPEED;
	m_fRotateSpeed = ROT_SPEED;

	InitKeyMaps();
}

//
//	mxCameraController::HandleInputEvent
//
void mxCameraController::HandleInputEvent( const mxInputEvent& rEvent )
{
	if ( rEvent.Type == EInputEvent::InputEvent_Keyboard )
	{
		for ( mxUInt i = 0; i < NUM_MOVEMENT_ACTIONS; i++ )
		{
			if ( m_keyMaps[i].KeyCode == rEvent.Keyboard.Key )
			{
				m_keyStates[ m_keyMaps[i].Action ] = rEvent.Keyboard.PressedDown;
			}
		}
	}

	if ( rEvent.Type == EInputEvent::InputEvent_Mouse )
	{
		// when the mouse cursor moves, we reset its position to the center of the window

		// true if the cursor was placed to the window center;
		// this var is needed because on Windows SetCursorPos() causes a WM_MOUSEMOVE event.
		static bool bWasReset = false;

		if ( rEvent.Mouse.Type == mxMouseEvent::Cursor_Moved )
		{
			if ( !bWasReset )
			{
				if ( rEvent.Mouse.DeltaX != 0 )
				{
					FLOAT fYaw	= mxMath::FtoiFast( rEvent.Mouse.DeltaX ) * m_fRotateSpeed;
					m_camera.Yaw( fYaw );
				}
				if ( rEvent.Mouse.DeltaY != 0 )
				{
					FLOAT fPitch	= mxMath::FtoiFast( rEvent.Mouse.DeltaY ) * m_fRotateSpeed;
					m_camera.Pitch( fPitch );
				}

				// move the cursor to the center of the window
				mxUInt left, top, right, bottom;
				sys::GetWindowPosition( left, top, right, bottom );
				sys::SetMouseCursorPosition( (left + right)/2, (bottom + top)/2 );
				bWasReset = true;
			}
			else
			{
				bWasReset = false;
			}
		}
	}
}

//
//	mxCameraController::Update
//
void mxCameraController::Update( const mxTime elapsedTime )
{
	const FLOAT  MIN_MOVE_SPEED = 0.5f;
	const FLOAT  MAX_MOVE_SPEED = 0.9f;

	FLOAT  moveSpeed = m_fMoveSpeed * elapsedTime.fTime;
	moveSpeed = Clamp<FLOAT>( moveSpeed, MIN_MOVE_SPEED, MAX_MOVE_SPEED );

	// Forward/backward moving.
	if ( m_keyStates[ MOVE_FORWARD ] ) {
		m_camera.Walk( +moveSpeed );		
	}
	if ( m_keyStates[ MOVE_BACKWARD ] ) {
		m_camera.Walk( -moveSpeed );		
	}

	// Strafing.
	if ( m_keyStates[ STRAFE_LEFT ] ) {
		m_camera.Strafe( -moveSpeed );	
	}
	if ( m_keyStates[ STRAFE_RIGHT ] ) {
		m_camera.Strafe( +moveSpeed );	
	}

	// Jumping.
	if ( m_keyStates[ JUMP_UP ] ) {
		m_camera.Fly( +moveSpeed );		
	}
	// Crouching.
	if ( m_keyStates[ DUCK ] ) {
		m_camera.Fly( -moveSpeed );		
	}
}

//
//	mxCameraController::InitKeyMaps
//
void mxCameraController::InitKeyMaps()
{
	m_keyStates.Zero();
	m_keyMaps.Zero();

	m_keyMaps[ MOVE_FORWARD ].KeyCode	= EKeyCode::Key_W;
	m_keyMaps[ MOVE_FORWARD ].Action	= MOVE_FORWARD;

	m_keyMaps[ MOVE_BACKWARD ].KeyCode	= EKeyCode::Key_S;
	m_keyMaps[ MOVE_BACKWARD ].Action	= MOVE_BACKWARD;

	m_keyMaps[ STRAFE_LEFT ].KeyCode	= EKeyCode::Key_A;
	m_keyMaps[ STRAFE_LEFT ].Action		= STRAFE_LEFT;

	m_keyMaps[ STRAFE_RIGHT ].KeyCode	= EKeyCode::Key_D;
	m_keyMaps[ STRAFE_RIGHT ].Action	= STRAFE_RIGHT;

	m_keyMaps[ DUCK ].KeyCode			= EKeyCode::Key_C;
	m_keyMaps[ DUCK ].Action			= DUCK;

	m_keyMaps[ JUMP_UP ].KeyCode		= EKeyCode::Key_Space;
	m_keyMaps[ JUMP_UP ].Action			= JUMP_UP;


	m_keyMaps[ LOOK_UP ].KeyCode	= EKeyCode::Key_Up;
	m_keyMaps[ LOOK_UP ].Action		= LOOK_UP;

	m_keyMaps[ LOOK_DOWN ].KeyCode	= EKeyCode::Key_Down;
	m_keyMaps[ LOOK_DOWN ].Action	= LOOK_DOWN;

	m_keyMaps[ LOOK_LEFT ].KeyCode	= EKeyCode::Key_Left;
	m_keyMaps[ LOOK_LEFT ].Action	= LOOK_LEFT;

	m_keyMaps[ LOOK_RIGHT ].KeyCode	= EKeyCode::Key_Right;
	m_keyMaps[ LOOK_RIGHT ].Action	= LOOK_RIGHT;
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
