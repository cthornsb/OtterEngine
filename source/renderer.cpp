#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#include "Graphics.hpp"
#include "vector3.hpp"
#include "triangle.hpp"
#include "camera.hpp"
#include "cube.hpp"
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
	// Define a new cube
	cube myCube(vector3(), 1, 1, 1);
	
	// Set the render mode for our cube
	myCube.setDrawingMode(scene::SOLID); // Currently, draw options [WIREFRAME, MESH, SOLID, RENDER] are supported
	
	// Setup the camera at z=-1.5 m (facing the cube)
	camera cam(vector3(0, 0, -1.5));
	
	// Show some info about the camera
	cam.dump();
	
	// Setup the scene with our camera
	scene myScene(&cam);
	//myScene.getWorldLight()->setColor(Colors::RED);
	
	// Set the camera to draw surface normal vectors
	//myScene.setDrawNormals();
	myScene.setDrawOrigin();
	
	// Add the cube to the scene
	myScene.addObject(&myCube);
	
	// "Animate the cube by rotating it and moving the camera
	int count = 0;
	double timeElapsed;
	bool isDone = false;
	KeyStates *keys = myScene.getKeypress();	
	while(!isDone && myScene.update()){
		// Get time since the last frame
		timeElapsed = myScene.getRenderTime();

		// Check for keypresses 
		if(!keys->empty()){
			// Function keys (0xF1 to 0xFC)
			/*if (keys->poll(0xF1))      // F1  
				dummyFunc();*/
			
			// Movement in the horizontal plane
			if (keys->check(KEYBOARD_W)) // Move the camera forward
				cam.moveForward(5*timeElapsed);
			if (keys->check(KEYBOARD_A)) // Move the camera left
				cam.moveLeft(5*timeElapsed);
			if (keys->check(KEYBOARD_S)) // Move the camera backwards
				cam.moveBackward(5*timeElapsed);
			if (keys->check(KEYBOARD_D)) // Move the camera right
				cam.moveRight(5*timeElapsed);
				
			// Rotation about the vertical axis
			if (keys->check(KEYBOARD_Q)) // Rotate the camera ccw
				cam.rotate(-2.5*timeElapsed, 0, 0);
			if (keys->check(KEYBOARD_E)) // Rotate the camera cw
				cam.rotate(2.5*timeElapsed, 0, 0);
			
			// Vertical movement
			if (keys->check(KEYBOARD_Z)) // Move the camera down
				cam.moveDown(5*timeElapsed);
			if (keys->check(KEYBOARD_X)) // Move the camera up
				cam.moveUp(5*timeElapsed);
		}
		
		// Check the mouse
		/*if(myScene.getMouse()->down){
			cam.rotate(myScene.getMouse()->xrel*0.01, 0, -myScene.getMouse()->yrel*0.01);
			myScene.getMouse()->xrel = 0;
			myScene.getMouse()->yrel = 0;
		}*/
		
		if(count++ % 120 == 0) // Frame count (every 2 seconds by default)
			std::cout << myScene.getFramerate() << " fps\r" << std::flush;
			
		// Rotate the cube
		myCube.rotate(0.24*deg2rad, 0.14*deg2rad, 0.34*deg2rad);
		
		// Move the cube along the x-axis
		//myCube.setPosition(vector3(2*std::sin(0.25*timeElapsed), 2*std::cos(0.25*timeElapsed), 0));
		
		// Point the camera at the cube
		//cam.lookAt(myCube.getPosition());
	}
	
	return 0;
}
