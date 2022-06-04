# Complile graphics/3d sources
# External Depends: GLFW GLEW OpenGL GLU SOIL
add_library(OtterGraphics3dObj
	OBJECT 
	OTTCamera.cpp
	OTTLightSource.cpp
	OTTPlayer.cpp
	OTTScene.cpp
	OTTShader3d.cpp
	OTTWindow3d.cpp
)

# Target include directory
target_include_directories(OtterGraphics3dObj PRIVATE ..)

# Build Core library
if(WIN32 OR INSTALL_STATIC_LIBRARIES)
	# Building dlls on Windows is a nightmare, so we just won't do it
	add_library( OtterGraphics3d
		STATIC 
		$<TARGET_OBJECTS:OtterGraphics3dObj>
	)
else()
	add_library( OtterGraphics3d
		SHARED 
		$<TARGET_OBJECTS:OtterGraphics3dObj>
	)
endif()

if(INSTALL_LIBRARIES)
	# Install libraries
	install(
		TARGETS OtterGraphics3d
		EXPORT OtterConfig
		LIBRARY DESTINATION ${INSTALL_LIB_DIR}
		ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
		RUNTIME DESTINATION ${INSTALL_BIN_DIR}
		INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
	)
endif()

