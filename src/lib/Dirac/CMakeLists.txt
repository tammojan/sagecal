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

include_directories(./)
foreach (object ${objects})
    add_library(${object}.o ${object}.c)
endforeach (object)

add_library(dirac ${objects} Dirac.h)
