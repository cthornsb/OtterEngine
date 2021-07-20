# Compile source files
add_library(
	OtterMathObj 
	OBJECT
	DecayEnvelope.cpp
	Matrix.cpp
	MovingAverage.cpp
	OTTRandom.cpp
	Vector.cpp
	WrappedValue.cpp
)

# Generate math library
if(WIN32 OR INSTALL_STATIC_LIBRARIES)
	# Building dlls on Windows is a nightmare, so we just won't do it
	add_library(
		OtterMath
		STATIC 
		$<TARGET_OBJECTS:OtterMathObj>
	)
else()
	add_library(
		OtterMath
		SHARED 
		$<TARGET_OBJECTS:OtterMathObj>
	)
endif()

if(INSTALL_LIBRARIES)
	# Install libraries
	install(
		TARGETS OtterMath
		EXPORT OtterConfig
		LIBRARY DESTINATION ${INSTALL_LIB_DIR}
		ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
		RUNTIME DESTINATION ${INSTALL_BIN_DIR}
		INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
	)
endif()

