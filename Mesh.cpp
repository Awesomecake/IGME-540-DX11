#include "Mesh.h"
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <d3d11.h>
#include "Vertex.h"

Mesh::Mesh() 
{
	indexCount = 0;
};

Mesh::Mesh(Vertex vertices[], unsigned int vertexNum, unsigned int indices[], unsigned int indexNum, Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	indexCount = indexNum;

	// Create a VERTEX BUFFER
	// - This holds the vertex data of triangles for a single object
	// - This buffer is created on the GPU, which is where the data needs to
	//    be if we want the GPU to act on it (as in: draw it to the screen)
	{
		// First, we need to describe the buffer we want Direct3D to make on the GPU
		//  - Note that this variable is created on the stack since we only need it once
		//  - After the buffer is created, this description variable is unnecessary
		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_IMMUTABLE;			 // Will NEVER change
		vbd.ByteWidth = sizeof(Vertex) * vertexNum; // Number of vertices in the buffer
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;    // Tells Direct3D this is a vertex buffer
		vbd.CPUAccessFlags = 0;						 // Note: We cannot access the data from C++ (this is good)
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		// Create the proper struct to hold the initial vertex data
		// - This is how we initially fill the buffer with data
		// - Essentially, we're specifying a pointer to the data to copy
		D3D11_SUBRESOURCE_DATA initialVertexData = {};
		initialVertexData.pSysMem = vertices;

		// Actually create the buffer on the GPU with the initial data
		// - Once we do this, we'll NEVER CHANGE DATA IN THE BUFFER AGAIN
		device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());
	}

	// Create an INDEX BUFFER
	// - This holds indices to elements in the vertex buffer
	// - This is most useful when vertices are shared among neighboring triangles
	// - This buffer is created on the GPU, which is where the data needs to
	//    be if we want the GPU to act on it (as in: draw it to the screen)
	{
		// Describe the buffer, as we did above, with two major differences
		//  - Byte Width (3 unsigned integers vs. 3 whole vertices)
		//  - Bind Flag (used as an index buffer instead of a vertex buffer) 
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_IMMUTABLE;				   // Will NEVER change
		ibd.ByteWidth = sizeof(unsigned int) * indexCount; // Number of indices in the buffer
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;		   // Tells Direct3D this is an index buffer
		ibd.CPUAccessFlags = 0;							   // Note: We cannot access the data from C++ (this is good)
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		// Specify the initial data for this buffer, similar to above
		D3D11_SUBRESOURCE_DATA initialIndexData = {};
		initialIndexData.pSysMem = indices;

		// Actually create the buffer with the initial data
		// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
		device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
	}
};

Mesh::~Mesh() {};

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer() 
{
	return vertexBuffer;
};

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
	return indexBuffer;
};

unsigned int Mesh::GetIndexCount()
{
	return indexCount;
};