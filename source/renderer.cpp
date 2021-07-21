#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#include "OTTSystem.hpp" // ott::getAssetsPath
#include "OTTWindow3D.hpp"
#include "OTTTexture.hpp"
#include "OTTTextureManager.hpp"
#include "OTTShader.hpp"
#include "OTTJoypad.hpp"
#include "OTTPlayer.hpp"

#include "Constants.hpp"
#include "Graphics.hpp"
#include "triangle.hpp"
#include "camera.hpp"
#include "Primitives.hpp"
#include "ModelStl.hpp"
#include "ColorRGB.hpp"
#include "scene.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"

constexpr unsigned char KEYBOARD_W = 0x77;
constexpr unsigned char KEYBOARD_A = 0x61;
constexpr unsigned char KEYBOARD_S = 0x73;
constexpr unsigned char KEYBOARD_D = 0x64;

constexpr unsigned char KEYBOARD_Q = 0x71;
constexpr unsigned char KEYBOARD_E = 0x65;

constexpr unsigned char KEYBOARD_X = 0x78;
constexpr unsigned char KEYBOARD_Z = 0x7A;

constexpr unsigned char KEYBOARD_SPACE = 0x20;

// Shader data
struct ShaderData {
	Vector3 lightPos;
	Vector3 lightDir;
	Vector3 lightColor;

	float lightIntensity;
	float lightCosAngle;
	float lightDropoff;

	camera* camptr;
};

// Shader enable function
void testShaderEnable(OTTShader* shdr, const object* obj) {
	// Bind object texture (if available)
	if (obj->getTexture())
		glBindTexture(GL_TEXTURE_2D, obj->getTexture()->getContext());

	// Get shader data
	ShaderData* ptr = static_cast<ShaderData*>(shdr->getShaderDataPointer());

	// Lights
	shdr->setVector3("lightPos", ptr->lightPos);
	shdr->setVector3("lightDir", ptr->lightDir);
	shdr->setVector3("lightColor", ptr->lightColor);	

	shdr->setFloat("lightIntensity", ptr->lightIntensity);
	shdr->setFloat("lightCosAngle", ptr->lightCosAngle);
	shdr->setFloat("lightDropoff", ptr->lightDropoff);

	// Camera
	shdr->setVector3("camPos", ptr->camptr->getPosition());
	shdr->setVector3("camDir", ptr->camptr->getDirection());
}

