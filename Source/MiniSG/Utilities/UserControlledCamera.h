/*
=============================================================================
	File:	UserControlledCamera.h
	Desc:	A simple camera which can be controlled with mouse and keys.
=============================================================================
*/

#ifndef __MX_USER_CONTROLLED_CAMERA_H__
#define __MX_USER_CONTROLLED_CAMERA_H__

namespace abc {

//
//	mxCameraController - allows a user to control the camera with keyboard and mouse.
//
class mxCameraController {
public:
			mxCameraController( mxCamera& camera );

	const mxSceneView &	GetView() const;

	mxCamera &	GetCamera();

	// Don't forget to call these functions in your application!

	virtual void	HandleInputEvent( const mxInputEvent& rEvent );
	virtual void	Update( const mxTime elapsedTime );

	//-----------------------------------------------------

	void SetMoveSpeed( FLOAT newLinearMovementSpeed )
	{
		m_fMoveSpeed = newLinearMovementSpeed;
	}
	FLOAT GetMoveSpeed() const
	{
		return m_fMoveSpeed;
	}

	void SetRotateSpeed( FLOAT newAngularMovementSpeed )
	{
		m_fRotateSpeed = newAngularMovementSpeed;
	}
	FLOAT GetRotateSpeed() const
	{
		return m_fRotateSpeed;
	}

private:

	//
	//	EMovementAction - for mapping keys to camera controls.
	//
	enum EMovementAction
	{
		MOVE_FORWARD,
		MOVE_BACKWARD,
		STRAFE_LEFT,
		STRAFE_RIGHT,
		JUMP_UP,
		DUCK,

		LOOK_UP,
		LOOK_DOWN,
		LOOK_LEFT,
		LOOK_RIGHT,

		NUM_MOVEMENT_ACTIONS
	};

	//
	//	SKeyMap
	//
	struct SKeyMap
	{
		EMovementAction	Action;
		EKeyCode		KeyCode;
	};

private:
	void	InitKeyMaps();	// Binds actions to keys.

private:
	mxCamera &	m_camera;	// The camera we're controlling.

	FLOAT	m_fMoveSpeed;	// Movement speed.
	FLOAT	m_fRotateSpeed;	// Rotational speed.

	// Needed for continuous camera movement.
	TFixedArray< bool, NUM_MOVEMENT_ACTIONS >		m_keyStates;
	TFixedArray< SKeyMap, NUM_MOVEMENT_ACTIONS >	m_keyMaps;

private:
	NO_COPY_CONSTRUCTOR( mxCameraController );
	NO_ASSIGNMENT( mxCameraController );
};

inline const mxSceneView & mxCameraController::GetView() const
{
	return m_camera.GetView();
}

inline mxCamera & mxCameraController::GetCamera()
{
	return m_camera;
}

}//End of namespace abc

#endif // ! __MX_USER_CONTROLLED_CAMERA_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
