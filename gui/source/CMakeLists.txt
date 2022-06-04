add_library( OtterGuiObj
	OBJECT
	OTTGuiButton.cpp
	OTTGuiCheckbox.cpp
	OTTGuiContainer.cpp
	OTTGuiElement.cpp
	OTTGuiFrame.cpp	
	OTTGuiRadioButton.cpp
	OTTGuiSlider.cpp
)

# Target include directory
target_include_directories(OtterGuiObj PUBLIC ..)

# Build Gui library
if(WIN32 OR INSTALL_STATIC_LIBRARIES)
	add_library( OtterGui
		STATIC 
		$<TARGET_OBJECTS:OtterGuiObj>
	)
else()
	add_library( OtterGui
		SHARED 
		$<TARGET_OBJECTS:OtterGuiObj>
	)
endif()

if(INSTALL_LIBRARIES)
	# Install libraries
	install(
		TARGETS OtterGui
		EXPORT OtterConfig
		LIBRARY DESTINATION ${INSTALL_LIB_DIR}
		ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
		RUNTIME DESTINATION ${INSTALL_BIN_DIR}
		INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
	)
endif()
