
#include <core/OTTSystem.hpp> // ott::getAssetsPath
#include <graphics/3d/OTTCamera.hpp>
#include <graphics/3d/OTTLightSource.hpp>
#include <graphics/3d/OTTPlayer.hpp>
#include <graphics/3d/OTTScene.hpp>
#include <graphics/3d/OTTWindow3d.hpp>
#include <graphics/core/OTTShader.hpp>
#include <graphics/texture/OTTSprite.hpp>
#include <graphics/geometry/OTTModelObj.hpp>
#include <graphics/geometry/OTTPrimitives.hpp>
#include <graphics/texture/OTTTexture.hpp>
#include <graphics/texture/OTTTextureManager.hpp>
#include <input/OTTJoypad.hpp>
#include <input/OTTKeyboard.hpp>
#include <input/OTTMouse.hpp>
#include <math/OTTConstants.hpp>

#include <math/OTTConstants.hpp>
#include <math/OTTVector.hpp>

#include <iostream>
#include <string>
#include <cmath>
#include <vector>

constexpr uint8_t KEYBOARD_W = 0x77;
constexpr uint8_t KEYBOARD_A = 0x61;
constexpr uint8_t KEYBOARD_S = 0x73;
constexpr uint8_t KEYBOARD_D = 0x64;

constexpr uint8_t KEYBOARD_Q = 0x71;
constexpr uint8_t KEYBOARD_E = 0x65;

constexpr uint8_t KEYBOARD_X = 0x78;
constexpr uint8_t KEYBOARD_Z = 0x7A;

constexpr uint8_t KEYBOARD_SPACE = 0x20;

namespace ott {

// Shader data
struct ShaderData_t {
	Vector3 lightPos;
	Vector3 lightDir;
	Vector3 lightColor;

	float lightIntensity;
	float lightCosAngle;
	float lightDropoff;

	Camera* camptr;
};

// Shader enable function
void TestShaderEnable(Shader* shdr, const Texture* const tex, const void* const data) {

	// Get shader data
	const ShaderData_t* ptr = reinterpret_cast<const ShaderData_t*>(data);

	// Bind object texture (if available)
	if (tex) {
		glBindTexture(GL_TEXTURE_2D, tex->Context());
	}

	// Lights
	shdr->SetVector3("lightPos", ptr->lightPos);
	shdr->SetVector3("lightDir", ptr->lightDir);
	shdr->SetVector3("lightColor", ptr->lightColor);	

	shdr->SetFloat("lightIntensity", ptr->lightIntensity);
	shdr->SetFloat("lightCosAngle", ptr->lightCosAngle);
	shdr->SetFloat("lightDropoff", ptr->lightDropoff);

	// Camera
	shdr->SetVector3("camPos", ptr->camptr->Position());
	shdr->SetVector3("camDir", ptr->camptr->Direction());
}

} /* namespace ott */

using namespace ott;

