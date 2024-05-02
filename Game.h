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
#include "Sky.h"
#include "ShadowMap.h"
#include "PostProcess.h"

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
	void RenderScene();

	//ImGui test value
	bool showImGuiDemoWindow = false;
	bool randomizeColorOffset = false;
	int ImGuiMaterialIndex = 0;
	DirectX::XMFLOAT3 ambientColor = { 0.5f,0.5f,0.5f };

	std::vector<GameEntity> gameEntities;
	std::vector<Light> lights = std::vector<Light>();

	std::shared_ptr<Sky> sky;

	//Camera
	std::vector<std::shared_ptr<Camera>> cameras;
	int selectedCamera = 0;

	//Meshes
	std::shared_ptr<Mesh> cube;
	std::shared_ptr<Mesh> cylinder;
	std::shared_ptr<Mesh> helix;
	std::shared_ptr<Mesh> sphere;
	std::shared_ptr<Mesh> torus;
	std::shared_ptr<Mesh> quad;

	//Materials
	std::vector<std::shared_ptr<Material>> materials;

	//Shadow Map
	ShadowMap shadowMap;

	//Post Processing
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler;
	std::shared_ptr<SimpleVertexShader> ppVS;

	PostProcess postProcess1;
	PostProcess postProcess2;

	float blurAmount = 0;
	float pixelIntensity = 0;

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders(); 
	void CreateGeometry();
	void CreateMaterial(std::wstring albedoFile, std::wstring normalFile, std::wstring roughnessFile, std::wstring metalnessFile);
	
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimplePixelShader> ppPS1;
	std::shared_ptr<SimplePixelShader> ppPS2;

	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimpleVertexShader> shadowMapVertexShader;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

};

