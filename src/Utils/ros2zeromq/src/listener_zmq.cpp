/* rosrun ros2zeromq listener_zmq */

#include <ros/ros.h>
#include <zmq.hpp>
#include <traj_utils/PolyTraj.h>
#include <string.h>
#include <iostream>
#define ZMQ2NS_PORT 6666
#define BUF_LEN 1048576    // 1MB
#define BUF_LEN_SHORT 1024 // 1KB

using namespace std;

traj_utils::PolyTraj polytraj_msg_;
ros::Subscriber other_odoms_sub_, one_traj_sub_;
char udp_recv_buf_[BUF_LEN], udp_send_buf_[BUF_LEN];

//  Prepare our context and publisher
zmq::context_t context(1);
zmq::socket_t zmq2ns3(context, ZMQ_PUB);

enum MESSAGE_TYPE
{
  ODOM = 100,
  MULTI_TRAJ,
  ONE_TRAJ
} massage_type_;

void init(const int port){
  string addr = "tcp://*:";
  addr.append(to_string(port));
  ROS_INFO_STREAM(addr);

  zmq2ns3.bind(addr);
}

template <typename T>
int serializeTopic(const MESSAGE_TYPE msg_type, const T &msg)
{
  auto ptr = (uint8_t *)(udp_send_buf_);

  *((MESSAGE_TYPE*)ptr) = msg_type;
  ptr += sizeof(MESSAGE_TYPE);

  namespace ser = ros::serialization;
  uint32_t msg_size = ser::serializationLength(msg);

  *((uint32_t *)ptr) = msg_size;
  ptr += sizeof(uint32_t);

  ser::OStream stream(ptr, msg_size);
  ser::serialize(stream, msg);

  return msg_size + sizeof(MESSAGE_TYPE) + sizeof(uint32_t);
}


void one_traj_sub_udp_cb(const traj_utils::PolyTrajPtr &msg){

  int len = serializeTopic(MESSAGE_TYPE::ONE_TRAJ, *msg);
  zmq::message_t message(len);
  memcpy(message.data(), udp_send_buf_, len);

  //ROS_INFO_STREAM(message);
  zmq2ns3.send(message);

}

int main(int argc, char** argv) {
  ros::init(argc, argv, "listener_zmq");
  ros::NodeHandle nh("~");

  one_traj_sub_ = nh.subscribe("/broadcast_traj_from_planner", 100, one_traj_sub_udp_cb, ros::TransportHints().tcpNoDelay());

  init(ZMQ2NS_PORT);

  ros::Duration(0.1).sleep();
  ros::spin();

  return 0;
}
