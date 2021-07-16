//
// Created by Ryan Strauss on 12/10/19.
// Modified by V. Prins 2021-07-16
//

#ifndef Simulation_H
#define Simulation_H

#include <vector>
#include <string_view>
#include <iostream>
#include <memory>
#include <fstream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "ParticleManager.h"
#include "SpeciesProperties.h"

class Simulation
{
private:
    std::shared_ptr<sf::RenderTarget> _m_window;
    sf::View _m_view;
    int _m_generation = 0;
    int _m_log_interval;
    int _m_since_last_log = 0;
    int _m_exit_after;
    int _m_window_width, _m_window_height;
    int _m_simulation_width, _m_simulation_height;
    std::vector<SpeciesProperties> _m_species;
    std::vector<std::vector<int>> _m_population_count;
    ParticleManager _m_particle_manager;
    std::vector<sf::CircleShape> _m_shapes;
    bool _m_use_light_scheme;
    bool _m_log_screenshot;
    std::string _m_log_file;
    bool _m_is_renderframe = true;
    float _m_draw_size;
    bool _m_use_density_colors = false;
    bool _m_is_paused = false;
    bool _m_is_headless = false;
    void _add_particle(float x, float y, int species);
    void _update();
    void _render();
    void _log() const;
    void _reset_population_count();
    void _init_log() const;
    bool _handle_input(std::shared_ptr<sf::RenderWindow> window);
    float static _get_random_float();
    float _scale() const;
    sf::Color _neighborhood_color(const Particle& particle);

    void _try_log();

    void _take_screenshot();

    void _save_screenshot(std::string file_name);

public:
    constexpr static int frame_rate = 60;
    constexpr static float default_draw_size = 2;
    constexpr static int default_window_width = 600;
    constexpr static int default_window_height = 600;
    constexpr static int default_simulation_width = 150;
    constexpr static int default_simulation_height = 150;
    constexpr static int default_log_interval = 500;
    constexpr static int default_exit_after = 100000;
    constexpr static std::string_view default_config_file = "config.ini";
    constexpr static std::string_view default_log_file = "log.csv";
    constexpr static float default_particle_density = 0.08;
    Simulation(std::vector<SpeciesProperties> species, int window_width, int window_height, int simulation_width, int simulation_height, float draw_size, int log_interval, std::string log_file, int exit_after, bool headless = false, bool fullscreen = false, bool light_scheme = false, bool log_screenshot = false);
    ~Simulation();
    void run(float particle_density);
};

#endif
