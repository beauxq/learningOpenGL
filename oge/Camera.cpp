// Camera implementation

#include "Camera.h"

#include <iostream>

#include "OGLSystem.h"
#include "Scene.h"

glm::vec3 oge::Camera::getCameraLocation() const {
    return focusPoint + distanceFromFocusPoint * glm::normalize(directionFromFocusPoint);
}

void oge::Camera::setCameraLocationKeepingFocusPoint(const glm::vec3& loc) {
    directionFromFocusPoint = loc - focusPoint;
    distanceFromFocusPoint = glm::distance(focusPoint, loc);
    setFarClip();
}

void oge::Camera::setCameraLocationKeepingDirection(const glm::vec3& loc) {
    focusPoint = loc - distanceFromFocusPoint * glm::normalize(directionFromFocusPoint);
    setFarClip();
}

glm::vec3 oge::Camera::getDirection() const {
    return glm::vec3(0.0f - getDirectionFromFocusPoint()[0],
                     0.0f - getDirectionFromFocusPoint()[1],
                     0.0f - getDirectionFromFocusPoint()[2]);
}

glm::mat4 oge::Camera::getViewMatrix() const {
    // TODO: cache this to not have to calculate it on every call?
    return glm::lookAt(getCameraLocation(), focusPoint, upDirection);
}

glm::mat4 oge::Camera::getProjectionMatrix() const {
    // TODO: cache this to not have to calculate it on every call?
    // setters for attributes would update it
    // (and an event for the window size change)
    // TODO: option for ortho projection
    return glm::perspective(verticalFieldOfView,
                            (float)scene->getSystem()->getWindow().getSize().x / (float)scene->getSystem()->getWindow().getSize().y,
                            nearClip,
                            farClip);
}

void oge::Camera::zoom(float multiplier) {
    distanceFromFocusPoint *= multiplier;

    // limit zoom level
    if (distanceFromFocusPoint > 80)
        distanceFromFocusPoint = 80;
    else if (distanceFromFocusPoint < 0.1f)
        distanceFromFocusPoint = 0.1f;

    setFarClip();
}

void oge::Camera::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseWheelScrolled)
    {
        // change the zoom
        zoom(1 - event.mouseWheelScroll.delta / 10.0f);
    }
}

void oge::Camera::update() {
    glm::vec3 temp = getCameraLocation();
    // move camera with arrow keys
    if (scene->getSystem()->keyIsDown(sf::Keyboard::Up)) {
        focusPoint[1] += 0.1f;
        setCameraLocationKeepingFocusPoint(temp);
    }
    if (scene->getSystem()->keyIsDown(sf::Keyboard::Down)) {
        focusPoint[1] -= 0.1f;
        setCameraLocationKeepingFocusPoint(temp);
    }
    if (scene->getSystem()->keyIsDown(sf::Keyboard::Left)) {
        focusPoint[0] -= 0.1f;
        setCameraLocationKeepingFocusPoint(temp);
    }
    if (scene->getSystem()->keyIsDown(sf::Keyboard::Right)) {
        focusPoint[0] += 0.1f;
        setCameraLocationKeepingFocusPoint(temp);
    }
}

oge::Camera::Camera(oge::Scene* _scene) {
    scene = _scene;

    // defaults
    focusPoint = glm::vec3(0.0f, 0.0f, 0.0f);
    directionFromFocusPoint = glm::vec3(0.0f, 0.0f, 1.0f);
    distanceFromFocusPoint = 1;
    nearClip = 0.25f;  // has to be bigger than 0 - TODO: magic number
    farClip = 20.0f;
}

const glm::vec3& oge::Camera::getFocusPoint() const {
    return focusPoint;
}

void oge::Camera::setFocusPointKeepingDirectionAndDistance(const glm::vec3& _focusPoint) {
    focusPoint = _focusPoint;

    setFarClip();
}

void oge::Camera::setFocusPointKeepingCameraLocation(const glm::vec3& _focusPoint) {
    glm::vec3 temp = getCameraLocation();
    if (_focusPoint != temp) {
        focusPoint = _focusPoint;
        setCameraLocationKeepingFocusPoint(temp);

        setFarClip();
    }
    else {
        std::cerr << "error: attempt to set camera focus point to camera location\n";
    }
}

float oge::Camera::getDistanceFromFocusPoint() const {
    return distanceFromFocusPoint;
}

void oge::Camera::setDistanceFromFocusPoint(float _distanceFromFocusPoint) {
    distanceFromFocusPoint = _distanceFromFocusPoint;

    setFarClip();
}

const glm::vec3& oge::Camera::getDirectionFromFocusPoint() const {
    return directionFromFocusPoint;
}

void oge::Camera::setDirectionFromFocusPoint(const glm::vec3& _directionFromFocusPoint) {
    directionFromFocusPoint = _directionFromFocusPoint;

    setFarClip();
}

const glm::vec3& oge::Camera::getUpDirection() const {
    return upDirection;
}

void oge::Camera::setUpDirection(const glm::vec3& _upDirection) {
    upDirection = _upDirection;
}

float oge::Camera::getVerticalFieldOfView() const {
    return verticalFieldOfView;
}

void oge::Camera::setVerticalFieldOfView(float _verticalFieldOfView) {
    verticalFieldOfView = _verticalFieldOfView;
}

float oge::Camera::getNearClip() const {
    return nearClip;
}

void oge::Camera::setNearClip(float _nearClip) {
    if (_nearClip > 0) {
        nearClip = _nearClip;
    }

    setFarClip();
}

float oge::Camera::getFarClip() const {
    return farClip;
}

void oge::Camera::setFarClip(float _farClip) {
    if (_farClip > nearClip) {
        farClip = _farClip;
    }
}

void oge::Camera::setFarClip() {
    // project all of the scene bounds onto the camera line of sight
    // take the distance that is furthest down that line of sight
    float furthestDistance = nearClip + 0.25f;  // TODO: magic number, needs to be bigger than nearClip
    glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 shift = origin - getCameraLocation();
    glm::vec3 cameraLineOfSight = origin - getDirectionFromFocusPoint();
    for (auto boundI = scene->getSceneBounds().begin(); boundI != scene->getSceneBounds().end(); ++boundI) {
        // test
        float x = (glm::dot((*boundI + shift), cameraLineOfSight) / glm::dot(cameraLineOfSight, cameraLineOfSight));
        glm::vec3 projection = x * cameraLineOfSight;
        float thisDistance = glm::distance(origin, projection);
        if (thisDistance > furthestDistance) {
            furthestDistance = thisDistance;
        }
    }

    farClip = furthestDistance + 0.25f;
    std::cout << "set far clip to " << farClip << std::endl;
}
