#pragma once
#include "SFML/Graphics.hpp"
#include "Math.h"
#include "InputEvents.h"


// ����� ��� �������� ������� � FPS
struct Timer {
    Math::MiddleAverageFilter<float, 100> fpscounter{};
    sf::Clock clock{};
    float lasttime{};
    float deltatime{};

    Timer() {
        lasttime = clock.restart().asSeconds();
    }

    void makeStep() {
        float current_time = clock.getElapsedTime().asSeconds();
        deltatime = current_time - lasttime;
        fpscounter.push(1.0f / (current_time - lasttime));
        lasttime = current_time;
    }

    float getDeltaTime() {
        return deltatime;
    }

    float getFps() {
        return fpscounter.getAverage();
    }
};


// ����� �������� ����������� ��������� ���������. � ������� ���������
struct GlobalState  : InputListener  {
    int WINDOW_X = 1024;
    int WINDOW_Y = 768;
    const int MAX_BALLS = 99;
    const int MIN_BALLS = 101;
    const int COMPUTE_ITERATIONS = 8;

    sf::RenderWindow window;
    Timer timer;
    bool should_continue = true;

    // ������� ������� �� InputManager
    void listenerUpdate(InputEventType eventType, sf::Event event) final {
        if (eventType == InputEventType::WindowClose) {
            window.close();
            stopMainLoop();
        }
    }

    GlobalState() : window(sf::VideoMode(WINDOW_X, WINDOW_Y), "ball collision demo") {
        srand(time(NULL));
    }

    void refreshTimer() {
        timer.makeStep();
    }
    
    void stopMainLoop() {
        should_continue = false;
    }

    bool shouldContinueMainLoop() {
        return should_continue;
    }
};