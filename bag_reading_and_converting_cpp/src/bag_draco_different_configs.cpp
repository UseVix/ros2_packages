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
#include <draco_point_cloud_transport/draco_publisher.hpp>

using namespace std::chrono_literals;

/*
class PlaybackNode : public rclcpp::Node
{
  public:
    PlaybackNode(const std::string & bag_filename, const std::string & output_filename,const std::string & compressiontype, const int EncodeSpeed,const int DecodeSpeed,const int QuantizationPosition)
    : Node("playback_node")
    {
      std::unique_ptr<rosbag2_cpp::Reader> reader_;
  std::unique_ptr<rosbag2_cpp::Writer> writer_;
  const rclcpp::Serialization<point_cloud_interfaces::msg::CompressedPointCloud2> serialization_;
  rosbag2_storage::StorageOptions storage_options;
  ConfigurableDracoPublisher codec;

  // Declare DracoPublisher parameters on this node so set_parameter works
  
  // Optionally, list all parameters for debug
  declare_parameter<int>("asdadasdas",42);
  auto result = this->list_parameters({}, 10);
  
  for (const auto & name : result.names) {
    RCLCPP_INFO(this->get_logger(), "Parameter: %s", name.c_str());
  }
  storage_options.uri = bag_filename;
  reader_ = rosbag2_transport::ReaderWriterFactory::make_reader(storage_options);
  writer_ = std::make_unique<rosbag2_cpp::Writer>();
  set_parameter(rclcpp::Parameter("param_topic/encode_speed", EncodeSpeed));
  this->set_parameter(rclcpp::Parameter("param_topic/decode_speed", DecodeSpeed));
  this->set_parameter(rclcpp::Parameter("param_topic/quantization_POSITION", QuantizationPosition));
  reader_ = rosbag2_transport::ReaderWriterFactory::make_reader(storage_options);
  writer_ = std::make_unique<rosbag2_cpp::Writer>();

  writer_->open(output_filename);
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

    if (nametotype[msg->topic_name] != "sensor_msgs/msg/PointCloud2") {
      continue;
    }

    rclcpp::SerializedMessage serialized_msg(*msg->serialized_data);
    //rclcpp::SerializedMessage compressed_msg;
    auto compressed_msg = std::make_shared<rclcpp::SerializedMessage>();
    const auto ros_msg = std::make_shared<sensor_msgs::msg::PointCloud2>();
    serialization_.deserialize_message(&serialized_msg, ros_msg.get());
    compressed_msg = codec.encode(*ros_msg).value().value();
    writer_->write(*compressed_msg,msg->topic_name+"/"+compressiontype+"_ES:"+std::to_string(EncodeSpeed)+"_DS:"+std::to_string(DecodeSpeed)+"_QP:"+std::to_string(QuantizationPosition),"point_cloud_interfaces/msg/CompressedPointCloud2",ros_msg->header.stamp);
      }
    }
};
*/
  

int main(int argc, char ** argv)
{
  if (argc != 7) {
    std::cerr << "Usage: " << argv[0] << " <bag> <output> <compression_type> <encode_speed> <decode_speed> <quantization_position>" << std::endl;
    return 1;
  }
  
  //rclcpp::init(argc, argv);
  //rclcpp::spin(std::make_shared<PlaybackNode>(argv[1],argv[2],argv[3],std::stoi(argv[4]),std::stoi(argv[5]),std::stoi(argv[6])));
  //rclcpp::shutdown();
    //rclcpp::init(argc, argv);
  //rclcpp::spin(std::make_shared<PlaybackNode>(argv[1]));
  //rclcpp::shutdown();
  std::unique_ptr<rosbag2_cpp::Reader> reader_;
  std::unique_ptr<rosbag2_cpp::Writer> writer_;

  const rclcpp::Serialization<sensor_msgs::msg::PointCloud2> serialization_;

  std::string bag_filename = argv[1];
  std::string output_filename =argv[2];
  std::string compressiontype =argv[3];
  int EncodeSpeed = std::stoi(argv[4]);
  int DecodeSpeed = std::stoi(argv[5]);
  int QuantizationPosition = std::stoi(argv[6]);

  rosbag2_storage::StorageOptions storage_options;
  storage_options.uri = bag_filename;
  storage_options.storage_id = "sqlite3";

  reader_ = rosbag2_transport::ReaderWriterFactory::make_reader(storage_options);
  writer_ = std::make_unique<rosbag2_cpp::Writer>();

  writer_->open(output_filename);
  reader_->open(storage_options);

  draco_point_cloud_transport::DracoPublisher codec;
  codec.config_.encode_speed=EncodeSpeed;
  codec.config_.decode_speed=DecodeSpeed;
  codec.config_.quantization_POSITION=QuantizationPosition;
  codec.config_.force_quantization=true;
    auto topics = reader_->get_all_topics_and_types();
  std::string topic_name;
  std::map<std::string, std::string> nametotype;
  std::string output_topic;
  for (auto & t : topics) {
    //writer_->create_topic(t);
    if (t.type == "sensor_msgs/msg/PointCloud2") {
      topic_name = t.name;
      t.name=topic_name+"/"+compressiontype+"_ES:"+std::to_string(EncodeSpeed)+"_DS:"+std::to_string(DecodeSpeed)+"_QP:"+std::to_string(QuantizationPosition);
      output_topic = t.name;
      t.type="point_cloud_interfaces/msg/CompressedPointCloud2";
      writer_->create_topic(t);
    } else {
      nametotype[t.name] = t.type;
    }
  }
  while (reader_->has_next()) {
    rosbag2_storage::SerializedBagMessageSharedPtr msg = reader_->read_next();
    //std::cout << "Reading message from topic: " << msg->topic_name << std::endl;
    if (msg->topic_name != topic_name) {
      continue;
    }
    rclcpp::SerializedMessage serialized_msg(*msg->serialized_data);
    rclcpp::SerializedMessage compressed_msg_ser;
    const auto ros_msg = std::make_shared<sensor_msgs::msg::PointCloud2>();
    serialization_.deserialize_message(&serialized_msg, ros_msg.get());
    const auto compressed_msg = codec.encode(*ros_msg);
    if (!compressed_msg) {
      std::cerr << "Error during compression: " << compressed_msg.error() << std::endl;
    }
    if (!compressed_msg.value()) {
      std::cerr << "Error during compression: " << compressed_msg.error() << std::endl;
    }
    compressed_msg_ser = *(compressed_msg.value()->get());
    writer_->write(compressed_msg_ser,output_topic,"point_cloud_interfaces/msg/CompressedPointCloud2",ros_msg->header.stamp);
      }
  return 0;
}