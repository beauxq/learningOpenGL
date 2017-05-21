//
// Created by Doug on 5/11/2017.
//

#include "OGLSystem.h"

#include <iostream>  // for debug and error messages

#include "Scene.h"

glm::vec3 oge::crossVec(const glm::vec3& lhs, const glm::vec3& rhs) {
    return glm::vec3(lhs[1] * rhs[2] - lhs[2] * rhs[1],
                     -(lhs[0] * rhs[2] - lhs[2] * rhs[0]),
                     lhs[0] * rhs[1] - lhs[1] * rhs[0]);
}

void oge::OGLSystem::initialize() {
    // main window
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



    GLint currentFBID;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBID);
    std::cout << "current FrameBuffer binding before creating shadowmap: " << currentFBID << std::endl;


    // shadow map framebuffer
    // sfml way?
    /*
    shadowMap.create(1024, 1024, true);
    if (! shadowMap.getTexture().getNativeHandle()) {
        std::cerr << "error: shadowMap has no native handle\n";
    }
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &shadowMapFrameBufferID);
    std::cout << "current FrameBuffer binding after creating shadowmap: " << shadowMapFrameBufferID << std::endl;
     */

    // opengl tutorial way
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);


    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);
    // We don't use bias in the shader, but instead we draw back faces,
    // which are already separated from the front faces by a small distance
    // (if your geometry is made this way)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
    // check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "error: check framebuffer status failed\n";

    // window.setActive();

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

            if (currentScene != -1) {  // TODO: I think I made it so currentScene can't be -1
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
    // sf::Texture myDummyTexture;

    // Create and compile our GLSL program from the shaders


    // program to create shadow map
    sf::Shader::bind(&createShadowMapProgram);
    createShadowMapProgram.loadFromFile("resources/CreateShadowMap.vertexshader", "resources/CreateShadowMap.fragmentshader");
    createShadowMapProgram.setUniform("MVP", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));


    // program for colored (not textured) objects
    // loader from tutorial
    // GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

    // loader from sfml
    sf::Shader::bind(&colorProgram);
    colorProgram.loadFromFile("resources/ShadowMappingColor.vertexshader", "resources/ShadowMappingColor.fragmentshader");
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "glGetError: " << err << std::endl;
    }

    // reserve a spot for MVP matrix
    // we reserve at initialization because it's probably faster, haven't tested it

    // normal opengl way
    // GLint MatrixID = glGetUniformLocation(colorProgram.getNativeHandle(), "MVP");

    // sfml way
    colorProgram.setUniform("MVP", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));
    colorProgram.setUniform("V", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));
    colorProgram.setUniform("M", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));
    colorProgram.setUniform("DepthBiasMVP", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));
    //colorProgram.setUniform("shadowMap", myDummyTexture);
    ShadowMapIDColor = glGetUniformLocation(colorProgram.getNativeHandle(), "shadowMap");
    //colorProgram.setUniform("LightPosWorldSpace", sf::Glsl::Vec3(0.0f, 0.0f, 0.0f));
    colorProgram.setUniform("LightColor", sf::Glsl::Vec3(0, 0, 0));
    colorProgram.setUniform("LightPower", 0.0f);
    colorProgram.setUniform("LightInvDirection_worldspace", sf::Glsl::Vec3(0, 0, 0));


    // program for textured objects
    sf::Shader::bind(&textureProgram);
    textureProgram.loadFromFile("resources/ShadowMappingTexture.vertexshader", "resources/ShadowMappingTexture.fragmentshader");
    textureProgram.setUniform("MVP", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));
    textureProgram.setUniform("V", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));
    textureProgram.setUniform("M", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));
    textureProgram.setUniform("DepthBiasMVP", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));
    //textureProgram.setUniform("shadowMap", myDummyTexture);
    ShadowMapIDTexture = glGetUniformLocation(textureProgram.getNativeHandle(), "shadowMap");
    //textureProgram.setUniform("LightPosWorldSpace", sf::Glsl::Vec3(0.0f, 0.0f, 0.0f));
    textureProgram.setUniform("LightColor", sf::Glsl::Vec3(0, 0, 0));
    textureProgram.setUniform("LightPower", 0.0f);
    colorProgram.setUniform("LightInvDirection_worldspace", sf::Glsl::Vec3(0, 0, 0));

    //textureProgram.setUniform("myTextureSampler", myDummyTexture);
    TextureIDTexture = glGetUniformLocation(textureProgram.getNativeHandle(), "myTextureSampler");

    std::cout << "loaded shaders from files and reserved uniforms\n";
    std::cout << "shadow map ids: color: " << ShadowMapIDColor << "  texture: " << ShadowMapIDTexture << std::endl;
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

sf::Shader& oge::OGLSystem::getTextureProgram() {
    return textureProgram;
}

sf::Shader& oge::OGLSystem::getCreateShadowMapProgram() {
    return createShadowMapProgram;
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

sf::RenderWindow& oge::OGLSystem::getWindow() {
    return window;
}
