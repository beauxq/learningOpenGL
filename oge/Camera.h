// openGL scene camera

#ifndef NEWEROPENGLTEST_CAMERA_H
#define NEWEROPENGLTEST_CAMERA_H

#include <SFML/Window.hpp>

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"


namespace oge {
    class OGLSystem;

    class Camera {
    public:  // TODO: make private and make getters and setters where needed
        glm::vec3 focusPoint;
        float distanceFromFocusPoint;
        glm::vec3 directionFromFocusPoint;
        glm::vec3 upDirection;

        glm::mat4 projection;

        OGLSystem* oglSystem;

    public:
        Camera(OGLSystem* oglSystem);

        glm::vec3 getCameraLocation() const;

        void setCameraLocation(const glm::vec3& loc);

        /** for MVP */
        glm::mat4 getViewMatrix() const;

        void zoom(float multiplier);

        /**
         * mousewheel zooms
         * arrow keys change direction
         * @param event
         */
        virtual void handleEvent(const sf::Event& event);

        virtual void update();
    };
}

#endif //NEWEROPENGLTEST_CAMERA_H
