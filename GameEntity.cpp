#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> refMesh, std::shared_ptr<Material> _material)
{
	mesh = refMesh;
	material = _material;
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

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return material;
}

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera, std::vector<Light> lights)
{
	DirectX::XMFLOAT2 mousePos = DirectX::XMFLOAT2((float)Input::GetInstance().GetMouseX(), (float)Input::GetInstance().GetMouseY());
	material->PrepareMaterial();

	//Set Shaders and Load Data
	{
		material->pixelShader->SetFloat4("surfaceColor", material->surfaceColor);
		
		material->pixelShader->SetFloat2("mousePos", mousePos);

		material->pixelShader->SetFloat("roughness", material->roughness);
		material->pixelShader->SetFloat3("cameraPos", camera->GetTransform().GetPosition());

		material->pixelShader->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());

		material->pixelShader->CopyAllBufferData();
	}

	{
		material->vertexShader->SetMatrix4x4("world", transform.GetWorldMatrix());
		material->vertexShader->SetMatrix4x4("view", camera->GetViewMatrix());
		material->vertexShader->SetMatrix4x4("projection", camera->GetProjectionMatrix());
		material->vertexShader->SetMatrix4x4("worldInvTranspose", transform.GetWorldInverseTransposeMatrix());

		material->vertexShader->CopyAllBufferData();
	}


	//Load Buffers

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

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