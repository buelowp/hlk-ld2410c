cmake_policy (SET CMP0054 NEW)
cmake_minimum_required (VERSION 3.10)

project (hlkld2410)

set (THREADS_PREFER_PTHREAD_FLAG ON)
set (CMAKE_THREAD_PREFER_PTHREAD TRUE)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)

add_subdirectory (src)
add_subdirectory (example)
