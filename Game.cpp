#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Mesh.h"
#include <string>
#include "WICTextureLoader.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// Direct3D itself, and our window, are not ready at this point!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,			// The application's handle
		L"DirectX Game",	// Text for the window's title bar (as a wide-character string)
		1280,				// Width of the window's client area
		720,				// Height of the window's client area
		false,				// Sync the framerate to the monitor refresh? (lock framerate)
		true)				// Show extra stats (fps) in title bar?
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Delete all objects manually created within this class
//  - Release() all Direct3D objects created within this class
// --------------------------------------------------------
Game::~Game()
{
	// Call delete or delete[] on any objects or arrays you've
	// created using new or new[] within this class
	// - Note: this is unnecessary if using smart pointers

	// Call Release() on any Direct3D objects made within this class
	// - Note: this is unnecessary for D3D objects stored in ComPtrs

	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// --------------------------------------------------------
// Called once per program, after Direct3D and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;		// Can make this a "Graphics Setting"
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures with Normal Maps/cobblestone.png").c_str(), nullptr, cobblestoneShaderResourceView.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures with Normal Maps/cobblestone_normals.png").c_str(), nullptr, cobblestone_normalsShaderResourceView.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures with Normal Maps/cushion.png").c_str(), nullptr, cushionShaderResourceView.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures with Normal Maps/cushion_normals.png").c_str(), nullptr, cushion_normalsShaderResourceView.GetAddressOf());

	mat1 = std::make_shared<Material>(XMFLOAT4(1, 1, 1,1), pixelShader, vertexShader,0.1f);
	mat1->textureSRVs.insert({ "SurfaceTexture", cobblestoneShaderResourceView});
	mat1->textureSRVs.insert({ "NormalMap", cobblestone_normalsShaderResourceView });
	mat1->samplers.insert({ "BasicSampler",samplerState });

	mat2 = std::make_shared<Material>(XMFLOAT4(1, 1, 1,1), pixelShader, vertexShader,0.1f);
	mat2->textureSRVs.insert({ "SurfaceTexture", cushionShaderResourceView });
	mat2->textureSRVs.insert({ "NormalMap", cushion_normalsShaderResourceView });
	mat2->samplers.insert({ "BasicSampler",samplerState });

	mat1->PrepareMaterial();
	mat2->PrepareMaterial();

	CreateGeometry();

	sky = std::make_shared<Sky>(cube, samplerState, device, context, FixPath(L"../../Assets/Skies/Planet/").c_str());

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device.Get(), context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	cameras = std::vector<std::shared_ptr<Camera>>();
	cameras.push_back(std::make_shared<Camera>((float)this->windowWidth / this->windowHeight, 45.f, XMFLOAT3(0.5, 0, -5)));
	cameras.push_back(std::make_shared<Camera>((float)this->windowWidth / this->windowHeight, 90.f, XMFLOAT3(-0.5, 0, -5)));
	cameras.push_back(std::make_shared<Camera>((float)this->windowWidth / this->windowHeight, 45.f, XMFLOAT3(0, 0.5, -5)));
	cameras.push_back(std::make_shared<Camera>((float)this->windowWidth / this->windowHeight, 90.f, XMFLOAT3(0, -0.5, -5)));

#pragma region Constructing Lights
	Light light = Light();
	light.Type = LIGHT_TYPE_POINT;
	light.Color = XMFLOAT3(1, 0, 0);
	light.Position = XMFLOAT3(0, 0, 0);
	light.Range = 10;
	light.Intensity = 2;

	lights = std::vector<Light>();
	lights.push_back(light);

	Light light2 = Light();
	light2.Type = LIGHT_TYPE_POINT;
	light2.Color = XMFLOAT3(0, 0, 1);
	light2.Position = XMFLOAT3(0, 0, 5);
	light2.Range = 10;
	light2.Intensity = 2;

	lights.push_back(light2);

	Light light3 = Light();
	light3.Direction = XMFLOAT3(0, 0, 1);
	light3.Color = XMFLOAT3(0, 0, 1);

	lights.push_back(light3);

	Light light4 = light3;
	light4.Direction = XMFLOAT3(-1, 0, 0);
	light4.Color = XMFLOAT3(1, 1, 0);

	lights.push_back(light4);

	Light light5 = light3;
	light5.Direction = XMFLOAT3(0, 0, -1);
	light5.Color = XMFLOAT3(0, 1, 1);

	lights.push_back(light5);
