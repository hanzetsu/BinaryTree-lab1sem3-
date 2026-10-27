cmake_minimum_required(VERSION 3.14)
project(Lab1_BinaryTree)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(tree_lib INTERFACE)
target_include_directories(tree_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(main main.cpp)
target_link_libraries(main PRIVATE tree_lib)

add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE tree_lib GTest::gtest_main)