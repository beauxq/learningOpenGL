// implementation for stlLoader class

#include "OGLObject.h"

#include <fstream>
#include <iostream>
#include <cctype>
#include <random>

#include <SFML/Graphics.hpp>

#include "Camera.h"
#include "Scene.h"

const std::vector< float > oge::OGLObject::xTextureCoordinates = {0, 1, 0.5};
const std::vector< float > oge::OGLObject::yTextureCoordinates = {0, 0, 1};

bool oge::OGLObject::loadFromSTLFileAscii(const std::string& filename) {
    std::cout << "loading " << filename << std::endl;

    std::ifstream inFile(filename);

    if (! inFile) {
        std::cerr << "can't open file\n";
        return false;
    }

    std::string line;

    std::getline(inFile, line);

    if (! beginsWith(line, "solid")) {
        std::cerr << "incorrect file format (doesn't begin with \"solid\")\n";
        return false;
    }

    // TODO: remove debug message
    std::cout << line << std::endl;

    buffers.clear();

    std::vector< float > pointArray;
    std::vector< float > colorArray;
    std::vector< float > textureCoordArray;  // not used
    std::vector< float > normalArray;

    while (true) {
        inFile >> line;
        if (beginsWith(line, "endsolid")) {
            break;
        }

        // line should now be "facet"
        inFile >> line;  // "normal"
        // parse facet normal into normalArray
        float normalX;
        float normalY;
        float normalZ;
        inFile >> normalX >> normalY >> normalZ;

        std::getline(inFile, line);  // "\n"
        std::getline(inFile, line);  // "outer loop"

        for (int i = 0; i < 3; ++i) {
            char x = 0;
            while (x != 'x') {  // in "vertex"
                inFile >> x;
            }
            /*
            while (isspace(inFile.peek())) {
                inFile >> x;  // space after "vertex"

                // TODO: remove debug message
                std::cout << "just removed space after vertex:  '" << x << "'" << std::endl;
            }
             */

            // now we're at the number we want to read
            float number;

            inFile >> number;
            pointArray.push_back(number);
            inFile >> number;
            pointArray.push_back(number);
            inFile >> number;
            pointArray.push_back(number);

            // 2 texture coordinates
            textureCoordArray.push_back(xTextureCoordinates[i]);
            textureCoordArray.push_back(yTextureCoordinates[i]);

            // colors
            // red
            colorArray.push_back(random() / 20.0f + 0.475f);
            // green
            colorArray.push_back(random() / 20.0f + 0.575f);
            // blue
            colorArray.push_back(random() / 20.0f + 0.675f);

            normalArray.push_back(normalX);
            normalArray.push_back(normalY);
            normalArray.push_back(normalZ);
        }

        std::getline(inFile, line);  // "\n"

        std::getline(inFile, line);  // "endloop"

        std::getline(inFile, line);  // "endfacet"
    }

    addBufferArray(pointArray);
    addBufferArray(colorArray);
    // addBufferArray(textureCoordArray);
    addBufferArray(normalArray);

    std::cout << "finished loading " << filename << "\nlength of array: " << pointArray.size() << std::endl;

    return true;
}

bool oge::OGLObject::beginsWith(const std::string &string, const std::string &beginning) {
    if (string.size() < beginning.size()) {
        return false;
    }

    return std::equal(beginning.begin(), beginning.end(), string.begin());
}

oge::OGLObject::OGLObject(Scene& _scene) : gen(rd()),
                              dis(0, 1),
                              location(0.0f, 0.0f, 0.0f),
                              scale(1.0f, 1.0f, 1.0f),
                              currentRotation(1.0f),
                              vertexArrayObjectID(0),
                              scene(&_scene) {}

float oge::OGLObject::random() {
    return (float)dis(gen);
}

