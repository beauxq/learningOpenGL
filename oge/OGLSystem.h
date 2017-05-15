// window with openGL context

#ifndef NEWEROPENGLTEST_OGLSYSTEM_H
#define NEWEROPENGLTEST_OGLSYSTEM_H

#include <GL/glew.h>

// #include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "../glm/glm.hpp"

#include "Scene.h"

#include <unordered_map>

namespace oge {

    class OGLSystem {
    private:
        sf::RenderWindow window;  // TODO: change this to RenderWindow if we want to do any SFML drawing
        //sf::RenderTexture shadowMap;  // TODO: I don't know if this needs to be "Render" (if not using sfml drawing)
        //GLint shadowMapFrameBufferID;
    public:  // TODO: private with getter
        GLuint FramebufferName;  // opengl handle
        GLuint depthTexture;  // opengl handle
        GLuint ShadowMapIDColor;  // uniform handle "shadowMap" color shader
        GLuint ShadowMapIDTexture;  // uniform handle "shadowMap" texture shader
    private:

        sf::Shader createShadowMapProgram;
        sf::Shader colorProgram;
        sf::Shader textureProgram;

        // keep track of input
        bool _leftMouseButtonIsDown;
        bool _rightMouseButtonIsDown;
        sf::Vector2i previousMouseLocation;
        std::unordered_map<int, bool> _keyIsDown;

        sf::Clock clock;

        bool running;

        void initializeShaders();

        Scene emptyScene;
        std::vector< Scene* > scenes;
        size_t currentScene;  // index to scenes vector

    public:
        OGLSystem() : _leftMouseButtonIsDown(false),
                      _rightMouseButtonIsDown(false),
                      running(true),
                      emptyScene(*this) {}

        sf::RenderWindow& getWindow();
        /*
        sf::RenderTexture& getShadowMap();
        const GLint& getShadowMapFrameBufferID() const;
         */

        sf::Shader& getColorProgram();
        sf::Shader& getTextureProgram();
        sf::Shader& getCreateShadowMapProgram();

        bool leftMouseButtonIsDown() const;
        bool rightMouseButtonIsDown() const;
        const sf::Vector2i& getPreviousMouseLocation() const;
        bool keyIsDown(int);

        sf::Clock& getClock();

        /**
         * put the scene in the list of scenes
         * where the loop can run it if it is the current scene
         * @param scene
         * @param makeCurrentScene - sets current scene if true
         * @return scene number
         */
        int registerScene(Scene& scene, bool makeCurrentScene);

        /**
         * call destructor to delete openGL objects
         * in the list, replace this object with an empty scene
         * (Scene constructed with the same OGLSystem)
         */
        void deleteScene(Scene& scene);

        // TODO: delete scene by index "scene number"

        /**
         * create SFML window
         * initialize GLEW
         * enable GL depth test and face culling
         * initialize shaders
         * register emptyScene and set it to current scene (scene 0)
         */
        void initialize();

        void loop();
    };
}

#endif //NEWEROPENGLTEST_OGLSYSTEM_H
