# find_package (Threads REQUIRED)
# find_package (Qt6 COMPONENTS Core REQUIRED)

add_executable(test main.cpp testrun.cpp)
# target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}")
target_link_libraries(test PRIVATE ${PROJECT_NAME})
