#include "PostProcess.h"

PostProcess::PostProcess(Microsoft::WRL::ComPtr<ID3D11Device> device, int _windowWidth, int _windowHeight, Microsoft::WRL::ComPtr<ID3D11SamplerState> _ppSampler, std::shared_ptr<SimplePixelShader> _ppPS)
{
	// Describe the texture we're creating
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = _windowWidth;
	textureDesc.Height = _windowHeight;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	// Create the resource (no need to track it after the views are created below)
	Microsoft::WRL::ComPtr<ID3D11Texture2D> ppTexture;
	device->CreateTexture2D(&textureDesc, 0, ppTexture.GetAddressOf());

	// Create the Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	device->CreateRenderTargetView(ppTexture.Get(), &rtvDesc, ppRTV.ReleaseAndGetAddressOf());
	// Create the Shader Resource View
	// By passing it a null description for the SRV, we
	// get a "default" SRV that has access to the entire resource
	device->CreateShaderResourceView(ppTexture.Get(), 0, ppSRV.ReleaseAndGetAddressOf());

	ppSampler = _ppSampler;
	windowWidth = _windowWidth;
	windowHeight = _windowHeight;
	ppPS = _ppPS;
}

void PostProcess::ClearRTV(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, float *bgColor)
{
	context->ClearRenderTargetView(ppRTV.Get(), bgColor);
}

void PostProcess::RenderPostProcess(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTarget, Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthBufferSRV)
{
	context->OMSetRenderTargets(1, renderTarget.GetAddressOf(), 0);

	ppPS->SetShader();
	ppPS->SetShaderResourceView("Pixels", ppSRV.Get());
	ppPS->SetSamplerState("ClampSampler", ppSampler.Get());

	//Post Process assumes that any data that expects window dimensions will use these names
	ppPS->SetFloat("windowWidth", windowWidth);
	ppPS->SetFloat("windowHeight", windowHeight);

	for (auto& t : pixelShaderFloatData) { ppPS->SetFloat(t.first.c_str(), *t.second); }

	ppPS->CopyAllBufferData();

	context->Draw(3, 0); // Draw exactly 3 vertices (one triangle)
}