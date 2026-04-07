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
        registry.emplace<collider>(player.entity, 16.0f, 16.0f);
        registry.emplace<renderer>(player.entity, nullptr, SDL_FRect{}, SDL_FRect{ .x = 0.0f, .y = 0.0f, .w = 16.0f, .h = 16.0f });

        return player;
    }

    void deinit_player(player &player, entt::registry &registry) {
        registry.destroy(player.entity);
    }

    void update_player(player &player, entt::registry &registry, const input::manager &inputs, Uint64 dt_ns) {
        for (auto event : inputs.get_events()) {
            // use event information???
            (void)event;
        }
        (void)player;
        (void)registry;

        if (player.is_grounded) {
            player.jump_grace_timer_ns = player::jump_grace_time_ns;
        }
        else if (player.jump_grace_timer_ns > 0) {
            player.jump_grace_timer_ns -= dt_ns;
        }

        auto &velocity = registry.get<clayborne::velocity>(player.entity);
        auto &position = registry.get<clayborne::velocity>(player.entity);
        (void)position;

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

        const float move_x{ static_cast<float>(player.right - player.left) };
        const float sign_x{ static_cast<float>((velocity.x > 0.0f) - (velocity.x < 0.0f)) };
        const float mult{ player.is_grounded ? 1.0f : player.air_multiplier };
        const float delta_time{ static_cast<float>(static_cast<double>(dt_ns) / SDL_NS_PER_SECOND) };

        if (std::abs(velocity.x) > player::run_speed && sign_x == move_x) {
            printf("DEC: dir = %f, sign = %f, from = %f, to = %f, amount = %f\n", 
                static_cast<double>(move_x),
                static_cast<double>(sign_x),
                static_cast<double>(velocity.x),
                static_cast<double>(move_x * player::run_speed),
                static_cast<double>(player::run_deceleration * mult * delta_time)
            );
            velocity.x = approach(velocity.x, move_x * player::run_speed, player::run_deceleration * mult * delta_time);
        }
        else {
            printf("ACC: dir = %f, sign = %f, from = %f, to = %f, amount = %f\n", 
                static_cast<double>(move_x),
                static_cast<double>(sign_x),
                static_cast<double>(velocity.x),
                static_cast<double>(move_x * player::run_speed),
                static_cast<double>(player::run_deceleration * mult * delta_time)
            );
            velocity.x = approach(velocity.x, move_x * player::run_speed, player::run_acceleration * mult * delta_time);
        }

        // ------------------------ //
        // Temporary Input Handling //
        // ------------------------ //
        if (player.jump == player::button::just_pressed) {
            player.jump = player::button::pressed;
        }
        else if (player.jump == player::button::just_released) {
            player.jump = player::button::released;
        }
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