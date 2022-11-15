#pragma once
#include "SFML/Graphics.hpp"
#include "GlobalState.h"
#include "InputEvents.h"


// Получает ввод от пользователя и рассылает информацию об этом всем подписчикам
struct InputManager  {
    EventManager subscribers;
    GlobalState& state;

    InputManager(GlobalState& s) : state(s) {

    }

    void processInput() {
        sf::Event event;
        while (state.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                subscribers.notify(InputEventType::WindowClose, event);
            }
        }
    }
};