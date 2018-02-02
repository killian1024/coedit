cmake_minimum_required(VERSION 3.9)

project(coedit)

set(CMAKE_CXX_STANDARD 17)

find_package(Curses REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(SOURCE_FILES
        src/main.cpp
        src/core/basic_file_editor.hpp
        src/tui/curses_interface.hpp
        src/core/basic_line.hpp
        src/core/fundamental_types.hpp src/core/basic_lines_cache.hpp src/core/basic_characters_buffer.hpp src/core/basic_characters_buffer_cache.hpp src/core/file_editor_command.hpp src/core/core_exception.hpp src/core/line_flags.hpp)

add_executable(coedit ${SOURCE_FILES})

target_link_libraries(coedit -lkboost ${CURSES_LIBRARIES})
