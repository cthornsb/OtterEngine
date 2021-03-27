# Complile core graphics sources
# External Depends: GLFW GLEW OpenGL GLU SOIL
add_library(
	OtterCoreObj 
	OBJECT 
	ColorRGB.cpp
	OTTFrameTimer.cpp
	OTTImageBuffer.cpp
	OTTJoypad.cpp
	OTTKeyboard.cpp
	OTTLogicalColor.cpp
	OTTMouse.cpp
	OTTSprite.cpp
	OTTTexture.cpp
	OTTWindow.cpp
)

# Build Core library
if(NOT WIN32)
	add_library( OtterCore
		SHARED 
		$<TARGET_OBJECTS:OtterCoreObj>
	)

else()
	# Building dlls on Windows is a nightmare, so we just won't do it
	add_library( OtterCore
		STATIC 
		$<TARGET_OBJECTS:OtterCoreObj>
	)
endif()
