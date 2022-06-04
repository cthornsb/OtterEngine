set(GraphicsSourceFiles
	OTTModel.cpp
	OTTModel3ds.cpp
	OTTModelObj.cpp
	OTTModelStl.cpp
	OTTObject.cpp
	OTTPlane.cpp
	OTTPolygonContainer.cpp
	OTTPrimitives.cpp
	OTTRay.cpp
	OTTPolygon.cpp
	OTTVertex.cpp
	OTTVertexContainer.cpp
)

if(SOFTWARE_RENDERER)
list(APPEND GraphicsSourceFiles OTTPixelTriplet.cpp)
endif(SOFTWARE_RENDERER)

# Complile graphics/geometry sources
# External Depends: GLFW GLEW OpenGL GLU SOIL
add_library(OtterGraphicsGeometryObj
	OBJECT 
	${GraphicsSourceFiles}
)

if(SOFTWARE_RENDERER)
target_compile_definitions(OtterGraphicsGeometryObj PUBLIC SOFTWARE_RENDERER)
endif(SOFTWARE_RENDERER)

# Target include directory
target_include_directories(OtterGraphicsGeometryObj PRIVATE ..)

# Build Core library
if(WIN32 OR INSTALL_STATIC_LIBRARIES)
	# Building dlls on Windows is a nightmare, so we just won't do it
	add_library( OtterGraphicsGeometry
		STATIC 
		$<TARGET_OBJECTS:OtterGraphicsGeometryObj>
	)
else()
	add_library( OtterGraphicsGeometry
		SHARED 
		$<TARGET_OBJECTS:OtterGraphicsGeometryObj>
	)
endif()

if(INSTALL_LIBRARIES)
	# Install libraries
	install(
		TARGETS OtterGraphicsGeometry
		EXPORT OtterConfig
		LIBRARY DESTINATION ${INSTALL_LIB_DIR}
		ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
		RUNTIME DESTINATION ${INSTALL_BIN_DIR}
		INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
	)
endif()

