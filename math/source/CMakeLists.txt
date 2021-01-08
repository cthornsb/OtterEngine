set(MATH_SOURCES Matrix.cpp
                 Vector.cpp)

#Add the sources to the library.
add_library(MATH_OBJECTS OBJECT ${MATH_SOURCES})

#Generate a static library.
add_library(MATH_LIB STATIC $<TARGET_OBJECTS:MATH_OBJECTS>)