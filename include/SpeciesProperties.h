//
// Created by V. Prins on 2021-06-14.
//

#ifndef SpeciesProperties_H
#define SpeciesProperties_H

#include <SFML/Graphics.hpp>

struct SpeciesProperties
{
    float speed;
    float perception;
    float alpha;
    float beta;
    sf::Color color;

    SpeciesProperties(float speed, float perception, float alpha, float beta, sf::Color color)
        : speed{speed}, perception{perception}, alpha{alpha}, beta{beta}, color{color}{};
};

#endif
