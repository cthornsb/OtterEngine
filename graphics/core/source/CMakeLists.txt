# Complile graphics/core sources
# External Depends: GLFW GLEW OpenGL GLU SOIL
add_library(OtterGraphicsCoreObj
	OBJECT 
	OTTColor.cpp
	OTTImageBuffer.cpp
	OTTLogicalColor.cpp
	OTTShader.cpp
	OTTWindow.cpp
)

# Target include directory
target_include_directories(OtterGraphicsCoreObj PRIVATE ..)

# Build Core library
if(WIN32 OR INSTALL_STATIC_LIBRARIES)
	# Building dlls on Windows is a nightmare, so we just won't do it
	add_library( OtterGraphicsCore
		STATIC 
		$<TARGET_OBJECTS:OtterGraphicsCoreObj>
	)
else()
	add_library( OtterGraphicsCore
		SHARED 
		$<TARGET_OBJECTS:OtterGraphicsCoreObj>
	)
endif()

if(INSTALL_LIBRARIES)
	# Install libraries
	install(
		TARGETS OtterGraphicsCore
		EXPORT OtterConfig
		LIBRARY DESTINATION ${INSTALL_LIB_DIR}
		ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
		RUNTIME DESTINATION ${INSTALL_BIN_DIR}
		INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
	)
endif()

