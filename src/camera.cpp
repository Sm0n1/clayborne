#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <entt/entt.hpp>
#include "camera.hpp"
#include "physics.hpp"
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <cstdio>
#include <entt/entt.hpp>
#include <vector>
#include "camera.hpp"
#include "physics.hpp"
#include "interactables.hpp"
#include "utils.hpp"

namespace clayborne {
    struct sdl_circle {
        double radius{ 1.0 };
        std::vector<SDL_Vertex> vertices{};
        std::vector<int> indices{};
    };

    static void sdl_circle_set_position(
        sdl_circle &circle,
        const double center_x,
        const double center_y
    ) {
        const std::size_t vertex_count{ circle.vertices.size() - 1 };
        const double fan_rotation_angle{ (2.0 * pi) / static_cast<double>(vertex_count) };

        circle.vertices[0].position.x = static_cast<float>(center_x);
        circle.vertices[0].position.y = static_cast<float>(center_y);

        for (std::size_t i{ 1 }; i < vertex_count + 1; i += 1) {
            circle.vertices[i].position.x = static_cast<float>(center_x + circle.radius * std::cos(fan_rotation_angle * static_cast<double>(i)));
            circle.vertices[i].position.y = static_cast<float>(center_y + circle.radius * std::sin(fan_rotation_angle * static_cast<double>(i)));
        }
    }

    static sdl_circle sdl_circle_init(
        const std::size_t vertex_count,
        const double radius,
        const double center_x,
        const double center_y
    ) {
        assert(vertex_count >= 3);
        assert(radius > 0.0);

        sdl_circle result{};

        result.radius = radius;
        result.vertices.resize(vertex_count + 1);
        result.indices.resize(vertex_count * 3);

        const double fan_rotation_angle{ (2.0 * pi) / static_cast<double>(vertex_count) };

        auto &center_vertex{ result.vertices[0] };

        // Set center vertex position
        center_vertex.position.x = static_cast<float>(center_x);
        center_vertex.position.y = static_cast<float>(center_y);

        // Set center vertex color
        center_vertex.color.r = 1.0f;
        center_vertex.color.g = 1.0f;
        center_vertex.color.b = 1.0f;
        center_vertex.color.a = 1.0f;

        // Set center vertex texture coordinates
        center_vertex.tex_coord.x = 0.0f;
        center_vertex.tex_coord.y = 0.0f;

        for (std::size_t i{ 0 }; i < vertex_count; i += 1) {
            auto &vertex{ result.vertices[i + 1] };
            
            // Set vertex position
            vertex.position.x = static_cast<float>(center_x + radius * std::cos(fan_rotation_angle * static_cast<double>(i)));
            vertex.position.y = static_cast<float>(center_y + radius * std::sin(fan_rotation_angle * static_cast<double>(i)));
            
            // Set vertex color
            vertex.color.r = 1.0f;
            vertex.color.g = 1.0f;
            vertex.color.b = 1.0f;
            vertex.color.a = 0.0f;

            // Set center vertex texture coordinates
            vertex.tex_coord.x = 0.0f;
            vertex.tex_coord.y = 0.0f;

            // Set triangle indices
            result.indices[i * 3 + 0] = 0;
            result.indices[i * 3 + 1] = static_cast<int>(i + 1);
            result.indices[i * 3 + 2] = static_cast<int>(i + 2);
        }

        // Wrap around the last index
        result.indices[vertex_count * 3 - 1] = 1;

        return result;
    }

    // TODO: match destination rectangle with window
    entt::entity init_camera(entt::registry &registry) {
        auto camera{ registry.create() };

        registry.emplace<clayborne::position>(camera, 0.0f, 0.0f);

        return camera;
    }

    void deinit_camera(const entt::entity camera, entt::registry &registry) {
        registry.destroy(camera);
    }

    void render(const entt::entity camera, const entt::registry &registry, SDL_Renderer *renderer, SDL_Texture *canvas) {
        // Clear last frame
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, canvas);
        SDL_RenderClear(renderer);

        // ---------------- //
        // Draw Camera View //
        // ---------------- //

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        auto &camera_position{ registry.get<const clayborne::position>(camera) };

        // Render drawable objects
        auto view = registry.view<const clayborne::position, const clayborne::renderer>();
        for (auto [entity, position, renderable]: view.each()) {
            const SDL_FRect dstrect{
                .x = renderable.dstrect.x + position.x - camera_position.x,
                .y = renderable.dstrect.y + position.y - camera_position.y,
                .w = renderable.dstrect.w,
                .h = renderable.dstrect.h,
            };
            if (renderable.texture) {
                // Debug stuff for showing drawbox
                //SDL_SetRenderDrawColor(renderer, 255, 155, 255, 255);
                //SDL_RenderRect(renderer, &dstrect);
                //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

                // Render the sprite itself
                SDL_RenderTexture(renderer, renderable.texture, &renderable.srcrect, &dstrect);
            }
            else {
                auto d = registry.try_get<door>(entity);
                if (d && d->is_open) {
                    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
                    SDL_RenderFillRect(renderer, &dstrect);
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    continue;
                }
                SDL_RenderFillRect(renderer, &dstrect);
            }
        }

        sdl_circle circle{ sdl_circle_init(7, 5.0, 0.0, 0.0) };

        // Draw light sources
        auto light_sources{ registry.view<const clayborne::position, const light_source>() };
        for (auto [e, p] : light_sources.each()) {
            sdl_circle_set_position(circle, static_cast<double>(p.x), static_cast<double>(p.y));
            SDL_RenderGeometry(
                renderer,
                nullptr,
                circle.vertices.data(),
                static_cast<int>(circle.vertices.size()),
                circle.indices.data(),
                static_cast<int>(circle.indices.size())
            );
        }

        // Render camera view
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderTexture(renderer, canvas, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    void camera_player_follow(const entt::entity camera, const entt::entity player, entt::registry &registry) {
        auto &camera_position{ registry.get<position>(camera) };
        auto &player_position{ registry.get<const position>(player) };
        auto &player_collider{ registry.get<const collider>(player) };

        // If player's collider is fully outside the camera,
        // then set the camera position to the player's current room.

        collider temporary_camera_collider{ .w = 320.0f, .h = 184.0f, .collide = nullptr };

        if (!overlap(camera_position, temporary_camera_collider, player_position, player_collider)) {
            camera_position.x = std::floor(player_position.x / 320.0f) * 320.0f;
            camera_position.y = std::floor(player_position.y / 184.0f) * 184.0f;
        }
    }
}