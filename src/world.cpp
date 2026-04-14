#include <fstream>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <entt/entt.hpp>
#include "world.hpp"
#include "physics.hpp"
#include "camera.hpp"
#include "clay.hpp"

namespace clayborne {
    

    void init_room(entt::registry &registry, SDL_Renderer *renderer) {
        // Quick and dirty ldtk super simple level format reader
        SDL_Texture *room_sprite{ IMG_LoadTexture(renderer, "data/levels/sprite.png") };
        if (!room_sprite) {
            SDL_Log("IMG load texture failed: %s", SDL_GetError());
            //return SDL_APP_FAILURE;
        }

        auto room{ registry.create() };
        registry.emplace<clayborne::position>(room, 0.0f, 0.0f);
        registry.emplace<clayborne::renderer>(room, room_sprite, SDL_FRect{ .x = 0.0f, .y = 0.0f, .w = 320.0f, .h = 180.0f }, SDL_FRect{ .x = 0.0f, .y = 0.0f, .w = 320.0f, .h = 180.0f }, -1);

        std::ifstream file("data/levels/tiles.csv");
        if (!file) {
            SDL_Log("file stream failed to open: %s", strerror(errno));
            //return SDL_APP_FAILURE;
        }

        std::string line;

        float x{0};
        float y{0};

        float tile_size{ 8.0f };

        while (getline(file, line, ',')) {
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

            if (line == "1") {
                auto tile{ registry.create() };
                registry.emplace<clayborne::position>(tile, x * tile_size, y * tile_size);
                registry.emplace<clayborne::collider>(tile, tile_size, tile_size);
                if (y >= 22) {
                    registry.emplace<clayborne::clay>(tile);
                }
                //registry.emplace<clayborne::renderer>(tile, nullptr, SDL_FRect{}, SDL_FRect{ .x = 0.0f, .y = 0.0f, .w = tile_size, .h = tile_size });
            }

            if (line == "0" ||  line == "1") {
                x = x + 1;
                if (x >= 40) {
                    x = 0;
                    y = y + 1;
                }
            }
        }
    }
}