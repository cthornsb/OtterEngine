# OtterEngine README

Last updated: April 6th, 2021

This README gives basic information on how to install and use OtterEngine.

## About

OtterEngine is a cross-platform


Author: Cory Thornsberry 

Email: <desertedotter@gmail.com>

Repository: <https://github.com/cthornsb/OtterEngine>

## Acknowledgements

OtterEngine would not be possible without the following resources:

[OpenGL](https://www.opengl.org/)

[OpenGL Extension Wrangler (GLEW)](http://glew.sourceforge.net) or on [github](https://github.com/nigels-com/glew)

[Graphics Library Framework (GLFW)](https://www.glfw.org) or on [github](https://github.com/glfw/glfw)

[Simple OpenGL Image Library (SOIL)](https://github.com/paralin/soil)

[PortAudio](http://www.portaudio.com) or on [github](https://github.com/PortAudio/portaudio)

## Building From Source

### Linux (Ubuntu)

OtterEngine requires the following packages; OpenGL, GLU, GLFW3, GLEW2, SOIL, 
and PortAudio. To install all required packages use:

```bash
    sudo apt install mesa-common-dev libglu1-mesa-dev libglfw3-dev libglew-dev libsoil-dev libportaudio2
```

Once the pre-requisite packages have been installed, OtterEngine may be built:

```bash
    git clone https://github.com/cthornsb/OtterEngine
    cd OtterEngine
    mkdir build
    cd build
    cmake -DINSTALL_LIBRARIES=ON ..
```

OtterEngine uses CMake's built-in FindGLEW and FindOpenGL scripts in order to 
find where OpenGL and GLEW are installed on the system. The *cmake/modules*
directory contains additional scripts which should automatically find GLFW,
SOIL, and PortAudio. If these scripts fail to find the installs (e.g. they were
installed locally), you may specify their root install directory using the 
following CMake variables:

```bash
    -DUSER_GLFW3_ROOT="/path/to/glfw/install"
    -DUSER_SOIL_ROOT="/path/to/soil/install"
    -DUSER_PORT_AUDIO_ROOT="/path/to/portaudio/install"
```

The optional **INSTALL_LIBRARIES** option will install all headers and shared libraries to
the install directory (*OtterEngine/install/* by default). It will also generate CMake
scripts which make it easier for external projects to use the libraries. 

Once all packages are located simply type:

```bash
    make install
```

To install the test executable and the shared libraries and headers.

### Windows

Building the project on Windows is a lot more work because there is no standard way
to install required packages like in Linux. We will have to download them all manually.

#### OpenGL

Your Windows install hopefully already has the required OpenGL headers and dlls. If not,
you will need to look up how to install the correct OpenGL drivers for your hardware first.
*[This](https://www.khronos.org/opengl/wiki/Getting_Started)* is probably a good starting point.

#### OpenGL Extnesion Wrangler (GLEW)

Pre-compiled Windows binaries may be downloaded *[here](http://glew.sourceforge.net/index.html)*.
Make sure to download the latest 64-bit binaries and extract the archive somewhere convenient
because we will need the path later.

#### Graphics Library Framework (GLFW)

Similarly, GLFW binaries can be downloaded *[here](https://www.glfw.org/download)*.

#### Simple OpenGL Image Library (SOIL)

The SOIL source code can be downloaded *[here](https://github.com/paralin/soil)*. Unfortunately,
no pre-compiled binaries exist at this time, so they will need to be built using Visual Studio
or MinGW.

#### PortAudio

Like SOIL, there are no pre-compiled binaries for PortAudio on Windows. The source code can be
downloaded from *[here](https://github.com/PortAudio/portaudio/releases)*.

#### OtterEngine

Finally we are ready to install OtterEngine. Using Visual Studio, clone the 
*[OtterEngine repository](https://github.com/cthornsb/gbcemu)* and open it. Generate
the CMakeCache and look at the output from CMake. Since we had to install most of the 
libraries manually, CMake will almost certainly not be able to find them and will fail.
Open the CMake settings and in the *CMake command arguments:* field, input the following:

```bash
    -DUSER_GLEW_ROOT="/path/to/glew/install"
    -DUSER_GLFW3_ROOT="/path/to/glfw/install"
    -DUSER_SOIL_ROOT="/path/to/soil/install"
    -DUSER_PORT_AUDIO_ROOT="/path/to/portaudio/install"
```

Setting the root install paths for the downloaded libraries will hopefully help CMake 
find the required libraries. 

Additionally, the *-INSTALL_LIBRARIES=ON* option will install all headers and shared 
libraries to the install directory (*OtterEngine/install/* by default). It will also 
generate CMake scripts which make it easier for external projects to use the libraries. 

Save the CMake settings and hopefully CMake will complete successfully. Once this is
done, build the project by clicking *Build->Build All* followed by *Build->Install OtterEngine* 
to install the headers and static libraries to the install directory (*OtterEngine/out/install/* 
by default).

### Additional Build Options

| Option                   | Default | Description |
|--------------------------|---------|-------------|
| CMAKE_BUILD_TYPE         | Release | CMake build type: None Debug Release RelWithDebInfo MinSizeRel |
| INSTALL_DIR              |         | Install Prefix                                              |
| INSTALL_LIBRARIES        | OFF     | Install OtterEngine libraries                               |
| INSTALL_STATIC_LIBRARIES | OFF     | Build static libraries instead of shared (Linux)            |
| ASSETS_DIRECTORY         |         | Path string supplied as a target definition at compile time |

Since most OtterEngine classes use graphical or audio libraries, you will almost
always want to set **CMAKE_BUILD_TYPE** to *Release* or *RelWithDebInfo* for
maximum performance. Using build type *Debug* may severely impact framerates.

If **INSTALL_DIR** is not specified, the install directory will usually default 
to *./install* on Linux and *./out/install/* on Windows, but it may vary by device.

On Linux, CMake will generate shared libraries by default, but you may change this
behavior by setting *-DINSTALL_STATIC_LIBRARIES=ON*. Static Linux libraries may be 
desirable in some cases.

## Using OtterEngine Libraries

OtterEngine generates configuration scripts which you may use in your own projects to
easily access OtterEngine graphics and audio classes as well as more easily locating
the required packages OpenGL, GLFW, GLEW, SOIL, and PortAudio. By default, this script
is installed in the base install directory [*OtterEngine/install* on linux and *OtterEngine/out/install*
on Windows]. This configuration script may be used in external projects.

In your project's *CMakeLists.txt*, include the following:

```cmake
    #Set OtterEngine install directory.
    if(NOT OTTER_DIRECTORY)
	    set(OTTER_DIRECTORY "" CACHE STRING "OtterEngine install directory" FORCE)
    endif(NOT OTTER_DIRECTORY)

    #Find required packages (sourced from OtterEngine)
    include("${OTTER_DIRECTORY}/OtterConfig.cmake")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OTTER_MODULE_PATH})

    #Include pre-requisite finder module
    include(PreRequisites)
```

The file *OtterConfig.cmake* sets your CMake environment to more easily include
the libraries from OtterEngine as well as finding the graphical and audio packages
that they require. The configuration script sets the following path variables:

| Path Variable      | Description                                 |
|--------------------|---------------------------------------------|
| OTTER_INCLUDE_DIRS | Include directories for OtterEngine classes |
| OTTER_LIBRARY_DIR  | Shared/static library install directory     |
| OTTER_BINARY_DIR   | Binary install directory                    |
| OTTER_MODULE_PATH  | Path to OtterEngine cmake modules           |

As well as the following libraries:

| Library          | Associated Path  | Description                         |
|------------------|------------------|-------------------------------------|
| Ott::OtterCore   | OTTER_CORE_LIB   | Core graphics library (2d graphics) |
| Ott::Otter3dCore | OTTER_3DCORE_LIB | Core 3d graphics library            |
| Ott::OtterAudio  | OTTER_AUDIO_LIB  | Audio output management library     |
| Ott::OtterMath   | OTTER_MATH_LIB   | Math library, including vectors and matrices |

These libraries are static on Windows and shared on Linux (by default, but you
can make them static by setting the CMake variable *-DINSTALL_STATIC_LIBRARIES=ON*). 

The configuration script also exports the **USER_<package>_ROOT** variables so
that downstream projects do not need to set them manually. 

The file *PreRequisites.cmake* contains many functions which automatically find 
packages and set CMake variables for libraries and include directories. The 
included CMake functions are listed in the table below.

| Function                | Description
|-------------------------|--------------------|
| ott_use_core()          | Enable the use of 2d graphics (requires opengl, glfw3, glew, and soil)  |
| ott_use_3d()            | Enable the use of 2d and 3d graphics (requires opengl, glfw3, glew, and soil) |
| ott_use_audio()         | Enable the use of audio classes (requires PortAudio) |
| ott_find_all_packages() | Find all required packages |
| ott_find_glfw()         | Find GLFW3         |
| ott_find_opengl()       | Find OpenGL        |
| ott_find_glew()         | Find GLEW          |
| ott_find_soil()         | Find SOIL          |
| ott_find_port_audio()   | Find PortAudio     |

```cmake
    #Find all required graphics packages
    ott_use_core()
```

If you have done everything properly, you can use any of the defined include 
directory paths in your CMake project:

```cmake
    #Add the global include directories.
    include_directories(
	    ${GLFW3_INCLUDE_DIRS}
	    ${OPENGL_INCLUDE_DIR}
	    ${GLEW_INCLUDE_DIRS}
	    ${SOIL_INCLUDE_DIRS}
	    ${PORT_AUDIO_INCLUDE_DIRS}
	    ${OTTER_INCLUDE_DIRS}
    )
```

or by using *target_include_directories*. Additionally, you may link against any 
of the graphical and audio libraries such as:

```cmake
    target_link_libraries( <TARGET>
		Ott::OtterCore
		Ott::Otter3dCore 
		Ott::OtterAudio
		Ott::OtterMath	
		OpenGL::GL
		OpenGL::GLU
		GLEW::GLEW
		${GLFW3_LIBRARY}
		${SOIL_LIBRARY}
		${PORT_AUDIO_LIBRARY}
    )
```

## Documentation

OtterEngine supports documentation generation using Doxygen. The project includes
a doxygen input file to simplifiy the process. To generate documentation do:

```bash
    cd OtterEngine
    doxygen Doxyfile
```

Doxygen will generate documentation into a directory named *html*. Open *html/index.html*
in a browser to access the documentation.
