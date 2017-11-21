include_directories(./ ../lib)

FILE(GLOB SRCFILES *.cpp)
foreach (cppfile ${SRCFILES})
    get_filename_component(fname ${cppfile} NAME_WE)
    add_library(${fname}.o SHARED ${cppfile})
endforeach (cppfile)

# add_executable(sagecal $(SRCFILES))

# add_library(main.o SHARED ${object}.c)
#
# add_library(dirac STATIC ${objects} Dirac.h)
