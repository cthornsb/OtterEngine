# Build OTTApplication test executable.
add_executable( apptest 
	apptest.cpp
)
target_link_libraries( apptest 
	OtterCore
	OtterGui
	OtterMath	
	OpenGL::GL
	OpenGL::GLU
	${GLFW3_LIBRARY}
	GLEW::GLEW
	${SOIL_LIBRARY}
)
if(WIN32)
	target_link_libraries( apptest 
		Winmm
	)
endif()

# Build OTTGui test executable.
add_executable( guitest 
	guitest.cpp
)
target_link_libraries( guitest 
	OtterCore
	OtterGui
	OtterMath	
	OpenGL::GL
	OpenGL::GLU
	${GLFW3_LIBRARY}
	GLEW::GLEW
	${SOIL_LIBRARY}
)
if(WIN32)
	target_link_libraries( guitest 
		Winmm
	)
endif()