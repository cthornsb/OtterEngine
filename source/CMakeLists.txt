# 3d base classes
add_library(
	Otter3dCoreObj 
	OBJECT
	camera.cpp 
	lightSource.cpp
	Model.cpp
	Model3ds.cpp
	ModelObj.cpp
	ModelStl.cpp
	object.cpp
	OTTShader.cpp
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

# Build renderer executable.
add_executable(render3d renderer.cpp)
target_link_libraries(
	render3d 
	OtterCore
	Otter3dCore 
	OtterAudio
	OtterMath	
	OpenGL::GL
	OpenGL::GLU
	${GLFW3_LIBRARY}
	GLEW::glew
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