void oge::OGLObject::rotate(int xDiff, int yDiff) {
    // TODO: make more generic rotate function
    glm::mat4 newRotation = glm::rotate(glm::mat4(1.0f), yDiff * 0.02f, glm::vec3(1, 0, 0));
    newRotation = glm::rotate(newRotation, xDiff * 0.02f, glm::vec3(0, 1, 0));

    currentRotation = newRotation * currentRotation;
}

void oge::OGLObject::translate(float xDiff, float yDiff, float zDiff) {
    location[0] += xDiff;
    location[1] += yDiff;
    location[2] += zDiff;
}

void oge::OGLObject::translate(const glm::vec3& vector) {
    location += vector;
}

glm::mat4 oge::OGLObject::getModelMatrix() {
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), location);

    return translationMatrix * currentRotation * scaleMatrix;  // scale, then rotate, then translate
}

const glm::vec3& oge::OGLObject::getLocation() const {
    return location;
}

void oge::OGLObject::setLocation(const glm::vec3& _location) {
    location = _location;
}

const glm::vec3& oge::OGLObject::getScale() const {
    return scale;
}

void oge::OGLObject::setScale(const glm::vec3& _scale) {
    scale = _scale;
}

const glm::mat4& oge::OGLObject::getCurrentRotation() const {
    return currentRotation;
}

void oge::OGLObject::setCurrentRotation(const glm::mat4& _currentRotation) {
    currentRotation = _currentRotation;
}

bool oge::OGLObject::loadFromFile(const std::string &filename) {
    // TODO: other file formats

    return loadFromSTLFileAscii(filename);
}

void oge::OGLObject::loadTextureFromFile(const std::string& filename) {
    surfaceColorTexture.loadFromFile(filename);
    surfaceColorTexture.setSmooth(true);  // TODO: make this optional?
}

sf::Texture& oge::OGLObject::getSurfaceColorTexture() {
    return surfaceColorTexture;
}

void oge::OGLObject::createVAO() {
    if (vertexArrayObjectID) {
        deleteAllBuffers();
        deleteVAO();
    }

    glGenVertexArrays(1, &vertexArrayObjectID);
    bindVAO();
}

void oge::OGLObject::deleteVAO() {
    if (vertexArrayObjectID) {
        glDeleteVertexArrays(1, &vertexArrayObjectID);
        vertexArrayObjectID = 0;
    }

    // TODO: test - remove after thorough testing
    if (vertexArrayObjectID) {
        std::cerr << "error: vertexArrayObjectID still evaluates to true after glDeleteVertexArrays\nvalue: "
                  << vertexArrayObjectID << std::endl;
    }
}

void oge::OGLObject::bindVAO() {
    if (vertexArrayObjectID) {
        glBindVertexArray(vertexArrayObjectID);
    }
    else {
        std::cerr << "error: attempt to bind unregistered VAO\n";
    }
}

void oge::OGLObject::uploadBufferData(GLuint* bufferIDPtr, float *dataArrayPtr, size_t dataByteCount) {
    // Generate 1 buffer, put the resulting identifier in bufferID
    glGenBuffers(1, bufferIDPtr);
    // The following commands will talk about our 'bufferID' buffer
    glBindBuffer(GL_ARRAY_BUFFER, *bufferIDPtr);
    // Give our data to OpenGL
    glBufferData(GL_ARRAY_BUFFER, dataByteCount, dataArrayPtr, GL_STATIC_DRAW);
}

