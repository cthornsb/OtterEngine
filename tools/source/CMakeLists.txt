
if(STRICT_OPENGL_VERSION)
	# Build renderer executable.
	set(VERSION_FINDER_SOURCE glVersionFinder.cpp)
	add_executable( glVersionFinder 
		${VERSION_FINDER_SOURCE}
		VersionNumber.cpp
	)
	target_include_directories( glVersionFinder
		PUBLIC
		"../include"
	)
	target_link_libraries( glVersionFinder 
		OpenGL::GL
		OpenGL::GLU
		${GLFW3_LIBRARY}
		GLEW::GLEW
	)

	set(VERSION_HEADER_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "Auto-generated graphics library version header")
	set(VERSION_HEADER_FILENAME "${VERSION_HEADER_DIR}/OTTVersions.hpp")
	mark_as_advanced(VERSION_HEADER_DIR)

	get_filename_component(
		GLFW_LIB_DIRECTORY
		${GLFW3_LIBRARY}
		DIRECTORY 
	)

	if(NOT WIN32)
		add_custom_command(
			COMMAND glVersionFinder ${CMAKE_CURRENT_BINARY_DIR}
			OUTPUT ${VERSION_HEADER_FILENAME}
			DEPENDS glVersionFinder
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		)
	else()
		add_custom_command(
			COMMAND set "PATH=${GLEW_DLL_DIRECTORY};${GLFW_LIB_DIRECTORY};%PATH%"
			COMMAND glVersionFinder ${CMAKE_CURRENT_BINARY_DIR}
			OUTPUT ${VERSION_HEADER_FILENAME}
			DEPENDS glVersionFinder
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		)
	endif()

	add_custom_target(
		OtterVersionHeader
		DEPENDS glVersionFinder ${VERSION_HEADER_FILENAME}
	)
endif(STRICT_OPENGL_VERSION)
