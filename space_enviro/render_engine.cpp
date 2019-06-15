//
// Created by overlord on 25/01/19.
//

#include "render_engine.h"

namespace rendering{
//
//    void RenderEngine::RenderAsteroids(entities::EntityManager a) {
//        for (int i = 0; i < a.n_asteroids; ++i) {
//            asteroid_vertices_[i*3+0].position =sf::Vector2f(a.asteroids[i].x+a.asteroids[i].size/2,a.asteroids[i].y-a.asteroids[i].size/3);
//            asteroid_vertices_[i*3+1].position =sf::Vector2f(a.asteroids[i].x-a.asteroids[i].size/2,a.asteroids[i].y-a.asteroids[i].size/3);
//            asteroid_vertices_[i*3+2].position =sf::Vector2f(a.asteroids[i].x,a.asteroids[i].y+2*a.asteroids[i].size/3);
//        }
//
//    }
//
//    void RenderEngine::RenderRays(entities::EntityManager a){
//
//        for (int i = 0; i < a.n_rays;i++) {
//            if(reinterpret_cast<float*>(a.ships_wiev.get_data())[i]>0){
//                ray_vertices_[i*2+1].position = sf::Vector2f(
//                        a.wiev_range*(1-reinterpret_cast<float*>(a.ships_wiev.get_data())[i])*sinf(-a.begin+i*a.step+a.angle),
//                        a.wiev_range*(1-reinterpret_cast<float*>(a.ships_wiev.get_data())[i])*cosf(-a.begin+i*a.step+a.angle));
//            }
//            else{
//                ray_vertices_[i*2+1].position = sf::Vector2f(0,0);
//            }
//        }
//        ray_vertices_[1].position = sf::Vector2f(
//                a.wiev_range*sinf(-a.begin+a.angle),
//                a.wiev_range*cosf(-a.begin+a.angle));
//        ray_vertices_[1].color = sf::Color::Red;
//        ray_vertices_[a.n_rays-1].position = sf::Vector2f(
//                a.wiev_range*sinf(-a.begin+(a.n_rays-1)*a.step+a.angle),
//                a.wiev_range*cosf(-a.begin+(a.n_rays-1)*a.step+a.angle));
//        ray_vertices_[a.n_rays-1].color = sf::Color::Green;
//
//    }
//
//    void RenderEngine::RenderInfo(entities::EntityManager a) {
//
//        info_text_.setString(
//        +"Epoch counter "+std::to_string(a.epoch_counter_)
//        +"\nBest reward "+std::to_string(a.max_reward)
//        +"\nCurrent reward "+std::to_string(a.total_reward)
//        +"\n\nTO RENDER ENVIRONMENT"
//        +"\nHOLD SPACEBAR "
//
//        );
//
//    }
}