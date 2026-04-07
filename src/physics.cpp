#include <SDL3/SDL.h>
#include <SDL3/SDL_timer.h>
#include <entt/entt.hpp>
#include "physics.hpp"

namespace clayborne {
    void update_physics(entt::registry &registry, Uint64 dt_ns) {
        float delta_time{ static_cast<float>(static_cast<double>(dt_ns) / SDL_NS_PER_SECOND )};

        auto view = registry.view<clayborne::position, const clayborne::velocity>();

        for (auto [entity, pos, vel]: view.each()) {
            pos.x += vel.x * delta_time;
            pos.y += vel.y * delta_time;
        }
    }
}