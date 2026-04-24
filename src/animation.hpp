#ifndef CLAYBORNE_ANIMATION_HPP
#define CLAYBORNE_ANIMATION_HPP

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>
#include <vector>
#include <fstream>

// The animation 
// are implemented as per the following:
// https://github.com/skypjack/entt/wiki/Resource-management

namespace clayborne {
    // TODO add a texture resource type, with a cache, and include it in here.
    struct texture_resource {
        SDL_Texture* tex;
    };

    struct texture_loader {
        using result_type = std::shared_ptr<texture_resource>;

        result_type operator()(const std::filesystem::path& path, SDL_Renderer* r) {
            texture_resource resource{};
            // This is kind of ugly, feel free to clean up if you have some free time
            auto p = path;
            p.replace_extension(".png");
            auto tex = SDL_CreateTextureFromSurface(r, IMG_Load(p.string().c_str()));
            resource.tex = tex;
            return std::make_shared<texture_resource>(resource);
        }
    };

    using texture_cache = entt::resource_cache<texture_resource, texture_loader>;

    struct animation_resource {
        std::vector<SDL_FRect> frames;
        clayborne::texture_resource spritesheet;
    };

    struct animation_loader {
        texture_cache* textures;
        using result_type = std::shared_ptr<animation_resource>;

        result_type operator()(const std::filesystem::path& path, SDL_Renderer* r) {
            animation_resource resource{};

            auto p = path;
            p.replace_extension(".json");
            std::ifstream file{ p };
            if (!file) {
                return nullptr;
            }

            auto data = nlohmann::json::parse(file, nullptr, false);
            if (data.is_discarded()) {
                return nullptr;
            }

            auto frames = data["frames"];
            for (auto frame : frames) {
                resource.frames.push_back({
                    .x = frame["frame"]["x"],
                    .y = frame["frame"]["y"],
                    .w = frame["frame"]["w"],
                    .h = frame["frame"]["h"],
                });

                SDL_Log("%d, %d, %d, %d",
                static_cast<int>(frame["frame"]["x"]),
                static_cast<int>(frame["frame"]["y"]),
                static_cast<int>(frame["frame"]["w"]),
                static_cast<int>(frame["frame"]["h"])
                );
            }

            // the id being the path here is real ugly
            resource.spritesheet = textures->load(entt::hashed_string(path.string().c_str()), path, r).first->second;

            return std::make_shared<animation_resource>(resource);
        }
    };

    using animation_cache = entt::resource_cache<animation_resource, animation_loader>;

    struct animator {
        entt::resource<animation_resource> resource;
        std::size_t current_frame;
        bool is_looping;
    };

    struct animation_set {
        std::unordered_map<entt::id_type, entt::resource<animation_resource>> animators;
    };

    void animate(entt::registry &registry);
}

#endif // CLAYBORNE_ANIMATION_HPP