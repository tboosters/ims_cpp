cmake_minimum_required(VERSION 3.10)
project(graph_serializer)

set(CMAKE_CXX_STANDARD 11)

add_library(map_graph SHARED src/map_graph.cpp include/ims/map_graph.h src/partition.cpp src/partition.h src/preprocess.cpp src/preprocess.h)
add_library(incident_manager SHARED src/incident_manager.cpp include/ims/incident_manager.h)
add_library(router SHARED src/router.cpp include/ims/router.h)
add_library(ims::map_graph ALIAS map_graph)
add_library(ims::incident_manager ALIAS incident_manager)
add_library(ims::router ALIAS router)

add_executable(map_graph_test tests/map_graph_test.cpp)
add_executable(map_graph_real tests/map_graph_real.cpp)
add_executable(incidents_test tests/incidents_test.cpp)
add_executable(router_test tests/router_test.cpp)

target_include_directories(map_graph PUBLIC ${PROJECT_SOURCE_DIR}/include ../experiment/include)

# pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(map_graph pthread)
target_link_libraries(incident_manager pthread)

# Dependencies
target_link_libraries(router ims::map_graph ims::incident_manager exp::logger)
target_link_libraries(map_graph_test ims::map_graph)
target_link_libraries(map_graph_real ims::map_graph)
target_link_libraries(incidents_test ims::incident_manager)
target_link_libraries(router_test ims::router)

# Boost
find_path(Boost_INCLUDE_DIRS boost/align.hpp)
find_library(Boost_LIBRARIES libboost_thread.so)
include_directories("${Boost_INCLUDE_DIRS}")
target_link_libraries(map_graph ${Boost_LIBRARIES} boost_serialization boost_thread)
target_link_libraries(incident_manager ${Boost_LIBRARIES} boost_thread)


# RoutingKit
find_path(RoutingKit_INCLUDE_DIRS routingkit/all.h)
find_library(RoutingKit_LIBRARIES libroutingkit.so)
include_directories("${RoutingKit_INCLUDE_DIRS}")
target_link_libraries(map_graph ${RoutingKit_LIBRARIES})

