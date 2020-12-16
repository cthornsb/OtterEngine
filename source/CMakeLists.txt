set(CORE_SOURCES matrix3.cpp 
                 vector3.cpp 
                 plane.cpp 
                 triangle.cpp 
                 ray.cpp 
                 object.cpp
                 cube.cpp 
                 lightSource.cpp 
                 camera.cpp 
                 scene.cpp)

#Add the sources to the library.
add_library(CORE_OBJECTS OBJECT ${CORE_SOURCES})

#Generate a static library.
add_library(CORE_LIB STATIC $<TARGET_OBJECTS:CORE_OBJECTS>)

#Build renderer executable.
add_executable(render3d renderer.cpp)
target_link_libraries(render3d CORE_LIB GRAPHICS_LIB ${GRAPHICS_LIBS})
install(TARGETS render3d DESTINATION bin)

if(WIN32 AND INSTALL_DLLS)
	#Install required dlls
	install(FILES ${FREEGLUT_DLL_LIST} DESTINATION bin)
	if(BUILD_DEBUG)
		install(FILES ${QT_DLL_LIST} DESTINATION bin)
	endif(BUILD_DEBUG)
endif(WIN32 AND INSTALL_DLLS)