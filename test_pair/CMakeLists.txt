cmake_minimum_required(VERSION 3.1)
project(demo_nanomsg)


list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/..")
include(libnanomsg_static)


set(binary_name_server server)
add_executable(${binary_name_server} ${CMAKE_CURRENT_SOURCE_DIR}/server.c)
target_link_libraries(${binary_name_server} nanomsg)


set(binary_name_client client)
add_executable(${binary_name_client} ${CMAKE_CURRENT_SOURCE_DIR}/client.c)
target_link_libraries(${binary_name_client} nanomsg)





