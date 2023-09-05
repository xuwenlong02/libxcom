set(MODULE utils)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} DIRSRCS)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include)
add_library(${MODULE} ${DIRSRCS})