add_subdirectory(freetype)

set_property (TARGET freetype PROPERTY FOLDER "3rdParty/freetype")
set_property (TARGET dist PROPERTY FOLDER "3rdParty/freetype")

set_target_properties(freetype PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries) 