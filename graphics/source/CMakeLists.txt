# Complile core graphics sources
# External Depends: glfw OpenGL GLU 
add_library(
	OtterCoreObj 
	OBJECT 
	ColorRGB.cpp
	OTTFrameTimer.cpp
	OTTImageBuffer.cpp
	OTTJoypad.cpp
	OTTKeyboard.cpp
	OTTMouse.cpp
	OTTWindow.cpp
)

# Compile graphics sources
# External Depends: SOIL
add_library(
	OtterGraphicsObj
	OBJECT
	OTTLogicalColor.cpp
	OTTShader.cpp
	OTTSprite.cpp
	OTTTexture.cpp
	OTTWindow3D.cpp
)

# Compile 3d geometry sources
# External Depends: GLEW
#	OTTShader.cpp
#	OTTWindow3D.cpp
# PolygonContainer.cpp

# Generate the libraries
add_library(
	OtterCoreStatic 
	STATIC 
	$<TARGET_OBJECTS:OtterCoreObj>
)
add_library(
	OtterGraphicsStatic 
	STATIC 
	$<TARGET_OBJECTS:OtterGraphicsObj>
)

# Install static libraries
if(INSTALL_STATIC)
	install(
		TARGETS 
		OtterCoreStatic 
		OtterGraphicsStatic 
		DESTINATION 
		lib
	)
endif(INSTALL_STATIC)

# Build and install shared libraries
if(INSTALL_SHARED)
	add_library(
		OtterCore 
		SHARED 
		$<TARGET_OBJECTS:OtterCoreObj>
	)
	add_library(
		OtterGraphics
		SHARED 
		$<TARGET_OBJECTS:OtterGraphicsObj>
	)
	install(
		TARGETS 
		OtterCore
		OtterGraphics
		DESTINATION 
		lib
	)
endif(INSTALL_SHARED)

