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

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<SimplePixelShader> pixelShader, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<Camera> camera)
{
	vertexShader->SetShader();
	pixelShader->SetShader();

	vertexShader->SetFloat4("colorTint", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	vertexShader->SetMatrix4x4("world", transform.GetWorldMatrix());
	vertexShader->SetMatrix4x4("view", camera.get()->GetViewMatrix());
	vertexShader->SetMatrix4x4("projection", camera.get()->GetProjectionMatrix());

	vertexShader->CopyAllBufferData();

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