# project info
cmake_minimum_required(VERSION 3.18)
project(client LANGUAGES CXX)

# general stuff
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(OpenCV_DIR "C:\\Program Files\\opencv\\build")

# useful directories
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# OpenSLL directories
#set(OPENSSL_ROOT_DIR "C:\\Program Files\\OpenSSL-Win64")
#set(OPENSSL_INCLUDE_DIR "C:\\Program Files\\OpenSSL-Win64\\include")
#set(OPENSSL_LIBRARIES "C:\\Program Files\\OpenSSL-Win64\\lib")
find_package(OpenSSL REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

# ignore for now but keep
#set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/lib/opencv)
#find_package(OpenCV REQUIRED)
#include_directories( ${OpenCV_INCLUDE_DIRS})

# idk even know man
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/artifacts)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/artifacts)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/artifacts)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/artifacts)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/artifacts)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../..)

# set libs
set(LIBS XInput)

# include folders
include_directories("${PROJECT_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}" "${OPENSSL_INCLUDE_DIR}")

# the exe
add_executable(client ${PROJECT_SOURCE_DIR}/controller.cpp ${PROJECT_SOURCE_DIR}/comm.cpp ${PROJECT_SOURCE_DIR}/client.cpp)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
#target_compile_options(client PUBLIC /MT)
#target_link_options(client PUBLIC /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
    

# file(COPY ${PROJECT_SOURCE_DIR}/build/artifacts/client.exe DESTINATION ../)

# link libraries
TARGET_LINK_LIBRARIES(client ${LIBS} ${OPENSSL_LIBRARIES} ${OpenCV_LIBS})
#TARGET_LINK_LIBRARIES(client ${LIBS} ${OpenCV_LIBS})