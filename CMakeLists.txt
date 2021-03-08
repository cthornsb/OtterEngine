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
endif(NOT CMAKE_BUILD_TYPE)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

#Add additional CXX flags if we use the Debug option
if(CMAKE_BUILD_TYPE MATCHES "Debug")
	if(NOT WIN32)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
	endif()
endif(CMAKE_BUILD_TYPE MATCHES "Debug")

#------------------------------------------------------------------------------

#Install options
if(WIN32)
	option(INSTALL_DLLS "Install required DLLs when installing executable." ON)
endif(WIN32)

option(INSTALL_STATIC "Install static libraries." OFF)
option(INSTALL_SHARED "Build and install shared libraries." OFF)

#------------------------------------------------------------------------------

#Find required packages.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

#Include helper macro
include(PreRequisites)

# To install OpenGL and glfw on Ubunut (apt)
# sudo apt install libglu1-mesa-dev libglfw3-dev mesa-common-dev libsoil-dev libglew-dev

#Find glfw, OpenGL, GLU, GLEW, SOIL, and portaudio
ott_find_all_packages()

include_directories(${GLFW3_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${SOIL_INCLUDE_DIR})
include_directories(${PORT_AUDIO_INCLUDE_DIR})

#Add the include directories.
include_directories(graphics/include)
include_directories(audio/include)
include_directories(math/include)
include_directories(include)

#Add the graphics directory.
add_subdirectory(graphics/source)
add_subdirectory(audio/source)
add_subdirectory(math/source)

#Add the source directory.
add_subdirectory(source)

#Build/install the miscellaneous stuff
#add_subdirectory(share)
