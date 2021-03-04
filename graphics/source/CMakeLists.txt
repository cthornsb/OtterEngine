# Complile graphics sources
add_library(
	OtterCoreObj 
	OBJECT 
	ColorRGB.cpp
	OTTImageBuffer.cpp
	OTTKeyboard.cpp
	OTTMouse.cpp
	OTTWindow.cpp
)
add_library(
	OtterGraphicsObj
	OBJECT
	OTTLogicalColor.cpp
	OTTShader.cpp
	OTTSprite.cpp
	OTTTexture.cpp
	OTTWindow3D.cpp
)

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

