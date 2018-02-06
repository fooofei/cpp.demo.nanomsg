cmake_minimum_required(VERSION 3.1)
project(demo_nanomsg)


list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/..")
include(libnanomsg_static)


set(self_surveyor test_surveyor)
add_executable(${self_surveyor} ${CMAKE_CURRENT_SOURCE_DIR}/surveyor.c)
target_link_libraries(${self_surveyor} nanomsg)
if(WIN32)
        target_compile_options(${self_surveyor} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

set(self_respondent test_respondent)
add_executable(${self_respondent} ${CMAKE_CURRENT_SOURCE_DIR}/respondent.c)
target_link_libraries(${self_respondent} nanomsg)
if(WIN32)
        target_compile_options(${self_respondent} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()




