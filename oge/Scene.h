// openGL scene

#ifndef NEWEROPENGLTEST_SCENE_H
#define NEWEROPENGLTEST_SCENE_H

#include <GL/glew.h>

// #include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

#include <vector>
#include <unordered_set>

#include "Camera.h"
#include "Light.h"

namespace oge {

    // forward declarations
    class OGLObject;
    class OGLSystem;

    /** OGLObject with registered shader identifier */
    struct SceneObject {
        OGLObject* object;
        sf::Shader* program;

        bool operator==(const SceneObject& rhs) const { return object == rhs.object; }

        SceneObject(OGLObject& _object, sf::Shader& _program) : object(&_object), program(&_program) {}

        // hash function so I can put it in an unordered_set
        // http://stackoverflow.com/questions/20953390/what-is-the-fastest-hash-function-for-pointers
        struct hash {
            size_t operator()(const SceneObject& k) const {
                static const size_t shift = (size_t)log2(1 + sizeof(SceneObject));
                return (size_t)(k.object) >> shift;
            }
        };
    };

    class Scene {
    private:
        static const glm::vec4 DEFAULT_BACKGROUND_COLOR;
        static const std::vector< glm::vec3 > DEFAULT_SCENE_BOUNDS;  // cube width 40 centered on origin
        static const glm::vec3 DEFAULT_SCENE_CENTER;  // 0, 0, 0

        OGLSystem* system;

        Camera camera;
        Light light;

        glm::vec3 lightInvDir;  // the direction from the scene to the light source
        glm::mat4 lightViewMatrix;

        bool lightPOV;  // display from point of view of the light

        std::unordered_set< SceneObject, SceneObject::hash > objects;

        std::vector< glm::vec3 > sceneBounds;  // the furthest points away from the middle of the scene
        glm::vec3 sceneCenter;

        /**
         * calculate the direction from the scene to the light source
         */
        void setLightInvDir();
        /**
         * return projection transformation matrix from the point of view of the light
         * precondition: setLightViewMatrix()
         */
        glm::mat4 getLightProjectionMatrix() const;
        /**
         * return view transformation matrix from the point of view of the light
         * precondition: setLightViewMatrix()
         */
        glm::mat4 getLightViewMatrix() const;
        /**
         * calculate view transformation matrix from the point of view of the light
         * precondition: setLightInvDir()
         */
        void setLightViewMatrix();

    public:
        Scene(OGLSystem& _system);

        OGLSystem*& getSystem();

        Camera& getCamera();
        void setCamera(const Camera& _camera);

        Light& getLight();
        void setLight(const Light& _light);

        void setLightPOV(const bool _lightPOV  /* default value true */);
        const bool& getLightPOV() const;

        std::vector< glm::vec3 >& getSceneBounds();
        void setSceneBounds(std::vector< glm::vec3 >& bounds);

        glm::vec3& getSceneCenter();
        void setSceneCenter(const glm::vec3& center);

        const glm::vec3& getLightInvDir() const;

        void setBackGroundColor(const glm::vec4 &_color);

        /** define settings load resources and unique to this scene */
        virtual void initialize();

        /**
         * override with events that depend on multiple elements
         * Scene::handleEvent calls handleEvent on camera and all registered scene objects
         */
        virtual void handleEvent(const sf::Event& event);

        /** override with interactions between objects in a scene */
        virtual void update();

        void draw();

        void registerObject(OGLObject& object, sf::Shader& program);
    };
}

// TODO: is this the best way to do this?
// so inheritors don't need includes or forward declarations
#include "OGLObject.h"
#include "Camera.h"
#include "OGLSystem.h"

#endif //NEWEROPENGLTEST_SCENE_H
