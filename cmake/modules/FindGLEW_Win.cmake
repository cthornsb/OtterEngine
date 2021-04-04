# Locate the glew library
#
# This module defines the following variables:
#
# GLEW_STATIC_LIBRARIES 
# GLEW_SHARED_LIBRARIES
# GLEW_DLL_DIRECTORY
# GLEW_INCLUDE_DIRS where to find glfw include files.
# GLEW_FOUND true if both the GLEW_LIBRARIES and GLEW_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you can define a
# variable called USER_GLEW_ROOT which points to the root of the GLEW library
# installation.
#

# Default search dirs
# These will almost certainly not work
set( _GLEW_HEADER_SEARCH_DIRS
	"C:/Program Files (x86)/glew-2.1.0/include"
)
set( _GLEW_LIB_SEARCH_DIRS
	"C:/Program Files (x86)/glew-2.1.0/lib/x64"
)
set( _GLEW_BIN_SEARCH_DIRS
	"C:/Program Files (x86)/glew-2.1.0/bin/x64"
)

# Check environment for root search directory
set( _GLEW_ENV_ROOT $ENV{USER_GLEW_ROOT} )
if( NOT USER_GLEW_ROOT AND _GLEW_ENV_ROOT )
	set(USER_GLEW_ROOT ${_GLEW_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( USER_GLEW_ROOT )
	list( INSERT _GLEW_HEADER_SEARCH_DIRS 0 "${USER_GLEW_ROOT}/include" )
	list( INSERT _GLEW_LIB_SEARCH_DIRS 0 "${USER_GLEW_ROOT}/lib/x64" "${USER_GLEW_ROOT}/lib/Debug/x64" )
	list( INSERT _GLEW_BIN_SEARCH_DIRS 0 "${USER_GLEW_ROOT}/bin/x64" "${USER_GLEW_ROOT}/bin/Debug/x64" )
endif()

# Search for the header
FIND_PATH( GLEW_INCLUDE_DIRS
	NAMES "GL/glew.h"
	PATHS ${_GLEW_HEADER_SEARCH_DIRS} 
)

# Search for dll install
find_path( GLEW_DLL_DIRECTORY
	NAMES "glew32.dll" "glew32d.dll"
	PATHS ${_GLEW_BIN_SEARCH_DIRS} 
)

# Search for the library
FIND_LIBRARY( GLEW_SHARED_LIBRARIES 
	NAMES "glew32" "glew32d"
	PATHS ${_GLEW_LIB_SEARCH_DIRS} 
)

# And the static library
FIND_LIBRARY( GLEW_STATIC_LIBRARIES 
	NAMES "glew32s" "glew32sd"
	PATHS ${_GLEW_LIB_SEARCH_DIRS} 
)

# Set glew library
if(GLEW_SHARED_LIBRARIES)
	add_library(GLEW::glew STATIC IMPORTED GLOBAL)
	set_property(
		TARGET GLEW::glew
		PROPERTY IMPORTED_LOCATION ${GLEW_SHARED_LIBRARIES}
	)
endif()

# Set glew_s library
if(GLEW_STATIC_LIBRARIES)
	add_library(GLEW::glew_s STATIC IMPORTED GLOBAL)
	set_property(
		TARGET GLEW::glew_s
		PROPERTY IMPORTED_LOCATION ${GLEW_STATIC_LIBRARIES}
	)
endif()

# Set GLEW_LIBRARY and GLEW aliases
if(GLEW_SHARED_LIBRARIES)
	set(GLEW_LIBRARY ${GLEW_SHARED_LIBRARIES})
	add_library(GLEW::GLEW ALIAS GLEW::glew)
elseif(GLEW_STATIC_LIBRARIES)
	set(GLEW_LIBRARY ${GLEW_STATIC_LIBRARIES})
	add_library(GLEW::GLEW ALIAS GLEW::glew_s)
else()
	message(FATAL_ERROR "Failed to find GLEW library")
endif()

# Remove some unnecessary clutter
mark_as_advanced(
	GLEW_INCLUDE_DIRS
	GLEW_SHARED_LIBRARIES
	GLEW_STATIC_LIBRARIES
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	GLEW_Win
	DEFAULT_MSG
	GLEW_LIBRARY 
	GLEW_INCLUDE_DIRS
)
