cmake_minimum_required(VERSION 3.21)
project(mks_log)

set(MKS_LOG_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
list(APPEND CMAKE_MODULE_PATH ${MKS_LOG_CMAKE_DIR})

set(LOAD_VALID TRUE)
find_package(Threads)
message(STATUS "Threads_FOUND: ${Threads_FOUND}")
message(STATUS "CMAKE_THREAD_LIBS_INIT: ${CMAKE_THREAD_LIBS_INIT}")
message(STATUS "CMAKE_USE_WIN32_THREADS_INIT: ${CMAKE_USE_WIN32_THREADS_INIT}")
message(STATUS "CMAKE_USE_PTHREADS_INIT: ${CMAKE_USE_PTHREADS_INIT}")
message(STATUS "CMAKE_HP_PTHREADS_INIT: ${CMAKE_HP_PTHREADS_INIT}")
message(STATUS "THREADS_PREFER_PTHREAD_FLAG: ${THREADS_PREFER_PTHREAD_FLAG}")
message("")
if(NOT Threads_FOUND)
    message(WARNING "Could not find Threads")
    set(LOAD_VALID FALSE)
endif()

find_package(fmt)
message(STATUS "fmt_FOUND: ${fmt_FOUND}")
message(STATUS "fmt_INCLUDE_DIR: ${fmt_INCLUDE_DIR}")
message(STATUS "fmt_INCLUDE_DIRS: ${fmt_INCLUDE_DIRS}")
message(STATUS "fmt_LIBRARY: ${fmt_LIBRARY}")
message(STATUS "fmt_LIBRARIES: ${fmt_LIBRARIES}")
message(STATUS "fmt_VERSION_STRING: ${fmt_VERSION_STRING}")
message("")
if(NOT fmt_FOUND)
    message(WARNING "Could not find fmt")
    set(LOAD_VALID FALSE)
endif()

if(NOT LOAD_VALID)
    message(WARNING "Could not load ${PROJECT_NAME}")
    return()
endif()

file(GLOB_RECURSE SOURCE_FILES FILES_MATCHING PATTERN "${CMAKE_CURRENT_SOURCE_DIR}/src/*")
add_library(mks_log STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(mks_log ${fmt_LIBRARY} Threads::Threads)
set_property(TARGET mks_log PROPERTY CXX_STANDARD 17)
target_include_directories(mks_log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${fmt_INCLUDE_DIR})
target_compile_definitions(mks_log PRIVATE MKS_LOG_ROOT_PROJECT_PATH=\"${PROJECT_ROOT_DIR}/\")

add_subdirectory(example)



