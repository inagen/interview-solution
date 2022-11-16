#pragma once
#include "SFML/Graphics.hpp"
#include "GlobalState.h"
#include <array>

std::array<sf::Color, 4> drawable_colors = {
    sf::Color::Red,
    sf::Color::Green,
    sf::Color::Blue,
    sf::Color::Yellow
};

// Ѕазовый класс дл¤ любого рисуемого объекта
struct Drawable {
    virtual void draw(sf::RenderWindow&) const = 0;
    virtual ~Drawable() = default;
    sf::Color fillColor;
};


//  ласс дл¤ отрисовки графики
struct Renderer {
    GlobalState& state;
    Renderer(GlobalState& s) : state(s) {
        // state.window.setFramerateLimit(60);
    }

    void drawFps() {
        char c[32];
        snprintf(c, 32, "FPS: %f", state.timer.getFps());
        std::string string(c);
        sf::String str(c);
        state.window.setTitle(str);
    }


    template<typename ContainerType>
    void drawObjects(const ContainerType& objects) {
        state.window.clear();
        for (const auto& object : objects) {
            object.draw(state.window);
        }
        state.window.display();
    }
};