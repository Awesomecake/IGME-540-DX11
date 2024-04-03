#pragma once
#include "DXCore.h"
#include <memory>
#include "Mesh.h"
#include "SimpleShader.h"
#include "Camera.h"

class Sky
{
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampleState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapTexture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> stencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<SimplePixelShader> ps;
	std::shared_ptr<SimpleVertexShader> vs;

public:
	Sky(std::shared_ptr<Mesh> _mesh,Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampleState,Microsoft::WRL::ComPtr<ID3D11Device> device,Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,std::wstring cubeMapFilePath);
	~Sky();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, 
		const wchar_t* right, 
		const wchar_t* left, 
		const wchar_t* up, 
		const wchar_t* down, 
		const wchar_t* front, 
		const wchar_t* back);

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera);

	DirectX::XMFLOAT3 ambient;
};