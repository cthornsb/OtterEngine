#include <graphics/app/OTTApplication.hpp>
#include <input/OTTMouse.hpp>
#include <math/OTTMatrix.hpp>
#include <math/OTTWrappedValue.hpp>

#include <iostream>
#include <cmath>

namespace ott {

void WindowFocus(GLFWwindow* win, int32_t focused) {
	if (focused == GLFW_TRUE) {
		std::cout << " Window gained focus :D" << std::endl;
	}
	else {
		std::cout << " Window lost focus :(" << std::endl;
	}
}

void WindowPosition(GLFWwindow* win, int32_t xpos, int32_t ypos) {
	std::cout << " Window moved: W=" << xpos << ", H=" << ypos << std::endl;
}

void WindowResize(GLFWwindow* win, int32_t width, int32_t height) {
	std::cout << " Window resized: W=" << width << ", H=" << height << std::endl;
}

void WindowIconify(GLFWwindow* win, int32_t iconified) {
	if (iconified == GLFW_TRUE) { // Iconified
		std::cout << " Window iconified" << std::endl;
	}
	else { // Restored
		std::cout << " Window restored" << std::endl;
	}
}

class AppTest : public Application {
public:

	AppTest() :
		Application(640, 480),
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

	bool OnUserCreateWindow() override {
		// Enable mouse and keyboard support
		this->EnableKeyboard();
		this->EnableMouse();
		this->EnableImageBuffer(false);

		// Set callback functions
		this->SetWindowFocusCallback(WindowFocus);
		this->SetWindowPositionCallback(WindowPosition);
		this->SetWindowResizeCallback(WindowResize);
		this->SetWindowIconifyCallback(WindowIconify);

		// Success
		return true;
	}

	bool OnUserLoop() override {
		this->Clear(); // Clear the screen
		buffer.Fill(0); // Fill image buffer w/ zeros (black)

		// Left mouse button held
		if (mouse->Check(0)) {
			const Vector2 xaxis = Vector2(1.f, 0.f);
			Vector2 mousePosition = Vector2(mouse->X(), mouse->Y()) - origin;
			fTheta = mousePosition.Angle(xaxis);
			if (mouse->Y() > origin[1]) { // Bottom half of screen
				fTheta = (6.28318f - fTheta());
			}
		}
		else {
			fTheta += 0.35f * (float)dTotalFrameTime;
		}

		// Get a new rotation matrix
		Matrix2 rotation = Matrix2::RotationMatrix(fTheta());
		for (int32_t i = 0; i < 4; i++) {
			transformed[i] = rotation.Transform(vertices[i]) + origin;
		}
		Vector2 unitX = rotation.Transform(Vector2(50.f, 0.f)) + origin;
		Vector2 unitY = rotation.Transform(Vector2(0.f, -50.f)) + origin; // Flipped because screen space Y is inverted
		buffer.SetDrawColor(colors::Green);
		buffer.DrawLine(origin[0], origin[1], unitX[0], unitX[1]);
		buffer.SetDrawColor(colors::Black);
		buffer.DrawLine(origin[0], origin[1], unitY[0], unitY[1]);
		buffer.SetDrawColor(colors::Red);
		buffer.DrawQuad(
			transformed[0][0], transformed[0][1],
			transformed[1][0], transformed[1][1],
			transformed[2][0], transformed[2][1],
			transformed[3][0], transformed[3][1]
		);

		if (nLoops++ % 120 == 0) // Frame count (every 2 seconds by default)
			std::cout << this->Framerate() << " fps\r" << std::flush;

		// Draw the screen
		this->RenderBuffer();

		return true;
	}

private:
	int32_t nLoops; ///< Total number of main loops executed

	Vector2 origin; ///< Origin of the viewport

	Vector2 vertices[4]; ///< Array of triangle vertices (wrt upper left corner of screen)

	Vector2 transformed[4]; ///< Array of transformed triangle vertices

	WrappedValue fTheta; ///< Floating point angle wrapped between 0 and 2pi radians
};

} /* namespace ott */

int32_t main(int32_t argc, char* argv[]) {
	// Declare a new 2d application
	ott::AppTest app;

	// Initialize application and open output window
	app.Start(argc, argv);

	// Run the main loop
	return app.Execute();
}