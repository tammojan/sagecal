set (objects
 readsky
 predict
 stationbeam
 predict_withbeam
 transforms
)

include_directories(./ ../Dirac)
foreach (object ${objects})
    add_library(${object}.o ${object}.c)
endforeach (object)

add_library(radio ${objects} Radio.h)
