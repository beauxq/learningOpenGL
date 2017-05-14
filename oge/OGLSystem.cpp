//
// Created by Doug on 5/11/2017.
//

#include "OGLSystem.h"

#include <iostream>  // for debug and error messages

#include "Scene.h"

void oge::OGLSystem::initialize() {
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 32;
    contextSettings.majorVersion = 3;
    contextSettings.minorVersion = 3;
    contextSettings.antialiasingLevel = 4;
    // contextSettings.attributeFlags = sf::ContextSettings::Core;

    window.create(sf::VideoMode(1000, 600), "openGL tutorial", sf::Style::Default, contextSettings);
    //sf::Window window(sf::VideoMode(800, 600), "openGL");
    window.setVerticalSyncEnabled(true);

    // If true, you will continue to receive keyboard events when a key is held down
    // If false, it will only fire one event per press until released
    // window.setKeyRepeatEnabled(false);  // TODO: is this needed? what is the default?

    // activate the window
    window.setActive(true);

    contextSettings = window.getSettings();

    std::cout << contextSettings.attributeFlags << std::endl;

    // TODO: test shadow maps with MacOS X with a retina screen
    // ( glfwGetFramebufferSize(window, &windowWidth, &windowHeight); ) may not be the same size as window.getSize()

    // Initialize GLEW
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        window.close();
    }

    // Enable Z-buffer read and write
    // without this, it would draw all of the triangles (in whatever order it finds them)
    // so this is to not draw things that are behind other things
    glEnable(GL_DEPTH_TEST);
    // glEnable(EnableCap.DepthTest);
    glDepthMask(GL_TRUE);
    glClearDepth(1);
    // one tutorial used this and didn't use the previous 2 lines
    // Accept fragment if it closer to the camera than the former one
    // glDepthFunc(GL_LESS);

    // only draw triangles that face the camera
    // (right-hand rule default - there's a setting that can change it to left-hand)
    glEnable(GL_CULL_FACE);

    initializeShaders();

    registerScene(emptyScene, true);
}

void oge::OGLSystem::loop() {
    while (running) {
        // handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                // end the program
                running = false;
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    _leftMouseButtonIsDown = true;
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    _rightMouseButtonIsDown = true;
                }
                previousMouseLocation = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            } else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    _leftMouseButtonIsDown = false;
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    _rightMouseButtonIsDown = false;
                }
            } else if (event.type == sf::Event::KeyPressed) {
                _keyIsDown[event.key.code] = true;
            } else if (event.type == sf::Event::KeyReleased) {
                _keyIsDown[event.key.code] = false;
            }

            if (currentScene != -1) {
                scenes[currentScene]->handleEvent(event);
            }

            // after handling scene events that may query previousMouseLocation
            // set previousMouseLocation
            if (event.type == sf::Event::MouseMoved) {
                previousMouseLocation = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
            }
        }

        if (currentScene != -1) {
            scenes[currentScene]->update();
            scenes[currentScene]->draw();
        }

        window.display();
    }
}

void oge::OGLSystem::initializeShaders() {
    glm::mat4 myDummyFloatMatrix;  // need the address of a float in memory to reserve uniform

    // Create and compile our GLSL program from the shaders

    // loader from tutorial
    // GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

    // loader from sfml
    std::cout << "created shader object\n";
    colorProgram.loadFromFile("StandardColor.vertexshader", "StandardColor.fragmentshader");
    std::cout << "loaded shaders from files\n";

    // reserve a spot for MVP matrix

    // normal opengl way
    // GLint MatrixID = glGetUniformLocation(colorProgram.getNativeHandle(), "MVP");

    // sfml way
    colorProgram.setUniform("MVP", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));
    colorProgram.setUniform("V", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));
    colorProgram.setUniform("M", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));
    colorProgram.setUniform("LightPosition_worldspace", sf::Glsl::Vec3(0, 0, 0));


    // different program for textured objects
    textureProgram.loadFromFile("StandardTexture.vertexshader", "StandardTexture.fragmentshader");
    textureProgram.setUniform("MVP", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));
    textureProgram.setUniform("V", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));
    textureProgram.setUniform("M", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));
    textureProgram.setUniform("LightPosition_worldspace", sf::Glsl::Vec3(0, 0, 0));

    sf::Texture myDummyTexture;

    textureProgram.setUniform("myTextureSampler", myDummyTexture);
}

bool oge::OGLSystem::leftMouseButtonIsDown() const {
    return _leftMouseButtonIsDown;
}

bool oge::OGLSystem::rightMouseButtonIsDown() const {
    return _rightMouseButtonIsDown;
}

const sf::Vector2i& oge::OGLSystem::getPreviousMouseLocation() const {
    return previousMouseLocation;
}

bool oge::OGLSystem::keyIsDown(int key) {
    if (_keyIsDown[key])
        return true;
    return false;
}

sf::Clock& oge::OGLSystem::getClock() {
    return clock;
}

sf::Shader& oge::OGLSystem::getColorProgram() {
    return colorProgram;
}

sf::Shader &oge::OGLSystem::getTextureProgram() {
    return textureProgram;
}

int oge::OGLSystem::registerScene(oge::Scene& scene, bool makeCurrentScene) {
    scenes.push_back(&scene);
    if (makeCurrentScene) {
        currentScene = scenes.size() - 1;
    }
    return scenes.size() - 1;
}

void oge::OGLSystem::deleteScene(oge::Scene& scene) {
    size_t i = 1;  // don't delete scene 0 empty scene
    while (i < scenes.size() && scenes[i] != &scene) {
        ++i;
    }
    if (i != scenes.size()) {  // found it
        scene.~Scene();  // this should delete openGL objects - TODO: test
        scenes[i] = &emptyScene;
    }


}

void oge::OGLSystem::setCurrentTexture(sf::Texture &texture) {
    textureProgram.setUniform("myTextureSampler", texture);
}

sf::Window& oge::OGLSystem::getWindow() {
    return window;
}
