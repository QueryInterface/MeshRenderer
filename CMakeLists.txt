cmake_minimum_required(VERSION 2.8.11)
project(MeshRenderer)

# Global variables
set(3RDPARTY_DIR 3rdParty)

# Add projects
add_subdirectory(3rdParty)
add_subdirectory(Utils)

if(CMAKE_SIZEOF_VOID_P EQUAL 8) 
    set(ARCH_64 TRUE) 
else() 
    set(ARCH_64 FALSE) 
endif()

set(DX_DIR $ENV{DXSDK_DIR})

# Add libraries
find_library(d3d9 d3d9)
add_library(d3dx9 STATIC IMPORTED )
if (ARCH_64)
	set_target_properties(d3dx9 PROPERTIES IMPORTED_LOCATION ${DX_DIR}Lib\\x64\\d3dx9.lib)
else()
	set_target_properties(d3dx9 PROPERTIES IMPORTED_LOCATION ${DX_DIR}Lib\\x86\\d3dx9.lib)
endif()

# Include directories
include_directories(SYSTEM ${DX_DIR}\\Include)
include_directories(${3RDPARTY_DIR}/freetype/include)

# Form executable
add_executable(MeshRenderer
			  IRenderable.h
			  MathBasics.h
              main.cpp
              common.h
              Sprite.h
              Sprite.cpp
              RenderContext.h
              RenderContext.cpp
              Console.h
              Console.cpp
			  LayoutManager.h
			  LayoutManager.cpp)

target_link_libraries(MeshRenderer d3d9 d3dx9 freetype Utils)

# Organize projects in folders
set_property (GLOBAL PROPERTY USE_FOLDERS ON)
set_property (TARGET MeshRenderer	PROPERTY FOLDER "Executables")
set_property (TARGET Utils			PROPERTY FODLER "Libraries")
set_property (TARGET freetype		PROPERTY FOLDER "3rdParty/freetype")
set_property (TARGET dist			PROPERTY FOLDER "3rdParty/freetype")

# Set output folder
set_target_properties(MeshRenderer	PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries)
set_target_properties(freetype		PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries)
set_target_properties(Utils			PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries)

# Post-build steps
file(COPY Media DESTINATION ${CMAKE_BINARY_DIR}/Binaries)
