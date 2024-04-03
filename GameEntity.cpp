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

	mesh->Draw(context);
}