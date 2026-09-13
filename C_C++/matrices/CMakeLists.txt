cmake_minimum_required(VERSION 3.20)

project(matrices)

find_package(Eigen3 REQUIRED)

# Puntos 1, 3 y 4
add_executable(matrices main_jimmy_sebastian.cpp)
target_link_libraries(matrices PRIVATE Eigen3::Eigen)

# Punto 2
add_executable(punto2 punto2_sistema_10x10.cpp)
target_link_libraries(punto2 PRIVATE Eigen3::Eigen)

# Punto 5
add_executable(punto5 punto5_minimos_cuadrados.cpp)
target_link_libraries(punto5 PRIVATE Eigen3::Eigen)