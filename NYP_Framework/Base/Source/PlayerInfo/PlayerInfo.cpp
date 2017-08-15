#include "PlayerInfo.h"
#include <iostream>

#include "MouseController.h"
#include "KeyboardController.h"
#include "Mtx44.h"
#include "../HardwareAbstraction/Keyboard.h"
#include "../BuildingManager.h"
#include "../HardwareAbstraction/Mouse.h"
#include "../Sound_Engine.h"

// Allocating and initializing Player's static data member.  
// The pointer is allocated but not the object's constructor.

Player::Player(void)
	: m_dSpeed(40.0)
	, m_dAcceleration(10.0)
	, m_dElapsedTime(0.0)
	, attachedCamera(NULL)
	, m_pTerrain(NULL)
	, speedMultiplier(1.0)
{
}

Player::~Player(void)
{
	m_pTerrain = NULL;
}

// Initialise this class instance
void Player::Init(void)
{
	// Set the current values
	position.Set(MAX_CELLS * CELL_SIZE / 2, 0, MAX_CELLS * CELL_SIZE / 2);

	// Set the default values
	defaultPosition = position;

	// Set Boundary
	maxBoundary.Set(1,1,1);
	minBoundary.Set(-1, -1, -1);

	this->keyboard = new Keyboard();
	keyboard->Create(this);
	this->mouse = new Mouse();
	mouse->Create(this);


	CSoundEngine::GetInstance()->Init();
	//CSoundEngine::GetInstance()->Addthefuckingsound("HELLO", "Image//Hello.mp3");

}

// Set position
void Player::SetPos(const Vector3& pos)
{
	position = pos;
}

// Set the boundary for the player info
void Player::SetBoundary(Vector3 max, Vector3 min)
{
	maxBoundary = max;
	minBoundary = min;
}

// Set the terrain for the player info
void Player::SetTerrain(GroundEntity* m_pTerrain)
{






}

// Reset this player instance to default
void Player::Reset(void)
{
	// Set the current values to default values
	position = defaultPosition;
}

// Get position x of the player
Vector3 Player::GetPos(void) const
{
	return position;
}

/********************************************************************************
 Hero Update
 ********************************************************************************/
void Player::Update(double dt)
{
	double mouse_diff_x, mouse_diff_y;
	MouseController::GetInstance()->GetMouseDelta(mouse_diff_x, mouse_diff_y);

	double camera_yaw = mouse_diff_x * 0.0174555555555556;		// 3.142 / 180.0
	double camera_pitch = mouse_diff_y * 0.0174555555555556;	// 3.142 / 180.0

	keyboard->Read((float)dt);
	mouse->Read((float)dt);

	if (attachedCamera == NULL)
		std::cout << "No camera attached! Please make sure to attach one" << std::endl;
	direction = attachedCamera->GetCameraTarget() - attachedCamera->GetCameraPos();
	direction.Normalize();

	Vector3 up(0, 1, 0);

	//if it is a FPS Camera
	if (dynamic_cast<FPSCamera*>(attachedCamera))
	{
		Vector3 rightUV;
		if (KeyboardController::GetInstance()->IsKeyDown('W'))
		{
			position += direction.Normalized() * (float)m_dSpeed * (float)speedMultiplier * (float)dt;
		}
		else if (KeyboardController::GetInstance()->IsKeyDown('S'))
		{
			position -= direction.Normalized() * (float)m_dSpeed * (float)speedMultiplier * (float)dt;
		}
		if (KeyboardController::GetInstance()->IsKeyDown('A'))
		{
			rightUV = (direction.Normalized()).Cross(up);
			rightUV.y = 0;
			rightUV.Normalize();
			position -= rightUV * (float)m_dSpeed * (float)speedMultiplier * (float)dt;
		}
		else if (KeyboardController::GetInstance()->IsKeyDown('D'))
		{
			rightUV = (direction.Normalized()).Cross(up);
			rightUV.y = 0;
			rightUV.Normalize();
			position += rightUV * (float)m_dSpeed * (float)speedMultiplier * (float)dt;
		}
		// Constrain the position
		Constrain();
	}

	// If a camera is attached to this playerInfo class, then update it
	if (attachedCamera)
	{
		if (dynamic_cast<FPSCamera*>(attachedCamera)) {
			Vector3 cameraView = attachedCamera->GetCameraTarget() - attachedCamera->GetCameraPos();
			attachedCamera->SetCameraPos(position);
			attachedCamera->SetCameraTarget(position + cameraView.Normalized());
			dynamic_cast<FPSCamera*>(attachedCamera)->Update(dt);
		}
		else if (dynamic_cast<TopDownCamera*>(attachedCamera)) {
			attachedCamera->SetCameraPos(position + dynamic_cast<TopDownCamera*>(attachedCamera)->GetHeight());
			attachedCamera->SetCameraTarget(position);
			dynamic_cast<TopDownCamera*>(attachedCamera)->Update(dt);
		}
	}
}

// Constrain the position within the borders
void Player::Constrain(void)
{
















}

CameraBase * Player::getCamera()
{
	return attachedCamera;
}

void Player::AttachCamera(CameraBase* _cameraPtr)
{
	attachedCamera = _cameraPtr;
	if (dynamic_cast<FPSCamera*>(attachedCamera)) {
		Vector3 target = position + Vector3(1, 0, 0);
		Vector3 view = target - position;
		Vector3 up = Vector3(0, 0, 1).Cross(view).Normalized();
		dynamic_cast<FPSCamera*>(attachedCamera)->Init(position, target, up);
		std::cout << "FPS Camera loaded" << std::endl;
	}
	else if (dynamic_cast<TopDownCamera*>(attachedCamera)){
		Vector3 target = position;
		Vector3 up(0, 0, -1);
		dynamic_cast<TopDownCamera*>(attachedCamera)->Init(Vector3(position.x, position.y + 20, position.z), target, up, 100);
		std::cout << "Top down camera Loaded" << std::endl;
	}
}

void Player::DetachCamera()
{
	attachedCamera = nullptr;
}

bool Player::MoveFrontBack(const float deltaTime, const bool direction, const float speedMultiplier)
{
	if (direction)
	{
		position -= Vector3(0, 0, 1) * (float)m_dSpeed * (float)speedMultiplier * (float)deltaTime;
		Constrain();
		return true;
	}
	else
	{
		position += Vector3(0, 0, 1) * (float)m_dSpeed * (float)speedMultiplier * (float)deltaTime;
		Constrain();
		return true;
	}
	return false;
}

bool Player::MoveLeftRight(const float deltaTime, const bool direction, const float speedMultiplier)
{
	if (!direction)
	{
		position -= Vector3(1, 0, 0) * (float)m_dSpeed * (float)speedMultiplier * (float)deltaTime;
		Constrain();
		return true;
	}
	else
	{
		position += Vector3(1, 0, 0) * (float)m_dSpeed * (float)speedMultiplier * (float)deltaTime;
		Constrain();
		return true;
	}
	return false;
}
