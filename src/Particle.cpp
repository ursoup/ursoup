//
// Created by Ryan Strauss on 12/9/19.
// Modified by V. Prins 2021-07-16
//

#include <cmath>
#include "Particle.h"

Particle::Particle(float x, float y, float phi, int species)
{
    position = Vector2D{x, y};
    this->phi = phi;
    this->species = species;
}

Particle::Particle(const Particle &other)
{
    position = other.position;
    phi = other.phi;
    species = other.species;
    n_neighbors = other.n_neighbors;
    n_close_neighbors = other.n_close_neighbors;
}

Particle::~Particle() = default;

Particle &Particle::operator=(const Particle &other) = default;

void Particle::update_phi(const std::vector<Particle *> &neighbors, const float alpha, const float beta, const float width, const float height)
{
    int left = 0, right = 0;
    n_close_neighbors = 0;

    for (const Particle *particle : neighbors)
    {
        if (this == particle)
            continue;

        Vector2D pos = particle->position - this->position;

        if (pos.x > width / 2)
            pos.x -= width;
        if (pos.x <= -width / 2)
            pos.x += width;

        if (pos.y > height / 2)
            pos.y -= height;
        if (pos.y <= -height / 2)
            pos.y += height;

        if (pos.length2() < 1.3 * 1.3)
        {
            n_close_neighbors++;
        }

        float y = (cos(phi * 2 * M_PI) * pos.y) - (sin(phi * 2 * M_PI) * pos.x);
        (y > 0) ? left++ : right++;
    }

    n_neighbors = right + left;
    float sign = (left > right) ? 1 : -1; // Favour going right.
    phi += (alpha / 360.0) + ((beta / 360.0) * n_neighbors * sign);

    if (phi < 0)
        phi += 1;
    if (phi >= 1)
        phi -= 1;
}

void Particle::move(const float width, const float height, const float speed)
{
    Vector2D velocity(cos(phi * 2 * M_PI), sin(phi * 2 * M_PI));
    position += this->velocity() * speed;

    // If particle leaves the screen, update position so the particle wraps around
    if (position.x < 0)
        position.x += width;
    if (position.y < 0)
        position.y += height;
    if (position.x >= width)
        position.x -= width;
    if (position.y >= height)
        position.y -= height;
}

Vector2D Particle::velocity() const
{
    return Vector2D(cos(phi * 2 * M_PI), sin(phi * 2 * M_PI));
}

float Particle::angle() const
{
    Vector2D velocity = this->velocity();
    return (float)(atan2(velocity.x, -velocity.y) * 180 / M_PI);
}
