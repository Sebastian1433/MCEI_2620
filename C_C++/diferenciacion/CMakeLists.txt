cmake_minimum_required( VERSION 3.20 )
project( diferenciacion CXX )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release )
endif()

find_package( PkgConfig REQUIRED )
pkg_check_modules( GSL REQUIRED gsl )

add_executable( ${PROJECT_NAME} parte3_cpp.cpp )
target_include_directories( ${PROJECT_NAME} PRIVATE ${GSL_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} ${GSL_LIBRARIES} m )

# El programa lee el CSV desde su directorio de trabajo
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/trayectoria_robot.csv
                ${CMAKE_CURRENT_BINARY_DIR}/trayectoria_robot.csv COPYONLY )