# Add all test programs
add_executable( apptest 
	apptest.cpp
)
add_executable( guitest 
	guitest.cpp
)
add_executable( animtest 
	animtest.cpp
)
add_executable( consoletest 
	consoletest.cpp
)
add_executable( stringtest 
	stringtest.cpp
)
add_executable( randtest 
	randtest.cpp
)
add_executable( synthtest 
	synthtest.cpp
)

# Build a list of all programs
set( TestPrograms 
	apptest 
	guitest 
	animtest
	consoletest
	stringtest
	randtest
	synthtest
)

foreach(PROG IN LISTS TestPrograms)
	target_link_libraries( ${PROG}
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
		target_link_libraries( ${PROG}
			Winmm
		)
	endif()
endforeach()

target_link_libraries( synthtest
	OtterAudio
	${PORT_AUDIO_LIBRARY}
)