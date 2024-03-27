#pragma once
#include <DirectXMath.h>
#include "SimpleShader.h"
#include <memory>

class Material
{
public:
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	DirectX::XMFLOAT4 surfaceColor;
	float roughness;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;

	Material(DirectX::XMFLOAT4 _colorTint, std::shared_ptr<SimplePixelShader> _ps, std::shared_ptr<SimpleVertexShader> _vs, float _roughness);
	void PrepareMaterial();
	~Material();
};