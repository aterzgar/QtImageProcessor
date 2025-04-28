
cmake_minimum_required(VERSION 3.10)
project(QtImageProcessor)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(OpenCV REQUIRED)

# Tell CMake to run moc when necessary
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Optionally, you can also use these:
set(CMAKE_AUTOUIC ON) # if you have .ui files
set(CMAKE_AUTORCC ON) # if you have .qrc files

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ui)
include_directories(${OpenCV_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
)

# Header files
set(HEADERS
    include/mainwindow.h
)

# UI files (if any)
set(UI_FILES
    ui/mainwindow.ui
)

# Set the UI directory for AUTOUIC
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    Qt5::Core 
    Qt5::Widgets
    ${OpenCV_LIBS}
)
