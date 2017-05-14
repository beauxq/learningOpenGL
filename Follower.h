//
// Created by Doug on 5/11/2017.
//

#ifndef NEWEROPENGLTEST_FOLLOWER_H
#define NEWEROPENGLTEST_FOLLOWER_H


#include "oge/OGLObject.h"

class Follower : public oge::OGLObject {
public:
    Follower(oge::Scene& _scene);

    void update();
};


#endif //NEWEROPENGLTEST_FOLLOWER_H
