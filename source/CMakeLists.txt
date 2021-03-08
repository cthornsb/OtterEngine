# 3d classes
add_library(
	Otter3dCoreObj 
	OBJECT
	camera.cpp 
	lightSource.cpp
	plane.cpp
	PixelTriplet.cpp
	ray.cpp 
	scene.cpp
	triangle.cpp
	Vertex.cpp
)

# 3d geometry classes
add_library(
	Otter3dGeomObj 
	OBJECT
	Model.cpp
	Model3ds.cpp
	ModelObj.cpp
	ModelStl.cpp
	object.cpp
	VertexContainer.cpp
	PolygonContainer.cpp
	Primitives.cpp
)

# Build static libraries.
add_library(
	Otter3dCoreStatic 
	STATIC 
	$<TARGET_OBJECTS:Otter3dCoreObj>
)
add_library(
	Otter3dGeomStatic 
	STATIC 
	$<TARGET_OBJECTS:Otter3dGeomObj>
)

# Install static libraries.
if(INSTALL_STATIC)
	install(TARGETS Otter3dCoreStatic Otter3dGeomStatic DESTINATION lib)
endif(INSTALL_STATIC)

# Build renderer executable.
add_executable(render3d renderer.cpp)
target_link_libraries(
	render3d 
	OtterCoreStatic
	OtterGraphicsStatic
	Otter3dCoreStatic 
	Otter3dGeomStatic 
	OtterMathStatic 
	${GLFW3_LIBRARY}
	OpenGL::GL
	OpenGL::GLU
	GLEW::GLEW
	${SOIL_LIBRARY}
	${PORT_AUDIO_LIBRARY}
)
install(TARGETS render3d DESTINATION bin)

# Build and install shared libraries.
if(INSTALL_SHARED)
	add_library(
		Otter3dCore 
		SHARED 
		$<TARGET_OBJECTS:Otter3dCoreObj>
	)
	add_library(
		Otter3dGeom 
		SHARED 
		$<TARGET_OBJECTS:Otter3dGeomObj>
	)
	install(TARGETS Otter3dCore Otter3dGeom DESTINATION lib)
endif(INSTALL_SHARED)

#Install required dlls (windows)
if(WIN32 AND INSTALL_DLLS)
	install(FILES ${WINDOWS_DLL_LIST} DESTINATION bin)
endif(WIN32 AND INSTALL_DLLS)
