#include <graphics/app/OTTApplication.hpp>
#include <graphics/texture/OTTSprite.hpp>
#include <graphics/texture/OTTSpriteSet.hpp>
#include <input/OTTKeyboard.hpp>
#include <math/OTTWrappedValue.hpp>

#include <iostream>
#include <cmath>

namespace ott {

class SpriteApp : public Application {
public:
	SpriteApp() :
		Application(500, 500),
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

	bool OnUserCreateWindow() override {
		// Enable mouse and keyboard support
		this->EnableKeyboard();

		// Generate a RGBA texture
		ImageBuffer sheet(300, 400, 4);
		
		auto drawBox = [&](const int32_t& x0, const int32_t& y0, const int32_t& W, const int32_t& H, const ColorRGB& color){
			sheet.SetDrawColor(color);
			sheet.DrawRectangle(x0, y0, W, H, true);
		};
		
		// Row 0
		drawBox(0, 0, 99, 99, colors::Red);
		drawBox(100, 0, 199, 99, colors::Green);
		drawBox(200, 0, 299, 99, colors::Blue);
		
		// Row 1
		drawBox(0, 100, 99, 199, colors::Yellow);
		drawBox(100, 100, 199, 199, colors::Magenta);
		drawBox(200, 100, 299, 199, colors::Cyan);
		
		// Row 2
		drawBox(0, 200, 99, 299, colors::Orange);
		drawBox(100, 200, 199, 299, colors::Chartreuse);
		drawBox(200, 200, 299, 299, colors::Spring);
		
		// Row 3
		drawBox(0, 300, 99, 399, colors::Azure);
		drawBox(100, 300, 199, 399, colors::Violet);
		drawBox(200, 300, 299, 399, colors::Rose);
		
		animatedSprites[0].AddSprites(&sheet, 0, 0, 100, 100, 3, 1);
		animatedSprites[1].AddSprites(&sheet, 0, 100, 100, 100, 3, 1);
		animatedSprites[2].AddSprites(&sheet, 0, 200, 100, 100, 3, 1);
		animatedSprites[3].AddSprites(&sheet, 0, 300, 100, 100, 3, 1);
		
		for(int32_t i = 0; i < 4; i++){
			animatedSprites[i].SetFramerate(6);
		}
		
		// Set initial sprite positions
		updateBoxes(0);
		
		// Select this window
		this->SetCurrent();
		
		std::cout << " Press (q) or (w) to rotate sprites" << std::endl;
		std::cout << " Press (p) to pause sprite animation" << std::endl;
		std::cout << " Press (z) or (x) to select previous or next sprite frame respectively" << std::endl;
		std::cout << " Press escape to quit" << std::endl;

		// Success
		return true;
	}

	bool OnUserLoop() override {
		this->Clear(); // Clear the screen

		// Draw the background sprites (y <= 0)
		for(int32_t i = 0; i < 4; i++){
			if(!bForeground[i]){
				animatedSprites[i].Draw();
			}
		}

		// Draw the foreground sprites (y > 0)
		for(int32_t i = 0; i < 4; i++){
			if(bForeground[i]){
				animatedSprites[i].Draw();
			}
		}
		
		if(bAnimated){
			for(int32_t i = 0; i < 4; i++){ // Update current animation time
				animatedSprites[i].Update(dTotalFrameTime);
			}
		}

		if (!keys->Empty()){ // Rotate CCW
			if(keys->Check('q')){
				updateBoxes(1.f * dTotalFrameTime);
			}
			if(keys->Check('w')){ // Rotate CW
				updateBoxes(-1.f * dTotalFrameTime);
			}
			if(keys->Poll('z')){
				for(int32_t i = 0; i < 4; i++)
					animatedSprites[i].Prev();
			}
			if(keys->Poll('x')){
				for(int32_t i = 0; i < 4; i++)
					animatedSprites[i].Next();
			}
			if(keys->Poll('p'))
				bAnimated = !bAnimated;
		}

		if (nLoops++ % 120 == 0) // Frame count (every 2 seconds by default)
			std::cout << this->Framerate() << " fps\r" << std::flush;

		// Draw the screen
		this->Render();

		return true;
	}

private:
	bool bAnimated;

	int32_t nLoops; ///< Total number of main loops executed
	
	WrappedValue theta;
	
	bool bForeground[4];
	
	Vector2 positions[4];
	
	SpriteSet animatedSprites[4]; ///< Animated sprites
	
	void updateBoxes(const double& dTheta) {
		Matrix2 rotation = Matrix2::RotationMatrix(theta += dTheta);
		for(int32_t i = 0; i < 4; i++){
			Vector2 rotated = rotation.Transform(positions[i]);
			animatedSprites[i].SetPosition(250.f + 125 * rotated[0], 250);
			animatedSprites[i].SetScale((2.f + rotated[1]) * 0.25f);
			if(rotated[1] <= 0.f) // Background
				bForeground[i] = false;
			else // Foreground
				bForeground[i] = true;
		}
	}
};

} /* namespace ott */

int32_t main(int32_t argc, char* argv[]) {
	// Declare a new 2d application
	ott::SpriteApp app;

	// Initialize application and open output window
	app.Start(argc, argv);

	// Run the main loop
	return app.Execute();
}
