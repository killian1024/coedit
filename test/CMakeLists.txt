project(coedit_test)

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/../src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(coedit_test
        main.cpp
        core_test/basic_file_editor_test.cpp)

target_link_libraries(coedit_test -lkboost ${GTEST_BOTH_LIBRARIES} -lpthread)
target_link_libraries(coedit_test coedit_core coedit_tui)
