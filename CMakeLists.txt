cmake_minimum_required(VERSION 3.1)
project(demo_nanomsg)



set(NN_STATIC_LIB ON)
set(NN_TESTS OFF)
set(NN_ENABLE_DOC OFF)


set(nanomsg_home ${CMAKE_CURRENT_SOURCE_DIR}/../nanomsg-1.1.2)
set(nanomsg_src ${nanomsg_home}/src)

if (NOT TARGET nanomsg)
  add_subdirectory(${nanomsg_home}  build_nanomsg)
endif ()

# target_compile_definitions(nanomsg PRIVATE "NN_EXPORT")

set(binary_name_server server)
add_executable(${binary_name_server} ${CMAKE_CURRENT_SOURCE_DIR}/server.c)
target_include_directories(${binary_name_server} PRIVATE ${nanomsg_src})
target_link_libraries(${binary_name_server} nanomsg)
target_compile_definitions(${binary_name_server} PRIVATE "NN_STATIC_LIB")


set(binary_name_client client)
add_executable(${binary_name_client} ${CMAKE_CURRENT_SOURCE_DIR}/client.c)
target_include_directories(${binary_name_client} PRIVATE ${nanomsg_src})
target_link_libraries(${binary_name_client} nanomsg)
target_compile_definitions(${binary_name_client} PRIVATE "NN_STATIC_LIB")





