#define DEBUG_RENDER

#include "SFML/Graphics.hpp"
#include "Math.h"
#include "Renderer.h"
#include "Rigids.h"
#include "InputManager.h"
#include "GlobalState.h"
#include "PhysicsEngine.h"

/*
* �������� ����������� ��������� ������������ �������� ����� ����� � ������ ���� ��� ������������� ������� 
* ������� ��������� ������. ������� ��������� �������, 
* ����������� ������� ��������� ������� � ����� �������� ������ ��������. 
* ������� ��������� � ��������� ���� � ��������� ������ ����������� ���� �� ����� � �� ���� ����. 
* ����� �������� ��������������� ������� ������, �� �����������.
*/

/*
* �� �������:
* ��-������ ��� ���������� ��������� ����������� ����, �� ������������� ���� ����
* ��������� ��������, ������������� �� ������������ ����� �����. 
* ��� �� ��� �������� ��������� �������� �� �������, ����������� ������� ���������
* ������������� �� ������� ����� ������������.  
*/


/*
* ��� ����� �������� �� �������?
* �� �� �������� ���������� ��������������� � ������ �������. ������ ������ ����� �������� � ��������� ������
* � �������� ��������� � ����������.
* ��� �� ���� ���-�� ���������� �� �������� ������� ����� ����� ���� ����� � �����
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

        // ���� ���� ���������� ��� ����, ����� ������� �����������
        // ����, ��� ���� ����������� ����� ���� ����� ��� ����� ������.
        // ��� ��� � ����������� deltaTime, ����������� � ���������� "������"
        // ������ ��� �� ������� �� �������� �������� ���������...
        for (size_t i = 0; i < state.COMPUTE_ITERATIONS; i++) {
            physics.proceed(deltaTime / state.COMPUTE_ITERATIONS);
            renderer.drawObjects(physics.balls);
            state.refreshTimer();
        }
        renderer.drawFps();
    }
    return 0;
}
