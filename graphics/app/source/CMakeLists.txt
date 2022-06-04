# Complile graphics/app sources
# External Depends: GLFW GLEW OpenGL GLU SOIL
add_library(OtterGraphicsAppObj
	OBJECT 
	OTTApplication.cpp
	OTTCharacterMap.cpp
	OTTConsole.cpp
	OTTString.cpp
)

# Target include directory
target_include_directories(OtterGraphicsAppObj PRIVATE ..)

# Build Core library
if(WIN32 OR INSTALL_STATIC_LIBRARIES)
	# Building dlls on Windows is a nightmare, so we just won't do it
	add_library( OtterGraphicsApp
		STATIC 
		$<TARGET_OBJECTS:OtterGraphicsAppObj>
	)
else()
	add_library( OtterGraphicsApp
		SHARED 
		$<TARGET_OBJECTS:OtterGraphicsAppObj>
	)
endif()

if(INSTALL_LIBRARIES)
	# Install libraries
	install(
		TARGETS OtterGraphicsApp
		EXPORT OtterConfig
		LIBRARY DESTINATION ${INSTALL_LIB_DIR}
		ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
		RUNTIME DESTINATION ${INSTALL_BIN_DIR}
		INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
	)
endif()

