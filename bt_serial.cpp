#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <string>

class BTSerialNode : public rclcpp::Node
{
public:
  BTSerialNode() : Node("bt_serial_node")
  {
    serial_fd_ = open("/dev/rfcomm0", O_RDWR | O_NOCTTY | O_SYNC);
    if (serial_fd_ < 0) {
      RCLCPP_ERROR(this->get_logger(), "Failed to open /dev/rfcomm0");
      return;
    }

    // 送信（ROSトピック受け取り → ESP32に送信）
    sub_ = this->create_subscription<std_msgs::msg::String>(
      "bt_send", 10,
      [this](std_msgs::msg::String::SharedPtr msg) {
        write(serial_fd_, msg->data.c_str(), msg->data.size());
        write(serial_fd_, "\n", 1); // 改行（ESP32側がreadStringUntil('\n')のため）
      });

    // 受信（ESP32 → ROSトピックで出力）
    pub_ = this->create_publisher<std_msgs::msg::String>("bt_receive", 10);

    timer_ = this->create_wall_timer(
      std::chrono::milliseconds(100),
      [this]() {
        char buf[256];
        int n = read(serial_fd_, buf, sizeof(buf));
        if (n > 0) {
          std_msgs::msg::String msg;
          msg.data = std::string(buf, n);
          pub_->publish(msg);
        }
      });
  }

private:
  int serial_fd_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<BTSerialNode>());
  rclcpp::shutdown();
  return 0;
}
