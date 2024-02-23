#pragma once
#include <DirectXMath.h>
#include "SimpleShader.h"
#include <memory>

class Material
{
public:
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	DirectX::XMFLOAT4 colorTint;

	Material(DirectX::XMFLOAT4 _colorTint, std::shared_ptr<SimplePixelShader> _ps, std::shared_ptr<SimpleVertexShader> _vs);
	~Material();
};