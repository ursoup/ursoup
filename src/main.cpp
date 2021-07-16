//
// Modified by V. Prins 2021-07-16
//

#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include "Simulation.h"
#include "cxxopts.hpp"

int main(int argc, char **argv)
{
    cxxopts::Options options("Ursoup", "Runs a simulation of a Primordial Particle System (PPS) based on work by Schmickl et al. (2016).");

    options.add_options()
        ("window_width", "Width of the window.",cxxopts::value<int>()->default_value(std::to_string(Simulation::default_window_width)))
        ("window_height", "Height of the window.",cxxopts::value<int>()->default_value(std::to_string(Simulation::default_window_height)))
        ("simulation_width", "Width of the simulation.",cxxopts::value<int>()->default_value(std::to_string(Simulation::default_simulation_width)))
        ("simulation_height", "Height of the simulation.",cxxopts::value<int>()->default_value(std::to_string(Simulation::default_simulation_height)))
        ("draw_size", "The draw size of the particles.",cxxopts::value<float>()->default_value(std::to_string(Simulation::default_draw_size)))
        ("particle_density", "Initial particle density.",cxxopts::value<float>()->default_value(std::to_string(Simulation::default_particle_density)))
        ("log_interval", "Population logging interval.",cxxopts::value<int>()->default_value(std::to_string(Simulation::default_log_interval)))
        ("config_file", "Species configuration file.",cxxopts::value<std::string>()->default_value(std::string(Simulation::default_config_file)))
        ("log_file", "Species logging file.",cxxopts::value<std::string>()->default_value(std::string(Simulation::default_log_file)))
        ("exit_after", "Exit program after [n] steps.",cxxopts::value<int>()->default_value(std::to_string(Simulation::default_exit_after)))
        ("headless", "Run headless.")
        ("fullscreen", "Runs the simulation in a fullscreen window.")
        ("light_scheme", "Uses a light color scheme.")
        ("log_screenshot", "Whether or not to log screenshots.")
        ("help", "Displays this help message.");

    auto result = options.parse(argc, argv);

    if (result["help"].as<bool>())
    {
        std::cout << options.help() << std::endl;
        return EXIT_SUCCESS;
    }

    std::ifstream config_file(result["config_file"].as<std::string>(), std::ios::in);

    if (!config_file.is_open() || !config_file.good())
    {
        std::cout << "Could not open config file." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<SpeciesProperties> species;

    while (!config_file.eof())
    {
        float speed, perception, alpha, beta;
        int R, G, B, A;

        config_file >> speed;
        config_file >> perception;
        config_file >> alpha;
        config_file >> beta;

        config_file >> R;
        config_file >> G;
        config_file >> B;
        config_file >> A;

        if (config_file.eof())
        {
            break;
        }

        species.emplace_back(SpeciesProperties(speed, perception, alpha, beta, sf::Color(R, G, B, A)));
    }

    config_file.close();

    Simulation simulation(species, result["window_width"].as<int>(), result["window_height"].as<int>(),
                          result["simulation_width"].as<int>(), result["simulation_height"].as<int>(),
                          result["draw_size"].as<float>(), result["log_interval"].as<int>(), result["log_file"].as<std::string>(), result["exit_after"].as<int>(),
                          result["headless"].as<bool>(), result["fullscreen"].as<bool>(), result["light_scheme"].as<bool>(), result["log_screenshot"].as<bool>());

    simulation.run(result["particle_density"].as<float>());

    return EXIT_SUCCESS;
}
