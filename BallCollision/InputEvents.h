#pragma once
#include <map>
#include <set>
#include <deque>

enum class InputEventType { 
    WindowClose 
};


// ����� ���������� �� �����
struct InputListener {
    virtual void listenerUpdate(InputEventType, sf::Event) = 0;
};

// ����� ��� ��������������� ������� �����������
struct EventManager {
    std::map<InputEventType, std::deque<InputListener*>> listeners;

    void subscribe(InputEventType eventType, InputListener* subscriber) {
        listeners[eventType].push_back(subscriber);
    }

    void notify(InputEventType eventType, sf::Event event) {
        for (auto& listener : listeners[eventType]) {
            listener->listenerUpdate(eventType, event);
        }
    }
};