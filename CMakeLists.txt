cmake_minimum_required (VERSION 3.20)

project(robot)

include (FetchContent)
FetchContent_Declare(cmake_tool GIT_REPOSITORY https://github.com/nearmeng/cmake_tools.git GIT_TAG master)
FetchContent_MakeAvailable(cmake_tool)

fetch_content(common https://github.com/nearmeng/common.git master)
fetch_content(cs_proto_common https://github.com/nearmeng/cs_proto_common.git master)
fetch_content(app_proto https://github.com/nearmeng/app_proto.git master)

init_project ()
pre_project(robot RUNTIME)
add_project()
post_project()

target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries (${TARGET_NAME}
	mango::cs_proto
	mango::common
	mango::cs_proto_common
    3rd::tgcpapi
)
target_compile_definitions(${TARGET_NAME} PUBLIC __ROBOT__)