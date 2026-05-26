#ifndef CLAYBORNE_MENU_HPP
#define CLAYBORNE_MENU_HPP

// This file is very poorly written.

#include <entt/entt.hpp>
#include "physics.hpp"
#include "sprite.hpp"

using entt::literals::operator""_hs;

namespace clayborne {
    struct menu{
        float y{ 0 };
    };

    inline bool init_menu(entt::registry &registry, texture_cache &textures, SDL_Renderer *renderer) {
        if (!textures.load("menu_background"_hs, "data/textures/menu/background.png", renderer).first->second) {
            return false;
        }

        if (!textures.load("menu_foreground"_hs, "data/textures/menu/foreground.png", renderer).first->second) {
            return false;
        }

        if (!textures.load("menu_title"_hs, "data/textures/menu/title.png", renderer).first->second) {
            return false;
        }

        {
            auto background{ registry.create() };

            registry.emplace<struct menu>(background);
            registry.emplace<struct position>(background);

            auto &sprite_renderer{ registry.emplace<struct sprite_renderer>(background) };
            sprite_renderer.texture = "menu_background"_hs;
            sprite_renderer.z = 4;
        }

        {
            auto foreground{ registry.create() };

            registry.emplace<struct menu>(foreground, 180.0f * 0.65f);
            registry.emplace<struct velocity>(foreground);

            auto &position{ registry.emplace<struct position>(foreground) };
            position.x = 320.0f * 0.5f;

            auto &sprite_renderer{ registry.emplace<struct sprite_renderer>(foreground) };
            sprite_renderer.texture = "menu_foreground"_hs;
            sprite_renderer.z = 5;
            
            float w;
            float h;

            SDL_GetTextureSize(textures[sprite_renderer.texture].handle().get(), &w, &h);
            
            sprite_renderer.x_offset = -0.5f * w;
            sprite_renderer.y_offset = -0.5f * h;
        }

        {
            auto title{ registry.create() };

            registry.emplace<struct menu>(title);

            auto &position{ registry.emplace<struct position>(title) };
            position.x = 320.0f * 0.5f;
            position.y = 180.0f * 0.2f;

            auto &sprite_renderer{ registry.emplace<struct sprite_renderer>(title) };
            sprite_renderer.texture = "menu_title"_hs;
            sprite_renderer.z = 5;
            
            float w;
            float h;

            SDL_GetTextureSize(textures[sprite_renderer.texture].handle().get(), &w, &h);
            
            sprite_renderer.x_offset = -0.5f * w;
            sprite_renderer.y_offset = -0.5f * h;
        }

        return true;
    }

    inline void update_menu(entt::registry &registry, const Uint64 dt_ns, bool is_started, Uint64 start_timer) {
        if (is_started && start_timer >= 2 * SDL_NS_PER_SECOND) {
            return;
        }

        {
            static float time{ 0.0f };
            const float delta_time{ static_cast<float>(static_cast<double>(dt_ns) / SDL_NS_PER_SECOND) };

            auto view{ registry.view<struct menu, struct position, struct velocity>() };
            for (auto entity : view) {
                auto &menu{ view.get<struct menu>(entity) };
                auto &position{ view.get<struct position>(entity) };

                position.y = menu.y + SDL_sinf(time) * 7.5f;
            }

            time += delta_time;
        }
        
        if (is_started && start_timer >= SDL_NS_PER_SECOND) {
            auto view{ registry.view<struct menu, struct position>() };
            for (auto entity : view) {
                registry.destroy(entity);
            }
        }
    }
}

#endif // CLAYBORNE_MENU_HPP