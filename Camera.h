#pragma once
#include "Input.h"
#include "Transform.h"
#include <DirectXMath.h>

class Camera
{
private:
	Transform transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;

	bool isDirty = false;

	float fov = 45;
	float moveSpeed = 1;
	float mouseSensitivity = 0.005;

public:
	Camera(float aspect, float _fov, DirectX::XMFLOAT3 position);
	~Camera();

	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	Transform GetTransform();
	float GetFOV();

	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt);
};