

set(NN_STATIC_LIB ON CACHE "" BOOL FORCE)
set(NN_TESTS OFF CACHE "" BOOL FORCE)
set(NN_ENABLE_DOC OFF CACHE "" BOOL FORCE)


set(nanomsg_home ${CMAKE_CURRENT_SOURCE_DIR}/nanomsg)
set(nanomsg_src ${nanomsg_home}/src)

if (NOT TARGET nanomsg)
  add_subdirectory(${nanomsg_home}  build_nanomsg)
endif ()

target_compile_definitions(nanomsg INTERFACE "NN_STATIC_LIB")
target_include_directories(nanomsg INTERFACE "${nanomsg_src}")
target_compile_options(nanomsg PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/MP>)

# target_compile_definitions(nanomsg PRIVATE "NN_EXPORT")
