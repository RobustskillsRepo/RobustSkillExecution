cmake_minimum_required(VERSION 3.8)
project(deliberative_layer)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(skill_execution_manager REQUIRED)
find_package(rclpy REQUIRED)

ament_python_install_package(${PROJECT_NAME})

install(PROGRAMS scripts/deliberative_node.py
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY PDDL_models/
  DESTINATION share/${PROJECT_NAME}/PDDL_models)

install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
