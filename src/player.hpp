#ifndef CLAYBORNE_PLAYER_HPP
#define CLAYBORNE_PLAYER_HPP

#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <entt/entt.hpp>
#include "engine/input/action.hpp"
#include "engine/input/manager.hpp"

namespace clayborne {
    struct player {
        // ------------------------ //
        // Temporary Input Handling //
        // ------------------------ //
        enum class button { pressed, just_pressed, released, just_released };
        button jump;
        button head;
        bool left;
        bool right;
        bool up;
        bool down;
        // ------------------------ //

        enum class facing {
            left,
            right,
        };

        static constexpr float run_speed{ 90.0f };
        static constexpr float run_acceleration{ 1000.0f };
        static constexpr float run_deceleration{ 400.0f };
        static constexpr float air_multiplier{ 0.65f };

        static constexpr float jump_speed{ -150.0f };
        static constexpr float jump_horizontal_boost{ 40.0f };
        static constexpr Uint64 jump_grace_time_ns{ SDL_NS_PER_SECOND / 10 };

        static constexpr Uint64 wall_speed_retention_time{ 6 * (SDL_NS_PER_SECOND / 10) };

        bool is_jumping{ false };
        bool is_grounded{ true };
        facing facing{ facing::right };
        Uint64 jump_grace_timer_ns{ jump_grace_time_ns };

        Uint64 wall_speed_retention_timer{ 0 };
        float wall_speed_retention{ 0.0f };

        input::action::descriptor move_action;
        input::action::descriptor jump_action;
        input::action::descriptor use_head_action;

        entt::entity entity{ entt::null };
    };

    player init_player(entt::registry &registry);
    void deinit_player(player &player, entt::registry &registry);
    void update_player(player &player, entt::registry &registry, const input::manager &inputs, const Uint64 dt_ns);
}

#endif // CLAYBORNE_PLAYER_HPP