# project info
cmake_minimum_required(VERSION 3.18)
project(server LANGUAGES CXX)

# general stuff
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# useful directories
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)


# find opencv and openssl
find_package(OpenCV REQUIRED)
find_package(OpenSSL REQUIRED)

# set where all the stuff goes
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# add thread support
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# include folders
include_directories("${PROJECT_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}" "${OpenCV_INCLUDE_DIRS}")

# the exe
add_executable(server ${PROJECT_SOURCE_DIR}/stream.cpp ${PROJECT_SOURCE_DIR}/nano.cpp ${PROJECT_SOURCE_DIR}/comm.cpp ${PROJECT_SOURCE_DIR}/main.cpp)

#set_target_properties(server PROPERTIES
#    COMPILE_OPTIONS "$<$<CONFIG:Debug>:/MTd>$<$<CONFIG:Release>:/MT>"
#    )
# link libraries
target_link_libraries(server ${OpenCV_LIBS} Threads::Threads OpenSSL::SSL)
#TARGET_LINK_LIBRARIES(client ${LIBS} ${OpenCV_LIBS})
