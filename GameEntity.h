#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Lights.h"

#include <memory>

class GameEntity
{
private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;

public:

	GameEntity(std::shared_ptr<Mesh> refMesh, std::shared_ptr<Material> _material);
	~GameEntity();
	std::shared_ptr<Mesh> GetMesh();
	Transform& GetTransform();
	std::shared_ptr<Material> GetMaterial();

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera, float totalTime, std::vector<Light> lights);
};