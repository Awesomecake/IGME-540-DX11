#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"
#include "SimpleShader.h"
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

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<SimplePixelShader> pixelShader, std::shared_ptr<SimpleVertexShader> vertexShader , std::shared_ptr<Camera> camera);
};