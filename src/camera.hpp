#ifndef CLAYBORNE_CAMERA_HPP
#define CLAYBORNE_CAMERA_HPP

#include <SDL3/SDL.h>
#include <entt/entt.hpp>

namespace clayborne {
    struct camera {
        entt::entity entity;
    };

    clayborne::camera init_camera(entt::registry &registry);
    void deinit_camera(clayborne::camera &camera, entt::registry &registry);
    void render(const clayborne::camera &camera, const entt::registry &registry, SDL_Renderer *renderer, SDL_Texture *canvas);
}

#endif // CLAYBORNE_CAMERA_HPP