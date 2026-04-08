#include <SDL3/SDL.h>
#include <SDL3/SDL_timer.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <entt/entt.hpp>
#include "player.hpp"
#include "physics.hpp"
#include "camera.hpp"

[[nodiscard]] static inline constexpr float approach(const float from, const float to, const float amount) noexcept {
    const float delta{ to - from };
    const float sign{ static_cast<float>((delta > 0.0f) - (delta < 0.0f)) };
    const float change{ std::clamp(amount * sign, -std::abs(delta), std::abs(delta)) };
    return from + change;
}

namespace clayborne {
    player init_player(entt::registry &registry) {
        player player{};

        player.entity = registry.create();
        registry.emplace<position>(player.entity, 0.0f, 0.0f);
        registry.emplace<velocity>(player.entity, 0.0f, 0.0f);
        registry.emplace<collider>(player.entity, 8.0f, 11.0f);
        registry.emplace<renderer>(player.entity, nullptr, SDL_FRect{}, SDL_FRect{ .x = 0.0f, .y = 0.0f, .w = 8.0f, .h = 11.0f });

        // TODO: remove
        auto ground{ registry.create() };
        registry.emplace<position>(ground, 0.0f, 160.0f);
        registry.emplace<collider>(ground, 320.0f, 20.0f);
        registry.emplace<renderer>(ground, nullptr, SDL_FRect{}, SDL_FRect{ .x = 0.0f, .y = 0.0f, .w = 320.0f, .h = 20.0f });

        return player;
    }

    void deinit_player(player &player, entt::registry &registry) {
        registry.destroy(player.entity);
    }

    void update_player(player &player, entt::registry &registry, const input::manager &inputs, Uint64 dt_ns) {
        const float delta_time{ static_cast<float>(static_cast<double>(dt_ns) / SDL_NS_PER_SECOND) };

        for (auto event : inputs.get_events()) {
            // use event information???
            (void)event;
        }

        auto &velocity{ registry.get<clayborne::velocity>(player.entity) };
        auto &position{ registry.get<clayborne::position>(player.entity) };
        auto &collider{ registry.get<clayborne::collider>(player.entity) };

        // Check if grounded
        player.is_grounded = false;
        if (velocity.y >= 0) {
            auto below{ position };
            below.y += 1.0f;

            auto view{ registry.view<const clayborne::position, const clayborne::collider>() };
            for (auto [e, p, c] : view.each()) {
                if (player.entity == e) {
                    continue;
                }
                
                if (clayborne::overlap(below, collider, p, c)) {
                    player.is_grounded = true;
                    break;
                }
            }
        }

        // No idea
        // if (player.is_grounded) {
        //     highestAirY = Y;
        // }
        // else {
        //     highestAirY = Math.Min(Y, highestAirY);
        // }

        // Update jump grace timer
        if (player.is_grounded) {
            player.jump_grace_timer = player::jump_grace_time;
        }
        else if (player.jump_grace_timer > 0.0f) {
            player.jump_grace_timer -= delta_time;
        }

        if (player.jump_boost_timer > 0) {
            player.jump_boost_timer -= delta_time;
        }
                    
        // Update facing
        if (velocity.x > 0.0f) {
            player.facing = player::facing::right;
        }
        else if (velocity.x < 0.0f) {
            player.facing = player::facing::left;
        }

        // if (player.wall_speed_retention_timer > 0) {
        //     if (velocity.x * player.wall_speed_retention < 0.0f) {
        //         player.wall_speed_retention_timer = 0;
        //     }
        //     else if (!CollideCheck<Solid>(Position + Vector2.UnitX * Math.Sign(wallSpeedRetained))) {
        //         velocity.x = player.wall_speed_retention;
        //         player.wall_speed_retention_timer = 0;
        //     }
        //     else {
        //         player.wall_speed_retention_timer -= dt_ns;
        //     }
        // }

        // ------------------- //
        // Horizontal Movement //
        // ------------------- //

        const float move_input{ static_cast<float>(player.right - player.left) };
        const float velocity_sign{ static_cast<float>((velocity.x > 0.0f) - (velocity.x < 0.0f)) };
        const float multiplier{ player.is_grounded ? 1.0f : player.air_multiplier };

        if (std::abs(velocity.x) > player::run_speed && velocity_sign == move_input) {
            velocity.x = approach(velocity.x, move_input * player::run_speed, player::run_deceleration * multiplier * delta_time);
        }
        else {
            velocity.x = approach(velocity.x, move_input * player::run_speed, player::run_acceleration * multiplier * delta_time);
        }

        // ----------------- //
        // Vertical Movement //
        // ----------------- //

        // The apex of the jump has lower gravity if holding the jump button
        if (!player.is_grounded) {
            bool is_half_gravity_applicable{ std::abs(velocity.y) < player::half_gravity_threshold };
            float mult = (is_half_gravity_applicable && player.jump_pressed) ? 0.5f : 1.0f;
            velocity.y = approach(velocity.y, player::fall_speed, player::gravity * mult * delta_time);
        }

        // Holding the jump button from the start gives higher jumps
        if (player.jump_boost_timer > 0.0f) {
            if (player.jump_pressed) {
                velocity.y = std::min(velocity.y, player.jump_boost_speed);
            }
            else {
                player.jump_boost_timer = 0.0f;
            }
        }

        // Jumping is allowed some time after beginning to fall
        // TODO: add jump buffer
        if (player.jump_just_pressed && player.jump_grace_timer > 0.0f) {
            // TODO: consume jump buffer here!
            player.jump_grace_timer = 0;
            player.jump_boost_timer = player::jump_boost_time;

            velocity.x += player::jump_horizontal_boost * move_input;
            velocity.y = player::jump_speed;
            // jump pad boost goes here
            player.jump_boost_speed = velocity.y;
        }

        // ------------------------ //
        // Temporary Input Handling //
        // ------------------------ //
        player.jump_just_pressed = false;
        player.head_just_pressed = false;
        // ------------------------ //
    }

    // void on_collide_horizontal(player &player, void *collision_data) {
    //     (void)player;
    //     (void)collision_data;
    //     if (player.wall_speed_retention_timer <= 0) {
    //         player.wall_speed_retention = velocity.x;
    //         player.wall_speed_retention_timer = player::wall_speed_retention_time;
    //     }
    // }
}