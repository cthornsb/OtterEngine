# Locate the soil library
#
# This module defines the following variables:
#
# SOIL_LIBRARY the name of the library;
# SOIL_INCLUDE_DIR where to find glfw include files.
# SOIL_FOUND true if both the SOIL_LIBRARY and SOIL_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you can define a
# variable called SOIL_ROOT which points to the root of the SOIL library
# installation.
#
# default search dirs

set( _SOIL_HEADER_SEARCH_DIRS
	"/usr/include"
	"/usr/local/include"
)
set( _SOIL_LIB_SEARCH_DIRS
	"/usr/lib"
	"/usr/local/lib"
)

# Check environment for root search directory
set( _SOIL_ENV_ROOT $ENV{SOIL_ROOT} )
if( NOT SOIL_ROOT AND _SOIL_ENV_ROOT )
	set(SOIL_ROOT ${_SOIL_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( SOIL_ROOT )
	list( INSERT _SOIL_HEADER_SEARCH_DIRS 0 "${SOIL_ROOT}/inc" )
	list( INSERT _SOIL_LIB_SEARCH_DIRS 0 "${SOIL_ROOT}/lib" )
endif()

# Search for the header
FIND_PATH(
	SOIL_INCLUDE_DIR 
	"SOIL/SOIL.h"
	PATHS 
	${_SOIL_HEADER_SEARCH_DIRS} 
)

# Search for the library
FIND_LIBRARY(
	SOIL_LIBRARY 
	NAMES 
	SOIL 
	soil
	PATHS 
	${_SOIL_LIB_SEARCH_DIRS} 
)
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	SOIL 
	DEFAULT_MSG
	SOIL_LIBRARY 
	SOIL_INCLUDE_DIR
)

