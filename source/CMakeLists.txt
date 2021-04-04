# 3d base classes
add_library( Otter3dCoreObj 
	OBJECT
	camera.cpp 
	lightSource.cpp
	Model.cpp
	Model3ds.cpp
	ModelObj.cpp
	ModelStl.cpp
	object.cpp
	OTTPlayer.cpp
	OTTShader.cpp
	OTTTextureManager.cpp
	OTTWindow3D.cpp
	plane.cpp
	PixelTriplet.cpp
	PolygonContainer.cpp
	Primitives.cpp
	ray.cpp 
	scene.cpp
	triangle.cpp
	Vertex.cpp
	VertexContainer.cpp
)

# Build 3d libraries
if(NOT WIN32)
	add_library( Otter3dCore
		SHARED 
		$<TARGET_OBJECTS:Otter3dCoreObj>
	)
else()
	# Building dlls on Windows is a nightmare, so we just won't do it
	add_library( Otter3dCore
		STATIC 
		$<TARGET_OBJECTS:Otter3dCoreObj>
	)
endif()

if(INSTALL_LIBRARIES)
	# Install libraries
	install(
		TARGETS Otter3dCore 
		EXPORT OtterConfig
		LIBRARY DESTINATION ${INSTALL_LIB_DIR}
		ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
		RUNTIME DESTINATION ${INSTALL_BIN_DIR}
		INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
	)
endif()

# Build renderer executable.
add_executable(render3d renderer.cpp)
if(ASSETS_DIRECTORY)
	message(STATUS "Assets directory: ${ASSETS_DIRECTORY}")
	target_compile_definitions( render3d
		PUBLIC 
		ASSETS_DIRECTORY="${ASSETS_DIRECTORY}"
	)
endif()
target_link_libraries( render3d 
	OtterCore
	Otter3dCore 
	OtterAudio
	OtterMath	
	OpenGL::GL
	OpenGL::GLU
	${GLFW3_LIBRARY}
	GLEW::GLEW
	${SOIL_LIBRARY}
	${PORT_AUDIO_LIBRARY}
)
if(WIN32)
	target_link_libraries( render3d 
		Winmm
	)
endif()
install(
	TARGETS render3d 
	DESTINATION bin
)

