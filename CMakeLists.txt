cmake_minimum_required (VERSION 3.1)

project (OtterEngine)

if(NOT WIN32) # Unix systems (Ubuntu)
	#Compile with c++11 or c++0x depending on the compiler version
	message(STATUS "Building for Linux")
	if (CMAKE_COMPILER_IS_GNUCXX)
		#Make compiler messages nice with colored tags.
		if (${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER "4.9")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto")
		endif()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic -fPIC")
	endif()
else()
	message(STATUS "Building for Windows")
endif()

#if user does not specify prefix we assign it to the exec directory
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
   message(STATUS "Install Prefix not specified.")
   file(MAKE_DIRECTORY install)
   get_filename_component(INSTALL_DIR ${CMAKE_SOURCE_DIR}/install REALPATH)
   set(CMAKE_INSTALL_PREFIX ${INSTALL_DIR} CACHE PATH "Install Prefix" FORCE)
endif()
message(STATUS "Installing to ${CMAKE_INSTALL_PREFIX}")

#Define the default build type to be Release
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Build type, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
   message(STATUS "Build type not defined, using default: ${CMAKE_BUILD_TYPE}")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

#Add additional CXX flags if we use the Debug option
if(CMAKE_BUILD_TYPE MATCHES "Debug")
	if(NOT WIN32)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
	endif()
endif()

#------------------------------------------------------------------------------

#Install options
option(INSTALL_LIBRARIES "Install OtterEngine shared libraries." OFF)
if(NOT WIN32)
	#Build static libraries instead of shared (Linux)
	option(INSTALL_STATIC_LIBRARIES "Build static libraries instead of shared." OFF)
endif()

#------------------------------------------------------------------------------

# Optional assets directory string
if(NOT ASSETS_DIRECTORY)
	set(ASSETS_DIRECTORY "" CACHE STRING "Set optional assets directory for easier file loading." FORCE)
endif()

#Find required packages.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

#Include helper macro
include(PreRequisites)

# To install OpenGL, glu, glfw, glew and soil on Ubuntu (apt)
# sudo apt install libglu1-mesa-dev libglfw3-dev mesa-common-dev libsoil-dev libglew-dev

#Find glfw, OpenGL, GLU, GLEW, SOIL, and portaudio
ott_find_all_packages()

set(INSTALL_BIN_DIR     ${CMAKE_INSTALL_PREFIX}/bin)
set(INSTALL_LIB_DIR     ${CMAKE_INSTALL_PREFIX}/lib)
set(INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(INSTALL_SHARE_DIR   ${CMAKE_INSTALL_PREFIX}/cmake)

set(INCLUDE_DIR_GRAPHICS ${CMAKE_SOURCE_DIR}/graphics/include)
set(INCLUDE_DIR_AUDIO    ${CMAKE_SOURCE_DIR}/audio/include)
set(INCLUDE_DIR_MATH     ${CMAKE_SOURCE_DIR}/math/include)
set(INCLUDE_DIR_3D       ${CMAKE_SOURCE_DIR}/3d/include)

#Add the global include directories.
include_directories(
	${GLFW3_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	${GLEW_INCLUDE_DIRS}
	${SOIL_INCLUDE_DIRS}
	${PORT_AUDIO_INCLUDE_DIRS}
	graphics/include
	audio/include
	math/include
	include
)

#Check OpenGL, GLFW, and GLEW versions
add_subdirectory(tools/source)

#Add the newly generated library versions header
include_directories(${VERSION_HEADER_DIR})

#Add the graphics directory.
add_subdirectory(graphics/source)
add_subdirectory(audio/source)
add_subdirectory(math/source)
add_subdirectory(source)

#Build/install the miscellaneous stuff
# Build and install shared libraries.
if(INSTALL_LIBRARIES)
	# Install header files
	file(GLOB_RECURSE OTTER_HEADERS *.hpp)
	install(
		FILES ${OTTER_HEADERS} "${VERSION_HEADER_DIR}/OTTVersions.hpp"
		DESTINATION ${INSTALL_INCLUDE_DIR}
	)
	# Export libraries
	export(
		TARGETS
		OtterCore
		Otter3dCore 
		OtterAudio
		OtterMath
		NAMESPACE Ott::
		FILE "${INSTALL_SHARE_DIR}/OtterExport.cmake"
	)
	install(
		EXPORT OtterConfig
		DESTINATION "${INSTALL_INCLUDE_DIR}/cmake"
		NAMESPACE Ott::
	)
	# Install configuration file
	configure_file( "share/OtterConfig.cmake.in"
		"${CMAKE_INSTALL_PREFIX}/OtterConfig.cmake" @ONLY
	)
endif()

