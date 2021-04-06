# Locate the glfw3 library
#
# This module defines the following variables:
#
# GLFW3_LIBRARY the name of the library;
# GLFW3_INCLUDE_DIRS where to find glfw include files.
# GLFW3_FOUND true if both the GLFW3_LIBRARY and GLFW3_INCLUDE_DIRS have been found.
# GLFW3_DLL_DIRECTORY (on Windows only)
# GLFW3_DLL_PATH (on Windows only)
#
# To help locate the library and include file, you can define a
# variable called USER_GLFW3_ROOT which points to the root of the glfw library
# installation.
#
# default search dirs
# 
# Cmake file from: https://github.com/daw42/glslcookbook

set( _glfw3_HEADER_SEARCH_DIRS
	"/usr/include"
	"/usr/local/include"
	"${CMAKE_SOURCE_DIR}/includes"
	"C:/Program Files (x86)/glfw/include" 
)
set( _glfw3_LIB_SEARCH_DIRS
	"/usr/lib"
	"/usr/local/lib"
	"${CMAKE_SOURCE_DIR}/lib"
	"C:/Program Files (x86)/glfw/lib-vc2019" 
)
set( _glfw3_BIN_SEARCH_DIRS
	"C:/Program Files (x86)/glfw/lib-vc2019" 
)

# Check environment for root search directory
set( _glfw3_ENV_ROOT $ENV{USER_GLFW3_ROOT} )
if( NOT USER_GLFW3_ROOT AND _glfw3_ENV_ROOT )
	set(USER_GLFW3_ROOT ${_glfw3_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( USER_GLFW3_ROOT )
	list( INSERT _glfw3_HEADER_SEARCH_DIRS 0 "${USER_GLFW3_ROOT}/include" )
	list( INSERT _glfw3_LIB_SEARCH_DIRS 0 "${USER_GLFW3_ROOT}/lib-vc2019" )
	list( INSERT _glfw3_BIN_SEARCH_DIRS 0 "${USER_GLFW3_ROOT}/lib-vc2019" )
endif()

# Search for the header
FIND_PATH(GLFW3_INCLUDE_DIRS "GLFW/glfw3.h"
PATHS ${_glfw3_HEADER_SEARCH_DIRS} )

# Search for the library
FIND_LIBRARY(
	GLFW3_LIBRARY 
	NAMES 
	glfw3 
	glfw
	PATHS 
	${_glfw3_LIB_SEARCH_DIRS}
)

if(WIN32)
	# Search for dll install
	find_file( GLFW3_DLL_PATH
		NAMES "glfw3.dll"
		PATHS ${_glfw3_BIN_SEARCH_DIRS} 
	)	
	if(GLFW3_DLL_PATH)
		get_filename_component( GLFW3_DLL_DIRECTORY
			${GLFW3_DLL_PATH}
			DIRECTORY
		)
	endif()
endif()

# Remove some unnecessary clutter
mark_as_advanced(
	GLFW3_INCLUDE_DIRS
	GLFW3_LIBRARY
)

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
	GLFW3 
	DEFAULT_MSG
	GLFW3_LIBRARY 
	GLFW3_INCLUDE_DIRS
)

