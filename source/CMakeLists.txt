set(CORE_SOURCES matrix3.cpp vector3.cpp plane.cpp triangle.cpp ray.cpp object.cpp cube.cpp colors.cpp lightSource.cpp sdlWindow.cpp camera.cpp scene.cpp)

#Add the sources to the library.
add_library(CORE_OBJECTS OBJECT ${CORE_SOURCES})

#Generate a static library.
add_library(CORE_LIB STATIC $<TARGET_OBJECTS:CORE_OBJECTS>)

#Build renderer executable.
add_executable(renderer renderer.cpp)
target_link_libraries(renderer CORE_LIB -lSDL2)
install(TARGETS renderer DESTINATION bin)
