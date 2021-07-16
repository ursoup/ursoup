//
// Created by Ryan Strauss on 12/9/19.
// Modified by V. Prins 2021-07-16
//

#ifndef Particle_H
#define Particle_H

#include <vector>
#include "Vector2D.h"

class Particle {
public:
    Vector2D position;
    float phi;
    int species;
    int n_neighbors = 0;
    int n_close_neighbors = 0;

    // Constructors
    Particle(float x, float y, float phi, int species);

    Particle(const Particle &other);

    ~Particle();

    // Operators
    Particle &operator=(const Particle &other);

    // Methods
    void update_phi(const std::vector<Particle*> &neighbors, const float alpha, const float beta, const float width, const float height);

    void move(const float max_width, const float max_height, const float speed);

    Vector2D velocity() const;

    float angle() const;

};

#endif
