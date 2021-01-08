set(CORE_SOURCES camera.cpp 
                 lightSource.cpp
                 object.cpp                 
                 plane.cpp
                 PixelTriplet.cpp
                 PolygonContainer.cpp
                 Primitives.cpp
                 ray.cpp 
                 scene.cpp
                 Shader.cpp
                 Sprite.cpp
                 StlObject.cpp
                 Texture.cpp
                 triangle.cpp 
                 Vertex.cpp
                 VertexContainer.cpp
                 WrappedValue.cpp
                 ZBuffer.cpp)

#Add the sources to the library.
add_library(CORE_OBJECTS OBJECT ${CORE_SOURCES})

#Generate a static library.
add_library(CORE_LIB STATIC $<TARGET_OBJECTS:CORE_OBJECTS>)

#Build renderer executable.
add_executable(render3d renderer.cpp)
target_link_libraries(render3d CORE_LIB MATH_LIB GRAPHICS_LIB ${GRAPHICS_LIBS})
install(TARGETS render3d DESTINATION bin)

if(WIN32 AND INSTALL_DLLS)
	#Install required dlls
	install(FILES ${WINDOWS_DLL_LIST} DESTINATION bin)
endif(WIN32 AND INSTALL_DLLS)