int main(){
	// Define a room object
	Object room;

	std::cout << " TEST: " << ASSETS_DIRECTORY << std::endl;
	
	// Define a new primitive
	primitives::Cube myShape(Vector3(0.f, 2.5f, 0.f), 1.f, 1.f, 1.f);
	//primitives::Circle myShape(Vector3(), 1, 12);
	//primitives::Cylinder myShape(Vector3(), 1, 1, 12);
	//primitives::Cone myShape(Vector3(), 1, 1, 12);
	//ModelStl myShape(getAssetsPath("Models/Spill_Rat.stl"));

	// Build the floor
	primitives::Plane floor(Vector3(), 20.f, 20.f);
	floor.SetRotation(-constants::kPi / 2, 0, 0);

	// Build the walls
	primitives::Plane walls[4] = {
		primitives::Plane(Vector3(10.f, 2.5f, 0.f), 20.f, 5.f),  // Rotated by 270
		primitives::Plane(Vector3(-10.f, 2.5f, 0.f), 20.f, 5.f), // Rotated by 90
		primitives::Plane(Vector3(0.f, 2.5f, 10.f), 20.f, 5.f),  // Rotated by 180
		primitives::Plane(Vector3(0.f, 2.5f, -10.f), 20.f, 5.f)  // Not rotated
	};

	walls[0].SetRotation(0.f, 3 * constants::kPi / 2, 0.f);
	walls[1].SetRotation(0.f, constants::kPi / 2, 0.f);
	walls[2].SetRotation(0.f, constants::kPi, 0.f);

	primitives::Cube boxes[4] = {
		primitives::Cube(Vector3(-5.f, 1.5f, 5.f), 3.f, 3.f, 3.f),
		primitives::Cube(Vector3(-5.f, 1.5f, -5.f), 3.f, 3.f, 3.f),
		primitives::Cube(Vector3(5.f, 1.5f, 5.f), 3.f, 3.f, 3.f),
		primitives::Cube(Vector3(5.f, 1.5f, -5.f), 3.f, 3.f, 3.f)
	};

	// Add all room components to the room object this will allow us to move and rotate the room as a whole.
	// Child objects will retain their relative positions and rotations within the world
	room.AddChild(&floor);
	for (int32_t i = 0; i < 4; i++) {
		room.AddChild(&walls[i]);
	}
	for (int32_t i = 0; i < 4; i++) {
		room.AddChild(&boxes[i]);
	}

	// Setup the camera at z=-1.5 m (facing the object)
	Camera cam(Vector3(0, 0, -1.5f), Vector3(0, 0, 1.f));
	cam.Dump();
	
	// Setup the scene with our camera
	Scene myScene(&cam);
	Window3d* window = myScene.GetWindow();

	// Set the color of the world light
	myScene.GetWorldLight()->Disable();

	// Enable VSync (framerate controlled by hardware)
	myScene.EnableVSync();

	// Build shader program
	ShaderData_t data;
	Shader shader(AssetsPath("Shaders/test.vert"), AssetsPath("Shaders/test.frag"));
	shader.SetShaderEnableFunction(TestShaderEnable);
	shader.SetShaderDisableFunction(Shader::DefaultShaderDisable);
	shader.SetShaderData(&data);

	// Set object shaders
	myShape.SetShader(&shader);
	room.SetShader(&shader, true);

	// Set the camera to draw surface normal vectors
	myScene.SetDrawOrigin();
	
	// Add the primitives to the scene
	myScene.AddObject(&myShape);
	myScene.AddObject(&room);

	// Player object
	Player player;
	player.SetEyes(&cam);
	player.SetEyeOffset(Vector3(0, 1.9f, 0));
	player.SetMass(100.f);
	player.ApplyForce(Vector3(0.f, -981.f, 0.f)); // Gravity: Fg = (g * 100 kg)

	// Load textures
	TextureManager* textures = &TextureManager::Instance();
	textures->Load(AssetsPath("Textures/floor.jpg"), "floor");
	textures->Load(AssetsPath("Textures/brick.jpg"), "brick");
	textures->Load(AssetsPath("Textures/crate.jpg"), "crate");
	textures->Load(AssetsPath("Textures/dumbDice.png"), "dice");
	textures->Load(AssetsPath("Textures/cursor.png"), "cursor");

	// Set object textures (be sure to add the objects to the scene first)
	textures->SetTexture(&floor, "floor");
	textures->SetTexture(&myShape, "dice");
	for (int32_t i = 0; i < 4; i++) {
		textures->SetTexture(&walls[i], "brick");
		textures->SetTexture(&boxes[i], "crate");
	}

	WrappedValue pitch(0, -constants::kPi, constants::kPi);
	WrappedValue yaw(0, -constants::kPi, constants::kPi);
	WrappedValue theta(0, 0, constants::kPi * 2);

	// Set the camera movement speed multiplier
	const float cameraMoveRate = 3.f;

	// Input device sensitivity
	const float controllerSensitivity = 0.1f;
	const float mouseSensitivity = 0.01f;

	// Set object's scale
	myShape.SetScaleFactor(0.75f);

	Keyboard* keys = window->GetKeyboard();
	Mouse* mouse = window->GetMouse();
	Joypad* controller = window->GetJoypad();

	window->EnableMouse(); // Enable mouse support
	window->EnableKeyboard(); // Enable keyboard support
	window->EnableGamepad(); // Enable controller support

	// Lock cursor inside window and hide it
	mouse->SetCursorState(MouseStates::Disabled); 

	// Set shader information
	data.camptr = &cam;
	data.lightColor = Vector3(1.f, 0.75f, 0.5f);
	data.lightDir   = Vector3(0.f, 0.f, 1.f);
	data.lightPos   = Vector3(0.f, 2.5f, 0.f);
	data.lightIntensity = 100.f;
	data.lightCosAngle = std::cos(30.f * constants::kDeg2rad);
	data.lightDropoff = 0.5f;

	// Sprite test
	Sprite cursor(textures->FindByName("cursor")->Context());
	cursor.SetPosition(0, 0);
	cursor.SetScale(0.25f);

	// Build sprite shader
	Shader cursor_shader(AssetsPath("Shaders/sprite.vert"), AssetsPath("Shaders/sprite.frag"));
	cursor.SetShader(&cursor_shader, true);

	// "Animate" the object by rotating it and moving the camera
	int32_t count = 0;
	double dX = 0;
	double dY = 0;
	float fStickX = 0.f;
	float fStickY = 0.f;
	float timeElapsed = 0.f;
	float totalTimeElapsed = 0.f;
	bool isDone = false;
	Matrix3 lightRotation;
	lightRotation.SetRotation(0, 0, 0.08f);
	while(!isDone && myScene.DrawOpenGL()){
		// Check for keypresses 
		if(!keys->Empty()){			
			// Pressed keys (toggles)
			if(keys->Poll('p'))
				cam.ResetOrientation();
			if (keys->Poll('f')){ // Flashlight
				if (data.lightIntensity > 0.f) {
					data.lightIntensity = 0.f;
				}
				else {
					data.lightIntensity = 100.f;
				}
			}

			// Held keys
			// Movement in the horizontal plane
			if (keys->Check(KEYBOARD_W)) // Move the camera forward
				player.Move(cameraMoveRate * timeElapsed * (keys->LeftShift() ? 2.f : 1.f));
			if (keys->Check(KEYBOARD_A)) // Move the camera left
				player.Strafe(cameraMoveRate * timeElapsed * (keys->LeftShift() ? 2.f : 1.f));
			if (keys->Check(KEYBOARD_S)) // Move the camera backwards
				player.Move(-cameraMoveRate * timeElapsed * (keys->LeftShift() ? 2.f : 1.f));
			if (keys->Check(KEYBOARD_D)) // Move the camera right
				player.Strafe(-cameraMoveRate * timeElapsed * (keys->LeftShift() ? 2.f : 1.f));
			if (keys->Poll(KEYBOARD_SPACE)) // Jump
				player.ApplyImpulse(Vector3(0.f, 500.f, 0.f));
		}

		// Update controller (if connected)
		if (!controller->Empty()){
			if (controller->Check(GamepadInput::RStick)) { // Free-look camera
				controller->RightStickPosition(fStickX, fStickY);
				player.Look(fStickY * controllerSensitivity, fStickX * controllerSensitivity);
			}
			if (controller->Check(GamepadInput::LStick)){ // Camera movement
				controller->LeftStickPosition(fStickX, fStickY);
				player.Move(-fStickY * controllerSensitivity, -fStickX * controllerSensitivity); // Both axes inverted
			}
			if (controller->Poll(GamepadInput::A)) { // Jump
				player.ApplyImpulse(Vector3(0.f, 500.f, 0.f));
			}
		}

		// Check mouse movement
		if(mouse->Delta(dX, dY)){
			player.Look((float)(dY * mouseSensitivity), (float)(dX * mouseSensitivity));
		}
		
		//if (count++ % 120 == 0) { // Frame count (every 2 seconds by default)
		//	std::cout << window->Framerate() << " fps\r" << std::flush;
		//}
			
		// Move the object along the x-axis
		theta += mouseSensitivity * 2.f;
		//myShape.setPosition(Vector3(1.f * std::sin(theta.get()), 1.f * std::cos(theta.get()), 2.5f));
		
		// Rotate the object
		myShape.SetRotation(-constants::kPi / 2, 0, theta.Get() + constants::kPi / 2); // Absolute rotation

		// Lock dynamic light source to the camera
		data.lightPos = cam.Position();
		data.lightDir = cam.Direction();
		data.lightPos += data.lightDir * 0.25;

		// Update player kinematics
		player.Update(timeElapsed);

		// Draw cursor
		cursor.Draw();
		//window->DrawTextureNDC(4, Vector2(0, 0), Vector2(0.5f, 0.5f), -1);

		// Render the scene
		window->Render();

		// Cap the framerate
		timeElapsed = (float)myScene.Sync();
		totalTimeElapsed += timeElapsed;
	}
	
	return 0;
}
