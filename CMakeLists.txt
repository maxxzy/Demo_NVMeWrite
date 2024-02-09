cmake_minimum_required(VERSION 3.5) # CMake install : https://cmake.org/download/
project(demo_window LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "/home/xzy/Qt5.12.8/5.12.8/gcc_64") # Qt Kit Dir
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED) # Qt COMPONENTS
find_package(Qt5 COMPONENTS Charts REQUIRED)

aux_source_directory(./src srcs)

# Specify MSVC UTF-8 encoding   
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_executable(${PROJECT_NAME}
    WIN32 # If you need a terminal for debug, please comment this statement 
    ${srcs} 
) 
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Charts) # Qt5 Shared Library