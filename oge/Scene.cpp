// implementation of Scene class

#include <iostream>
#include <sstream>
#include "Scene.h"

#include "OGLObject.h"
#include "OGLSystem.h"

#include "../glm/gtx/vector_angle.hpp"

// for debugging
#define VERBOSE
#ifdef VERBOSE
    #define log(out)  std::cout << out << std::endl;
#else
    #define log(out)  ;
#endif

std::string glm_to_string(const glm::vec3& vec) {
    return std::to_string(vec[0]) + ' ' + std::to_string(vec[1]) + ' ' + std::to_string(vec[2]);
}

const glm::vec4 oge::Scene::DEFAULT_BACKGROUND_COLOR = {0.18f, 0.0f, 0.4f, 0.0f};
const std::vector< glm::vec3 > oge::Scene::DEFAULT_SCENE_BOUNDS = {
    glm::vec3(30.0f, 30.0f, 30.0f),
    glm::vec3(30.0f, 30.0f, -30.0f),
    glm::vec3(30.0f, -30.0f, 30.0f),
    glm::vec3(30.0f, -30.0f, -30.0f),
    glm::vec3(-30.0f, 30.0f, 30.0f),
    glm::vec3(-30.0f, 30.0f, -30.0f),
    glm::vec3(-30.0f, -30.0f, 30.0f),
    glm::vec3(-30.0f, -30.0f, -30.0f)
};
const glm::vec3 oge::Scene::DEFAULT_SCENE_CENTER = glm::vec3(0.0f, 0.0f, 0.0f);

oge::Scene::Scene(OGLSystem& _system) : system(&_system),
                                        sceneBounds(DEFAULT_SCENE_BOUNDS),
                                        sceneCenter(DEFAULT_SCENE_CENTER),
                                        camera(this) {}

void oge::Scene::initialize() {
    glClearColor(DEFAULT_BACKGROUND_COLOR[0],
                 DEFAULT_BACKGROUND_COLOR[1],
                 DEFAULT_BACKGROUND_COLOR[2],
                 DEFAULT_BACKGROUND_COLOR[3]);

    // default camera settings  TODO: put defaults in constants
    // Projection matrix : 45° Field of View, aspect ratio, display range : 0.1 unit <-> 100 units
    // camera.setVerticalFieldOfView(45.0f);  // tutorial told me it was degrees, it's not
    camera.setVerticalFieldOfView(0.785398f);  // 45 degrees
    //camera.setNearClip(0.1f);
    //camera.setFarClip(200.0f);
    // Or, for an ortho camera :
    //camera.projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

    // location and direction
    camera.setFocusPointKeepingDirectionAndDistance(glm::vec3(0, 0, 0));
    camera.setDistanceFromFocusPoint(4.0f);
    camera.setDirectionFromFocusPoint(glm::vec3(0, 0, 1));
    camera.setUpDirection(glm::vec3(0, 1, 0));

    // TODO: put defaults in constants
    light.setPosition(sf::Glsl::Vec3(-30.0f, 150.0f, 45.0f));
    light.setColor(sf::Glsl::Vec3(1, 1, 1));
    light.setPower(1.0f);
}

oge::OGLSystem*& oge::Scene::getSystem() {
    return system;
}

oge::Camera& oge::Scene::getCamera() {
    return camera;
}

void oge::Scene::setCamera(const Camera& _camera) {
    camera = _camera;
}

oge::Light& oge::Scene::getLight() {
    return light;
}

void oge::Scene::setLight(const Light& _light) {
    light = _light;
}

void oge::Scene::setLightPOV(const bool _lightPOV = true) {
    lightPOV = _lightPOV;
}

const bool& oge::Scene::getLightPOV() const {
    return lightPOV;
}

std::vector< glm::vec3 >& oge::Scene::getSceneBounds() {
    return sceneBounds;
}

void oge::Scene::setSceneBounds(std::vector< glm::vec3 >& bounds) {
    sceneBounds = bounds;
}

glm::vec3& oge::Scene::getSceneCenter() {
    return sceneCenter;
}

void oge::Scene::setSceneCenter(const glm::vec3& center) {
    sceneCenter = center;
}

void oge::Scene::setBackGroundColor(const glm::vec4& _color) {
    glClearColor(_color[0], _color[1], _color[2], _color[3]);
}

void oge::Scene::update() {
    // override me
    camera.update();
    for (auto object : objects) {
        object.object->update();
    }
}