#pragma endregion
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	pixelShader = std::make_shared<SimplePixelShader>(device, context, FixPath(L"PixelShader.cso").c_str());
	pixelShader2 = std::make_shared<SimplePixelShader>(device, context, FixPath(L"PixelShader2.cso").c_str());
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, FixPath(L"VertexShader.cso").c_str());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateGeometry()
{
	cube = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/cube.igme540obj").c_str(), device);
	cylinder = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/cylinder.igme540obj").c_str(), device);
	helix = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/helix.igme540obj").c_str(), device);
	sphere = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/sphere.igme540obj").c_str(), device);
	torus = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/torus.igme540obj").c_str(), device);
	quad = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/quad.igme540obj").c_str(), device);

	gameEntities.push_back(GameEntity(cube, mat1));
	gameEntities.push_back(GameEntity(cylinder, mat1));
	gameEntities.push_back(GameEntity(helix, mat1));
	gameEntities.push_back(GameEntity(sphere,mat2));
	gameEntities.push_back(GameEntity(torus, mat2));
	gameEntities.push_back(GameEntity(quad, mat2));

	gameEntities[0].GetTransform().SetPosition(-9, -3, 0);
	gameEntities[1].GetTransform().SetPosition(-6, -3, 0);
	gameEntities[2].GetTransform().SetPosition(-3, -3, 0);
	gameEntities[3].GetTransform().SetPosition(-0, -3, 0);
	gameEntities[4].GetTransform().SetPosition(3, -3, 0);
	gameEntities[5].GetTransform().SetPosition(6, -3, 0);
}


// --------------------------------------------------------
// Handle resizing to match the new window size.
//  - DXCore needs to resize the back buffer
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	for (size_t i = 0; i < cameras.size(); i++)
	{
		cameras[i].get()->UpdateProjectionMatrix((float)this->windowWidth / this->windowHeight);
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

	cameras[selectedCamera].get()->Update(deltaTime);
	ImGuiUpdate(deltaTime, totalTime);
	BuildUI(deltaTime, totalTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erases what's on the screen)
		float bgColor[4] = { ambientColor.x, ambientColor.y, ambientColor.z, 1};
		context->ClearRenderTargetView(backBufferRTV.Get(), bgColor);

		// Clear the depth buffer (resets per-pixel occlusion information)
		context->ClearDepthStencilView(depthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	for(GameEntity entity : gameEntities)
	{
		entity.GetMaterial()->pixelShader->SetFloat3("ambient", ambientColor);
		entity.GetMaterial()->pixelShader->SetFloat("totalTime", totalTime);
		entity.Draw(context, cameras[selectedCamera],lights);
	}

	sky->ambient = ambientColor;
	sky->Draw(context,cameras[selectedCamera]);

	ImGui::Render(); // Turns this frame’s UI into renderable triangles
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present the back buffer to the user
		//  - Puts the results of what we've drawn onto the window
		//  - Without this, the user never sees anything
		bool vsyncNecessary = vsync || !deviceSupportsTearing || isFullscreen;
		swapChain->Present(vsyncNecessary ? 1 : 0, vsyncNecessary ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Must re-bind buffers after presenting, as they become unbound
		context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthBufferDSV.Get());
	}
}

#pragma region ImGui
void Game::ImGuiUpdate(float deltaTime, float totalTime)
{
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)this->windowWidth;
	io.DisplaySize.y = (float)this->windowHeight;
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input& input = Input::GetInstance();
	input.SetKeyboardCapture(io.WantCaptureKeyboard);
	input.SetMouseCapture(io.WantCaptureMouse);
	// Show the demo window

	if(showImGuiDemoWindow)
		ImGui::ShowDemoWindow();
}

