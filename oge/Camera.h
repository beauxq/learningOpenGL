// openGL scene camera

#ifndef NEWEROPENGLTEST_CAMERA_H
#define NEWEROPENGLTEST_CAMERA_H

#include <SFML/Window.hpp>

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"


namespace oge {
    // forward declaration
    class OGLSystem;
    class Scene;

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

        void setFarClip();

        Scene* scene;

    public:
        Camera(Scene* _scene);

        const glm::vec3& getFocusPoint() const;
        void setFocusPointKeepingDirectionAndDistance(const glm::vec3& focusPoint);
        void setFocusPointKeepingCameraLocation(const glm::vec3& focusPoint);
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
        /** must be greater than 0 */
        void setNearClip(float nearClip);
        float getFarClip() const;
        /**
         * must be greater than nearClip
         * overrides scene bounds
         */
        void setFarClip(float farClip);

        glm::vec3 getCameraLocation() const;
        void setCameraLocationKeepingFocusPoint(const glm::vec3& loc);
        void setCameraLocationKeepingDirection(const glm::vec3& loc);
        glm::vec3 getDirection() const;

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
