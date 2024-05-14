#pragma once
#include "DXCore.h"
#include <DirectXMath.h>
#include <memory>
#include "SimpleShader.h"
#include "GameEntity.h"

class ShadowMap
{
private:
	int shadowMapResolution = 1024;
	float lightProjectionSize = 30.0f; // Tweak for your scene!

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	int windowWidth;
	int windowHeight;
	std::shared_ptr<SimpleVertexShader> shadowMapVertexShader;

public:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;

	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;

	ShadowMap();
	ShadowMap(Microsoft::WRL::ComPtr<ID3D11Device> device, std::shared_ptr<SimpleVertexShader> _shadowMapVertexShader, int _windowWidth, int _windowHeight);
	~ShadowMap();

	void Resize(int _windowWidth, int _windowHeight);
	void MakeProjection(DirectX::XMFLOAT3 direction);
	void DrawShadowMap(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::vector<GameEntity> gameEntities, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferRTV, Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthBufferDSV);
};