cmake_policy (SET CMP0054 NEW)
cmake_minimum_required (VERSION 3.10)
project (hlkld2410)

option (HLKLD2410_BUILD_SHARED "Build as a shared object (default ON)" ON)

file (GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file (GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

set (CMAKE_CXX_STANDARD 17)

set(PROJ_VERSION_MAJOR 1)
set(PROJ_VERSION_MINOR 0)
set(PROJ_VERSION_PATCH 0)
set(PROJ_VERSION ${PROJ_VERSION_MAJOR}.${PROJ_VERSION_MINOR}.${PROJ_VERSION_PATCH} )

find_package (Threads REQUIRED)
find_package (Qt6 COMPONENTS Core REQUIRED)
find_package (Qt6 COMPONENTS SerialPort REQUIRED)

if(HLKLD2410_BUILD_SHARED)
    add_library (${PROJECT_NAME} SHARED ${SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJ_VERSION} SOVERSION ${PROJ_VERSION_MAJOR} )
else()
    add_library (${PROJECT_NAME} STATIC ${SOURCES})
endif()
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -pedantic)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries (${PROJECT_NAME} Qt6::Core Qt6::SerialPort)

install (TARGETS ${PROJECT_NAME} DESTINATION lib)
install (FILES ${HEADERS} DESTINATION include)
