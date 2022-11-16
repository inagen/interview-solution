#pragma once
#include "Rigids.h"
#include "GlobalState.h"
#include "InputEvents.h"
#include <iostream>


// Класс для сектора, на которые будет разбито игровое пространство
struct Sector {
    size_t id{};

    sf::Vector2f a;
    sf::Vector2f b;
    std::vector<Ball*> belonging_balls;

    void recalculateBelongings(std::vector<Ball>& balls) {
        belonging_balls.clear();
        for (auto& ball : balls) {
            if (containsBall(ball)) {
                belonging_balls.push_back(&ball);
            }
        }
    }

    bool containsBall(const Ball& ball) {
        bool result = false;

        auto r = ball.p.x + ball.r;
        auto l = ball.p.x - ball.r;
        auto u = ball.p.y + ball.r;
        auto d = ball.p.y - ball.r;

        result |= (r > a.x && r < b.x) && (u > a.y && u < b.y);
        result |= (l > a.x && l < b.x) && (u > a.y && u < b.y);
        result |= (r > a.x && r < b.x) && (d > a.y && d < b.y);
        result |= (l > a.x && l < b.x) && (d > a.y && d < b.y);
        
        return result;
    }
};


struct PhysicsEngine {
    GlobalState& state;
    std::vector<Ball> balls;
    std::vector<Wall> walls;
    std::vector<Sector> sectors;

    PhysicsEngine(GlobalState& s) : state(s) {

    }
    
    void initialize() {
        initializeBalls();
        initializeWalls(state.WINDOW_X, state.WINDOW_Y);
        initializeSectors();
    }

    void initializeBalls() {
        for (int i = 0; i < (rand() % (state.MAX_BALLS - state.MIN_BALLS) + state.MIN_BALLS); i++) {
            balls.push_back(makeRandomBall());
        }
    }

    Ball makeRandomBall() {
        Ball newBall;
        newBall.p.x = rand() % state.WINDOW_X;
        newBall.p.y = rand() % state.WINDOW_Y;
        newBall.dir.x = (-5 + (rand() % 10)) / 3.;
        newBall.dir.y = (-5 + (rand() % 10)) / 3.;
        newBall.dir /= std::sqrt(Math::dot(newBall.dir, newBall.dir));
        newBall.r = 5 + rand() % 5;
        newBall.speed = 100 + rand() % 30;
        return newBall;
    }
    
    // Создаём стены по краям экрана
    void initializeWalls(int w, int h) {
        walls.emplace_back(sf::Vector2f(0, 0), sf::Vector2f(w, 0));
        walls.emplace_back(sf::Vector2f(0, 0), sf::Vector2f(0, h));
        walls.emplace_back(sf::Vector2f(w, h), sf::Vector2f(w, 0));
        walls.emplace_back(sf::Vector2f(w, h), sf::Vector2f(0, h));
    }

    void initializeSectors() {
        sectors = createSectors(state.WINDOW_X, state.WINDOW_Y, 8);
        calculateNewSectorsBelongings();
    }

    std::vector<Sector> createSectors(int width, int height, size_t count) {
        auto sec_w = width / count;
        auto sec_h = height / count;

        std::vector<Sector> sectors(std::pow(count, 2));

        size_t id = 0;
        for (size_t i = 0; i < count; ++i) {
            for (size_t j = 0; j < count; ++j) {
                int lux = j * sec_w;
                int luy = i * sec_h;
                auto idx = i * count + j;

                sectors[idx].a = sf::Vector2f(lux, luy);
                sectors[idx].b = sf::Vector2f(lux + sec_w, luy + sec_h);
                sectors[idx].id = idx;
            }
        }

        return sectors;
    }

    void calculateNewSectorsBelongings() {
        for (auto& sector : sectors) {
            sector.recalculateBelongings(balls);
        }
    }

    void proceed(float deltaTime) {
        proceedBalls();
        moveBalls(state.timer.getDeltaTime());
    }


    /*
    * Обработка коллизий между шарами происходит следующим образом:
    * В целях производительности всё игровое пространство было разделено на секции, 
    * шар может приинадлежать нескольким секциям. Проверка на наличие коллизий происходит только между шарами
    * в одной секции. Это позволило увеличить производительность за счёт того, что не надо сравнивать шары
    * находящиеся далеко друг от друга. Асимптотическая сложность такого решения: O( (n^2)/k )
    */

