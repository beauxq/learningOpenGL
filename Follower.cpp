// object that follows another object

#include "Follower.h"

#include <iostream>

void Follower::update() {
    // spin
    rotate(getSystem()->getClock().getElapsedTime().asMilliseconds() % 3, 0);
}

Follower::Follower(oge::Scene& _scene) : OGLObject(_scene) {}
