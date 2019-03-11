//
// Created by overlord on 25/01/19.
//

#include "render_engine.h"
#include "constants.h"

namespace rendering{

    //TODO On my opinion this is quite ugly way to render this kind of stuff.
    //Also its probably not cash optimal
    void RenderEngine::RenderShip(entities::EntityManager a){

        float arr[] = {a.x,a.y,a.angle,a.size ,a.sideinput,a.maininput};

        int i = 0;
        float angle = arr[2];
        float size = arr[3];
        float mainpower = arr[5];
        float sidepower = arr[4];
        sf::Vector2f ap = sf::Vector2f(
                (-1.f/3)*size*sinf(angle)+arr[0],
                (-1.f/3)*size*cosf(angle)+arr[1]);

        //Side Engine
        ship_vertices_[i+0].position=sf::Vector2f(
                (2.f/3)*size*sinf(angle)+arr[0],
                (2.f/3)*size*cosf(angle)+arr[1]);
        ship_vertices_[i+1].position=sf::Vector2f(
                (1.f/3)*size*sinf(angle)+arr[0] -
                ((1.f/2)*sidepower*size*sinf(angle + M_PI_2f32) + (1.f / 4)),
                (1.f/3)*size*cosf(angle)+arr[1] -
                ((1.f/2)*sidepower*size*cosf(angle + M_PI_2f32) + (1.f / 4)));
        ship_vertices_[i+2].position=sf::Vector2f(
                (1.f/6)*size*sinf(angle)+arr[0],
                (1.f/6)*size*cosf(angle)+arr[1]);

        //Main Engine
        ship_vertices_[i+3].position=ap - sf::Vector2f(
                (1.f/2)*size*sinf(angle)*mainpower,
                (1.f/2)*size*cosf(angle)*mainpower);
        ship_vertices_[i+4].position=ap + sf::Vector2f(
                (1.f/4)*size*sinf(angle+M_PI_2f32),
                (1.f/4)*size*cosf(angle+M_PI_2f32));
        ship_vertices_[i+5].position=ap + sf::Vector2f(
                (1.f/4)*size*sinf(angle-M_PI_2f32),
                (1.f/4)*size*cosf(angle-M_PI_2f32));

        //Ship Body
        ship_vertices_[i+6].position=sf::Vector2f(
                (2.f/3)*size*sinf(angle)+arr[0],
                (2.f/3)*size*cosf(angle)+arr[1]);
        ship_vertices_[i+7].position=ap + sf::Vector2f(
                (1.f/2)*size*sinf(angle+M_PI_2f32),
                (1.f/2)*size*cosf(angle+M_PI_2f32));
        ship_vertices_[i+8].position=ap + sf::Vector2f(
                (1.f/2)*size*sinf(angle-M_PI_2f32),
                (1.f/2)*size*cosf(angle-M_PI_2f32));
    }

    void RenderEngine::RenderAsteroids(entities::EntityManager a) {
        for (int i = 0; i < a.n_asteroids; ++i) {
            asteroid_vertices_[i*3+0].position =sf::Vector2f(a.asteroids[i].x+a.asteroids[i].size/2,a.asteroids[i].y-a.asteroids[i].size/3);
            asteroid_vertices_[i*3+1].position =sf::Vector2f(a.asteroids[i].x-a.asteroids[i].size/2,a.asteroids[i].y-a.asteroids[i].size/3);
            asteroid_vertices_[i*3+2].position =sf::Vector2f(a.asteroids[i].x,a.asteroids[i].y+2*a.asteroids[i].size/3);
        }

    }

    void RenderEngine::RenderRays(entities::EntityManager a){

        for (int i = 0; i < a.n_rays;i++) {
            if(reinterpret_cast<float*>(a.ships_wiev.get_data())[i]>0){
                ray_vertices_[i*2+1].position = sf::Vector2f(
                        a.wiev_range*(1-reinterpret_cast<float*>(a.ships_wiev.get_data())[i])*sinf(-a.begin+i*a.step+a.angle),
                        a.wiev_range*(1-reinterpret_cast<float*>(a.ships_wiev.get_data())[i])*cosf(-a.begin+i*a.step+a.angle));
            }
            else{
                ray_vertices_[i*2+1].position = sf::Vector2f(0,0);
            }
        }
        ray_vertices_[1].position = sf::Vector2f(
                a.wiev_range*sinf(-a.begin+a.angle),
                a.wiev_range*cosf(-a.begin+a.angle));
        ray_vertices_[1].color = sf::Color::Red;
        ray_vertices_[a.n_rays-1].position = sf::Vector2f(
                a.wiev_range*sinf(-a.begin+(a.n_rays-1)*a.step+a.angle),
                a.wiev_range*cosf(-a.begin+(a.n_rays-1)*a.step+a.angle));
        ray_vertices_[a.n_rays-1].color = sf::Color::Green;

    }

    void RenderEngine::RenderInfo(entities::EntityManager a) {

        info_text_.setString(
        +"Epoch counter "+std::to_string(a.epoch_counter_)
        +"\nBest reward "+std::to_string(a.max_reward)
        +"\nCurrent reward "+std::to_string(a.total_reward)
        +"\n\nTO RENDER ENVIRONMENT"
        +"\nHOLD SPACEBAR "

        );

    }
}