#ifndef CLAYBORNE_WORLD_HPP
#define CLAYBORNE_WORLD_HPP

#include <SDL3/SDL_main.h>
#include <entt/entt.hpp>

namespace clayborne {
    void init_room(entt::registry &registry, SDL_Renderer *renderer);
}

#endif // CLAYBORNE_WORLD_HPP