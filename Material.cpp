#include "Material.h"

Material::Material(DirectX::XMFLOAT4 _colorTint, std::shared_ptr<SimplePixelShader> _ps, std::shared_ptr<SimpleVertexShader> _vs)
	: colorTint(_colorTint), pixelShader(_ps), vertexShader(_vs) { }

Material::~Material()
{

}