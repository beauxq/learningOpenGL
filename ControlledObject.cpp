// implementation fo ControlledObject

#include "ControlledObject.h"

#include <iostream>

ControlledObject::ControlledObject(oge::Scene& _scene) : OGLObject(_scene) {}

void ControlledObject::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved)
    {
        if (getSystem()->leftMouseButtonIsDown())
        {
            // rotate
            int xDiff = event.mouseMove.x - getSystem()->getPreviousMouseLocation().x;
            int yDiff = event.mouseMove.y - getSystem()->getPreviousMouseLocation().y;

            rotate(xDiff, yDiff);
        }
        if (getSystem()->rightMouseButtonIsDown()) {
            // translate
            int xDiff = event.mouseMove.x - getSystem()->getPreviousMouseLocation().x;
            int yDiff = event.mouseMove.y - getSystem()->getPreviousMouseLocation().y;

            translate(xDiff * 0.002f * getScene()->getCamera().distanceFromFocusPoint,
                      yDiff * -0.002f * getScene()->getCamera().distanceFromFocusPoint,
                      0);
        }
    }
}
