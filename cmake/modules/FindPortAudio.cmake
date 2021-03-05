# Locate the PORT_AUDIO library
#
# This module defines the following variables:
#
# PORT_AUDIO_LIBRARY the name of the library;
# PORT_AUDIO_INCLUDE_DIR where to find glfw include files.
# PORT_AUDIO_FOUND true if both the PORT_AUDIO_LIBRARY and PORT_AUDIO_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you can define a
# variable called PORT_AUDIO_ROOT which points to the root of the portaudio library
# installation.
#
# default search dirs

set( _PORT_AUDIO_HEADER_SEARCH_DIRS
	"/usr/include"
	"/usr/local/include"
)
set( _PORT_AUDIO_LIB_SEARCH_DIRS
	"/usr/lib"
	"/usr/local/lib"
)

# Check environment for root search directory
set( _PORT_AUDIO_ENV_ROOT $ENV{PORT_AUDIO_ROOT} )
if( NOT PORT_AUDIO_ROOT AND _PORT_AUDIO_ENV_ROOT )
	set(PORT_AUDIO_ROOT ${_PORT_AUDIO_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( PORT_AUDIO_ROOT )
	list( INSERT _PORT_AUDIO_HEADER_SEARCH_DIRS 0 "${PORT_AUDIO_ROOT}/include" )
	list( INSERT _PORT_AUDIO_LIB_SEARCH_DIRS 0 "${PORT_AUDIO_ROOT}/lib/x64/" )
endif()

# Search for the header
FIND_PATH(
	PORT_AUDIO_INCLUDE_DIR 
	"portaudio.h"
	PATHS 
	${_PORT_AUDIO_HEADER_SEARCH_DIRS} 
)

# Search for the library
FIND_LIBRARY(
	PORT_AUDIO_LIBRARY 
	NAMES 
	portaudio 
	portaudio_x64
	PATHS 
	${_PORT_AUDIO_LIB_SEARCH_DIRS}
)
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	PORT_AUDIO 
	DEFAULT_MSG
	PORT_AUDIO_LIBRARY 
	PORT_AUDIO_INCLUDE_DIR
)

