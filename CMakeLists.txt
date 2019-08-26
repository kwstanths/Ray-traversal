cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0015 NEW)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set (PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/)

set(appName RayT)
project(${appName})


if(UNIX)
    #If running on Unix
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")

    execute_process(COMMAND ln -s ${PROJECT_ROOT}/shaders)
    execute_process(COMMAND ln -s ${PROJECT_ROOT}/fonts)
    
    set(CMAKE_MODULE_PATH "${PROJECT_ROOT}/cmake_modules" ${CMAKE_MODULE_PATH})
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(SOIL REQUIRED)
    find_package(Freetype REQUIRED)

elseif(WIN32)
    # If running on Windows
    find_package(OpenGL REQUIRED)

    # Set GLUT headers and libraries
    set(GLEW_LIBRARY_DIRS ${PROJECT_ROOT}/glew-2.1.0/lib/Release/x64)
    set(GLEW_INCLUDE_DIRS ${PROJECT_ROOT}/glew-2.1.0/include)
    set(GLEW_LIBRARIES ${PROJECT_ROOT}/glew-2.1.0/lib/Release/x64/glew32.lib)
    
    # Set GLUT headers and libraries
    set(GLUT_LIBRARY_DIRS ${PROJECT_ROOT}/freeglut/lib/x64)
    set(GLUT_INCLUDE_DIRS ${PROJECT_ROOT}/freeglut/include)
    set(GLUT_LIBRARIES ${PROJECT_ROOT}/freeglut/lib/x64/freeglut.lib)
    
    # Set freetype libraries
    set(FREETYPE_LIBRARY_DIRS ${PROJECT_ROOT}/freetype291/win64)
    set(FREETYPE_INCLUDE_DIRS ${PROJECT_ROOT}/freetype291/include)
    set(FREETYPE_LIBRARIES ${PROJECT_ROOT}/freetype291/win64/freetype.lib)
    
    # Set SOIL libraries
    set(SOIL_LIBRARY_DIRS ${PROJECT_ROOT}/soil/libs)
    set(SOIL_INCLUDE_DIRS ${PROJECT_ROOT}/soil/include)
    set(SOIL_LIBRARIES ${PROJECT_ROOT}/soil/libs/SOIL.lib)
    
endif()

include_directories(${PROJECT_ROOT}/src/map)
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLUT_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories("${PROJECT_ROOT}/glm")
include_directories(${SOIL_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})

link_directories(${OPENGL_LIBRARY_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})
link_directories(${SOIL_LIBRARY_DIRS})
link_directories(${FREETYPE_LIBRARY_DIRS})

file(GLOB ${NAME}_HPP_HEADERS *.hpp)
file(GLOB ${NAME}_CPP_SOURCES *.cpp)
file(GLOB ${NAME}_H_HEADERS *.h)
file(GLOB ${NAME}_C_SOURCES *.c)

add_executable(${appName} 
    ${${NAME}_HPP_HEADERS}
    ${${NAME}_H_HEADERS}
    ${${NAME}_CPP_SOURCES}
    ${${NAME}_C_SOURCES}    
)

target_link_libraries(${appName} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} ${SOIL_LIBRARIES} ${FREETYPE_LIBRARIES})

if (MSVC)
    # If building with Visual Studio 
    # This is needed, because when loading very big models, memory usage goes above 1GB
    # and Visual Studio compiler throws exceptions without this option
    set_property(TARGET ${appName} APPEND PROPERTY LINK_FLAGS /LARGEADDRESSAWARE)
endif()
    

