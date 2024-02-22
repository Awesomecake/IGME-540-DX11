#include "GameEntity.h"
#include "BufferStructs.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> refMesh)
{
	mesh = refMesh;
}

GameEntity::~GameEntity()
{

}

Transform& GameEntity::GetTransform()
{
	return transform;
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return mesh;
}

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11Buffer> constBuffer, std::shared_ptr<Camera> camera)
{
	VertexShaderData vsData;
	vsData.colorTint = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vsData.worldMatrix = transform.GetWorldMatrix();
	vsData.viewMatrix = camera.get()->GetViewMatrix();
	vsData.projMatrix = camera.get()->GetProjectionMatrix();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	context->Map(constBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
	context->Unmap(constBuffer.Get(), 0);

	context->VSSetConstantBuffers(
		0, // Which slot (register) to bind the buffer to?
		1, // How many are we setting right now?
		constBuffer.GetAddressOf()); // Array of buffers (or address of just one)

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	// Set buffers in the input assembler (IA) stage
			//  - Do this ONCE PER OBJECT, since each object may have different geometry
			//  - For this demo, this step *could* simply be done once during Init()
			//  - However, this needs to be done between EACH DrawIndexed() call
			//     when drawing different geometry, so it's here as an example
	mesh.get()->GetVertexBuffer().Get();

	context->IASetVertexBuffers(0, 1, mesh.get()->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mesh.get()->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	// Tell Direct3D to draw
	//  - Begins the rendering pipeline on the GPU
	//  - Do this ONCE PER OBJECT you intend to draw
	//  - This will use all currently set Direct3D resources (shaders, buffers, etc)
	//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
	//     vertices in the currently set VERTEX BUFFER
	context->DrawIndexed(
		mesh.get()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
}