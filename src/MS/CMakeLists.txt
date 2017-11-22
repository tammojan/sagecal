include_directories(./ ../lib/Radio ../lib/Dirac)

FILE(GLOB SRCFILES *.cpp)

add_executable(sagecal ${SRCFILES})

link_directories(./ ../lib/Radio ../lib/Dirac)

target_link_libraries(sagecal
${CASACORE_LIBRARIES}
${BLAS_LIBRARIES}
${LAPACK_LIBRARIES}
${WCSLIB_LIBRARIES}
-lpthread
)
