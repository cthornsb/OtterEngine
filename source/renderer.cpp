#include <iostream>
#include <string>
#include <cmath>
#include <vector>

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
	
	// Setup the camera at z=-5 m
	camera cam(vector3(0, 0, -5));
	
	// Show some info about the camera
	cam.dump();
	
	// Setup the scene with our camera
	scene myScene(&cam);

	// Set the camera to draw surface normal vectors
	//myScene.setDrawNormals();
	myScene.setDrawOrigin();

	// Add the cube to the scene
	myScene.addObject(&myCube);

	// "Animate" the cube by rotating it and moving the camera
	while(myScene.update()){
		double t = myScene.getTimeElapsed();
	
		// Move the camera forward
		//cam.moveForward(0.0005*t);
		
		// Rotate the cube
		myCube.rotate(0.24*deg2rad, 0.14*deg2rad, 0.34*deg2rad);
		
		// Move the cube along the x-axis
		myCube.setPosition(vector3(2*std::sin(0.25*t), 2*std::cos(0.25*t), 0));

		// Point the camera at the cube
		cam.lookAt(myCube.getPosition());
	}
	
	return 0;
}
