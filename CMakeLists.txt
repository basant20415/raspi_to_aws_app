cmake_minimum_required(VERSION 3.0)

project(mqtt)  # Project name

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(includes utils)

# Source files
set(SOURCE
    src/aws.cpp
)

# Find MQTT library
find_package(PahoMqttCpp REQUIRED)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCE})

# Link the required libraries
target_link_libraries(${PROJECT_NAME} PahoMqttCpp::paho-mqttpp3)

# Install the executable
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)