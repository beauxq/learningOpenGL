// implementation of Scene class

#include <iostream>
#include "Scene.h"

#include "OGLObject.h"
#include "OGLSystem.h"

const glm::vec4 oge::Scene::DEFAULT_BACKGROUND_COLOR = {0.18f, 0.0f, 0.4f, 0.0f};

oge::Scene::Scene(OGLSystem& _system) : system(&_system), camera(&_system) {}

void oge::Scene::initialize() {
    glClearColor(DEFAULT_BACKGROUND_COLOR[0],
                 DEFAULT_BACKGROUND_COLOR[1],
                 DEFAULT_BACKGROUND_COLOR[2],
                 DEFAULT_BACKGROUND_COLOR[3]);

    // default camera settings  TODO: put defaults in constants
    // Projection matrix : 45° Field of View, aspect ratio, display range : 0.1 unit <-> 100 units
    camera.projection = glm::perspective(45.0f,
                                         (float)system->getWindow().getSize().x / (float)system->getWindow().getSize().y,
                                         0.1f,
                                         200.0f);
    // Or, for an ortho camera :
    //camera.projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

    // location and direction
    camera.focusPoint = glm::vec3(0, 0, 0);
    camera.distanceFromFocusPoint = 4.0f;
    camera.directionFromFocusPoint = glm::vec3(0, 0, 1);
    camera.upDirection = glm::vec3(0, 1, 0);

    // TODO: put defaults in constants
    light.setPosition(sf::Glsl::Vec3(-30.0f, 150.0f, 45.0f));
    light.setColor(sf::Glsl::Vec3(1, 1, 1));
    light.setPower(1.0f);
}

oge::OGLSystem*& oge::Scene::getSystem() {
    return system;
}

oge::Camera& oge::Scene::getCamera() {
    return camera;
}

void oge::Scene::setCamera(const Camera& _camera) {
    camera = _camera;
}

oge::Light& oge::Scene::getLight() {
    return light;
}

void oge::Scene::setLight(const Light& _light) {
    light = _light;
}

void oge::Scene::setBackGroundColor(const glm::vec4& _color) {
    glClearColor(_color[0], _color[1], _color[2], _color[3]);
}

void oge::Scene::update() {
    // override me
    camera.update();
    for (auto object : objects) {
        object.object->update();
    }
}

void oge::Scene::handleEvent(const sf::Event& event) {
    // keep parent in override unless you don't want it to call these handlers automatically
    camera.handleEvent(event);
    for (auto object : objects) {
        object.object->handleEvent(event);
    }
}

void oge::Scene::draw() {
    // create shadow map

    glBindFramebuffer(GL_FRAMEBUFFER, system->FramebufferName);
    glViewport(0,0,1024,1024);  // TODO: magic numbers

    // Clear the screen
    // glClear( GL_COLOR_BUFFER_BIT );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sf::Shader::bind(&(system->getCreateShadowMapProgram()));

    // Compute the MVP matrix from the light's point of view
    lightInvDir = glm::vec3(light.getPosition().x,
                            light.getPosition().y,
                            light.getPosition().z) -
                  camera.focusPoint;  // glm::vec3(0.5f,2,2);  // TODO: test with different focus points and light positions
    glm::mat4 projectionMatrix = glm::ortho<float>(-100,100,-40,40,-10,300);  // TODO: compute these numbers from the camera
    glm::mat4 viewMatrix = glm::lookAt(lightInvDir, camera.focusPoint, camera.upDirection);
    // TODO: make option for spot light :
    //glm::vec3 lightPos(5, 20, 20);
    //glm::mat4 projectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
    //glm::mat4 viewMatrix = glm::lookAt(lightPos, lightPos-lightInvDir, glm::vec3(0,1,0));


    for (auto object : objects) {
        object.object->draw(projectionMatrix, viewMatrix, system->getCreateShadowMapProgram(), true);
    }

    // TODO: if we don't need display() then get rid of these
    //system->getShadowMap().display();  // update the texture in memory
    // display activates new context, so back to original
    //system->getWindow().setActive();


    // testing
    /*
    sf::Image imageAfterCreateShadowMap(system->getShadowMap().getTexture().copyToImage());
    imageAfterCreateShadowMap.saveToFile("resources/imageOut.bmp");
    std::cout << "size: " << imageAfterCreateShadowMap.getSize().x << ", " << imageAfterCreateShadowMap.getSize().y << std::endl;
    sf::Color pixel = imageAfterCreateShadowMap.getPixel(0, 0);
    std::cout << "0, 0: " << pixel.r << ' ' << pixel.g << ' ' << pixel.b << ' ' << pixel.a << std::endl;
    // found no color data "0, 0: [nothing]"

    exit(2);
     */


    // now the image to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, system->getWindow().getSize().x, system->getWindow().getSize().y);
    // TODO: do we need this viewport command? (without it, test edges of big windows > 1024)
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
    // bind shadow map texture in Texture Unit 1
    glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, depthTexture);
    sf::Texture::bind(&(system->getShadowMap().getTexture()));
     */

    for (auto object : objects) {
        sf::Shader::bind(object.program);  // use the shader for this object
        // glUseProgram(programID);

        /*
        // Bind our surface color texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
         */
        // glBindTexture(GL_TEXTURE_2D, Texture);
        //sf::Texture::bind(&(object.object->getSurfaceColorTexture()));
        /*
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(object.object->getSurfaceColorTexture().getNativeHandle(), 0);
        // set "shadowMap" sampler to use Texture Unit 1
        glUniform1i(system->getShadowMap().getTexture().getNativeHandle(), 1);
        */

        object.object->draw(camera.projection, camera.getViewMatrix(), *(object.program));
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  // TODO: probably don't need this
}

void oge::Scene::registerObject(oge::OGLObject& object, sf::Shader& program) {
    objects.emplace(object, program);
}

const glm::vec3& oge::Scene::getLightInvDir() const {
    return lightInvDir;
}
