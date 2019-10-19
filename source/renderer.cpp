#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#include "sdlWindow.hpp"
#include "vector3.hpp"
#include "triangle.hpp"
#include "camera.hpp"
#include "cube.hpp"
#include "colors.hpp"
#include "scene.hpp"

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
	bool isDone = false;
	while(!isDone && myScene.update()){
		double t = myScene.getTimeElapsed();
		
		// Check if a key was pressed
		if(myScene.getKeypress()->down){
			//std::cout << " key pressed: " << myScene.getKeypress()->key << std::endl;
			switch(myScene.getKeypress()->key){
				case 0x1B:
					// Escape key
					isDone = true;
					break;
				case 'w':
					// Move the camera forward
					cam.moveForward(0.001*t);
					break;
				case 'a':
					// Move the camera left
					cam.moveLeft(0.001*t);
					break;
				case 's':
					// Move the camera backwards
					cam.moveBackward(0.001*t);
					break;
				case 'd':
					// Move the camera right
					cam.moveRight(0.001*t);
					break;
				case 'z':
					// Move the camera down
					cam.moveDown(0.001*t);
					break;
				case 'x':
					// Move the camera up
					cam.moveUp(0.001*t);
					break;
				default:
					break;
			}
		}
		
		// Check the mouse
		if(myScene.getMouse()->down){
			cam.rotate(myScene.getMouse()->xrel*0.01, 0, -myScene.getMouse()->yrel*0.01);
			myScene.getMouse()->xrel = 0;
			myScene.getMouse()->yrel = 0;
		}
		
		if(count++ % 100 == 0) // Frame count
			std::cout << myScene.getFramerate() << " fps\r" << std::flush;
			
		// Rotate the cube
		//myCube.rotate(0.24*deg2rad, 0.14*deg2rad, 0.34*deg2rad);
		
		// Move the cube along the x-axis
		//myCube.setPosition(vector3(2*std::sin(0.25*t), 2*std::cos(0.25*t), 0));
		
		// Point the camera at the cube
		//cam.lookAt(myCube.getPosition());
	}
	
	return 0;
}
