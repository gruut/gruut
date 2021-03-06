cmake_minimum_required(VERSION 3.10)

find_package(Boost REQUIRED COMPONENTS system thread random)
find_package(Protobuf REQUIRED)
find_package(GRPC REQUIRED)

file(GLOB HEADERS
        "include/*.hpp"
        "rpc_services/proto/include/*.h"
        "rpc_services/include/*.hpp"
        "middlewares/include/*.hpp"
        )

file(GLOB SOURCES
        "rpc_services/proto/*.cc"
        "rpc_services/*.cpp"
        "*.cpp"
        )

add_library(admin_plugin
        ${HEADERS}
        ${SOURCES})

set(LIB_PREFIX "/usr/local/lib")
set(BOTAN_LIB "${LIB_PREFIX}/libbotan-2.a")

find_package(Soci)
if (SOCI_FOUND)
    target_link_libraries(admin_plugin ${SOCI_LIBRARY} ${SOCI_mysql_PLUGIN})
    target_include_directories(admin_plugin PUBLIC ${SOCI_INCLUDE_DIR})
else (SOCI_FOUND)
    message(WARNING "SOCI NOT FOUND")
endif (SOCI_FOUND)

target_link_libraries(admin_plugin
        appbase
        log
        json
        ${BOTAN_LIB}
        ${Boost_LIBRARIES}
        ${Protobuf_LIBRARIES}

        ${GRPC_GRPC++_LIBRARY}
        ${GRPC_LIBRARY}
        ${GRPC_GRPC++_REFLECTION_LIBRARY}
        )

target_include_directories(admin_plugin PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/appbase"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/log"
        "${CMAKE_CURRNET_SOURCE_DIR}/../../../lib/json"
        "${GRUUT_INSTALL_DIR}/include"
        )
