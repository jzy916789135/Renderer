cmake_minimum_required(VERSION 3.19)

set(PROJECT_NAME Renderer)
project(${PROJECT_NAME} LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories("../Renderer/Libraries/Includes")

link_libraries("../../Renderer/Libraries/Libs/glfw3")
link_libraries("../../Renderer/Libraries/Libs/assimpd")

file(GLOB HEADER_FILES "Source/*.h")
file(GLOB SOURCE_FILES "Source/*.cpp")
file(GLOB CPP_FILES "Source/*.c")

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${CPP_FILES})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Bin)


set(CMAKE_COMMAND xcopy /I /E /Y /D "\"${CMAKE_SOURCE_DIR}/Renderer/Shader\""  "\"${CMAKE_SOURCE_DIR}/Bin/$(Configuration)/Shader\"")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND})
set(CMAKE_COMMAND xcopy /I /E /Y /D "\"${CMAKE_SOURCE_DIR}/Renderer/Texture\""  "\"${CMAKE_SOURCE_DIR}/Bin/$(Configuration)/Texture\"")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND})
set(CMAKE_COMMAND xcopy /I /E /Y /D "\"${CMAKE_SOURCE_DIR}/Renderer/Model\""  "\"${CMAKE_SOURCE_DIR}/Bin/$(Configuration)/Model\"")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND})

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin/$(Configuration)")