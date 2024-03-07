#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects

#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Lights.h"

#include <memory>
#include <vector>

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void ImGuiUpdate(float deltaTime, float totalTime);
	void BuildUI(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	//ImGui test value
	bool showImGuiDemoWindow = false;
	bool randomizeColorOffset = false;
	DirectX::XMFLOAT4 backgroundColor = DirectX::XMFLOAT4(0.25, 0.0f, 0.25f, 1.0f);

	//Meshes
	std::shared_ptr<Mesh> cube;
	std::shared_ptr<Mesh> cylinder;
	std::shared_ptr<Mesh> helix;
	std::shared_ptr<Mesh> sphere;
	std::shared_ptr<Mesh> torus;
	std::shared_ptr<Mesh> quad;

	std::vector<GameEntity> gameEntities;

	//Materials
	std::shared_ptr<Material> mat1;
	std::shared_ptr<Material> mat2;
	std::shared_ptr<Material> mat3;

	//Camera
	std::vector<std::shared_ptr<Camera>> cameras;
	int selectedCamera = 0;

	DirectX::XMFLOAT3 ambientColor = { 0.1,0.1,0.1};

	std::vector<Light> lights = std::vector<Light>();

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders(); 
	void CreateGeometry();
	
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimplePixelShader> pixelShader2;
	std::shared_ptr<SimpleVertexShader> vertexShader;
};

