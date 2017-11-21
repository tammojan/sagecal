set (objects
  lmfit_nocuda
  clmfit_nocuda
  lbfgs_nocuda
  myblas
  residual
  robustlm
  updatenu
  robust_lbfgs_nocuda
  rtr_solve
  rtr_solve_robust
  manifold_average
  consensus_poly
  rtr_solve_robust_admm
  admm_solve
)
set (ALL_OBJECTS "")

include_directories(./)
foreach (object ${objects})
    # add_executable (${test} ${test}.cc)
    # target_link_libraries (${test} casa_tables)
    # add_test (${test} ${CMAKE_SOURCE_DIR}/cmake/cmake_assay ./${test})
    # add_dependencies(check ${test})

    add_library(${object}.o ${object}.c)
#    list (APPEND ALL_OBJECTS ${object})

endforeach (object)



add_library(dirac STATIC ${objects} Dirac.h)
