#include "Material.h"

Material::Material(DirectX::XMFLOAT4 _colorTint, std::shared_ptr<SimplePixelShader> _ps, std::shared_ptr<SimpleVertexShader> _vs, float _roughness)
	: surfaceColor(_colorTint), pixelShader(_ps), vertexShader(_vs) 
{ 
	if (_roughness < 0)
		_roughness = 0;

	if (_roughness > 1)
		_roughness = 1;

	roughness = _roughness;
}

Material::~Material()
{

}