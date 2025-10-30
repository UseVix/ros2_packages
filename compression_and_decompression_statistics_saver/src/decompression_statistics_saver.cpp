#include <point_cloud_transport/point_cloud_transport.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <iostream>
#include <fstream> 
#include <point_cloud_interfaces> 
/*
std::string global_name, relative_name, default_param;
   3 if (nh.getParam("/global_name", global_name))
*/
std::string compressiontype;
std::string bonusinfo;
point_cloud_transport::PointCloudCodec codec;
ofstream filewithresults("filewithresults.txt");
int callback(const point_cloud_interfaces::msg::CompressedPointCloud2 & msg){
  //TODO
  sensor_msgs::msg::PointCloud2 decompressed_msg;
  #include <chrono>

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  auto succes=codec.decodeTyped(compressiontype,msg,decompressed_msg);
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  auto duration=std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
  filewithresults << "compressiontype:"+compressiontype+"; duration:"+std::to_string(duration)+"; width:"+std::to_string(msg.width)"; height:"+std::to_string(msg.height)+"; succes:"+(succes ? "true" : "false")+"; bonusinfo:"+bonusinfo+"\n";

}
int main(int argc, char ** argv)
{
  
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("point_cloud_subscriber");
  
  std::string topic;
  
  node->declare_parameter("topic", "world");
  node->declare_parameter("compressiontype", "raw");
  node->declare_parameter("bonusinfo", "");

  node->getParam("topic", topic);
  node->getParam("compressiontype", compressiontype);
  node->getParam("bonusinfo", bonusinfo);

  auto subscription_ = node->create_subscription<point_cloud_interfaces::msg::CompressedPointCloud2>(topic, 10,callback);
  
  auto param_subscriber_ = std::make_shared<rclcpp::ParameterEventHandler>(node);
  auto cb_topic = [node](const rclcpp::Parameter & p) {
        RCLCPP_INFO(
          node->get_logger(), "cb: Received an update to parameter \"%s\" of type %s: \"%s\". Changing subscribed topic",
          p.get_name().c_str(),
          p.get_type_name().c_str(),
          p.as_str());
        topic=p.as_str()
        subscription_ = node->create_subscription<point_cloud_interfaces::msg::CompressedPointCloud2>(topic, 10,callback);
      };
  auto cb_compressiontype = [node](const rclcpp::Parameter & p) {
        RCLCPP_INFO(
          node->get_logger(), "cb: Received an update to parameter \"%s\" of type %s: \"%s\".",
          p.get_name().c_str(),
          p.get_type_name().c_str(),
          p.as_str());
        compressiontype=p.as_str()
      };
  auto cb_bonusinfo = [node](const rclcpp::Parameter & p) {
        RCLCPP_INFO(
          node->get_logger(), "cb: Received an update to parameter \"%s\" of type %s: \"%s\".",
          p.get_name().c_str(),
          p.get_type_name().c_str(),
          p.as_str());
        bonusinfo=p.as_str()
      };
  
  auto cb_handle_topic = param_subscriber_->add_parameter_callback("topic", cb_topic);
  auto cb_handle_compressiontype = param_subscriber_->add_parameter_callback("compressiontype", cb_compressiontype);
  auto cb_handle_bonusinfo = param_subscriber_->add_parameter_callback("bonusinfo", cb_bonusinfo);
  
  

  rclcpp::spin(node);

  rclcpp::shutdown();

  return 0;
}