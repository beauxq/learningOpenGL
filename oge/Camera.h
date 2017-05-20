// openGL scene camera

#ifndef NEWEROPENGLTEST_CAMERA_H
#define NEWEROPENGLTEST_CAMERA_H

#include <SFML/Window.hpp>

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"


namespace oge {
    // forward declaration
    class OGLSystem;

    class Camera {
    private:
        // view parameters
        glm::vec3 focusPoint;
        float distanceFromFocusPoint;
        glm::vec3 directionFromFocusPoint;  // normalized
        glm::vec3 upDirection;

        // projection parameters
        float verticalFieldOfView;
        float nearClip;
        float farClip;

        OGLSystem* oglSystem;

    public:
        Camera(OGLSystem* oglSystem);

        const glm::vec3& getFocusPoint() const;
        void setFocusPoint(const glm::vec3& focusPoint);
        float getDistanceFromFocusPoint() const;
        void setDistanceFromFocusPoint(float distanceFromFocusPoint);
        const glm::vec3& getDirectionFromFocusPoint() const;
        void setDirectionFromFocusPoint(const glm::vec3& directionFromFocusPoint);
        const glm::vec3& getUpDirection() const;
        void setUpDirection(const glm::vec3& upDirection);
        /** degrees */
        float getVerticalFieldOfView() const;
        /** degrees */
        void setVerticalFieldOfView(float _verticalFieldOfView);
        float getNearClip() const;
        void setNearClip(float nearClip);
        float getFarClip() const;
        void setFarClip(float farClip);

        glm::vec3 getCameraLocation() const;
        void setCameraLocation(const glm::vec3& loc);

        /** for MVP */
        glm::mat4 getViewMatrix() const;
        glm::mat4 getProjectionMatrix() const;

        // TODO: rename?
        /**
         * not really a zoom, it moves the camera towards or away from focus point
         * multiply the distance by something between 0 and 1, or greater than 1
         */
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
