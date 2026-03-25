cmake_minimum_required(VERSION 3.8)
project(skill_execution_manager)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(quadruped_example_skillset_interfaces REQUIRED)
find_package(rclpy REQUIRED)

ament_python_install_package(skill_execution_manager_core)

rosidl_generate_interfaces(${PROJECT_NAME}
  "srv/ExecutePlan.srv"
)

install(PROGRAMS scripts/execution_manager_node.py
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config)
install(DIRECTORY robust_model/
  DESTINATION share/${PROJECT_NAME}/robust_model)
install(DIRECTORY PDDL_plan/
  DESTINATION share/${PROJECT_NAME}/PDDL_plan)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
