// implementation of MainScene

#include "MainScene.h"

#include <iostream>

MainScene::MainScene(oge::OGLSystem &_system) : Scene(_system), sphere(*this), ring(*this), background(*this) {}

void MainScene::update() {
    oge::Scene::update();
    // sphere moves toward ring
    if (glm::distance(sphere.getLocation(), ring.getLocation()) > 0.05f) {
        sphere.translate(glm::normalize(ring.getLocation() - sphere.getLocation()) * 0.04f);
    }
}

void MainScene::initialize() {
    oge::Scene::initialize();

    ring.loadFromFile("resources/bath spout cover new.stl");
    ring.uploadData();
    ring.setScale(glm::vec3(0.1f));
    registerObject(ring, getSystem()->getColorProgram());


    sphere.loadFromFile("resources/myascii205.stl");
    // 0 vertices
    // 1 color
    // calculate vertex normals (just away from center)
    glm::vec3 thisPoint;
    std::vector< float > normalArray;
    for (size_t i = 0; i < sphere.getBufferArray(0).size(); i += 3) {
        thisPoint[0] = sphere.getBufferArray(0)[i];
        thisPoint[1] = sphere.getBufferArray(0)[i+1];
        thisPoint[2] = sphere.getBufferArray(0)[i+2];
        thisPoint = glm::normalize(thisPoint);
        normalArray.push_back(thisPoint[0]);
        normalArray.push_back(thisPoint[1]);
        normalArray.push_back(thisPoint[2]);
        // This isn't what it looks like... honest...
        // (a vertex normal is not usually just a normalized vertex (only for a sphere))
    }
    sphere.setBufferArray(2, normalArray);
    // 2 vertex normal
    // TODO: use this for lighting
    sphere.uploadData();
    sphere.setScale(glm::vec3(0.1f));
    sphere.setLocation(glm::vec3(0.5f, 0.0f, 0.0f));
    registerObject(sphere, getSystem()->getColorProgram());

    // TODO: 2 textured items

    background.addBufferArray(std::vector<float>({
        -95.0f, -62.0f, -1.0f,
        95.0f, -62.0f, -1.0f,
        -95.0f, 62.0f, -21.0f,
        95.0f, -62.0f, -1.0f,
        95.0f, 62.0f, -21.0f,
        -95.0f, 62.0f, -21.0f
    }));
    background.loadTextureFromFile("resources/background.jpg");
    // vertexUV will be at layout location 1 / attribute 1
    background.addBufferArray(std::vector<float>({
         0.0f, 1.0f,
         1.0f, 1.0f,
         0.0f, 0.0f,
         1.0f, 1.0f,
         1.0f, 0.0f,
         0.0f, 0.0f
    }));
    // vertexNormals will be at layout location 2 / attribute 2
    background.addBufferArray(std::vector<float>({
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    }));
    background.uploadData();
    registerObject(background, getSystem()->getTextureProgram());
}

void MainScene::handleEvent(const sf::Event &event) {
    oge::Scene::handleEvent(event);

    // scene events would go here
}
