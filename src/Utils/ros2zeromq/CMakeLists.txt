cmake_minimum_required(VERSION 2.8.3)
project(ros2zeromq)

set(CMAKE_CXX_FLAGS "-std=c++11  ${CMAKE_CXX_FLAGS}")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake CACHE INTERNAL "" FORCE)

find_package(ZeroMQ)
find_package(catkin REQUIRED COMPONENTS
  roscpp
  traj_utils
)

catkin_package(
 CATKIN_DEPENDS traj_utils
)

include_directories(include ${catkin_INCLUDE_DIRS} ${ZeroMQ_INCLUDE_DIRS})

add_executable(listener_zmq src/listener_zmq.cpp)
target_link_libraries(listener_zmq ${catkin_LIBRARIES} ${ZeroMQ_LIBRARIES})


add_executable(listener src/listener.cpp)
target_link_libraries(listener ${catkin_LIBRARIES} ${ZeroMQ_LIBRARIES})