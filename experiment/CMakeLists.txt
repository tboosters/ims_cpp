cmake_minimum_required(VERSION 3.10)
project(experiment)

set(CMAKE_CXX_STANDARD 11)

add_library(logger SHARED include/exp_log.h src/exp_log.cpp)
add_library(exp::logger ALIAS logger)

add_executable(experiment src/experiment.cpp)

# Dependencies
# MapGraph and Graph Serializer
target_link_libraries(experiment ims::map_graph)
target_link_libraries(experiment ims::incident_manager)
target_link_libraries(experiment ims::router)
target_link_libraries(experiment exp::logger)