void oge::OGLObject::draw(const glm::mat4& projectionMatrix,
                          const glm::mat4& viewMatrix,
                          sf::Shader& mvpShader,
                          bool shadowMap /* = false */) {


    glBindVertexArray(vertexArrayObjectID);

    // calculate new mvp matrix
    // model, then view, then projection
    glm::mat4 MVP = projectionMatrix * viewMatrix * getModelMatrix();

    // upload mvp matrix
    // normal opengl way
    // glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    // sfml way
    mvpShader.setUniform("MVP", sf::Glsl::Mat4(&MVP[0][0]));
    if (shadowMap) {
        // we need this shadow map mvp matrix later
        // (in the next call to this function on this object
        //   - the non shadow map call)
        lastShadowMVP = MVP;
    }
    else {
        mvpShader.setUniform("M", sf::Glsl::Mat4(&getModelMatrix()[0][0]));
        mvpShader.setUniform("V", sf::Glsl::Mat4(&viewMatrix[0][0]));

        glm::mat4 biasMatrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );
        glm::mat4 depthBiasMVP = biasMatrix*lastShadowMVP;

        mvpShader.setUniform("LightColor", scene->getLight().getColor());
        mvpShader.setUniform("LightPower", scene->getLight().getPower());
        mvpShader.setUniform("DepthBiasMVP", sf::Glsl::Mat4(&depthBiasMVP[0][0]));
        mvpShader.setUniform("shadowMap", scene->getSystem()->getShadowMap().getTexture());
        mvpShader.setUniform("LightInvDirection_worldspace", sf::Glsl::Vec3(scene->getLightInvDir()[0],
                                                                            scene->getLightInvDir()[1],
                                                                            scene->getLightInvDir()[2]));
        if (&mvpShader == &(scene->getSystem()->getTextureProgram())) {
            mvpShader.setUniform("myTextureSampler", surfaceColorTexture);
        }
    }
    // texture uniforms are not set until bind is called
    sf::Shader::bind(&mvpShader);



    // assume attribute/buffer 0 is triangle vertices
    // number of triangles is number of corners / 3
    GLsizei triangleCount = buffers[0].array.size() / 3;

    // if only rendering shadow map, only need first attribute
    size_t attributeCount = (shadowMap) ? 1 : buffers.size();

    for (GLuint attribute = 0; attribute < attributeCount; ++attribute) {
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(attribute);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[attribute].id);
        glVertexAttribPointer(
            attribute,          // attribute - must match the layout in the shader
            buffers[attribute].array.size() / triangleCount,  // number of components for the attribute
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void *) 0          // array buffer offset
        );
    }

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, triangleCount); // Starting from vertex 0; how many triangles

    for (GLuint attribute = 0; attribute < attributeCount; ++attribute) {
        glDisableVertexAttribArray(attribute);
    }
}

void oge::OGLObject::deleteBuffer(GLuint *bufferIDPtr) {
    glDeleteBuffers(1, bufferIDPtr);
    *bufferIDPtr = 0;
}

void oge::OGLObject::deleteAllBuffers() {
    for (auto bufferIt = buffers.begin(); bufferIt != buffers.end(); ++bufferIt) {
        if (bufferIt->id) {
            deleteBuffer(&(bufferIt->id));
        }
    }
}

oge::OGLObject::~OGLObject() {
    std::cout << "info: OGLObject destructor called\n";
    deleteAllBuffers();
    deleteVAO();
}

void oge::OGLObject::uploadData() {
    if (! vertexArrayObjectID) {
        createVAO();
    }

    bindVAO();

    // upload buffers
    for (auto bufferIt = buffers.begin(); bufferIt != buffers.end(); ++bufferIt) {
        uploadBufferData(&(bufferIt->id), &(bufferIt->array[0]), sizeof(float) * bufferIt->array.size());
    }
}

const std::vector<float>& oge::OGLObject::getBufferArray(GLuint attributeNumber) const {
    return buffers[attributeNumber].array;
}

void oge::OGLObject::setBufferArray(GLuint attributeNumber, const std::vector< float >& newArray) {
    if (attributeNumber < buffers.size())
        buffers[attributeNumber] = newArray;
    // TODO: else throw exception
}

GLuint oge::OGLObject::addBufferArray(const std::vector<float> &newArray) {
    buffers.push_back(Buffer(newArray));
    return buffers.size() - 1;
}

void oge::OGLObject::update() {
    // override me
}

void oge::OGLObject::handleEvent(const sf::Event& event) {
    // override me
}

oge::OGLSystem* oge::OGLObject::getSystem() const {
    return scene->getSystem();
}

oge::Scene* oge::OGLObject::getScene() const {
    return scene;
}
