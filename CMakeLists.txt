cmake_minimum_required (VERSION 2.8)

project (render3d)

if(NOT WIN32) # Unix systems (Ubuntu)
	#Compile with c++11 or c++0x depending on the compiler version
	message(STATUS "Building for Linux")
	if (CMAKE_COMPILER_IS_GNUCXX)
		if (${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER "4.3" AND
				${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS "4.7")
				set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
		elseif(${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER "4.6")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
		else()
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
		endif(${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER "4.3" AND
				${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS "4.7")

		#Make compiler messages nice with colored tags.
		if (${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER "4.9")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto")
		endif()

		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -fPIC")
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

#if(SHARED_LIBS)
#	option(SHARED_
#endif(SHARED_LIBS)

#------------------------------------------------------------------------------

#Find required packages.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(TOP_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_definitions(-DTOP_DIRECTORY="${TOP_DIRECTORY}")

#Set the graphics library
if(NOT GRAPHICS_LIBRARY)
	set(GRAPHICS_LIBRARY OpenGL CACHE STRING "Graphics library, options are: SDL OpenGL" FORCE)
endif()

if(GRAPHICS_LIBRARY MATCHES "OpenGL")
	if(NOT WIN32)
		# To install OpenGL and glfw:
		#(Ubuntu) sudo apt-get install libglu1-mesa-dev libglfw3-dev mesa-common-dev libsoil-dev libglew-dev
		set(GRAPHICS_LIBS -lglfw -lGLU -lGL -lSOIL -lGLEW)
	else()
		#Find OpenGL install on Windows (eventually make a CMake module for this)
		#Precompiled glfw libs from 
		if(NOT GLFW_DIRECTORY)
			set(GLFW_DIRECTORY "" CACHE STRING "GLFW install directory" FORCE)
		endif(NOT GLFW_DIRECTORY)
		set(GLFW_INCLUDE ${GLFW_DIRECTORY}/include)
		set(GLFW_LIBRARY ${GLFW_DIRECTORY}/lib/x64/freeglut.lib)
		set(WINDOWS_DLL_LIST ${GLFW_DIRECTORY}/bin/x64/freeglut.dll)
		include_directories(${GLFW_INCLUDE})

		#Set GLEW directory
		if(NOT GLEW_DIRECTORY)
			set(GLEW_DIRECTORY "" CACHE STRING "GLEW install directory" FORCE)
		endif(NOT GLEW_DIRECTORY)
		set(GLEW_INCLUDE ${GLEW_DIRECTORY}/include)
		set(GLEW_LIBRARY ${GLEW_DIRECTORY}/lib/Debug/x64/glew32d.lib)
		set(WINDOWS_DLL_LIST ${WINDOWS_DLL_LIST} ${GLEW_DIRECTORY}/bin/Debug/x64/glew32d.dll)
		include_directories(${GLEW_INCLUDE})
		set(GRAPHICS_LIBS ${GLFW_LIBRARY} ${SOIL_LIBRARY} ${GLEW_LIBRARY} opengl32.lib)
		
		#Set SOIL directory
		if(NOT SOIL_DIRECTORY)
			set(SOIL_DIRECTORY "" CACHE STRING "SOIL install directory" FORCE)
		endif(NOT SOIL_DIRECTORY)
		set(SOIL_INCLUDE ${SOIL_DIRECTORY}/inc)
		set(SOIL_LIBRARY ${SOIL_DIRECTORY}/lib/soil.lib)
		include_directories(${SOIL_INCLUDE})
	endif(NOT WIN32)
	
elseif(GRAPHICS_LIBRARY MATCHES "SDL")
	# Support for SDL is heavily deprecated at this point and needs fixed.
	set(GRAPHICS_LIBS -lSDL2)
	add_definitions(-DUSE_SDL_RENDERER)
endif()

#Add the include directories.
include_directories(graphics/include)
include_directories(math/include)
include_directories(include)

#Add the graphics directory.
add_subdirectory(graphics/source)
add_subdirectory(math/source)

#Add the source directory.
add_subdirectory(source)

#Build/install the miscellaneous stuff
#add_subdirectory(share)
