//
// Created by Ryan Strauss on 12/10/19.
// Modified by V. Prins 2021-07-16
//

#include <random>
#include <chrono>
#include <string_view>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <memory>
#include "Simulation.h"

Simulation::Simulation(std::vector<SpeciesProperties> species, int window_width, int window_height, int simulation_width, int simulation_height,
                       float draw_size, int log_interval, std::string log_file, int exit_after, bool headless, bool fullscreen, bool light_scheme, bool log_screenshot)
{
    this->_m_is_headless = headless;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    if (!_m_is_headless)
    {
        if (fullscreen)
        {
            _m_window = std::make_shared<sf::RenderWindow>(sf::VideoMode(desktop.width, desktop.height, desktop.bitsPerPixel), "Ursoup", sf::Style::Fullscreen);
        }
        else
        {
            _m_window = std::make_shared<sf::RenderWindow>(sf::VideoMode(window_width, window_height, desktop.bitsPerPixel), "Ursoup", sf::Style::Titlebar);
        }
    }
    else
    {
        auto render_texture = std::make_shared<sf::RenderTexture>();
        render_texture->create(simulation_width, simulation_height);
        _m_window = render_texture;
    }

    this->_m_window_width = (fullscreen ? desktop.width : window_width);
    this->_m_window_height = (fullscreen ? desktop.height : window_height);
    this->_m_simulation_width = simulation_width;
    this->_m_simulation_height = simulation_height;
    this->_m_draw_size = draw_size * _scale();
    this->_m_log_interval = log_interval;
    this->_m_use_light_scheme = light_scheme;
    this->_m_species = species;
    this->_m_log_file = log_file;
    this->_m_exit_after = exit_after;
    this->_m_log_screenshot = log_screenshot;

    for (size_t i = 0; i < _m_species.size(); i++)
    {
        _m_population_count.push_back({0, 0, 0, 0, 0});
    }

    _init_log();

    _m_view = sf::View(sf::FloatRect(0, 0, simulation_width, simulation_height));
    _m_window->setView(_m_view);
}

void Simulation::_reset_population_count()
{
    for (auto &&species : _m_population_count)
    {
        species.clear();
        species.insert(species.begin(), 5, 0);
    }
}

Simulation::~Simulation() = default;

void Simulation::_try_log()
{
    if (_m_since_last_log >= _m_log_interval)
    {
        _m_since_last_log = 0;
        _log();
        _reset_population_count();
        if (_m_log_screenshot)
        {
            _take_screenshot();
        }
    }
}

void Simulation::run(float particle_density)
{
    int n_particles = floor(particle_density * _m_simulation_width * _m_simulation_height);

    for (int i = 0; i < n_particles; ++i)
    {
        _add_particle(_get_random_float() * _m_simulation_width, _get_random_float() * _m_simulation_height, i % _m_species.size());
    }

    if (!_m_is_headless)
    {
        std::shared_ptr<sf::RenderWindow> window = std::dynamic_pointer_cast<sf::RenderWindow>(_m_window);

        while (window->isOpen())
        {
            if (_handle_input(window) || _m_generation > _m_exit_after)
                break;

            if (!_m_is_paused)
                _update();

            _try_log();
            window->display();
        }
    }
    else
    {
        while (_m_generation <= _m_exit_after)
        {
            _update();
            for (size_t i = 0; i < _m_particle_manager.size(); i++)
            {
                _neighborhood_color(_m_particle_manager[i]);
            }
            _try_log();
        }
    }
}

void Simulation::_add_particle(float x, float y, int species)
{
    float phi = _get_random_float();

    Particle particle = Particle(x, y, phi, species);

    sf::CircleShape shape(_m_draw_size, 6);

    shape.setPosition(x, y);
    shape.setFillColor(_m_species[species].color);
    shape.setOutlineColor(_m_use_light_scheme ? sf::Color::White : sf::Color::Black);
    shape.setOutlineThickness(0);

    _m_particle_manager.add(particle);
    _m_shapes.emplace_back(shape);
}

void Simulation::_update()
{
    _m_generation++;
    _m_since_last_log++;
    _m_particle_manager.update(_m_simulation_width, _m_simulation_height, _m_species);

    // Skip every other frame (useful for alpha=180).
    if (!_m_is_headless && _m_is_renderframe)
    {
        _render();
    }
    _m_is_renderframe = !_m_is_renderframe;
}

