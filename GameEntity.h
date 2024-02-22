#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"
#include <memory>

class GameEntity
{
private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;

public:

	GameEntity(std::shared_ptr<Mesh> refMesh);
	~GameEntity();
	std::shared_ptr<Mesh> GetMesh();
	Transform& GetTransform();

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11Buffer> constBuffer, std::shared_ptr<Camera> camera);
};