# Complile core graphics sources
# External Depends: GLFW GLEW OpenGL GLU SOIL
add_library( OtterCoreObj 
	OBJECT 
	ColorRGB.cpp
	OTTApplication.cpp
	OTTFrameTimer.cpp
	OTTImageBuffer.cpp
	OTTJoypad.cpp
	OTTKeyboard.cpp
	OTTLogicalColor.cpp
	OTTMouse.cpp
	OTTSprite.cpp
	OTTSystem.cpp
	OTTTexture.cpp
	OTTWindow.cpp
)

if(STRICT_OPENGL_VERSION)
	add_dependencies( OtterCoreObj 
		OtterVersionHeader
	)
endif()

# Build Core library
if(WIN32 OR INSTALL_STATIC_LIBRARIES)
	# Building dlls on Windows is a nightmare, so we just won't do it
	add_library( OtterCore
		STATIC 
		$<TARGET_OBJECTS:OtterCoreObj>
	)
else()
	add_library( OtterCore
		SHARED 
		$<TARGET_OBJECTS:OtterCoreObj>
	)
endif()

if(INSTALL_LIBRARIES)
	# Install libraries
	install(
		TARGETS OtterCore
		EXPORT OtterConfig
		LIBRARY DESTINATION ${INSTALL_LIB_DIR}
		ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
		RUNTIME DESTINATION ${INSTALL_BIN_DIR}
		INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
	)
endif()

