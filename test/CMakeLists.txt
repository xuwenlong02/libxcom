include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/libxcom ${CMAKE_SOURCE_DIR}/include)

add_executable(basic_test basic_test.cpp)
target_link_libraries(basic_test xcom)
add_test(NAME basic_test COMMAND basic_test)