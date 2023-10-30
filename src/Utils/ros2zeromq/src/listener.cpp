#include <ros/ros.h>
#include <zmq.hpp>
#include <traj_utils/PolyTraj.h>
#include <string.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  ros::init(argc, argv, "listener", ros::init_options::AnonymousName);
  
  zmq::context_t context(1);
  zmq::socket_t subscriber(context, ZMQ_SUB);

  std::string TOPIC = "";
  subscriber.setsockopt(ZMQ_SUBSCRIBE, TOPIC.c_str(), TOPIC.length());  // allow all messages

  int linger = 0;  // Proper shutdown ZeroMQ
  subscriber.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

  subscriber.connect("tcp://localhost:6666");
  cout << "[listener] start running" << endl;
  while (ros::ok()) {
    zmq::message_t message;
    int rc = subscriber.recv(&message);
    cout << "while loop" << endl;
    if (rc) {
      cout << "if" << endl;
      std::string recv_string;

      std::istringstream iss(static_cast<char*>(message.data()));
      iss >> recv_string;
      ROS_INFO_STREAM("I heard " << recv_string);
    }
  }

  // Clean up your socket and context here
  subscriber.close();
  context.close();
  return 0;
}