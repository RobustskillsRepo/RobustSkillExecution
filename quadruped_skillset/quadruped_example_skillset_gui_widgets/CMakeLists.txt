cmake_minimum_required(VERSION 3.5)
project(quadruped_example_skillset_gui_widgets)

# Default to C++20
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
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
find_package(gui_tools REQUIRED)
find_package(quadruped_example_skillset_interfaces REQUIRED)
#--------------------------------------------------

include_directories(include)

#--------------------------------------------------------------------

add_library(quadruped_example_skillset_gui_widgets SHARED
  src/client.cpp
  src/widget.cpp
)

target_include_directories(quadruped_example_skillset_gui_widgets PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(quadruped_example_skillset_gui_widgets 
  rclcpp std_msgs gui_tools
  quadruped_example_skillset_interfaces
)

install(DIRECTORY include/
  DESTINATION include)

install(
  TARGETS quadruped_example_skillset_gui_widgets
  EXPORT quadruped_example_skillset_gui_widgets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

#--------------------------------------------------------------------

add_executable(quadruped_example_skillset_gui_widgets_node src/main.cpp)
target_link_libraries(quadruped_example_skillset_gui_widgets_node
  quadruped_example_skillset_gui_widgets
)
install(TARGETS
  quadruped_example_skillset_gui_widgets_node
  DESTINATION lib/${PROJECT_NAME})

#--------------------------------------------------------------------

set(PROJECT_LIBS quadruped_example_skillset_gui_widgets)

ament_export_targets(quadruped_example_skillset_gui_widgets)

#--------------------------------------------------------------------

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
