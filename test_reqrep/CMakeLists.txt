cmake_minimum_required(VERSION 3.1)
project(demo_nanomsg)


list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/..")
include(libnanomsg_static)


set(self_req test_req)
add_executable(${self_req} ${CMAKE_CURRENT_SOURCE_DIR}/req.c)
target_link_libraries(${self_req} nanomsg)
if(WIN32)
        target_compile_options(${self_req} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

set(self_rep test_rep)
add_executable(${self_rep} ${CMAKE_CURRENT_SOURCE_DIR}/rep.c)
target_link_libraries(${self_rep} nanomsg)
if(WIN32)
        target_compile_options(${self_rep} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()