int main(){
	// Define a room object
	object room;
	
	// Define a new primitive
	Primitives::Cube myShape(Vector3(0.f, 2.5f, 0.f), 1.f, 1.f, 1.f);
	//Primitives::Circle myShape(Vector3(), 1, 12);
	//Primitives::Cylinder myShape(Vector3(), 1, 1, 12);
	//Primitives::Cone myShape(Vector3(), 1, 1, 12);
	//ModelStl myShape(ott::getAssetsPath("Models/Spill_Rat.stl"));

	// Build the floor
	Primitives::Plane floor(Vector3(), 20.f, 20.f);
	floor.setRotation(-ott::pi / 2, 0, 0);

	// Build the walls
	Primitives::Plane walls[4] = {
		Primitives::Plane(Vector3(10.f, 2.5f, 0.f), 20.f, 5.f),  // Rotated by 270
		Primitives::Plane(Vector3(-10.f, 2.5f, 0.f), 20.f, 5.f), // Rotated by 90
		Primitives::Plane(Vector3(0.f, 2.5f, 10.f), 20.f, 5.f),  // Rotated by 180
		Primitives::Plane(Vector3(0.f, 2.5f, -10.f), 20.f, 5.f)  // Not rotated
	};

	walls[0].setRotation(0.f, 3 * ott::pi / 2, 0.f);
	walls[1].setRotation(0.f, ott::pi / 2, 0.f);
	walls[2].setRotation(0.f, ott::pi, 0.f);

	Primitives::Cube boxes[4] = {
		Primitives::Cube(Vector3(-5.f, 1.5f, 5.f), 3.f, 3.f, 3.f),
		Primitives::Cube(Vector3(-5.f, 1.5f, -5.f), 3.f, 3.f, 3.f),
		Primitives::Cube(Vector3(5.f, 1.5f, 5.f), 3.f, 3.f, 3.f),
		Primitives::Cube(Vector3(5.f, 1.5f, -5.f), 3.f, 3.f, 3.f)
	};

	// Add all room components to the room object this will allow us to move and rotate the room as a whole.
	// Child objects will retain their relative positions and rotations within the world
	room.addChild(&floor);
	for (int i = 0; i < 4; i++) {
		room.addChild(&walls[i]);
	}
	for (int i = 0; i < 4; i++) {
		room.addChild(&boxes[i]);
	}

	// Setup the camera at z=-1.5 m (facing the object)
	camera cam(Vector3(0, 0, -1.5f), Vector3(0, 0, 1.f));
	cam.dump();
	
	// Setup the scene with our camera
	scene myScene(&cam);
	OTTWindow3D* window = myScene.getWindow();

	// Set the color of the world light
	myScene.getWorldLight()->disable();

	// Enable VSync (framerate controlled by hardware)
	myScene.enableVSync();

	// Build shader program
	ShaderData data;
	OTTShader3D shader(ott::getAssetsPath("Shaders/test.vert"), ott::getAssetsPath("Shaders/test.frag"));
	shader.setShaderEnableFunction(testShaderEnable);
	shader.setShaderDisableFunction(OTTShader3D::unbindObjectTexture);
	shader.setShaderDataPointer(&data);

	// Set object shaders
	myShape.setShader(&shader);
	room.setShader(&shader, true);

	// Set the camera to draw surface normal vectors
	//myScene.setDrawOrigin();
	
	// Add the primitives to the scene
	myScene.addObject(&myShape);
	myScene.addObject(&room);

	// Player object
	OTTPlayer player;
	player.setEyes(&cam);
	player.setEyeOffset(Vector3(0, 1.9f, 0));
	player.setMass(100.f);
	player.applyForce(Vector3(0.f, -981.f, 0.f)); // Gravity: Fg = (g * 100 kg)

	// Load textures
	OTTTextureManager* textures = &OTTTextureManager::getInstance();
	textures->loadTexture(ott::getAssetsPath("Textures/Floor.jpg"), "floor");
	textures->loadTexture(ott::getAssetsPath("Textures/Brick.jpg"), "brick");
	textures->loadTexture(ott::getAssetsPath("Textures/crate.jpg"), "crate");
	textures->loadTexture(ott::getAssetsPath("Textures/dumbDice.png"), "dice");

	// Set object textures (be sure to add the objects to the scene first)
	textures->setTexture(&floor, "floor");
	textures->setTexture(&myShape, "dice");
	for (int i = 0; i < 4; i++) {
		textures->setTexture(&walls[i], "brick");
		textures->setTexture(&boxes[i], "crate");
	}

	WrappedValue pitch(0, -ott::pi, ott::pi);
	WrappedValue yaw(0, -ott::pi, ott::pi);
	WrappedValue theta(0, 0, ott::pi * 2);

	// Set the camera movement speed multiplier
	const float cameraMoveRate = 3.f;

	// Input device sensitivity
	const float controllerSensitivity = 0.1f;
	const float mouseSensitivity = 0.01f;

	// Set object's scale
	myShape.scale(0.75f);

	OTTKeyboard* keys = myScene.getKeypress();
	OTTMouse* mouse = window->getMouse();
	OTTJoypad* controller = window->getJoypad();

	window->enableMouse(); // Enable mouse support
	window->enableKeyboard(); // Enable keyboard support
	window->enableGamepad(); // Enable controller support

	// Lock cursor inside window and hide it
	mouse->setCursorState(MouseStates::DISABLED); 

	// Set shader information
	data.camptr = &cam;
	data.lightColor = Vector3(1.f, 0.75f, 0.5f);
	data.lightDir   = Vector3(0.f, 0.f, 1.f);
	data.lightPos   = Vector3(0.f, 2.5f, 0.f);
	data.lightIntensity = 100.f;
	data.lightCosAngle = std::cos(30.f * ott::deg2rad);
	data.lightDropoff = 0.5f;

	// "Animate" the object by rotating it and moving the camera
	int count = 0;
	double dX = 0;
	double dY = 0;
	float fStickX = 0.f;
	float fStickY = 0.f;
	float timeElapsed = 0.f;
	float totalTimeElapsed = 0.f;
	bool isDone = false;
	Matrix3 lightRotation;
	lightRotation.setRotation(0, 0, 0.08f);
	while(!isDone && myScene.drawOpenGL()){
		// Check for keypresses 
		if(!keys->empty()){			
			// Pressed keys (toggles)
			if(keys->poll('p'))
				cam.resetOrientation();
			if (keys->poll('f')){ // Flashlight
				if (data.lightIntensity > 0.f)
					data.lightIntensity = 0.f;
				else
					data.lightIntensity = 100.f;
			}

			// Held keys
			// Movement in the horizontal plane
			if (keys->check(KEYBOARD_W)) // Move the camera forward
				player.move(cameraMoveRate * timeElapsed * (keys->leftShift() ? 2.f : 1.f));
			if (keys->check(KEYBOARD_A)) // Move the camera left
				player.strafe(cameraMoveRate * timeElapsed * (keys->leftShift() ? 2.f : 1.f));
			if (keys->check(KEYBOARD_S)) // Move the camera backwards
				player.move(-cameraMoveRate * timeElapsed * (keys->leftShift() ? 2.f : 1.f));
			if (keys->check(KEYBOARD_D)) // Move the camera right
				player.strafe(-cameraMoveRate * timeElapsed * (keys->leftShift() ? 2.f : 1.f));
			if (keys->poll(KEYBOARD_SPACE)) // Jump
				player.applyImpulse(Vector3(0.f, 500.f, 0.f));
		}

		// Update controller (if connected)
		if (!controller->empty()){
			if (controller->check(GamepadInput::RSTICK)) { // Free-look camera
				controller->getRightStickPosition(fStickX, fStickY);
				player.look(fStickY * controllerSensitivity, fStickX * controllerSensitivity);
			}
			if (controller->check(GamepadInput::LSTICK)){ // Camera movement
				controller->getLeftStickPosition(fStickX, fStickY);
				player.move(-fStickY * controllerSensitivity, -fStickX * controllerSensitivity); // Both axes inverted
			}
			if (controller->poll(GamepadInput::A)) { // Jump
				player.applyImpulse(Vector3(0.f, 500.f, 0.f));
			}
		}

		// Check mouse movement
		if(mouse->delta(dX, dY)){
			player.look(dY * mouseSensitivity, dX * mouseSensitivity);
		}
		
		if(count++ % 120 == 0) // Frame count (every 2 seconds by default)
			std::cout << myScene.getFramerate() << " fps\r" << std::flush;
			
		// Move the object along the x-axis
		theta += mouseSensitivity * 2.f;
		//myShape.setPosition(Vector3(1.f * std::sin(theta.get()), 1.f * std::cos(theta.get()), 2.5f));
		
		// Rotate the object
		myShape.setRotation(-ott::pi / 2, 0, theta.get() + ott::pi / 2); // Absolute rotation

		// Lock dynamic light source to the camera
		data.lightPos = cam.getPosition();
		data.lightDir = cam.getDirection();
		data.lightPos += data.lightDir * 0.25;

		// Update player kinematics
		player.update(timeElapsed);

		// Render the scene
		window->render();

		// Cap the framerate
		timeElapsed = (float)myScene.sync();
		totalTimeElapsed += timeElapsed;
	}
	
	return 0;
}
