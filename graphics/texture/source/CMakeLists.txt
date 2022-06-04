# Complile graphics/texture sources
# External Depends: GLFW GLEW OpenGL GLU SOIL
add_library(OtterGraphicsTextureObj
	OBJECT 
	OTTBitmap.cpp
	OTTImage.cpp
	OTTSprite.cpp
	OTTSpriteSet.cpp
	OTTTexture.cpp
	OTTTextureManager.cpp
)

# Target include directory
target_include_directories(OtterGraphicsTextureObj PRIVATE ..)

# Build Core library
if(WIN32 OR INSTALL_STATIC_LIBRARIES)
	# Building dlls on Windows is a nightmare, so we just won't do it
	add_library( OtterGraphicsTexture
		STATIC 
		$<TARGET_OBJECTS:OtterGraphicsTextureObj>
	)
else()
	add_library( OtterGraphicsTexture
		SHARED 
		$<TARGET_OBJECTS:OtterGraphicsTextureObj>
	)
endif()

if(INSTALL_LIBRARIES)
	# Install libraries
	install(
		TARGETS OtterGraphicsTexture
		EXPORT OtterConfig
		LIBRARY DESTINATION ${INSTALL_LIB_DIR}
		ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
		RUNTIME DESTINATION ${INSTALL_BIN_DIR}
		INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
	)
endif()

