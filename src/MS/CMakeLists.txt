include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/Dirac)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/Radio)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/Radio/reserve)
include_directories(./)


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
${GLIB_PKG_LIBRARIES}
-lpthread
-lm
-ldirac
-lradio
)
