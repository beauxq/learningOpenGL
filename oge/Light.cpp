// implementation of Light

#include "Light.h"

const sf::Glsl::Vec3& oge::Light::getPosition() const {
    return position;
}

void oge::Light::setPosition(const sf::Glsl::Vec3& _position) {
    position = _position;
}

void oge::Light::setPosition(const glm::vec3& _position) {
    position.x = _position[0];
    position.y = _position[1];
    position.z = _position[2];
}

void oge::Light::setPosition(float x, float y, float z) {
    position.x = x;
    position.y = y;
    position.z = z;
}

const sf::Glsl::Vec3& oge::Light::getColor() const {
    return color;
}

void oge::Light::setColor(const sf::Glsl::Vec3& _color) {
    color = _color;
}

void oge::Light::setColor(const glm::vec3& _color) {
    color.x = _color[0];
    color.y = _color[1];
    color.z = _color[2];
}

void oge::Light::setColor(float r, float g, float b) {
    color.x = r;
    color.y = g;
    color.z = b;
}

float oge::Light::getPower() const {
    return power;
}

void oge::Light::setPower(float _power) {
    power = _power;
}
