#pragma once
#include "DXCore.h"
#include <DirectXMath.h>
#include <memory>
#include "SimpleShader.h"
#include "GameEntity.h"

class ShadowMap
{
	int shadowMapResolution = 1024;
	float lightProjectionSize = 15.0f; // Tweak for your scene!

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;

	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;

public:
	std::shared_ptr<SimpleVertexShader> shadowMapVertexShader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;

	ShadowMap() {};
	ShadowMap(Microsoft::WRL::ComPtr<ID3D11Device> device, std::shared_ptr<SimpleVertexShader> _shadowMapVertexShader);
	~ShadowMap();

	void MakeProjection(DirectX::XMFLOAT3 direction);
	void DrawShadowMap(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::vector<GameEntity> gameEntities);
};