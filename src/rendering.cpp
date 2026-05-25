#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "interactables.hpp"
#include "physics.hpp"
#include "sprite.hpp"
#include "rendering.hpp"

namespace clayborne {
    void sdl_circle_set_position(
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

    constexpr sdl_circle sdl_circle_init(
        const std::size_t vertex_count,
        const double radius,
        const double center_x,
        const double center_y,
        const SDL_FColor center_color,
        const SDL_FColor perimiter_color
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
        center_vertex.color = center_color;

        // Set center vertex texture coordinates
        center_vertex.tex_coord.x = 0.0f;
        center_vertex.tex_coord.y = 0.0f;

        for (std::size_t i{ 0 }; i < vertex_count; i += 1) {
            auto &vertex{ result.vertices[i + 1] };
            
            // Set vertex position
            vertex.position.x = static_cast<float>(center_x + radius * std::cos(fan_rotation_angle * static_cast<double>(i)));
            vertex.position.y = static_cast<float>(center_y + radius * std::sin(fan_rotation_angle * static_cast<double>(i)));
            
            // Set vertex color
            vertex.color = perimiter_color;

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

    SDL_Texture *init_vignette(
        SDL_Renderer *renderer
    ) {
        SDL_Texture *texture{ SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            canvas_width,
            canvas_height
        ) };

        if (!texture) {
            SDL_Log("SDL create texture failed: %s", SDL_GetError());
            return nullptr;
        }

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        Uint32 *pixels;
        int pitch;

        SDL_LockTexture(texture, nullptr,  static_cast<void **>(static_cast<void *>(&pixels)), &pitch);

        constexpr float center_x{ 0.5f };
        constexpr float center_y{ 0.5f };

        constexpr float smoothness{ 0.8f };
        constexpr float intensity{ 0.6f };

        // Inverted for cache locality
        for (int y{ 0 }; y < canvas_height; y += 1) {
            for (int x{ 0 }; x < canvas_width; x += 1) {
                const float u{ static_cast<float>(x) / static_cast<float>(canvas_width) };
                const float v{ static_cast<float>(y) / static_cast<float>(canvas_height) };

                const float delta_x{ u - center_x };
                const float delta_y{ v - center_y };

                const float delta{ SDL_sqrtf(delta_x * delta_x + delta_y * delta_y) };

                constexpr auto smoothstep{
                    [](
                        const float e1,
                        const float e2,
                        float t
                    ) {
                        t = SDL_clamp((t - e1) / (e2 - e1), 0.0f, 1.0f);
                        return t * t * (3.0f - 2.0f * t);
                    }
                };

                const float vignette{ smoothstep(0.2f, smoothness, delta * intensity) };

                const Uint8 alpha{ static_cast<Uint8>(vignette * 255.0f) };

                pixels[y * (pitch / 4) + x] = static_cast<Uint32>(alpha);
                // auto pixel = pixels[y * (pitch / 4) + x];
                // SDL_Log("x = %d, y = %d, pixel = %3d|%3d|%3d|%3d", x, y, pixel >> 24, (pixel << 8) >> 24, (pixel << 16) >> 24, (pixel << 24) >> 24);
            }
        }

        SDL_UnlockTexture(texture);

        return texture;
    }

    // This function takes a texture, and draws it in the same rectangle as the scaled canvas texture.
    // The position parameters are in the unscaled canvas coordinate space.
    // Note, this may render outside the scaled canvas rectangle.
    static void render_texture(
        SDL_Renderer *renderer,
        const struct position &position,
        const struct sprite_renderer &sprite_renderer,
        texture_cache &textures
    ) {
        auto texture{
            textures[sprite_renderer.texture]
        };

        if (!texture) {
            return;
        }

        SDL_FRect rect;

        SDL_GetRenderLogicalPresentationRect(renderer, &rect);
        SDL_SetRenderLogicalPresentation(renderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);

        const float canvas_w_scale{ rect.w / static_cast<float>(canvas_width) };
        const float canvas_h_scale{ rect.h / static_cast<float>(canvas_height) };

        float w;
        float h;

        SDL_GetTextureSize(texture.handle().get(), &w, &h);

        SDL_FRect dstrect{
            .x = rect.x + position.x * canvas_w_scale + sprite_renderer.x_offset * canvas_w_scale / 8.0f,
            .y = rect.y + position.y * canvas_h_scale + sprite_renderer.y_offset * canvas_w_scale / 8.0f,
            .w = w * canvas_w_scale / 8.0f,
            .h = h * canvas_h_scale / 8.0f,
        };

        SDL_RenderTexture(renderer, texture.handle().get(), nullptr, &dstrect);
        SDL_SetRenderLogicalPresentation(renderer, canvas_width, canvas_height, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    }

    void render(
        const entt::entity camera,
        const entt::registry &registry,
        texture_cache &textures,
        SDL_Renderer *renderer,
        SDL_Texture *canvas,
        SDL_Texture *vignette,
        bool is_started,
        Uint64 start_timer
    ) {
        // Clear last frame
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, canvas);
        SDL_RenderClear(renderer);

        // Draw camera view
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        auto &camera_position{ registry.get<struct position>(camera) };
        auto view{ registry.view<struct position, struct sprite_renderer>(entt::exclude<struct menu>) };
        view.use<struct sprite_renderer>();
        for (auto entity : view) {
            const auto &sprite_renderer{
                view.get<const struct sprite_renderer>(entity)
            };

            auto texture{ textures[sprite_renderer.texture] };
            if (!texture) {
                continue;
            }

            SDL_SetTextureAlphaMod(texture.handle().get(), sprite_renderer.alpha);

            const auto &position{
                view.get<const struct position>(entity)
            };

            if (sprite_renderer.is_tiled) {
                const SDL_FRect dstrect{
                    .x = sprite_renderer.x_offset + position.x - camera_position.x,
                    .y = sprite_renderer.y_offset + position.y - camera_position.y,
                    .w = sprite_renderer.w_tiled,
                    .h = sprite_renderer.h_tiled,
                };

                SDL_RenderTextureTiled(
                    renderer,
                    texture.handle().get(),
                    &sprite_renderer.srcrect,
                    1.0f,
                    &dstrect
                );
                continue;
            }

            const SDL_FRect dstrect{
                .x = sprite_renderer.x_offset + position.x - camera_position.x,
                .y = sprite_renderer.y_offset + position.y - camera_position.y,
                .w = sprite_renderer.srcrect.w,
                .h = sprite_renderer.srcrect.h,
            };

            SDL_RenderTextureRotated(
                renderer,
                texture.handle().get(),
                &sprite_renderer.srcrect,
                &dstrect,
                0.0,
                sprite_renderer.center ? &*sprite_renderer.center : nullptr,
                sprite_renderer.flip
            );
        }

        // Render camera view
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderTexture(renderer, canvas, nullptr, nullptr);
        SDL_RenderTexture(renderer, vignette, nullptr, nullptr);

        // Render menu items
        auto menu_items{ registry.view<struct menu, struct position, struct sprite_renderer>() };
        menu_items.use<struct sprite_renderer>();
        for (auto entity : menu_items) {
            const auto &sprite_renderer{
                menu_items.get<struct sprite_renderer>(entity)
            };

            const auto &position{
                view.get<const struct position>(entity)
            };

            render_texture(renderer, position, sprite_renderer, textures);
        }

        // Render transition
        if (is_started && start_timer < 2 * SDL_NS_PER_SECOND) {
            const float t{
                (static_cast<float>(start_timer) * SDL_PI_F / (2 * SDL_NS_PER_SECOND))
            };

            const Uint8 alpha{
                static_cast<Uint8>(SDL_fabsf(SDL_sinf(t)) * 255.0f)
            };
            
            SDL_Log("t = %f, alpha = %d", static_cast<double>(t), alpha);

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
            SDL_RenderFillRect(renderer, nullptr);
        }

        SDL_RenderPresent(renderer);
    }
}