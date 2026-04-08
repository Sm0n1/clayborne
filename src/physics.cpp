#include <SDL3/SDL.h>
#include <SDL3/SDL_timer.h>
#include <cmath>
#include <cstdio>
#include <entt/entt.hpp>
#include "physics.hpp"

namespace clayborne {
    void update_physics(entt::registry &registry, Uint64 dt_ns) {
        float delta_time{ static_cast<float>(static_cast<double>(dt_ns) / SDL_NS_PER_SECOND )};

        // Move entities that cannot collide
        auto noncollidable_view{ registry.view<position, const velocity>(entt::exclude<collider>) };
        for (auto [entity, pos, vel]: noncollidable_view.each()) {
            pos.x += vel.x * delta_time;
            pos.y += vel.y * delta_time;
        }
        
        // Move entities that may collide
        auto collidable_view{ registry.view<position, velocity, const collider>() };
        for (auto [entity, pos, vel, col]: collidable_view.each()) {
            vel.x_subpos += vel.x * delta_time;
            vel.y_subpos += vel.y * delta_time;

            auto x_move{ std::round(vel.x_subpos) };
            auto y_move{ std::round(vel.y_subpos) };
            auto x_sgn{ static_cast<float>(x_move > 0.0f) - static_cast<float>(x_move < 0.0f) };
            auto y_sgn{ static_cast<float>(y_move > 0.0f) - static_cast<float>(y_move < 0.0f) };

            vel.x_subpos -= x_move;
            vel.y_subpos -= y_move;

            printf("%f\n", static_cast<double>(x_move));

            // Move one pixel at a time in the x axis
            while (x_move != 0) {
                x_move -= x_sgn;
                pos.x += x_sgn;

                bool is_collision = false;
                auto view{ registry.view<const position, const collider>() };
                for (auto [other, p2, c2]: view.each()) {
                    if (entity == other) {
                        continue;
                    }

                    if (overlap(pos, col, p2, c2)) {
                        is_collision = true;
                        if (col.handler) {
                            col.handler.value()(other, true, false);
                        }
                        if (c2.handler) {
                            c2.handler.value()(entity, true, false);
                        }
                    }
                }

                // Undo move if it caused a collision 
                if (is_collision) {
                    pos.x -= x_sgn; // undo move
                    break;
                }
            }

            // Move one pixel at a time in the x axis
            while (y_move != 0) {
                y_move -= y_sgn;
                pos.y += y_sgn;

                bool is_collision = false;
                auto view{ registry.view<const position, const collider>() };
                for (auto [other, p2, c2]: view.each()) {
                    if (entity == other) {
                        continue;
                    }
                    
                    if (overlap(pos, col, p2, c2)) {
                        is_collision = true;
                        if (col.handler) {
                            col.handler.value()(other, false, true);
                        }
                        if (c2.handler) {
                            c2.handler.value()(entity, false, true);
                        }
                    }
                }

                // Undo move if it caused a collision 
                if (is_collision) {
                    pos.y -= y_sgn; // undo move
                    break;
                }
            }
        }
    }
}