void Game::BuildUI(float deltaTime, float totalTime)
{
	ImVec4 detailsColor = ImVec4(0.7, 0.7, 1, 1);

	ImGui::Begin("Inspector"); // Everything after is part of the window

	if (ImGui::TreeNode("App Details"))
	{
		ImGui::TextColored(detailsColor, " - Framerate: %f fps", ImGui::GetIO().Framerate);
		ImGui::TextColored(detailsColor, " - Window Resolution: %dx%d", windowWidth, windowHeight);
		ImGui::ColorEdit3("Ambient Color", &ambientColor.x);

		// Create a button and test for a click
		if (!showImGuiDemoWindow)
		{
			if (ImGui::Button("Show ImGui Demo Window"))
				showImGuiDemoWindow = true;
		}
		else
		{
			if (ImGui::Button("Hide ImGui Demo Window"))
				showImGuiDemoWindow = false;
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Meshes"))
	{
		ImGui::TextColored(detailsColor, " - Mesh 0: %u triangle(s)", cube.get()->GetIndexCount()/3);
		ImGui::TextColored(detailsColor, " - Mesh 1: %u triangle(s)", cylinder.get()->GetIndexCount()/3);
		ImGui::TextColored(detailsColor, " - Mesh 2: %u triangle(s)", helix.get()->GetIndexCount()/3);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Scene Entities"))
	{
		for (int i = 0; i < gameEntities.size(); i++)
		{
			std::string string = "Entity " + std::to_string(i);
			if (ImGui::TreeNode(string.data()))
			{
				XMFLOAT3 position = gameEntities[i].GetTransform().GetPosition();
				ImGui::DragFloat3("Position", &position.x, 0.005f, -5.0f, 5.0f, "%.3f");
				gameEntities[i].GetTransform().SetPosition(position);

				XMFLOAT3 rotation = gameEntities[i].GetTransform().GetPitchYawRoll();
				ImGui::DragFloat3("Rotation (Radians)", &rotation.x, 0.005f, -5.0f, 5.0f, "%.3f");
				gameEntities[i].GetTransform().SetRotation(rotation);

				XMFLOAT3 scale = gameEntities[i].GetTransform().GetScale();
				ImGui::DragFloat3("Scale", &scale.x, 0.005f, -5.0f, 5.0f, "%.3f");
				gameEntities[i].GetTransform().SetScale(scale);

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Current Camera"))
	{
		std::string string = "Camera #" + std::to_string(selectedCamera);
		ImGui::SeparatorText(string.c_str());

		ImGui::TextColored(detailsColor, "Camera Details:");
		{
			XMFLOAT3 position = cameras[selectedCamera].get()->GetTransform().GetPosition();
			ImGui::TextColored(detailsColor, "	Position: %0.2f, %0.2f, %0.2f", position.x, position.y, position.z);

			XMFLOAT3 rotation = cameras[selectedCamera].get()->GetTransform().GetPitchYawRoll();
			ImGui::TextColored(detailsColor, "	Rotation (Radians): %0.2f, %0.2f, %0.2f", rotation.x, rotation.y, rotation.z);

			float fov = cameras[selectedCamera].get()->GetFOV();
			ImGui::TextColored(detailsColor, "	FOV: %0.2f", fov);
		}

		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Change To Previous Camera"))
		{
			selectedCamera = (selectedCamera - 1) % cameras.size();
		}
		ImGui::SameLine();

		if (ImGui::Button("Change To Next Camera"))
		{
			selectedCamera = (selectedCamera + 1) % cameras.size();
		}


		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Lights"))
	{
		for (int i = 0; i < lights.size(); i++)
		{
			std::string string = "Light #" + std::to_string(i+1) + " - ";

			switch (lights[i].Type)
			{
			case LIGHT_TYPE_DIRECTIONAL:
				string += "Directional";
				break;
			case LIGHT_TYPE_POINT:
				string += "Point";
				break;
			case LIGHT_TYPE_SPOT:
				string += "Spot";
				break;
			}

			if (ImGui::TreeNode(string.data()))
			{
				XMFLOAT3 color = lights[i].Color;
				ImGui::DragFloat3("Color", &color.x, 0.005f, 0.f, 1.0f, "%.01f");
				lights[i].Color = color;

				float intensity = lights[i].Intensity;
				ImGui::DragFloat("Intensity", &intensity, 0.005f, 0.0f, 50.0f, "%.3f");
				lights[i].Intensity = intensity;

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Textures"))
	{
		if (!toggleMat)
		{
			if (ImGui::Button("Use Cushion Texture"))
			{
				toggleMat = true;
				for (int i = 0; i < gameEntities.size(); i++)
				{
					gameEntities[i].SetMaterial(mat2);
				}
			}
		}
		else
		{
			if (ImGui::Button("Use Cobblestone Texture"))
			{
				toggleMat = false;
				for (int i = 0; i < gameEntities.size(); i++)
				{
					gameEntities[i].SetMaterial(mat1);
				}
			}
		}

		ImGui::TreePop();
	}

	ImGui::End(); // Ends the current window
}
#pragma endregion