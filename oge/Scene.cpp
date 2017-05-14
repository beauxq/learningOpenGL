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
    light.setPower(30000.0f);
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
    // Clear the screen
    // glClear( GL_COLOR_BUFFER_BIT );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto object : objects) {
        sf::Shader::bind(object.program);  // use the shader for this object
        // glUseProgram(programID);
        object.object->draw(camera, *(object.program));
    }
}

void oge::Scene::registerObject(oge::OGLObject& object, sf::Shader& program) {
    objects.emplace(object, program);
}
