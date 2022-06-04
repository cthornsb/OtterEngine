if(ASSETS_DIRECTORY)
	# This does not work, but I'm not sure why
	message(STATUS "Assets directory: ${ASSETS_DIRECTORY}")
	add_compile_definitions(ASSETS_DIRECTORY="${ASSETS_DIRECTORY}")
endif()

# Complile core graphics sources
# External Depends: GLFW GLEW OpenGL GLU SOIL
add_library( OtterCoreObj 
	OBJECT 
	OTTConfigFile.cpp
	OTTFrameTimer.cpp
	OTTHighResTimer.cpp	
	OTTStringOps.cpp	
	OTTSystem.cpp
	OTTTextParser.cpp
)

# Target include directory
target_include_directories(OtterCoreObj PUBLIC ..)

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

