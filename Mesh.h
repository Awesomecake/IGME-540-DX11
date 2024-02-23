#pragma once
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <d3d11.h>
#include "Vertex.h"

class Mesh
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	unsigned int indexCount;

	void CreateBuffers(Vertex* vertices, int vertexNum, unsigned int* indices, Microsoft::WRL::ComPtr<ID3D11Device> device);
public:
	Mesh();
	Mesh(const wchar_t* filename, Microsoft::WRL::ComPtr<ID3D11Device> device);
	Mesh(Vertex vertices[], unsigned int vertexNum, unsigned int indices[], unsigned int indexNum, Microsoft::WRL::ComPtr<ID3D11Device> device);
	~Mesh();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	unsigned int GetIndexCount();
};

