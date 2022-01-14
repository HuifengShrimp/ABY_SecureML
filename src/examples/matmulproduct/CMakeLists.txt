
add_executable(matmulproduct_test matmulproduct_test.cpp common/matmulproduct.cpp)
target_link_libraries(matmulproduct_test ABY::aby ENCRYPTO_utils::encrypto_utils)