void oge::Scene::handleEvent(const sf::Event& event) {
    // keep parent in override unless you don't want it to call these handlers automatically
    camera.handleEvent(event);
    for (auto object : objects) {
        object.object->handleEvent(event);
    }
}

void oge::Scene::draw() {
    // TODO: test light inside the camera projection

    // create shadow map
    glBindFramebuffer(GL_FRAMEBUFFER, system->FramebufferName);
    glViewport(0,0,1024,1024);  // TODO: magic numbers

    // Clear the screen
    // glClear( GL_COLOR_BUFFER_BIT );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sf::Shader::bind(&(system->getCreateShadowMapProgram()));

    // Compute the MVP matrix from the light's point of view
    setLightInvDir();
    setLightViewMatrix();
    glm::mat4 projectionMatrix = getLightProjectionMatrix();
    glm::mat4 viewMatrix = getLightViewMatrix();

    for (auto object : objects) {
        object.object->draw(projectionMatrix, viewMatrix, system->getCreateShadowMapProgram(), true);
    }


    // now the image to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, system->getWindow().getSize().x, system->getWindow().getSize().y);
    // TODO: do we need this viewport command? (without it, test edges of big windows > 1024)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto object : objects) {
        sf::Shader::bind(object.program);  // use the shader for this object
        // glUseProgram(programID);

        if (lightPOV) {
            // use projection matrix and view matrix from the light's point of view
            object.object->draw(projectionMatrix, viewMatrix, *(object.program));
        }
        else {
            object.object->draw(camera.getProjectionMatrix(), camera.getViewMatrix(), *(object.program));
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  // TODO: probably don't need this
}

void oge::Scene::setLightViewMatrix() {
    // find light up direction
    // the normal of light position, focus point, camera position is the axis for rotation
    glm::vec3 rotateAxis = glm::cross(camera.getCameraLocation() - camera.getFocusPoint(),
                                      lightInvDir);
    float rotateAngle = glm::angle(normalize(camera.getCameraLocation() - camera.getFocusPoint()),
                                   normalize(lightInvDir));
    glm::mat4 upRotateMatrix = rotate(glm::mat4(1.0f), rotateAngle, rotateAxis);
    glm::vec3 lightUpDirection = upRotateMatrix * glm::vec4(camera.getUpDirection(), 0.0f);

    lightViewMatrix = lookAt(glm::vec3(light.getPosition().x,
                                       light.getPosition().y,
                                       light.getPosition().z),
                             sceneCenter,
                             lightUpDirection);
}

glm::mat4 oge::Scene::getLightViewMatrix() const {
    return lightViewMatrix;
}

glm::mat4 oge::Scene::getLightProjectionMatrix() const {
    // find the camera view area corners and the light location
    glm::vec3 corners[6];
    corners[0] = camera.getCameraLocation();
    glm::vec3 cameraDirection = camera.getDirection();
    log("direction got from camera: " + glm_to_string(cameraDirection));
    glm::vec3 middleOfFarClip = corners[0] + camera.getFarClip() * glm::normalize(cameraDirection);
    log("middle of far clip: " + glm_to_string(middleOfFarClip));
    glm::vec3 perpendicularSide(crossVec(cameraDirection, camera.getUpDirection()));
    // ^ perpendicular to both camera direction and up
    log("perpendicular side: " + glm_to_string(perpendicularSide));
    glm::vec3 perpendicularUp(crossVec(perpendicularSide, cameraDirection));
    // ^ this might be down, but we're going to go both directions, so it doesn't matter if it's up or down
    log("perp up: " + glm_to_string(perpendicularUp));
    perpendicularSide = glm::normalize(perpendicularSide);
    perpendicularUp = glm::normalize(perpendicularUp);
    log("degrees: " + std::to_string(camera.getVerticalFieldOfView() / 2.0f));
    log("radians: " + std::to_string(glm::radians(camera.getVerticalFieldOfView() / 2.0f)));
    log("tan:     " + std::to_string(glm::tan(glm::radians(camera.getVerticalFieldOfView() / 2.0f))));
    float distanceToTopOfFarClip = camera.getFarClip() * glm::tan(camera.getVerticalFieldOfView() / 2.0f);
    float distanceToSideOfFarClip = distanceToTopOfFarClip *
                                    (float)system->getWindow().getSize().x /
                                    (float)system->getWindow().getSize().y;
    corners[1] = middleOfFarClip + distanceToTopOfFarClip * perpendicularUp + distanceToSideOfFarClip * perpendicularSide;
    corners[2] = middleOfFarClip + distanceToTopOfFarClip * perpendicularUp - distanceToSideOfFarClip * perpendicularSide;
    corners[3] = middleOfFarClip - distanceToTopOfFarClip * perpendicularUp + distanceToSideOfFarClip * perpendicularSide;
    corners[4] = middleOfFarClip - distanceToTopOfFarClip * perpendicularUp - distanceToSideOfFarClip * perpendicularSide;
    corners[5] = glm::vec3(light.getPosition().x, light.getPosition().y, light.getPosition().z);
    log("bottom two corners: " + glm_to_string(corners[3]) + "  /  " + glm_to_string(corners[4]));

    //this section all for debugging
    log("untransformed corners:");
    for (size_t i = 0; i < 6; ++i) {
        log("[" + std::to_string(corners[i][0]) + ", " + std::to_string(corners[i][1]) + ", " + std::to_string(corners[i][2]) + "],  // " + std::to_string(i));
    }

    // transform them with light view matrix
    glm::vec3 transformedCorners[6];
    log("transformed corners:");
    for (size_t i = 0; i < 6; ++i) {
        glm::vec4 result = lightViewMatrix * glm::vec4(corners[i], 1.0f);
        transformedCorners[i][0] = result[0];
        transformedCorners[i][1] = result[1];
        transformedCorners[i][2] = result[2];
        // these debug messages produce code for a test in openScad
        log("[" + std::to_string(result[0]) + ", " + std::to_string(result[1]) + ", " + std::to_string(result[2]) + "],  // " + std::to_string(i));
    }
    // test - TODO: remove this after lots of testing
    if (transformedCorners[5].x > 0.0001f || transformedCorners[5].y > 0.0001f || transformedCorners[5].z < -0.0001f) {
        std::cerr << "warning: light location transformed to light space isn't 0: "
                  << transformedCorners[5].x << ' ' << transformedCorners[5].y << ' ' << transformedCorners[5].z << std::endl;
    }
    log("bot 2 corners after transform: " + glm_to_string(transformedCorners[3]) + "  /  " + glm_to_string(transformedCorners[4]));

    // make a box that includes all those points
    float maxX = 0, minX = 0, maxY = 0, minY = 0, maxZ = 0, minZ = 0;
    for (size_t i = 0; i < 6; ++i) {
        if (transformedCorners[i][0] < minX) {
            minX = transformedCorners[i][0];
        }
        else if (transformedCorners[i][0] > maxX) {
            maxX = transformedCorners[i][0];
        }
        if (transformedCorners[i][1] < minY) {
            minY = transformedCorners[i][1];
        }
        else if (transformedCorners[i][1] > maxY) {
            maxY = transformedCorners[i][1];
        }
        if (-(transformedCorners[i][2]) < minZ) {
            minZ = -(transformedCorners[i][2]);
        }
        else if (-(transformedCorners[i][2]) > maxZ) {
            maxZ = -(transformedCorners[i][2]);
        }
    }

    log("found box: " + std::to_string(minX) + ", " + std::to_string(maxX) + ", " + std::to_string(minY) + ", " + std::to_string(maxY) + ", " + std::to_string(minZ) + ", " + std::to_string(maxZ));

    return glm::ortho<float>(minX, maxX, minY, maxY, minZ, maxZ);

    //return glm::ortho<float>(-100, 100, -40, 40, 60, 200);
    //return glm::ortho<float>(-30, 30, -20, 20, 60, 200);
    // TODO: make option for spot light :
    // return glm::perspective<float>(45.0f, 1.0f, 60.0f, 200.0f);
}

void oge::Scene::setLightInvDir() {
    lightInvDir = glm::vec3(light.getPosition().x,
                            light.getPosition().y,
                            light.getPosition().z) -
                  sceneCenter;  // glm::vec3(0.5f,2,2);  // TODO: test with different scene centers and light positions
    //lightInvDir = glm::normalize(lightInvDir) * 70.0f;
}

void oge::Scene::registerObject(oge::OGLObject& object, sf::Shader& program) {
    objects.emplace(object, program);
}

const glm::vec3& oge::Scene::getLightInvDir() const {
    return lightInvDir;
}
