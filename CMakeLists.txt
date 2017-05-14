cmake_minimum_required(VERSION 3.7)
project(newerOpenGLTest)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        common/shader.cpp
        GLM2SF.h
        oge/OGLObject.cpp
        oge/Camera.cpp
        oge/Camera.h
        oge/OGLSystem.cpp
        oge/OGLSystem.h
        oge/Scene.cpp
        oge/Scene.h
        ControlledObject.cpp
        ControlledObject.h
        Follower.cpp
        Follower.h
        BackgroundObject.cpp
        BackgroundObject.h MainScene.cpp MainScene.h)
add_executable(newerOpenGLTest ${SOURCE_FILES})

find_package(OpenGL)

# tell it where to look for glew
set(CMAKE_PREFIX_PATH "F:/Downloads/glew-2.0.0")
set(CMAKE_LIBRARY_PATH "F:/Downloads/glew-2.0.0/lib")

find_package(GLEW REQUIRED)

if (GLEW_FOUND)
    message(${GLEW_INCLUDE_DIRS})
    message(${GLEW_LIBRARIES})

    # Detect and add SFML
    set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
    #Find any version 2.X of SFML
    #See the FindSFML.cmake file for additional details and instructions
    find_package(SFML 2 REQUIRED graphics window system)

    if(SFML_FOUND)
        include_directories(
                ${GLEW_INCLUDE_DIRS}
                ${SFML_INCLUDE_DIR}
        )
        target_link_libraries(
                newerOpenGLTest
                ${GLEW_LIBRARIES}
                ${SFML_LIBRARIES}
                ${SFML_DEPENDENCIES}
                ${OPENGL_gl_LIBRARY}
        )
    endif()

endif()