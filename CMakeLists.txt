cmake_minimum_required (VERSION 3.8)

project("MathEngine")

# Add all headers and code files
file(GLOB_RECURSE FILES_SRC "src/*.cpp" "src/*.h")

add_executable(MathEngine ${FILES_SRC})

# Set C++ version to 20
set_target_properties(MathEngine PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(MathEngine PROPERTIES CXX_STANDARD 20)

# Add all data files to copy over to build dir
file(GLOB_RECURSE FILES_DATA "data/*")
foreach(FILE IN LISTS FILES_DATA)
	# Make absolute path relative
	string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} "" FILE_REL ${FILE})
	add_custom_command(
        TARGET MathEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/${FILE_REL}
                ${CMAKE_CURRENT_BINARY_DIR}/${FILE_REL})

	configure_file(${FILE} ${FILE_REL})
endforeach()