    // В целях увеличения производительности можно написать свой класс ThreadPool
    // в котором каждый поток будет обрабатывать секции, за которые он ответственен
    void proceedBalls() {
        for (auto& sector : sectors) {
            for (size_t i = 0; i < sector.belonging_balls.size(); ++i) {
                for (size_t j = i + 1; j < sector.belonging_balls.size(); ++j) {
                    ballBallCollision(*sector.belonging_balls[i], *sector.belonging_balls[j]);
                }
            }
        }

        for (auto& ball : balls) {
            for (auto& wall : walls) {
                ballWallCollision(ball, wall);
            }
        }
        calculateNewSectorsBelongings();
    }

    void moveBalls(float deltaTime) {
        for (auto& ball : balls) {
            ball.move(deltaTime);
        }
    }

    sf::Vector2f collidingBallVelocityVector(Ball& lhs, Ball& rhs) {
        float impact = (2 * rhs.weight()) / (rhs.weight() + lhs.weight());

        sf::Vector2f pd = lhs.p - rhs.p;
        sf::Vector2f vd = lhs.speed * lhs.dir - rhs.speed * rhs.dir;

        return lhs.speed * lhs.dir - (impact * (Math::dot(vd, pd) / Math::dot(pd, pd))) * pd;
    }

    bool isBallsCollide(Ball& lhs, Ball& rhs) {
        float distance = std::pow(lhs.p.x - rhs.p.x, 2) + std::pow(lhs.p.y - rhs.p.y, 2);
        return distance <= std::pow(lhs.r + rhs.r, 2);
    }

    void ballsCollisionResponse(Ball& lhs, Ball& rhs) {

        // Освобождаем застрявшие шары
        auto between_centers = rhs.p - lhs.p;
        float distance_between_centers = std::sqrt(std::pow(lhs.p.x - rhs.p.x, 2) + std::pow(lhs.p.y - rhs.p.y, 2));
        float distance_between_edges = lhs.r + rhs.r - distance_between_centers;
        between_centers /= std::sqrt(Math::dot(between_centers, between_centers));
        rhs.p += between_centers * distance_between_edges / 2.0f;
        lhs.p -= between_centers * distance_between_edges / 2.0f;

        // Рассчитываем новое направление после столкновения
        auto lvel = collidingBallVelocityVector(lhs, rhs);
        auto lspeed = std::sqrt(Math::dot(lvel, lvel));
        lvel /= lspeed;

        auto rvel = collidingBallVelocityVector(rhs, lhs);
        auto rspeed = std::sqrt(Math::dot(rvel, rvel));
        rvel /= rspeed;

        lhs.dir = lvel;
        lhs.speed = lspeed;

        rhs.dir = rvel;
        rhs.speed = rspeed;
    }

    void ballBallCollision(Ball& lhs, Ball& rhs) {
        if (isBallsCollide(lhs, rhs)) {
            ballsCollisionResponse(lhs, rhs);

            lhs.fillColor = drawable_colors[rand() % drawable_colors.size()];
            rhs.fillColor = drawable_colors[rand() % drawable_colors.size()];
        }
    }
    
    
    void ballWallCollision(Ball& ball, const Wall& wall) {
        auto cp = Math::closest_point(wall.begin, wall.end, ball.p);
        
        float distance = std::sqrt(std::pow(ball.p.x - cp.x, 2) + std::pow(ball.p.y - cp.y, 2));
        

        if (distance <= ball.r) {

            // Освобождаем шар, застрявший в стене
            auto ball_to_cp = cp - ball.p;
            ball_to_cp = Math::normalize(ball_to_cp);
            ball_to_cp *= ball.r - distance;

            float projection = Math::dot(ball_to_cp, ball.dir) / std::sqrt(Math::dot(ball.dir, ball.dir));
            ball.p -= 1.01f * projection * ball.dir;
            

            // Рассчитываем новое направление движения
            auto n = Math::normalize(Math::normal_vector(wall.begin, wall.end));
            sf::Vector2f newdir = ball.dir - 2 * Math::dot(ball.dir, n) * n;
            ball.dir = Math::normalize(newdir);
        }
    }

};