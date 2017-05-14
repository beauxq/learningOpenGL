// convert GLM objects to sf::Glsl objects

#ifndef NEWEROPENGLTEST_GLM2SF_H
#define NEWEROPENGLTEST_GLM2SF_H

#include <SFML/Graphics.hpp>
#include "glm/glm.hpp"

#include <iostream>

sf::Glsl::Mat4 glmToSfMat4(const glm::mat4& input) {
    std::cout << "entered conversion function\n";
    sf::Glsl::Mat4 to_return(0);
    std::cout << "created sfml matrix object\n";

    for (size_t rowI = 0; rowI < 4; ++rowI) {
        for (size_t colI = 0; colI < 4; ++colI) {
            std::cout << input[colI][rowI] << ' ';
            to_return.array[rowI*4 + colI] = input[colI][rowI];
        }
        std::cout << std::endl;
    }

    return to_return;
}

#endif //NEWEROPENGLTEST_GLM2SF_H
