include_directories(./ ../lib/Radio ../lib/Dirac)

FILE(GLOB SRCFILES *.cpp)
# foreach (cppfile ${SRCFILES})
#     get_filename_component(fname ${cppfile} NAME_WE)
#     add_library(${fname}.o SHARED ${cppfile})
# endforeach (cppfile)

add_executable(sagecalApp ${SRCFILES})
target_compile_options(sagecalApp PUBLIC "-pthread")

#link_directories(/home/fdiblen/.pyenv/versions/anaconda3-4.3.1/pkgs/openblas-0.2.20-4/lib)

target_link_libraries(sagecalApp
${CASACORE_LIBRARIES}
${BLAS_LIBRARIES}
${LAPACK_LIBRARIES}
${WCSLIB_LIBRARIES}
-lgfortran -lpthread
)
