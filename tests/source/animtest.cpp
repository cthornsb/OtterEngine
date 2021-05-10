#include <iostream>
#include <cmath>

// Core
#include "OTTApplication.hpp"
#include "OTTSprite.hpp"
#include "OTTSpriteSet.hpp"

// Math
#include "WrappedValue.hpp"

class SpriteApp : public OTTApplication {
public:
	SpriteApp() :
		OTTApplication(500, 500),
		bAnimated(true),
		nLoops(0),
		theta(0.f, 0.f, 6.28318f),
		bForeground{
			true,
			false,
			false,
			false
		},
		positions{
			Vector2(0.f, -1.f),
			Vector2(1.f, 0.f),
			Vector2(0.f, 1.f),
			Vector2(-1.f, 0.f)
		},
		animatedSprites()
	{
	}

	~SpriteApp() override {
		// Nothing to clean up, window will be closed by OTTWindow class
	}

protected:
	bool onUserCreateWindow() override {
		// Enable mouse and keyboard support
		enableKeyboard();

		// Generate a RGBA texture
		OTTImageBuffer sheet(300, 400, 4);
		
		auto drawBox = [&](const int& x0, const int& y0, const int& W, const int& H, const ColorRGB& color){
			sheet.setDrawColor(color);
			sheet.drawRectangle(x0, y0, W, H, true);
		};
		
		// Row 0
		drawBox(0, 0, 99, 99, Colors::RED);
		drawBox(100, 0, 199, 99, Colors::GREEN);
		drawBox(200, 0, 299, 99, Colors::BLUE);
		
		// Row 1
		drawBox(0, 100, 99, 199, Colors::YELLOW);
		drawBox(100, 100, 199, 199, Colors::MAGENTA);
		drawBox(200, 100, 299, 199, Colors::CYAN);
		
		// Row 2
		drawBox(0, 200, 99, 299, Colors::ORANGE);
		drawBox(100, 200, 199, 299, Colors::CHARTREUSE);
		drawBox(200, 200, 299, 299, Colors::SPRING);
		
		// Row 3
		drawBox(0, 300, 99, 399, Colors::AZURE);
		drawBox(100, 300, 199, 399, Colors::VIOLET);
		drawBox(200, 300, 299, 399, Colors::ROSE);
		
		animatedSprites[0].addSprites(&sheet, 0, 0, 100, 100, 3, 1);
		animatedSprites[1].addSprites(&sheet, 0, 100, 100, 100, 3, 1);
		animatedSprites[2].addSprites(&sheet, 0, 200, 100, 100, 3, 1);
		animatedSprites[3].addSprites(&sheet, 0, 300, 100, 100, 3, 1);
		
		for(int i = 0; i < 4; i++){
			animatedSprites[i].setFramerate(6);
		}
		
		// Set initial sprite positions
		updateBoxes(0);
		
		// Select this window
		setCurrent();
		
		std::cout << " Press (q) or (w) to rotate sprites" << std::endl;
		std::cout << " Press (p) to pause sprite animation" << std::endl;
		std::cout << " Press (z) or (x) to select previous or next sprite frame respectively" << std::endl;
		std::cout << " Press escape to quit" << std::endl;

		// Success
		return true;
	}

	bool onUserLoop() override {
		clear(); // Clear the screen

		// Draw the background sprites (y <= 0)
		for(int i = 0; i < 4; i++){
			if(!bForeground[i]){
				animatedSprites[i].draw();
			}
		}

		// Draw the foreground sprites (y > 0)
		for(int i = 0; i < 4; i++){
			if(bForeground[i]){
				animatedSprites[i].draw();
			}
		}
		
		if(bAnimated){
			for(int i = 0; i < 4; i++){ // Update current animation time
				animatedSprites[i].update(dTotalFrameTime);
			}
		}

		if (!keys.empty()){ // Rotate CCW
			if(keys.check('q')){
				updateBoxes(1.f * dTotalFrameTime);
			}
			if(keys.check('w')){ // Rotate CW
				updateBoxes(-1.f * dTotalFrameTime);
			}
			if(keys.poll('z')){
				for(int i = 0; i < 4; i++)
					animatedSprites[i].prev();
			}
			if(keys.poll('x')){
				for(int i = 0; i < 4; i++)
					animatedSprites[i].next();
			}
			if(keys.poll('p'))
				bAnimated = !bAnimated;
		}

		if (nLoops++ % 120 == 0) // Frame count (every 2 seconds by default)
			std::cout << getFramerate() << " fps\r" << std::flush;

		// Draw the screen
		render();

		return true;
	}

private:
	bool bAnimated;

	int nLoops; ///< Total number of main loops executed
	
	WrappedValue theta;
	
	bool bForeground[4];
	
	Vector2 positions[4];
	
	OTTSpriteSet animatedSprites[4]; ///< Animated sprites
	
	void updateBoxes(const double& dTheta) {
		Matrix2 rotation = Matrix2::getRotationMatrix(theta += dTheta);
		for(int i = 0; i < 4; i++){
			Vector2 rotated = rotation.transform(positions[i]);
			animatedSprites[i].setPosition(250.f + 125 * rotated[0], 250);
			animatedSprites[i].setScale((2.f + rotated[1]) * 0.25f);
			if(rotated[1] <= 0.f) // Background
				bForeground[i] = false;
			else // Foreground
				bForeground[i] = true;
		}
	}
};

int main(int argc, char* argv[]) {
	// Declare a new 2d application
	SpriteApp app;

	// Initialize application and open output window
	app.start(argc, argv);

	// Run the main loop
	return app.execute();
}
