set(MODULE xcom)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} DIRSRCS)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR})
add_library(${MODULE} SHARED ${DIRSRCS})
target_link_libraries(${MODULE} utils)