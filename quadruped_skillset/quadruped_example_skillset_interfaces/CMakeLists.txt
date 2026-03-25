cmake_minimum_required(VERSION 3.5)
project(quadruped_example_skillset_interfaces)

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

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

#--------------------------------------------------
find_package(rosidl_default_generators REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(std_msgs REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
#========== GENERIC ==========
  # ----- Data -----
  "msg/DataRequest.msg"
  # ----- Event -----
  "msg/EventRequest.msg"
  "msg/EventResponse.msg"
  # ----- SkillSet -----
  "msg/SkillsetStatus.msg"
  "msg/ResourceState.msg"
  # ----- Skill -----
  "msg/SkillInterrupt.msg"
  # "msg/SkillResult.msg"
  # "msg/SkillTransition.msg"
#========== SPECIFIC ==========
# ----- data location -----
  "msg/DataLocation.msg"
  "msg/DataLocationResponse.msg"
# ----- data battery -----
  "msg/DataBattery.msg"
  "msg/DataBatteryResponse.msg"

# ----- Skill init_power -----
  
  
  "msg/SkillInitPowerRequest.msg"
  "msg/SkillInitPowerResponse.msg"
  
  "msg/SkillInitPowerStatus.msg"
# ----- Skill safe_poweroff -----
  
  
  "msg/SkillSafePoweroffRequest.msg"
  "msg/SkillSafePoweroffResponse.msg"
  
  "msg/SkillSafePoweroffStatus.msg"
# ----- Skill standup -----
  
  
  "msg/SkillStandupRequest.msg"
  "msg/SkillStandupResponse.msg"
  
  "msg/SkillStandupStatus.msg"
# ----- Skill sitdown -----
  
  
  "msg/SkillSitdownRequest.msg"
  "msg/SkillSitdownResponse.msg"
  
  "msg/SkillSitdownStatus.msg"
# ----- Skill go_to -----
  "msg/SkillGoToInput.msg"
  
  "msg/SkillGoToRequest.msg"
  "msg/SkillGoToResponse.msg"
  
  "msg/SkillGoToStatus.msg"
# ----- Skill recharge -----
  
  
  "msg/SkillRechargeRequest.msg"
  "msg/SkillRechargeResponse.msg"
  
  "msg/SkillRechargeStatus.msg"
# ----- Skill take_picture -----
  
  
  "msg/SkillTakePictureRequest.msg"
  "msg/SkillTakePictureResponse.msg"
  
  "msg/SkillTakePictureStatus.msg"

  DEPENDENCIES builtin_interfaces std_msgs
)
#--------------------------------------------------

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
