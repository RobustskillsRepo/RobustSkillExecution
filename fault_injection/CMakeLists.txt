cmake_minimum_required(VERSION 3.8)
project(fault_injection)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

install(PROGRAMS scripts/fault_injection_node.py
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME})
install(DIRECTORY docs
  DESTINATION share/${PROJECT_NAME}
  OPTIONAL)

ament_package()
