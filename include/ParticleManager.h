//
// Created by Ryan Strauss on 12/10/19.
// Modified by V. Prins 2021-07-16
//

#ifndef ParticleManager_H
#define ParticleManager_H

#include <vector>
#include "Particle.h"
#include "SpeciesProperties.h"

class ParticleManager {
private:
    std::vector<Particle> particles;

public:

    ParticleManager();

    ParticleManager(const ParticleManager &other);

    ~ParticleManager();

    Particle operator[](int i) const;

    void add(const Particle &particle);

    void update(const float simulation_width, const float simulation_height, const std::vector<SpeciesProperties> species);

    int size() const;

};

#endif
