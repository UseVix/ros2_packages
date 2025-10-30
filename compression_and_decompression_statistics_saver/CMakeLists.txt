cmake_minimum_required(VERSION 3.8)
project(compression_and_decompression_statistics_saver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(rclcpp REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(point_cloud_interfaces REQUIRED)
find_package(point_cloud_transport REQUIRED)
find_package(rosbag2_transport REQUIRED)
find_package(point_cloud_transport_plugins REQUIRED)
find_package(cloudini_lib REQUIRED)
find_package(zlib_point_cloud_transport REQUIRED)
find_package(zstd_point_cloud_transport REQUIRED)
find_package(draco_point_cloud_transport REQUIRED)
find_package(sensor_msgs REQUIRED)

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

add_executable(decompression_statistics_saver src/decompression_statistics_saver.cpp)
ament_target_dependencies(decompression_statistics_saver rclcpp rosbag2_transport sensor_msgs point_cloud_interfaces point_cloud_transport)

install(TARGETS
  decompression_statistics_saver
  DESTINATION lib/${PROJECT_NAME}
)
