include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/Dirac)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/Radio)
include_directories(./)

link_directories(./)
link_directories(${LIBRARY_OUTPUT_PATH})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/Dirac)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/Radio)

FILE(GLOB SRCFILES *.cpp)
add_executable(sagecal ${SRCFILES})

target_link_libraries(sagecal
${CASACORE_LIBRARIES}
${BLAS_LIBRARIES}
${LAPACK_LIBRARIES}
${WCSLIB_LIBRARIES}
-lpthread
-lm
-ldirac
#-lsagecal
-lradio
)
