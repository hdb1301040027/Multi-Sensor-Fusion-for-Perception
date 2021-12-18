/*
 * @Description: 订阅激光点云信息，并解析数据
 * @Author: Ren Qian
 * @Date: 2020-02-05 02:27:30
 */

#include "lidar_localization/subscriber/cloud_subscriber.hpp"

#include "glog/logging.h"
#include <iterator>

namespace lidar_localization {

CloudSubscriber::CloudSubscriber(ros::NodeHandle& nh, std::string topic_name, size_t buff_size)
    :nh_(nh) {
    subscriber_ = nh_.subscribe(topic_name, buff_size, &CloudSubscriber::msg_callback, this);
}

void CloudSubscriber::msg_callback(const sensor_msgs::PointCloud2::ConstPtr& cloud_msg_ptr) {
    buff_mutex_.lock();

    CloudData cloud_data;
    cloud_data.time = cloud_msg_ptr->header.stamp.toSec();
    pcl::fromROSMsg(*cloud_msg_ptr, *(cloud_data.cloud_ptr));

    new_cloud_data_.push_back(std::move(cloud_data));

    buff_mutex_.unlock();
}

void CloudSubscriber::ParseData(std::deque<CloudData>& cloud_data_buff) {
    buff_mutex_.lock();
    
    if (!new_cloud_data_.empty()) {
        std::copy(
            std::make_move_iterator(new_cloud_data_.begin()), 
            std::make_move_iterator(new_cloud_data_.end()), 
            std::back_inserter(cloud_data_buff)
        );

        new_cloud_data_.clear();
    }

    buff_mutex_.unlock();
}

} // namespace data_input