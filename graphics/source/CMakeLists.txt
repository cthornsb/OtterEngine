# Complile core graphics sources
# External Depends: GLFW GLEW OpenGL GLU SOIL
add_library( OtterCoreObj 
	OBJECT 
	ColorRGB.cpp
	OTTApplication.cpp
	OTTBitmap.cpp
	OTTCharacterMap.cpp
	OTTConsole.cpp
	OTTFrameTimer.cpp
	OTTHighResTimer.cpp
	OTTImageBuffer.cpp
	OTTJoypad.cpp
	OTTKeyboard.cpp
	OTTLogicalColor.cpp
	OTTMouse.cpp
	OTTShader.cpp
	OTTString.cpp
	OTTSprite.cpp
	OTTSpriteSet.cpp
	OTTTexture.cpp
	OTTTextParser.cpp
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

# Build system library
if(WIN32 OR INSTALL_STATIC_LIBRARIES)
	add_library( OtterSystem
		STATIC 
		OTTSystem.cpp
	)
else()
	add_library( OtterSystem
		SHARED 
		OTTSystem.cpp
	)
endif()

if(ASSETS_DIRECTORY)
	message(STATUS "Assets directory: ${ASSETS_DIRECTORY}")
	target_compile_definitions( OtterSystem
		PUBLIC 
		ASSETS_DIRECTORY="${ASSETS_DIRECTORY}"
	)
endif()

if(INSTALL_LIBRARIES)
	# Install libraries
	install(
		TARGETS OtterCore OtterSystem
		EXPORT OtterConfig
		LIBRARY DESTINATION ${INSTALL_LIB_DIR}
		ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
		RUNTIME DESTINATION ${INSTALL_BIN_DIR}
		INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
	)
endif()

