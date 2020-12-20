#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#include "Globals.hpp"
#include "Graphics.hpp"
#include "vector3.hpp"
#include "triangle.hpp"
#include "camera.hpp"
#include "Primitives.hpp"
#include "StlObject.hpp"
#include "colors.hpp"
#include "scene.hpp"

constexpr unsigned char KEYBOARD_W     = 0x77;
constexpr unsigned char KEYBOARD_A     = 0x61;
constexpr unsigned char KEYBOARD_S     = 0x73;
constexpr unsigned char KEYBOARD_D     = 0x64;

constexpr unsigned char KEYBOARD_Q     = 0x71;
constexpr unsigned char KEYBOARD_E     = 0x65;

constexpr unsigned char KEYBOARD_X     = 0x78;
constexpr unsigned char KEYBOARD_Z     = 0x7A;

void dummyFunc(){
}

int main(){
	// Define a new primitive
	//Primitives::Plane myShape(vector3(), 1, 1);
	//Primitives::Cube myShape(vector3(), 1, 1, 1);
	//Primitives::Circle myShape(vector3(), 1, 12);
	//Primitives::Cylinder myShape(vector3(), 1, 1, 12);
	Primitives::Cone myShape(vector3(), 1, 1, 12);

	// Setup the camera at z=-1.5 m (facing the object)
	camera cam(vector3(0, 0, -1.5));
	
	// Show some info about the camera
	cam.dump();
	
	// Setup the scene with our camera
	scene myScene(&cam);

	// Set the render mode for our cube
	myScene.setDrawingMode(drawMode::RENDER); // Currently, draw options [WIREFRAME, MESH, SOLID, RENDER] are supported

	// Set the color of the world light
	//myScene.getWorldLight()->setColor(Colors::RED);
	
	// Set the camera to draw surface normal vectors
	//myScene.setDrawNormals();
	//myScene.setDrawOrigin();
	//myScene.setDrawZDepth();
	
	// Add the cube to the scene
	myScene.addObject(&myShape);

	// Add a child object
	//float zoffset = (myShape.getSizeZ() + 1) / 2;
	//Primitives::Cone hat(vector3(), 1, 1, 12);
	//myShape.addChild(&hat, vector3(0, 0, zoffset));

	// Print the size of the object. This is undefined until calling addObject()
	std::cout << " Object size (x=" << myShape.getSizeX() << ", y=" << myShape.getSizeY() << ", z=" << myShape.getSizeZ() << ")" << std::endl;

	WrappedValue pitch(0, -pi, pi);
	WrappedValue yaw(0, -pi, pi);
	WrappedValue theta(0, 0, pi * 2);

	// Set the camera movement speed multiplier
	const float cameraMoveRate = 3.f;

	// Camera sensitivity
	const float cameraSensitivity = 0.01f;

	// "Animate" the object by rotating it and moving the camera
	int count = 0;
	int dX, dY;
	float timeElapsed = 0;
	bool isDone = false;
	KeyStates *keys = myScene.getKeypress();
	MouseState* mouse = myScene.getWindow()->getMouse();
	while(!isDone && myScene.update()){
		// Check for keypresses 
		if(!keys->empty()){
			// Function keys (0xF1 to 0xFC)
			/*if (keys->poll(0xF1))      // F1  
				dummyFunc();*/
			
			if(keys->poll('p'))
				cam.resetOrientation();

			// Movement in the horizontal plane
			if (keys->check(KEYBOARD_W)) // Move the camera forward
				cam.moveForward(cameraMoveRate * timeElapsed);
			if (keys->check(KEYBOARD_A)) // Move the camera left
				cam.moveLeft(cameraMoveRate * timeElapsed);
			if (keys->check(KEYBOARD_S)) // Move the camera backwards
				cam.moveBackward(cameraMoveRate * timeElapsed);
			if (keys->check(KEYBOARD_D)) // Move the camera right
				cam.moveRight(cameraMoveRate * timeElapsed);
				
			// Rotation about the vertical axis
			if (keys->check(KEYBOARD_Q)) // Rotate the camera ccw
				cam.rotate(0, 0, -cameraMoveRate * timeElapsed);
			if (keys->check(KEYBOARD_E)) // Rotate the camera cw
				cam.rotate(0, 0, cameraMoveRate * timeElapsed);
			
			// Vertical movement
			if (keys->check(KEYBOARD_Z)) // Move the camera down
				cam.moveDown(cameraMoveRate * timeElapsed);
			if (keys->check(KEYBOARD_X)) // Move the camera up
				cam.moveUp(cameraMoveRate * timeElapsed);
		}
		
		// Check mouse buttons
		/*if (!mouse->empty()) {
			if (mouse->poll(0)) // left button
				std::cout << " left button\n";
			if (mouse->poll(1)) // middle button
				std::cout << " middle button\n";
			if (mouse->poll(2)) // right button
				std::cout << " right button\n";
		}*/

		// Check mouse movement
		if(mouse->delta(dX, dY)){
			cam.rotate(dY * cameraSensitivity, dX * cameraSensitivity);
			/*pitch += (dY * cameraSensitivity/2);
			yaw += (dX * cameraSensitivity/2);
			myShape.setRotation(pitch.get(), 0, yaw.get());*/
		}
		
		if(count++ % 120 == 0) // Frame count (every 2 seconds by default)
			std::cout << myScene.getFramerate() << " fps\r" << std::flush;
			
		// Move the object along the x-axis
		//theta += cameraSensitivity;
		//myShape.setPosition(vector3(2*std::sin(theta.get()), 0, 2*std::cos(theta.get())));
		
		// Rotate the object
		myShape.rotate(0.3*deg2rad, 0.2*deg2rad, 0.4*deg2rad); // Relative rotation
		//myShape.setRotation(-pi/2, 0, theta.get()+pi/2); // Absolute rotation

		// Point the camera at the cube
		//cam.lookAt(myShape.getPosition());

		// Cap the framerate
		timeElapsed = myScene.sync();
	}
	
	return 0;
}
