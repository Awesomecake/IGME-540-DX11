#include "ShadowMap.h"

using namespace DirectX;

ShadowMap::ShadowMap() : shadowProjectionMatrix(XMFLOAT4X4()), shadowViewMatrix(XMFLOAT4X4()), windowHeight(0), windowWidth(0) { }

ShadowMap::ShadowMap(Microsoft::WRL::ComPtr<ID3D11Device> device, std::shared_ptr<SimpleVertexShader> _shadowMapVertexShader, int _windowWidth, int _windowHeight) 
	: shadowMapVertexShader(_shadowMapVertexShader), windowWidth(_windowWidth), windowHeight(_windowHeight)
{
	shadowProjectionMatrix = XMFLOAT4X4();
	shadowViewMatrix = XMFLOAT4X4();

	// Create the actual texture that will be the shadow map
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowMapResolution; // Ideally a power of 2 (like 1024)
	shadowDesc.Height = shadowMapResolution; // Ideally a power of 2 (like 1024)
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;
	device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());

	// Create the depth/stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(
		shadowTexture.Get(),
		&shadowDSDesc,
		shadowDSV.GetAddressOf());

	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(
		shadowTexture.Get(),
		&srvDesc,
		shadowSRV.GetAddressOf());

	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Min. precision units, not world units!
	shadowRastDesc.SlopeScaledDepthBias = 1.0f; // Bias more based on slope
	device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);

	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f; // Only need the first component
	device->CreateSamplerState(&shadowSampDesc, &shadowSampler);
}

ShadowMap::~ShadowMap() { }

void ShadowMap::MakeProjection(XMFLOAT3 direction)
{

	XMVECTOR lightDirection = XMVectorSet(direction.x, direction.y, direction.z, 0);

	XMMATRIX lightView = XMMatrixLookToLH(
		-lightDirection * 10, // Position: "Backing up" 20 units from origin
		lightDirection, // Direction: light's direction
		XMVectorSet(0, 1, 0, 0)); // Up: World up vector (Y axis)

	XMMATRIX lightProjection = XMMatrixOrthographicLH(
		lightProjectionSize,
		lightProjectionSize,
		1.0f,
		100.0f);


	XMStoreFloat4x4(&shadowViewMatrix, lightView);
	XMStoreFloat4x4(&shadowProjectionMatrix, lightProjection);
}

void ShadowMap::DrawShadowMap(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::vector<GameEntity> gameEntities, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferRTV, Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthBufferDSV)
{
	context->ClearDepthStencilView(shadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	ID3D11RenderTargetView* nullRTV{};
	context->OMSetRenderTargets(1, &nullRTV, shadowDSV.Get());

	context->PSSetShader(0, 0, 0);
	context->RSSetState(shadowRasterizer.Get());

	D3D11_VIEWPORT viewport = {};
	viewport.Width = (float)shadowMapResolution;
	viewport.Height = (float)shadowMapResolution;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	shadowMapVertexShader->SetShader();
	shadowMapVertexShader->SetMatrix4x4("view", shadowViewMatrix);
	shadowMapVertexShader->SetMatrix4x4("projection", shadowProjectionMatrix);

	// Loop and draw all entities
	for (GameEntity entity : gameEntities)
	{
		shadowMapVertexShader->SetMatrix4x4("world", entity.GetTransform().GetWorldMatrix());
		shadowMapVertexShader->CopyAllBufferData();

		// Draw the mesh directly to avoid the entity's material
		// Note: Your code may differ significantly here!
		entity.GetMesh()->Draw(context);
	}

	context->RSSetState(0);

	viewport.Width = (float)windowWidth;
	viewport.Height = (float)windowHeight;
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthBufferDSV.Get());
}