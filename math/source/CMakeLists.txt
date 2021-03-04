# Compile source files
add_library(
	OtterMathObj 
	OBJECT
	Matrix.cpp
	Vector.cpp
	WrappedValue.cpp
)

# Generate a static library.
add_library(
	OtterMathStatic 
	STATIC 
	$<TARGET_OBJECTS:OtterMathObj>
)

# Install static libraries.
if(INSTALL_STATIC)
	install(TARGETS OtterMathStatic DESTINATION lib)
endif(INSTALL_STATIC)

# Install shared libraries.
if(INSTALL_SHARED)
	add_library(
		OtterMath 
		SHARED 
		$<TARGET_OBJECTS:OtterMathObj>
	)
	install(TARGETS OtterMath DESTINATION lib)
endif(INSTALL_SHARED)

