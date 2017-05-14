//
// Created by Doug on 5/11/2017.
//

#ifndef NEWEROPENGLTEST_CONTROLLEDOBJECT_H
#define NEWEROPENGLTEST_CONTROLLEDOBJECT_H


#include "oge/OGLObject.h"

class ControlledObject : public oge::OGLObject {
public:
    ControlledObject(oge::Scene& _scene);

    void handleEvent(const sf::Event& event);
};

#endif //NEWEROPENGLTEST_CONTROLLEDOBJECT_H
