// implementation of Scene class

#include <iostream>
#include "Scene.h"

#include "OGLObject.h"
#include "OGLSystem.h"

#include "../glm/gtx/vector_angle.hpp"

const glm::vec4 oge::Scene::DEFAULT_BACKGROUND_COLOR = {0.18f, 0.0f, 0.4f, 0.0f};

oge::Scene::Scene(OGLSystem& _system) : system(&_system), camera(&_system) {}

void oge::Scene::initialize() {
    glClearColor(DEFAULT_BACKGROUND_COLOR[0],
                 DEFAULT_BACKGROUND_COLOR[1],
                 DEFAULT_BACKGROUND_COLOR[2],
                 DEFAULT_BACKGROUND_COLOR[3]);

    // default camera settings  TODO: put defaults in constants
    // Projection matrix : 45° Field of View, aspect ratio, display range : 0.1 unit <-> 100 units
    camera.setHorizontalFieldOfView(45.0f);
    camera.setNearClip(0.1f);
    camera.setFarClip(200.0f);
    // Or, for an ortho camera :
    //camera.projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

    // location and direction
    camera.setFocusPoint(glm::vec3(0, 0, 0));
    camera.setDistanceFromFocusPoint(4.0f);
    camera.setDirectionFromFocusPoint(glm::vec3(0, 0, 1));
    camera.setUpDirection(glm::vec3(0, 1, 0));

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
    setLightInvDir();
    glm::mat4 projectionMatrix = getLightProjectionMatrix();
    glm::mat4 viewMatrix = getLightViewMatrix();

    for (auto object : objects) {
        object.object->draw(projectionMatrix, viewMatrix, system->getCreateShadowMapProgram(), true);
    }


    // now the image to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, system->getWindow().getSize().x, system->getWindow().getSize().y);
    // TODO: do we need this viewport command? (without it, test edges of big windows > 1024)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto object : objects) {
        sf::Shader::bind(object.program);  // use the shader for this object
        // glUseProgram(programID);

        object.object->draw(camera.getProjectionMatrix(), camera.getViewMatrix(), *(object.program));
        // object.object->draw(projectionMatrix, viewMatrix, *(object.program));  // light pov
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  // TODO: probably don't need this
}

glm::mat4 oge::Scene::getLightViewMatrix() const {// find light up direction
    // the normal of light position, focus point, camera position is the axis for rotation
    glm::vec3 rotateAxis = glm::cross(camera.getCameraLocation() - camera.getFocusPoint(),
                                      lightInvDir);
    float rotateAngle = glm::angle(normalize(camera.getCameraLocation() - camera.getFocusPoint()),
                                   normalize(lightInvDir));
    glm::mat4 upRotateMatrix = rotate(glm::mat4(1.0f), rotateAngle, rotateAxis);
    glm::vec3 lightUpDirection = upRotateMatrix * glm::vec4(camera.getUpDirection(), 0.0f);

    return lookAt(glm::vec3(light.getPosition().x,
                            light.getPosition().y,
                            light.getPosition().z),
                  camera.getFocusPoint(),  // TODO: maybe use the center of the scene instead of the camera focus point?
                  lightUpDirection);
}

glm::mat4 oge::Scene::getLightProjectionMatrix() const {
    return glm::ortho<float>(-100, 80, -40, 40, -5, 180);  // TODO: compute these numbers from the camera
    // return glm::ortho<float>(-30, 30, -20, 20, -10, 100);
    // TODO: make option for spot light :
    // return glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
}

void oge::Scene::setLightInvDir() {
    // TODO: maybe use the center of the scene instead of the camera focus point?
    lightInvDir = glm::vec3(light.getPosition().x,
                            light.getPosition().y,
                            light.getPosition().z) -
                  camera.getFocusPoint();  // glm::vec3(0.5f,2,2);  // TODO: test with different focus points and light positions
    //lightInvDir = glm::normalize(lightInvDir) * 70.0f;
}

void oge::Scene::registerObject(oge::OGLObject& object, sf::Shader& program) {
    objects.emplace(object, program);
}

const glm::vec3& oge::Scene::getLightInvDir() const {
    return lightInvDir;
}
