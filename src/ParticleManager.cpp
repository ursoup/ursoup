//
// Created by Ryan Strauss on 12/10/19.
// Modified by V. Prins 2021-07-16
//

#include "ParticleManager.h"
#include "KDTree.h"

ParticleManager::ParticleManager() = default;

ParticleManager::ParticleManager(const ParticleManager &other)
{
    for (const Particle &particle : other.particles)
        particles.emplace_back(particle);
}

ParticleManager::~ParticleManager() = default;

Particle ParticleManager::operator[](int i) const
{
    return particles[i];
}

void ParticleManager::add(const Particle &particle)
{
    particles.emplace_back(particle);
}

void ParticleManager::update(const float simulation_width, const float simulation_height, const std::vector<SpeciesProperties> species)
{
    KDTree tree(simulation_width, simulation_height);
    for (Particle &particle : particles)
        tree.insert(&particle);

    for (Particle &particle : particles)
    {
        particle.update_phi(tree.search(&particle, species[particle.species].perception), species[particle.species].alpha, species[particle.species].beta, simulation_width, simulation_height);
        particle.move(simulation_width, simulation_height, species[particle.species].speed);

    }
}

int ParticleManager::size() const
{
    return particles.size();
}
