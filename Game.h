#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include "Mesh.h"
#include <memory>

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
	DirectX::XMFLOAT4 backgroundColor = DirectX::XMFLOAT4(1.0f, 0.0f, 0.5f, 1.0f);
	DirectX::XMFLOAT4 cbColorTint = DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
	DirectX::XMFLOAT3 cbOffset = DirectX::XMFLOAT3(0.25f, 0.0f, 0.0f);

	//Constant Buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> constBuffer;

	//Meshes
	std::shared_ptr<Mesh> triangle;
	std::shared_ptr<Mesh> trapezoid;
	std::shared_ptr<Mesh> shape;

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders(); 
	void CreateGeometry();
	
	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

};