sf::Color Simulation::_neighborhood_color(const Particle &particle)
{
    int n_neighbors = particle.n_neighbors;
    int n_close_neighbors = particle.n_close_neighbors;
    int species = particle.species;

    if (n_close_neighbors > 15) //Nt,r=1.3 > 15
    {
        _m_population_count[species][3]++;
        return sf::Color::Magenta;
    }
    else if (15 < n_neighbors && n_neighbors <= 35) //15 < Nt,r=5 ≤ 35
    {
        _m_population_count[species][1]++;
        return sf::Color::Blue;
    }
    else if (n_neighbors > 35) //Nt,r=5 > 35
    {
        _m_population_count[species][0]++;
        return sf::Color::Yellow;
    }
    else if (13 <= n_neighbors && n_neighbors <= 15) //13 ≤ Nt,r=5 ≤ 15
    {
        _m_population_count[species][2]++;
        return {200, 100, 0, 255}; // Brown
    }

    _m_population_count[species][4]++;
    return sf::Color::Green;
}

void Simulation::_render()
{
    _m_window->clear(_m_use_light_scheme ? sf::Color::White : sf::Color::Black);

    for (size_t i = 0; i < _m_shapes.size(); ++i)
    {
        Particle particle = _m_particle_manager[i];
        _m_shapes[i].setPosition(particle.position.x, particle.position.y);
        _m_shapes[i].setRotation(particle.angle());
        sf::Color density_color = _neighborhood_color(particle);
        sf::Color species_color = _m_species[particle.species].color;

        sf::Color c = _m_use_density_colors ? density_color : species_color;
        _m_shapes[i].setFillColor(c);

        _m_window->draw(_m_shapes[i]);
    }
}

bool Simulation::_handle_input(std::shared_ptr<sf::RenderWindow> window)
{
    sf::Event event;

    while (window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window->close();
            return true;
        }
        if (event.type == sf::Event::KeyPressed)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                _m_is_paused = !_m_is_paused;
            }
            if (_m_is_paused && sf::Keyboard::isKeyPressed(sf::Keyboard::F))
            {
                // Update twice because of skipped frame.
                _update();
                _update();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
            {
                _m_use_density_colors = !_m_use_density_colors;
                if (_m_is_paused)
                {
                    _render();
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                _take_screenshot();
            }
        }
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        sf::Vector2i mouse_position = sf::Mouse::getPosition(*window);
        int random_species = floor(_get_random_float() * _m_species.size());
        _add_particle(mouse_position.x * _scale(), mouse_position.y * _scale(), random_species);
    }

    return false;
}

void Simulation::_save_screenshot(std::string file_name)
{
    sf::Texture texture;
    if (!_m_is_headless)
    {
        texture.create(_m_window_width, _m_window_height);
        auto window = std::dynamic_pointer_cast<sf::RenderWindow>(_m_window);
        texture.update(*window);
    }
    else
    {
        auto window = std::dynamic_pointer_cast<sf::RenderTexture>(_m_window);
        texture = window->getTexture();
    }
    sf::Image screenshot = texture.copyToImage();
    screenshot.saveToFile(file_name + ".png");
}

void Simulation::_take_screenshot()
{
    _m_use_density_colors = true;
    _render();
    _save_screenshot("density_" + std::to_string(_m_generation));

    _m_use_density_colors = false;
    _render();
    _save_screenshot("species_" + std::to_string(_m_generation));
}

float Simulation::_get_random_float()
{
    static std::random_device rd;
    static std::mt19937 engine(rd());
    static std::uniform_real_distribution<float> dist(0, 1);
    return dist(engine);
}

float Simulation::_scale() const
{
    return static_cast<float>(_m_simulation_width) / static_cast<float>(_m_window_width);
}

void Simulation::_log() const
{
    std::ofstream log_file(_m_log_file, std::ios_base::app);
    log_file << _m_generation << ",";
    for (size_t i = 0; i < _m_species.size(); i++)
    {
        for (auto count : _m_population_count[i])
        {
            float avg = static_cast<float>(count) / static_cast<float>(_m_log_interval);
            log_file << avg << ",";
        }
    }
    log_file << std::endl;
}

void Simulation::_init_log() const
{
    std::ofstream log_file(_m_log_file);
    log_file << "Time,";
    std::vector<std::string> headers{"Yellow", "Blue", "Brown", "Magenta", "Green"};
    for (size_t i = 0; i < _m_species.size(); i++)
    {
        for (auto header : headers)
        {
            log_file << header << ",";
        }
    }
    log_file << std::endl;
}
