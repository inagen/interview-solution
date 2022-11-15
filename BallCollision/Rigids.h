#pragma once
#include "Renderer.h"

struct Ball : Drawable { // Шар надо уметь рисовать
    sf::Vector2f p;
    sf::Vector2f dir;
    float r = 0;
    float speed = 0;

    Ball() {
        fillColor = sf::Color::White;
    }

    float weight() {
        return r * r;
    }

    void draw(sf::RenderWindow& window) const final {
        sf::CircleShape gball;
        gball.setRadius(r);
        gball.setPosition(p.x - r, p.y - r);
        gball.setFillColor(fillColor);
        window.draw(gball);
    }

    void move(float deltaTime) {
        float dx = dir.x * speed * deltaTime;
        float dy = dir.y * speed * deltaTime;
        p.x += dx;
        p.y += dy;
    }
};

struct Wall {
    sf::Vector2f begin;
    sf::Vector2f end;
    
    Wall(const sf::Vector2f& b, const sf::Vector2f& e) : begin(b), end(e) {

    }
};