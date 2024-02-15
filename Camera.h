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

public:
	Camera(float aspect, DirectX::XMFLOAT3 position);
	~Camera();

	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt);
};