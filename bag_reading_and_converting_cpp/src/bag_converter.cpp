#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/serialization.hpp"
#include "rosbag2_transport/reader_writer_factory.hpp"
#include <point_cloud_interfaces/msg/compressed_point_cloud2.hpp>
#include <point_cloud_transport/point_cloud_codec.hpp>
using namespace std::chrono_literals;

class PlaybackNode : public rclcpp::Node
{
  public:
    PlaybackNode(const std::string & bag_filename, const std::string & topic,const std::string & compressiontype )
    : Node("playback_node")
    {
      std::unique_ptr<rosbag2_cpp::Reader> reader_;
  std::unique_ptr<rosbag2_cpp::Writer> writer_;
  const rclcpp::Serialization<point_cloud_interfaces::msg::CompressedPointCloud2> serialization_;
  rosbag2_storage::StorageOptions storage_options;
  point_cloud_transport::PointCloudCodec codec;
  storage_options.uri = bag_filename;
  reader_ = rosbag2_transport::ReaderWriterFactory::make_reader(storage_options);
  writer_ = std::make_unique<rosbag2_cpp::Writer>();

  writer_->open(bag_filename);
  storage_options.uri = bag_filename;
  reader_->open(storage_options);
  while (reader_->has_next()) {
    rosbag2_storage::SerializedBagMessageSharedPtr msg = reader_->read_next();

    if (msg->topic_name != topic) {
      continue;
    }

    rclcpp::SerializedMessage serialized_msg(*msg->serialized_data);
    rclcpp::SerializedMessage compressed_msg;

    const auto ros_msg = std::make_shared<sensor_msgs::msg::PointCloud2>();
    serialization_.deserialize_message(&serialized_msg, ros_msg.get());
    codec.encode(compressiontype, *ros_msg, compressed_msg);
    writer_->write(compressed_msg,topic+"/"+compressiontype,"point_cloud_interfaces/msg/CompressedPointCloud2",ros_msg->header.stamp);
      }
    }
};

  

int main(int argc, char ** argv)
{
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <bag>" << argc<<std::endl;
    return 1;
  }
  
  //rclcpp::init(argc, argv);
  //rclcpp::spin(std::make_shared<PlaybackNode>(argv[1]));
  //rclcpp::shutdown();
  std::unique_ptr<rosbag2_cpp::Reader> reader_;
  std::unique_ptr<rosbag2_cpp::Writer> writer_;
  const rclcpp::Serialization<sensor_msgs::msg::PointCloud2> serialization_;
  std::string bag_filename = argv[1];
  std::string output_filename =argv[2];
  std::string compressiontype =argv[3];
  rosbag2_storage::StorageOptions storage_options;
  point_cloud_transport::PointCloudCodec codec;
  storage_options.uri = bag_filename;
  reader_ = rosbag2_transport::ReaderWriterFactory::make_reader(storage_options);
  writer_ = std::make_unique<rosbag2_cpp::Writer>();

  writer_->open(output_filename);
  storage_options.uri = bag_filename;
  reader_->open(storage_options);
  auto topics = reader_->get_all_topics_and_types();
  std::string topic_name;
  std::map<std::string, std::string> nametotype;
  for (auto & t : topics) {
    writer_->create_topic(t);
    if (t.type == "sensor_msgs/msg/PointCloud2") {
      topic_name = t.name;
      t.name=topic_name+"/"+compressiontype;
      t.type="point_cloud_interfaces/msg/CompressedPointCloud2";
      writer_->create_topic(t);
    } else {
      nametotype[t.name] = t.type;
    }
  }
  while (reader_->has_next()) {
    rosbag2_storage::SerializedBagMessageSharedPtr msg = reader_->read_next();
    writer_->write(msg);
    if (msg->topic_name == topic_name) {
    rclcpp::SerializedMessage serialized_msg(*msg->serialized_data);
    rclcpp::SerializedMessage compressed_msg;
    const auto ros_msg = std::make_shared<sensor_msgs::msg::PointCloud2>();
    serialization_.deserialize_message(&serialized_msg, ros_msg.get());
    codec.encode(compressiontype, *ros_msg, compressed_msg);
    writer_->write(compressed_msg,topic_name+"/"+compressiontype,"point_cloud_interfaces/msg/CompressedPointCloud2",ros_msg->header.stamp);
    }
  }
  return 0;
}