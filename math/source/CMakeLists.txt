# Compile source files
add_library(
	OtterMathObj 
	OBJECT
	Matrix.cpp
	Vector.cpp
	WrappedValue.cpp
)

# Generate math library
if(NOT WIN32)
	add_library(
		OtterMath
		SHARED 
		$<TARGET_OBJECTS:OtterMathObj>
	)
else()
	# Building dlls on Windows is a nightmare, so we just won't do it
	add_library(
		OtterMath
		STATIC 
		$<TARGET_OBJECTS:OtterMathObj>
	)
endif()
