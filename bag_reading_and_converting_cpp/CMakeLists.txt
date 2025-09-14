cmake_minimum_required(VERSION 3.8)
project(bag_reading_and_converting_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rosbag2_transport REQUIRED)
find_package(point_cloud_transport REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(point_cloud_interfaces REQUIRED)
find_package(draco_point_cloud_transport REQUIRED)
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
add_executable(bag_converter src/bag_converter.cpp)
add_executable(bag_draco_different_configs src/bag_draco_different_configs.cpp)
ament_target_dependencies(bag_converter rclcpp rosbag2_transport sensor_msgs point_cloud_interfaces point_cloud_transport)
ament_target_dependencies(bag_draco_different_configs rclcpp rosbag2_transport sensor_msgs point_cloud_interfaces point_cloud_transport draco_point_cloud_transport)

install(TARGETS
  bag_converter
  bag_draco_different_configs
  DESTINATION lib/${PROJECT_NAME}
)