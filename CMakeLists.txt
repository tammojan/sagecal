# SageCal
project (SageCal)
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION_REVISION 1)
set(PROJECT_VERSION
  "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")


cmake_minimum_required (VERSION 2.8)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)
include(CheckFunctionExists)


# ## Load CMAKE configuration from environment variables
# set( CMAKE_MODULE_PATH $ENV{CMAKE_MODULE_PATH} )
# set( CMAKE_PREFIX_PATH $ENV{CMAKE_PREFIX_PATH} )

## Project Output Paths
SET(MAINFOLDER ${PROJECT_SOURCE_DIR})
SET(EXECUTABLE_OUTPUT_PATH "${MAINFOLDER}/bin")
SET(LIBRARY_OUTPUT_PATH "${MAINFOLDER}/lib")


# Add options
option (ENABLE_MPI "Enable MPI support" NO)
# if(USE_OPENMP)
#     set (USE_THREADS YES)
#     find_package (OpenMP)
#     if (OPENMP_FOUND)
#         if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
#             set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -qopenmp")
#             set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qopenmp")
#         else( )
#             set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#             set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#         endif( )
#     else (OPENMP_FOUND)
#         message(WARNING "Cannot fullfill USE_OPENMP, compiler does not support it")
#     endif (OPENMP_FOUND)
# endif(USE_OPENMP)

## Dependencies
find_package(PkgConfig REQUIRED)
find_package(CasaCore REQUIRED)
find_package(LAPACK REQUIRED)
find_package(BLAS REQUIRED)

pkg_check_modules(GLIB_PKG glib-2.0)
if (GLIB_PKG_FOUND)
    include_directories(${GLIB_PKG_INCLUDE_DIRS})
endif()



## Set libs
set(lapackblas_libraries ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES})
## Set flags
set (CMAKE_CXX_FLAGS "-Wall -g -pg -Wextra  -W -Wpointer-arith -Woverloaded-virtual -Wwrite-strings -pedantic -Wno-long-long")

add_subdirectory(src)





# Show summary.
message (STATUS "CMAKE_SYSTEM .......... = ${CMAKE_SYSTEM}")
message (STATUS "CMAKE_BUILD_TYPE ...... = ${CMAKE_BUILD_TYPE}")
message (STATUS "BUILD_SHARED_LIBS ..... = ${BUILD_SHARED_LIBS}")
message (STATUS "CMAKE_INSTALL_NAME_DIR  = ${CMAKE_INSTALL_NAME_DIR}")
message (STATUS "USE_MPI ............... = ${USE_MPI}")
message (STATUS "USE_STACKTRACE ........ = ${USE_STACKTRACE}")
message (STATUS "CMAKE_CXX_COMPILER .... = ${CMAKE_CXX_COMPILER}")
message (STATUS "CMAKE_CXX_FLAGS ....... = ${CMAKE_CXX_FLAGS}")
message (STATUS "C++11 support ......... = ${CXX11}")
message (STATUS "BLAS library? ......... = ${BLAS_LIBRARIES}")
message (STATUS "GLIB? ................. = ${GLIB_PKG_INCLUDE_DIRS}")
message (STATUS "LAPACK library? ....... = ${LAPACK_LIBRARIES}")


message (STATUS "BUILD_PYTHON .......... = ${BUILD_PYTHON}")
message (STATUS "BUILD_PYTHON3 ......... = ${BUILD_PYTHON3}")
