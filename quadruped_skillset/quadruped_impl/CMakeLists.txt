cmake_minimum_required(VERSION 3.5)
project(quadruped_impl)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-stdlib=libstdc++)
endif()

#--------------------------------------------------
# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(quadruped_example_skillset_interfaces REQUIRED)
find_package(quadruped_example_skillset REQUIRED)
#--------------------------------------------------

include_directories(include)

#--------------------------------------------------------------------

add_library(quadruped_impl
  src/Node.cpp
)

target_link_libraries(quadruped_impl)
ament_target_dependencies(quadruped_impl rclcpp std_msgs quadruped_example_skillset_interfaces quadruped_example_skillset)

install(DIRECTORY include/
  DESTINATION include)

install(
  TARGETS quadruped_impl
  EXPORT quadruped_impl
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

#--------------------------------------------------------------------
# DEBUG

add_library(quadruped_impl_debug
  src/Node.cpp
)


target_compile_definitions(quadruped_impl_debug PUBLIC SKILLSET_DEBUG_MODE)
target_link_libraries(quadruped_impl_debug)
ament_target_dependencies(quadruped_impl_debug rclcpp std_msgs quadruped_example_skillset_interfaces quadruped_example_skillset)

install(
  TARGETS quadruped_impl_debug
  EXPORT quadruped_impl_debug
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

#--------------------------------------------------------------------

add_executable(quadruped_impl_node src/quadruped_impl_node.cpp)
target_link_libraries(quadruped_impl_node
  quadruped_impl
)
install(TARGETS
  quadruped_impl_node
  DESTINATION lib/${PROJECT_NAME})

#--------------------------------------------------------------------
# DEBUG

add_executable(quadruped_impl_node_debug src/quadruped_impl_node.cpp)
target_link_libraries(quadruped_impl_node_debug
  quadruped_impl_debug
)
install(TARGETS
  quadruped_impl_node_debug
  DESTINATION lib/${PROJECT_NAME})

#--------------------------------------------------------------------

set(PROJECT_LIBS quadruped_impl)

ament_export_targets(quadruped_impl)

#--------------------------------------------------------------------

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
