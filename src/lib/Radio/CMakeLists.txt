set (objects
 readsky
 predict
 stationbeam
 predict_withbeam
 transforms
)

include_directories(./)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Dirac)
include_directories(${GLIB_PKG_INCLUDE_DIRS})

foreach (object ${objects})
    add_library(${object} ${object}.c)
endforeach (object)

add_library(radio ${objects} Radio.h)

target_link_libraries(radio
${CASACORE_LIBRARIES}
${BLAS_LIBRARIES}
${LAPACK_LIBRARIES}
${WCSLIB_LIBRARIES}
${GLIB_PKG_LIBRARIES}
)
