cmake_minimum_required(VERSION 3.1)
project(demo_nanomsg)


list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/..")
include(libnanomsg_static)


set(binary_name_pub puber)
add_executable(${binary_name_pub} ${CMAKE_CURRENT_SOURCE_DIR}/puber.c)
target_link_libraries(${binary_name_pub} nanomsg)
if(WIN32)
        target_compile_options(${binary_name_pub} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

set(binary_name_client suber)
add_executable(${binary_name_client} ${CMAKE_CURRENT_SOURCE_DIR}/suber.c)
target_link_libraries(${binary_name_client} nanomsg)
if(WIN32)
        target_compile_options(${binary_name_client} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()




