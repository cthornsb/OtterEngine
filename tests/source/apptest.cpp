#include <iostream>
#include <cmath>

// Core
#include "OTTApplication.hpp"

// Math
#include "Matrix.hpp"
#include "WrappedValue.hpp"

void windowFocus(GLFWwindow* win, int focused) {
	if (focused == GLFW_TRUE) {
		std::cout << " Window gained focus :D" << std::endl;
	}
	else {
		std::cout << " Window lost focus :(" << std::endl;
	}
}

void windowPosition(GLFWwindow* win, int xpos, int ypos) {
	std::cout << " Window moved: W=" << xpos << ", H=" << ypos << std::endl;
}


void windowResize(GLFWwindow* win, int width, int height) {
	std::cout << " Window resized: W=" << width << ", H=" << height << std::endl;
}

void windowIconify(GLFWwindow* win, int iconified) {
	if (iconified == GLFW_TRUE) { // Iconified
		std::cout << " Window iconified" << std::endl;
	}
	else { // Restored
		std::cout << " Window restored" << std::endl;
	}
}

class AppTest : public OTTApplication {
public:
	AppTest() :
		OTTApplication(640, 480),
		nLoops(0),
		origin(320.f, 240.f),
		vertices{
			Vector2(-100.f, +100.f),
			Vector2(+100.f, +100.f),
			Vector2(+100.f, -100.f),
			Vector2(-100.f, -100.f)
		},
		transformed(),
		fTheta(0.f, 0.f, 2 * 3.14159f) // Angle wrapped between 0 and 2pi
	{
	}

	~AppTest() override {
		// Nothing to clean up, window will be closed by OTTWindow class
	}

protected:
	bool onUserCreateWindow() override {
		// Enable mouse and keyboard support
		enableKeyboard();
		enableMouse();
		enableImageBuffer(false);

		// Set callback functions
		setWindowFocusCallback(windowFocus);
		setWindowPositionCallback(windowPosition);
		setWindowResizeCallback(windowResize);
		setWindowIconifyCallback(windowIconify);

		// Success
		return true;
	}

	bool onUserLoop() override {
		clear(); // Clear the screen
		buffer.fill(0); // Fill image buffer w/ zeros (black)

		// Left mouse button held
		if (mouse.check(0)) {
			const Vector2 xaxis = Vector2(1.f, 0.f);
			Vector2 mousePosition = Vector2(mouse.getX(), mouse.getY()) - origin;
			fTheta = mousePosition.angle(xaxis);
			if (mouse.getY() > origin[1]) { // Bottom half of screen
				fTheta = (6.28318f - fTheta());
			}
		}
		else {
			fTheta += 0.35f * (float)dTotalFrameTime;
		}

		// Get a new rotation matrix
		Matrix2 rotation = Matrix2::getRotationMatrix(fTheta());
		for (int i = 0; i < 4; i++) {
			transformed[i] = rotation.transform(vertices[i]) + origin;
		}
		Vector2 unitX = rotation.transform(Vector2(50.f, 0.f)) + origin;
		Vector2 unitY = rotation.transform(Vector2(0.f, -50.f)) + origin; // Flipped because screen space Y is inverted
		buffer.setDrawColor(Colors::GREEN);
		buffer.drawLine(origin[0], origin[1], unitX[0], unitX[1]);
		buffer.setDrawColor(Colors::BLUE);
		buffer.drawLine(origin[0], origin[1], unitY[0], unitY[1]);
		buffer.setDrawColor(Colors::RED);
		buffer.drawQuad(
			transformed[0][0], transformed[0][1],
			transformed[1][0], transformed[1][1],
			transformed[2][0], transformed[2][1],
			transformed[3][0], transformed[3][1]
		);

		if (nLoops++ % 120 == 0) // Frame count (every 2 seconds by default)
			std::cout << getFramerate() << " fps\r" << std::flush;

		// Draw the screen
		renderBuffer();

		return true;
	}

private:
	int nLoops; ///< Total number of main loops executed

	Vector2 origin; ///< Origin of the viewport

	Vector2 vertices[4]; ///< Array of triangle vertices (wrt upper left corner of screen)

	Vector2 transformed[4]; ///< Array of transformed triangle vertices

	WrappedValue fTheta; ///< Floating point angle wrapped between 0 and 2pi radians
};

int main(int argc, char* argv[]) {
	// Declare a new 2d application
	AppTest app;

	// Initialize application and open output window
	app.start(argc, argv);

	// Run the main loop
	return app.execute();
}