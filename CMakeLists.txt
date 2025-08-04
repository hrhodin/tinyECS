cmake_minimum_required(VERSION 3.10)

# set the project name
project(tinyECS VERSION 1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME}
	${CMAKE_CURRENT_SOURCE_DIR}/src/tiny_ecs.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	EXPORT_COMPILE_COMMANDS ON
)

# add the executable
add_executable(ecs_demo ${CMAKE_CURRENT_SOURCE_DIR}/src/ecs_demo.cpp)
target_link_libraries(ecs_demo tinyECS)

set_target_properties(ecs_demo PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	EXPORT_COMPILE_COMMANDS ON
)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets
	DESTINATION lib
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/tiny_ecs.hpp
	DESTINATION include
)

install(EXPORT ${PROJECT_NAME}-targets
	FILE ${PROJECT_NAME}-targets.cmake
	DESTINATION lib/cmake/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
	INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
	VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}
	COMPATIBILITY AnyNewerVersion
)

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
	DESTINATION lib/cmake/${PROJECT_NAME}
)

export(EXPORT ${PROJECT_NAME}-targets
	FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-targets.cmake
)

export(PACKAGE ${PROJECT_NAME})
