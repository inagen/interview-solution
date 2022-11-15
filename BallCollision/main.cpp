#define DEBUG_RENDER

#include "SFML/Graphics.hpp"
#include "Math.h"
#include "Renderer.h"
#include "Rigids.h"
#include "InputManager.h"
#include "GlobalState.h"
#include "PhysicsEngine.h"

/*
* Напишите оптимальный обрабочик столкновений объектов между собой и краями окна без использования готовых 
* движков симуляции физики. Объекты абсолютно упругие, 
* описываются кругами заданного радиуса и имеют заданный вектор скорости. 
* Объекты двигаются в плоскости окна и абсолютно упруго отскакивают друг от друга и от краёв окна. 
* Массы объектов пропорциональны площади кругов, их описывающих.
*/

/*
* Моё решение:
* Во-первых был произведен небольшой рефакторинг кода, из существующего кода были
* вычленены сущности, ответственные за определенный класс задач. 
* Так же был добавлен интерфейс подписок на события, позволяющий модулям программы
* подписываться на события ввода пользователя.  
*/


/*
* Как можно улучшить моё решение?
* На ум приходит добавление многопоточности и модели акторов. Каждый модуль будет работать в отдельном потоке
* и делиться событиями с остальными.
* Так же надо как-то избавиться от проблемы перелёта шаров через друг друга и стены
*/
int main() {    
    GlobalState state;

    InputManager input_manager(state);
    PhysicsEngine physics(state);
    Renderer renderer(state);

    input_manager.subscribers.subscribe(InputEventType::WindowClose, &state);    

    physics.initialize();
    state.refreshTimer();

    while (state.shouldContinueMainLoop()) {
        input_manager.processInput();

        float deltaTime = state.timer.getDeltaTime();

        // Этот цикл существует для того, чтобы снизить вероятность
        // того, что шары перепрыгнут через друг друга или через стенку.
        // Так как с уменьшением deltaTime, уменьшается и расстояние "прыжка"
        // Однако это не спасает от проблемы перелета полностью...
        for (size_t i = 0; i < state.COMPUTE_ITERATIONS; i++) {
            physics.proceed(deltaTime / state.COMPUTE_ITERATIONS);
            renderer.drawObjects(physics.balls);
            state.refreshTimer();
        }
        renderer.drawFps();
    }
    return 0;
}
