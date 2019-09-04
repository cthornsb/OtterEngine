# Render3d README

[Cory R. Thornsberry](https://github.com/cthornsb)

Last updated: September 4th, 2019

This README gives basic information on how to install Render3d from source.

## Author

- Cory R Thornsberry

## Compilation

- Required Prerequisites:
	- [cmake](https://cmake.org/) version 2.18.12 or later is confirmed to work
	- [SDL](https://www.libsdl.org/download-2.0.php) version 2.0 is required
		- Use `sudo apt-get install libsdl2-dev` on Ubuntu

- Recommended Prerequisites:
	- [ccmake](https://cmake.org/cmake/help/latest/manual/ccmake.1.html)
		- Use `sudo apt-get install cmake-curses-gui` on Ubuntu
	- [Environment Modules](http://modules.sourceforge.net/)
	
```bash
git clone https://github.com/cthornsb/Render3d
cd Render3d
mkdir build
cd build
cmake ..
```

Once CMake has successfully generated the makefile,
simply type


```bash
make install
```

to compile and install the program.

### Installing the Module File

By default, cmake will install binaries to

Render3d/install/bin

From there you may make symbolic links as required, or simply run
the executable in place e.g.

```bash
./Render3d/install/bin/renderer
```
