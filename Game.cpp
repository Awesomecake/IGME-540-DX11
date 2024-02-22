#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Mesh.h"
#include "BufferStructs.h"
#include <string>

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
	CreateGeometry();

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
	cameras.push_back(std::make_shared<Camera>((float)this->windowWidth / this->windowHeight, 45, XMFLOAT3(0.5, 0, -1)));
	cameras.push_back(std::make_shared<Camera>((float)this->windowWidth / this->windowHeight, 90, XMFLOAT3(-0.5, 0, -1)));
	cameras.push_back(std::make_shared<Camera>((float)this->windowWidth / this->windowHeight, 45, XMFLOAT3(0, 0.5, -1)));
	cameras.push_back(std::make_shared<Camera>((float)this->windowWidth / this->windowHeight, 90, XMFLOAT3(0, -0.5, -1)));

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
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, FixPath(L"VertexShader.cso").c_str());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red	= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green	= XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue	= XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in CPU memory
	//    over to a Direct3D-controlled data structure on the GPU (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself
	Vertex triVertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.2f, +0.0f), red },
		{ XMFLOAT3(+0.2f, -0.2f, +0.0f), blue },
		{ XMFLOAT3(-0.2f, -0.2f, +0.0f), green },
		{ XMFLOAT3(1.f, 1.f, +0.0f), red }
	};

	// Set up indices, which tell us which vertices to use and in which order
	// - This is redundant for just 3 vertices, but will be more useful later
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int triIndices[] = {0, 1, 2};

	triangle = std::make_shared<Mesh>(triVertices,4, triIndices, 3, device);

	Vertex trapVertices[] =
	{
		{ XMFLOAT3(0.1f, +0.15f, +0.0f), green },
		{ XMFLOAT3(0.2f, -0.15f, +0.0f), blue },
		{ XMFLOAT3(-0.2f, -0.15f, +0.0f), blue },
		{ XMFLOAT3(-0.1f, 0.15f, +0.0f), green }
	};

	unsigned int trapIndices[] = { 0, 1, 2, 0, 2, 3};
	trapezoid = std::make_shared<Mesh>(trapVertices, 4, trapIndices,6, device);

	Vertex shapeVertices[] =
	{
		{ XMFLOAT3(-0.0f, -0.0f, +0.0f), blue },
		{ XMFLOAT3(-0.2f, -0.0f, +0.0f), blue },

		{ XMFLOAT3(-0.1f, -0.1f, +0.0f), red },
		{ XMFLOAT3(-0.1f, 0.1f, +0.0f), red },

		{ XMFLOAT3(0.2f, -0.05f, +0.0f), red },
		{ XMFLOAT3(0.2f, 0.05f, +0.0f), red }
	};

	unsigned int shapeIndices[] = {1,0,2,0,1,3,0,3,5,2,0,4};
	complexShape = std::make_shared<Mesh>(shapeVertices, 6, shapeIndices, 12, device);

	gameEntities.push_back(GameEntity(triangle));
	gameEntities.push_back(GameEntity(trapezoid));
	gameEntities.push_back(GameEntity(complexShape));
	gameEntities.push_back(GameEntity(complexShape));
	gameEntities.push_back(GameEntity(triangle));
	gameEntities.push_back(GameEntity(trapezoid));

	gameEntities[0].GetTransform().SetPosition(-0.5, -0.5, 0);
	gameEntities[1].GetTransform().SetPosition(0.5, 0.5, 0);
	gameEntities[5].GetTransform().SetPosition(-0.5, 0.5, 0);

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

	//Scaling Entity 0
	{
		XMFLOAT3 scale = gameEntities[0].GetTransform().GetScale();
		scale.x = abs(1.5 * cos(totalTime));
		scale.y = abs(1.5 * sin(totalTime));
		gameEntities[0].GetTransform().SetScale(scale);
	}

	//Rotating Entity 1
	{
		XMFLOAT3 rot = gameEntities[1].GetTransform().GetPitchYawRoll();
		rot.z = totalTime;
		gameEntities[1].GetTransform().SetRotation(rot);
	}

	//Moving Entity 2
	{
		XMFLOAT3 pos = gameEntities[2].GetTransform().GetPosition();
		pos.x = 0.8 * cos(totalTime);
		pos.y = 0.8 * sin(totalTime);
		gameEntities[2].GetTransform().SetPosition(pos);
	}


	//Moving & rotating Entity 3
	{
		XMFLOAT3 pos = gameEntities[3].GetTransform().GetPosition();
		pos.x = -0.8 * cos(totalTime);
		gameEntities[3].GetTransform().SetPosition(pos);

		XMFLOAT3 rot = gameEntities[3].GetTransform().GetPitchYawRoll();
		rot.z = totalTime;
		gameEntities[3].GetTransform().SetRotation(rot);
	}

	//Moving & rotating Entity 4
	{
		XMFLOAT3 pos = gameEntities[4].GetTransform().GetPosition();
		pos.y = -0.8 * sin(totalTime);
		gameEntities[4].GetTransform().SetPosition(pos);

		XMFLOAT3 rot = gameEntities[4].GetTransform().GetPitchYawRoll();
		rot.z = totalTime;
		gameEntities[4].GetTransform().SetRotation(rot);
	}

	if (Input::GetInstance().KeyDown('1'))
	{
		selectedCamera = 0;
	}
	if (Input::GetInstance().KeyDown('2'))
	{
		selectedCamera = 1;
	}


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
		float bgColor[4] = { backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w };
		context->ClearRenderTargetView(backBufferRTV.Get(), bgColor);

		// Clear the depth buffer (resets per-pixel occlusion information)
		context->ClearDepthStencilView(depthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	for(GameEntity entity : gameEntities)
	{
		entity.Draw(context, pixelShader,vertexShader,cameras[selectedCamera]);
	}


	{
		ImGui::Render(); // Turns this frame’s UI into renderable triangles
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen
	}

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present the back buffer to the user
		//  - Puts the results of what we've drawn onto the window
		//  - Without this, the user never sees anything
		bool vsyncNecessary = vsync || !deviceSupportsTearing || isFullscreen;
		swapChain->Present(
			vsyncNecessary ? 1 : 0,
			vsyncNecessary ? 0 : DXGI_PRESENT_ALLOW_TEARING);

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
	ImGui::Begin("Inspector"); // Everything after is part of the window
	if (ImGui::TreeNode("App Details"))
	{
		ImGui::Text("Framerate: %f fps", ImGui::GetIO().Framerate);
		ImGui::Text("Window Resolution: %dx%d", windowWidth, windowHeight);

		// Can create a 3 or 4-component color editors, too!
		ImGui::ColorEdit4("RGBA color editor", &backgroundColor.x);


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

		float arr[] = { backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w };
		ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));

		if (!randomizeColorOffset)
		{
			if (ImGui::Button("Randomly change color"))
				randomizeColorOffset = true;
		}
		else
		{
			switch (rand() % 4)
			{
			case 0:
				backgroundColor.x += deltaTime * (rand() % 10 - 4.5) / 2.f;
				if (backgroundColor.x > 1) backgroundColor.x = 1;
				if (backgroundColor.x < 0) backgroundColor.x = 0;
				break;
			case 1:
				backgroundColor.y += deltaTime * (rand() % 10 - 4.5) / 2.f;
				if (backgroundColor.y > 1) backgroundColor.y = 1;
				if (backgroundColor.y < 0) backgroundColor.y = 0;
				break;
			case 2:
				backgroundColor.z += deltaTime * (rand() % 10 - 4.5) / 2.f;
				if (backgroundColor.z > 1) backgroundColor.z = 1;
				if (backgroundColor.z < 0) backgroundColor.z = 0;
				break;
			case 3:
				backgroundColor.w += deltaTime * (rand() % 10 - 4.5) / 2.f;
				if (backgroundColor.w > 1) backgroundColor.w = 1;
				if (backgroundColor.w < 0) backgroundColor.w = 0;
				break;
			}

			if (ImGui::Button("Turn off random color change"))
				randomizeColorOffset = false;
		}

		ImGui::TreePop();
		ImGui::Spacing();
	}

	if (ImGui::TreeNode("Meshes"))
	{
		ImGui::Text("Mesh 0: %u triangle(s)", triangle.get()->GetIndexCount()/3);
		ImGui::Text("Mesh 1: %u triangle(s)", trapezoid.get()->GetIndexCount()/3);
		ImGui::Text("Mesh 2: %u triangle(s)", complexShape.get()->GetIndexCount()/3);

		ImGui::TreePop();
		ImGui::Spacing();
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

	if (ImGui::TreeNode("Cameras"))
	{
		std::string string = "Camera #" + std::to_string(selectedCamera);
		ImGui::SeparatorText(string.c_str());

		XMFLOAT3 position = cameras[selectedCamera].get()->GetTransform().GetPosition();
		ImGui::DragFloat3("Position", &position.x, 0.005f, -100.0f, 100.0f, "%.3f");

		XMFLOAT3 rotation = cameras[selectedCamera].get()->GetTransform().GetPitchYawRoll();
		ImGui::DragFloat3("Rotation (Radians)", &rotation.x, 0.005f, -5.0f, 5.0f, "%.3f");

		float fov = cameras[selectedCamera].get()->GetFOV();
		ImGui::DragFloat("FOV", &fov, 0.005f, 45.0f, 90.0f, "%.3f");


		if (ImGui::Button("Change To Previous Camera"))
		{
			selectedCamera = (selectedCamera - 1) % cameras.size();
		}

		if (ImGui::Button("Change To Next Camera"))
		{
			selectedCamera = (selectedCamera + 1) % cameras.size();
		}


		ImGui::TreePop();
	}

	ImGui::End(); // Ends the current window
}
#pragma endregion