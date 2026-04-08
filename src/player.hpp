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
        bool jump_just_pressed{ false }; // Celeste clears its jump buffer when releasing the button
        bool jump_pressed{ false };
        bool head_just_pressed{ false };
        bool left{ false };
        bool right{ false };
        bool up{ false };
        bool down{ false };
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
        static constexpr float jump_grace_time{ 0.1f };
        static constexpr float jump_boost_time{ 0.2f };

        static constexpr float fall_speed{ 160.0f };
        static constexpr float gravity{ 900.0f };
        static constexpr float half_gravity_threshold{ 40.0f };

        static constexpr float wall_speed_retention_time{ 0.6f };

        bool is_grounded{ true };
        facing facing{ facing::right };

        // Allows jumping a few moments after beginning to fall.
        float jump_grace_timer{ 0.0f };

        float jump_boost_timer{ 0.0f };
        float jump_boost_speed{ 0.0f };

        float wall_speed_retention_timer{ 0.0f };
        float wall_speed_retention{ 0.0f };

        entt::entity entity{ entt::null };
    };

    player init_player(entt::registry &registry);
    void deinit_player(player &player, entt::registry &registry);
    void update_player(player &player, entt::registry &registry, const input::manager &inputs, const Uint64 dt_ns);
}

#endif // CLAYBORNE_PLAYER_HPP