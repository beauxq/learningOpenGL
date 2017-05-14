//
// Created by Doug on 5/11/2017.
//

#ifndef NEWEROPENGLTEST_MAINSCENE_H
#define NEWEROPENGLTEST_MAINSCENE_H

#include "oge/Scene.h"
#include "Follower.h"
#include "ControlledObject.h"

class MainScene : public oge::Scene {
private:
    Follower sphere;
    ControlledObject ring;
    oge::OGLObject background;
    sf::Texture backgroundTexture;

public:
    MainScene(oge::OGLSystem& _system);
    void initialize();
    void handleEvent(const sf::Event& event);
    void update();
};

#endif //NEWEROPENGLTEST_MAINSCENE_H
