// load (non-standard, negative coordinate) ascii stl into openGL object

#ifndef OPENGLTEST_STLLOADER_H
#define OPENGLTEST_STLLOADER_H

#include <GL/glew.h>

#include <vector>
#include <string>
#include <random>

#include <SFML/Graphics.hpp>

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

namespace oge {

    struct Buffer {
        GLuint id;
        std::vector<float> array;

        Buffer() {}

        Buffer(const std::vector<float> newArray) : array(newArray) {}
    };

    // forward declarations
    class OGLSystem;
    class Scene;

/** a 3D object to be displayed in OpenGL */
    class OGLObject {
    private:
        Scene* scene;

        glm::vec3 location;  // in world coordinates
        glm::vec3 scale;  // size compared to original data
        glm::mat4 currentRotation;

        glm::mat4 lastShadowMVP;

        static const std::vector<float> xTextureCoordinates;
        static const std::vector<float> yTextureCoordinates;

        //  openGL VAO ID
        GLuint vertexArrayObjectID;
        std::vector<Buffer> buffers;
        // sf::Texture surfaceColorTexture;
        GLuint textureID;

        static bool beginsWith(const std::string &string, const std::string &beginning);

        // RNG for random colors
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen; //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<> dis;

        /** [0, 1) */
        float random();

        bool loadFromSTLFileAscii(const std::string& filename);

        void createVAO();
        void bindVAO();
        void deleteVAO();

        void uploadBufferData(GLuint *bufferIDPtr, float *dataArrayPtr, size_t dataByteCount);

        /** precondition: buffer is active (it doesn't check, deleteAllBuffers checks) */
        void deleteBuffer(GLuint *bufferIDPtr);

        void deleteAllBuffers();

        /** checks for textureID and deletes it from OpenGL */
        void deleteTexture();

    public:
        OGLObject(Scene& _scene);

        /** delete OpenGL VAOs and buffers */
        ~OGLObject();

        OGLSystem* getSystem() const;

        Scene* getScene() const;

        void rotate(int xDiff, int yDiff);

        void translate(float xDiff, float yDiff, float zDiff);
        void translate(const glm::vec3& vector);

        /** for transform to world coordinates in MVP */
        glm::mat4 getModelMatrix();

        const glm::vec3& getLocation() const;
        void setLocation(const glm::vec3& _location);

        const glm::vec3& getScale() const;
        void setScale(const glm::vec3& _scale);

        const glm::mat4& getCurrentRotation() const;
        void setCurrentRotation(const glm::mat4& _currentRotation);

        /**
         * loads data into ram
         * call uploadData to load into openGL
         */
        bool loadFromFile(const std::string& filename);

        void loadTextureFromFile(const std::string& filename);
        // sf::Texture& getSurfaceColorTexture();
        // TODO: getter for texture handle

        void uploadData();

        const std::vector<float>& getBufferArray(GLuint attributeNumber) const;
        /** replace existing buffer array at index attributeNumber with new buffer array */
        void setBufferArray(GLuint attributeNumber, const std::vector< float >& newArray);
        /** returns attribute number / index */
        GLuint addBufferArray(const std::vector<float>& newArray);

        virtual void update();

        virtual void handleEvent(const sf::Event& event);

        /** precondition: shader attribute layout matches and shader takes MVP uniform */
        void draw(const glm::mat4& projectionMatrix,
                  const glm::mat4& viewMatrix,
                  sf::Shader& mvpShader,
                  bool shadowMap = false);
    };
}

// TODO: is this the best way to do this?
// so inheritors don't need includes or forward declarations
#include "Scene.h"
#include "OGLSystem.h"

#endif //OPENGLTEST_STLLOADER_H
