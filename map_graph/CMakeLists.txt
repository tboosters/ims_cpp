cmake_minimum_required(VERSION 3.10)
project(graph_serializer)

set(CMAKE_CXX_STANDARD 11)

add_library(map_graph SHARED src/map_graph.cpp include/ims/map_graph.h)
add_library(ims::map_graph ALIAS map_graph)

target_include_directories(map_graph PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Dependencies
# Boost
find_path(Boost_INCLUDE_DIRS boost/align.hpp)
find_library(Boost_LIBRARIES libboost_serialization.so)
include_directories("${Boost_INCLUDE_DIRS}")
target_link_libraries(map_graph ${Boost_LIBRARIES})

