cmake_minimum_required(VERSION 3.10)
project(graph_builder)

set(CMAKE_CXX_STANDARD 11)

add_executable(graph_builder src/main.cpp src/build_mapgraph.cpp src/build_mapgraph.h src/test_serialized_file.cpp src/test_serialized_file.h)

# Dependencies
# MapGraph and Graph Serializer
target_link_libraries(graph_builder ims::map_graph)

# RoutingKit
find_path(RoutingKit_INCLUDE_DIRS routingkit/all.h)
find_library(RoutingKit_LIBRARIES libroutingkit.so)
include_directories("${RoutingKit_INCLUDE_DIRS}")
target_link_libraries(graph_builder ${RoutingKit_LIBRARIES})