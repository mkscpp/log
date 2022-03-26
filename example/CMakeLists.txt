project(chain_logger_example)

add_executable(${PROJECT_NAME} chain_logger.cpp)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME} mks_log)
target_compile_options(${PROJECT_NAME} PUBLIC -O0 -Wall)
