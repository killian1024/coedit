find_package(Curses REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CORE_SOURCE_FILES
        core/basic_file_editor.hpp
        core/basic_line.hpp
        core/fundamental_types.hpp
        core/basic_line_cache.hpp
        core/basic_character_buffer.hpp
        core/basic_character_buffer_cache.hpp
        core/file_editor_command.hpp
        core/core_exception.hpp
        core/newline_format.hpp
        core/cursor_position.hpp
        core/basic_id_buffer_cache.hpp 
        core/basic_id_buffer.hpp
        )

set(TUI_SOURCE_FILES
        tui/curses_interface.hpp
        tui/curses_interface.cpp
        tui/curses_colors.hpp
        )

set(SYSTEM_SOURCE_FILES
        system/server.hpp
        system/client.hpp
        system/client.cpp
        system/client_data.hpp
        system/server_session.hpp
        system/system_exception.hpp
        system/simple.hpp
        system/simple.cpp
        system/server.cpp
        system/server_session.cpp
        system/tcp_segment.hpp
        system/client_data.cpp
        )
        
add_library(coedit_core STATIC ${CORE_SOURCE_FILES})
set_target_properties(coedit_core PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(coedit_core -lkboost -lstdc++fs)

add_library(coedit_tui STATIC ${TUI_SOURCE_FILES})
set_target_properties(coedit_tui PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(coedit_tui -lkboost ${CURSES_LIBRARIES})

add_library(coedit_system STATIC ${SYSTEM_SOURCE_FILES})
set_target_properties(coedit_system PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(coedit_system coedit_tui -lkboost -lpthread -lstdc++fs)

add_executable(coedit_client main_client.cpp)
target_link_libraries(coedit_client coedit_core coedit_tui coedit_system)

add_executable(coedit_server main_server.cpp)
target_link_libraries(coedit_server coedit_core coedit_tui coedit_system)

add_executable(coedit_simple main_simple.cpp)
target_link_libraries(coedit_simple coedit_core coedit_tui coedit_system)
