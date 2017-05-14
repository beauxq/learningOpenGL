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
    return glm::lookAt(getCameraLocation(), focusPoint, upDirection);
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
