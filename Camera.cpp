#include "Camera.h"
using namespace DirectX;

Camera::Camera(float aspect, float _fov, XMFLOAT3 position) : transform(position), fov(_fov)
{
	UpdateProjectionMatrix(aspect);
	UpdateViewMatrix();
}

Camera::~Camera()
{
	UpdateProjectionMatrix(16.f/9);
	UpdateViewMatrix();
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projMatrix;
}

Transform Camera::GetTransform()
{
	return transform;
}

float Camera::GetFOV()
{
	return fov;
}


void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	XMMATRIX matrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, 0.1, 10);
	XMStoreFloat4x4(&projMatrix, matrix);
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR pos = XMVectorSet(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z, 0);
	XMVECTOR forward = XMVectorSet(transform.GetForward().x, transform.GetForward().y, transform.GetForward().z, 0);
	XMMATRIX matrix = XMMatrixLookToLH(pos, forward, XMVectorSet(0, 1, 0,0));
	XMStoreFloat4x4(&viewMatrix, matrix);
}

void Camera::Update(float dt)
{
	Input& input = Input::GetInstance();

	if (input.KeyDown('W'))
	{
		transform.MoveRelative(0, 0, moveSpeed * dt);
		isDirty = true;
	}

	if (input.KeyDown('S'))
	{
		transform.MoveRelative(0, 0, -moveSpeed * dt);
		isDirty = true;
	}

	if (input.KeyDown('A'))
	{
		transform.MoveRelative(-moveSpeed * dt, 0, 0);
		isDirty = true;
	}

	if (input.KeyDown('D'))
	{
		transform.MoveRelative(moveSpeed * dt, 0, 0);
		isDirty = true;
	}

	if (input.KeyDown(VK_SPACE))
	{
		transform.MoveAbsolute(0, moveSpeed * dt, 0);
		isDirty = true;
	}

	if (input.KeyDown('X'))
	{
		transform.MoveAbsolute(0, -moveSpeed * dt, 0);
		isDirty = true;
	}

	if (input.MouseLeftDown()) {
		isDirty = true;

		transform.Rotate(0, input.GetMouseXDelta() * mouseSensitivity, 0);
		// Clamp the pitch rotation
		transform.Rotate(input.GetMouseYDelta() * mouseSensitivity, 0, 0);

		if (transform.GetPitchYawRoll().x > XM_PIDIV2 || transform.GetPitchYawRoll().x < -XM_PIDIV2)
			transform.Rotate(-input.GetMouseYDelta() * mouseSensitivity, 0, 0);
	}

	if(isDirty)
		UpdateViewMatrix();
}