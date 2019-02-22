cmake_minimum_required(VERSION 3.10)
project(graph_serializer)

set(CMAKE_CXX_STANDARD 11)

add_library(map_graph SHARED src/map_graph.cpp include/ims/map_graph.h src/partition.cpp src/partition.h src/routing.cpp src/routing.h)
add_library(ims::map_graph ALIAS map_graph)
add_executable(map_graph_test src/map_graph_test.cpp src/partition.cpp src/partition.h)

target_include_directories(map_graph PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Dependencies
target_link_libraries(map_graph_test ims::map_graph)

# Boost
find_path(Boost_INCLUDE_DIRS boost/align.hpp)
find_library(Boost_LIBRARIES libboost_serialization.so)
include_directories("${Boost_INCLUDE_DIRS}")
target_link_libraries(map_graph ${Boost_LIBRARIES})

