# Build OTTApplication test executable.
add_executable( apptest 
	apptest.cpp
)
target_link_libraries( apptest 
	OtterCore
	OtterSystem
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
	OtterSystem
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

# Build OTTSpriteSet test executable.
add_executable( animtest 
	animtest.cpp
)
target_link_libraries( animtest 
	OtterCore
	OtterSystem
	OtterGui
	OtterMath	
	OpenGL::GL
	OpenGL::GLU
	${GLFW3_LIBRARY}
	GLEW::GLEW
	${SOIL_LIBRARY}
)
if(WIN32)
	target_link_libraries( animtest 
		Winmm
	)
endif()

#Build OTTBitmap and OTTCharacterMap test program
add_executable( consoletest 
	consoletest.cpp
)
target_link_libraries( consoletest 
	OtterCore
	OtterSystem
	OtterGui
	OtterMath	
	OpenGL::GL
	OpenGL::GLU
	${GLFW3_LIBRARY}
	GLEW::GLEW
	${SOIL_LIBRARY}
)
if(WIN32)
	target_link_libraries( consoletest 
		Winmm
	)
endif()

#Build OTTString test program
add_executable( stringtest 
	stringtest.cpp
)
target_link_libraries( stringtest 
	OtterCore
	OtterSystem
	OtterGui
	OtterMath	
	OpenGL::GL
	OpenGL::GLU
	${GLFW3_LIBRARY}
	GLEW::GLEW
	${SOIL_LIBRARY}
)
if(WIN32)
	target_link_libraries( stringtest 
		Winmm
	)
endif()

#Build OTTString test program
add_executable( randtest 
	randtest.cpp
)
target_link_libraries( randtest 
	OtterCore
	OtterSystem
	OtterGui
	OtterMath	
	OpenGL::GL
	OpenGL::GLU
	${GLFW3_LIBRARY}
	GLEW::GLEW
	${SOIL_LIBRARY}
)
if(WIN32)
	target_link_libraries( randtest 
		Winmm
	)
endif()