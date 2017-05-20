// Camera implementation

#include "Camera.h"

#include <iostream>

#include "OGLSystem.h"

glm::vec3 oge::Camera::getCameraLocation() const {
    return focusPoint + distanceFromFocusPoint * glm::normalize(directionFromFocusPoint);
}

void oge::Camera::setCameraLocation(const glm::vec3& loc) {
    directionFromFocusPoint = loc - focusPoint;
    distanceFromFocusPoint = glm::distance(focusPoint, loc);
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
                            (float)oglSystem->getWindow().getSize().x / (float)oglSystem->getWindow().getSize().y,
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
    if (oglSystem->keyIsDown(sf::Keyboard::Up)) {
        focusPoint[1] += 0.1f;
    }
    if (oglSystem->keyIsDown(sf::Keyboard::Down)) {
        focusPoint[1] -= 0.1f;
    }
    if (oglSystem->keyIsDown(sf::Keyboard::Left)) {
        focusPoint[0] -= 0.1f;
    }
    if (oglSystem->keyIsDown(sf::Keyboard::Right)) {
        focusPoint[0] += 0.1f;
    }
    setCameraLocation(temp);
}

oge::Camera::Camera(oge::OGLSystem* _oglSystem) {
    oglSystem = _oglSystem;
}

const glm::vec3& oge::Camera::getFocusPoint() const {
    return focusPoint;
}

void oge::Camera::setFocusPoint(const glm::vec3& _focusPoint) {
    focusPoint = _focusPoint;
}

float oge::Camera::getDistanceFromFocusPoint() const {
    return distanceFromFocusPoint;
}

void oge::Camera::setDistanceFromFocusPoint(float _distanceFromFocusPoint) {
    distanceFromFocusPoint = _distanceFromFocusPoint;
}

const glm::vec3& oge::Camera::getDirectionFromFocusPoint() const {
    return directionFromFocusPoint;
}

void oge::Camera::setDirectionFromFocusPoint(const glm::vec3& _directionFromFocusPoint) {
    directionFromFocusPoint = _directionFromFocusPoint;
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
    nearClip = _nearClip;
}

float oge::Camera::getFarClip() const {
    return farClip;
}

void oge::Camera::setFarClip(float _farClip) {
    farClip = _farClip;
}
