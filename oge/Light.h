// information about a light source

#ifndef NEWEROPENGLTEST_LIGHT_H
#define NEWEROPENGLTEST_LIGHT_H


#include <SFML/Graphics/Glsl.hpp>
#include "../glm/vec3.hpp"

namespace oge {
    class Light {
    private:
        sf::Glsl::Vec3 position;
        sf::Glsl::Vec3 color;
        float power;

    public:
        // TODO: position and color setters for glm::vec3 and for (float, float, float)
        const sf::Glsl::Vec3& getPosition() const;
        void setPosition(const sf::Glsl::Vec3& _position);
        void setPosition(const glm::vec3& _position);
        void setPosition(float x, float y, float z);
        const sf::Glsl::Vec3& getColor() const;
        void setColor(const sf::Glsl::Vec3& _color);
        void setColor(const glm::vec3& _color);
        void setColor(float r, float g, float b);
        float getPower() const;
        void setPower(float _power);
    };
}

#endif //NEWEROPENGLTEST_LIGHT_H
