find_package(Curses REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CORE_SOURCE_FILES
        core/basic_file_editor.hpp
        core/basic_line.hpp
        core/fundamental_types.hpp
        core/basic_lines_cache.hpp
        core/basic_characters_buffer.hpp
        core/basic_characters_buffer_cache.hpp
        core/file_editor_command.hpp
        core/core_exception.hpp
        core/line_flags.hpp
        core/newline_format.hpp
        core/cursor_position.hpp
        )

set(TUI_SOURCE_FILES
        tui/curses_interface.hpp
        )
        
add_library(coedit_core STATIC ${CORE_SOURCE_FILES})
set_target_properties(coedit_core PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(coedit_core -lkboost)

add_library(coedit_tui STATIC ${TUI_SOURCE_FILES})
set_target_properties(coedit_tui PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(coedit_tui -lkboost ${CURSES_LIBRARIES})

add_executable(coedit main.cpp)
target_link_libraries(coedit coedit_core coedit_tui -lkboost ${CURSES_LIBRARIES})
