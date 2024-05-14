#pragma once
#include <memory>
#include "SimpleShader.h"

class PostProcess
{
public:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ppRTV; // For rendering

	std::unordered_map<std::string, float*> pixelShaderFloatData;

	PostProcess() {};
	PostProcess(Microsoft::WRL::ComPtr<ID3D11Device> device, int _windowWidth, int _windowHeight, Microsoft::WRL::ComPtr<ID3D11SamplerState> _ppSampler, std::shared_ptr<SimplePixelShader> _ppPS);
	~PostProcess() {};

	void ClearRTV(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, float *bgColor);
	void Resize(Microsoft::WRL::ComPtr<ID3D11Device> device, int _windowWidth, int _windowHeight);
	void RenderPostProcess(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTarget, Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthBufferSRV);
private:
	D3D11_TEXTURE2D_DESC textureDesc = {};
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};

	Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ppSRV; // For sampling
	std::shared_ptr<SimplePixelShader> ppPS;

	int windowWidth;
	int windowHeight;
};