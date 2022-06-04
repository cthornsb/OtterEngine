# Compile input sources
# External Depends: GLFW OpenGL
add_library( OtterInputObj 
	OBJECT 
	OTTAnalogStick.cpp
	OTTGamepad.cpp
	OTTJoypad.cpp
	OTTKeyboard.cpp
	OTTMouse.cpp
)

# Target include directory
target_include_directories(OtterInputObj PUBLIC ..)

# Build Input library
if(WIN32 OR INSTALL_STATIC_LIBRARIES)
	# Building dlls on Windows is a nightmare, so we just won't do it
	add_library( OtterInput
		STATIC 
		$<TARGET_OBJECTS:OtterInputObj>
	)
else()
	add_library( OtterInput
		SHARED 
		$<TARGET_OBJECTS:OtterInputObj>
	)
endif()

if(INSTALL_LIBRARIES)
	# Install libraries
	install(
		TARGETS OtterInput
		EXPORT OtterConfig
		LIBRARY DESTINATION ${INSTALL_LIB_DIR}
		ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
		RUNTIME DESTINATION ${INSTALL_BIN_DIR}
		INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
	)
endif()

