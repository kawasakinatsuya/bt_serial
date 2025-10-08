#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <chrono>

using namespace std::chrono_literals;

class BTserialNode : public rclcpp::Node
{
public:
    BTserialNode()
        : Node("BTserial_node")
    {
        int fd = open("/dev/rfcomm0", O_RDWR | O_NOCTTY | O_SYNC);
        if (fd < 0){
            perror("Error opening serial port");
            rclcpp::shutdown();
            return;
        }

        //publishr
        publisher_ = this->create_publisher<std_msgs::msg::String>("bt_receive", 10);
        auto timer_callback = [this]() -> void
        {
            char buffer[256];
            int n = read(this->fd, buffer, std::size(buffer));
            if(n==-1)
            {
                perror("Error opening serial port");
                rclcpp::shutdown();
                return;
            }
            auto message = std_msgs::msg::String();
            message.data = std::string(buffer, n);
            this->publisher_->publish(message);
        };
        timer_ = this->create_wall_timer(5000ms, timer_callback);

        //subscriber
        auto sub_callback = [this](std_msgs::msg::String::SharedPtr msg) -> void
        {
            write(this->fd, msg->data.c_str(), msg->data.size());
            write(this->fd, "\n", 1);
        };
        subscription_ = this->create_subscription<std_msgs::msg::String>("bt_send", 10, sub_callback);
    }

private:
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    int fd;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<BTserialNode>());
    rclcpp::shutdown();
    return 0;
}
