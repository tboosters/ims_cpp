cmake_minimum_required(VERSION 3.10)
project(cpp_server_cppcms)

include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 11)

add_executable(cpp_server_cppcms src/main.cpp src/IMSApp.cpp src/IMSApp.h src/utils.cpp src/utils.h)
target_link_libraries(cpp_server_cppcms ims::map_graph)
target_link_libraries(cpp_server_cppcms ims::incident_manager)

# Dependencies
# Booster
find_path(Booster_INCLUDE_DIRS booster/config.h)
find_library(Booster_LIBRARIES booster)
include_directories("${Booster_INCLUDE_DIRS}")
target_link_libraries(cpp_server_cppcms ${Booster_LIBRARIES})

# CPPCMS
find_path(CPPCMS_INCLUDE_DIRS cppcms/config.h)
find_library(CPPCMS_LIBRARIES cppcms)
include_directories("${CPPCMS_INCLUDE_DIRS}")
target_link_libraries(cpp_server_cppcms ${CPPCMS_LIBRARIES})