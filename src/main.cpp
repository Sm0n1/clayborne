#include "SDL3/SDL_events.h"
#include <SDL3/SDL_scancode.h>
#include <cstdio>
#define SDL_MAIN_USE_CALLBACKS

#include <utility>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <entt/entt.hpp>
// #include <print>
#include "engine/input/manager.hpp"
#include "camera.hpp"
#include "player.hpp"
#include "physics.hpp"
#include "engine/time.hpp"

struct gamestate {
    SDL_Window *window{ nullptr };
    SDL_Renderer *renderer{ nullptr };
    SDL_Texture *canvas{ nullptr };
    Uint64 current_time;
    Uint64 accumulated_time{ 0 };
    entt::registry registry;
    clayborne::player player;
    clayborne::camera camera;
    bool is_fullscreen{ false };

    clayborne::input::manager inputs;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    (void)argc;
    (void)argv;

    static gamestate gs;
    *appstate = &gs;

    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
        SDL_Log("SDL init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Initialize window
    gs.window = SDL_CreateWindow("Clayborne", 640, 360, SDL_WINDOW_RESIZABLE);
    if (!gs.window) {
        SDL_Log("SDL create window failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Initialize renderer
    gs.renderer = SDL_CreateRenderer(gs.window, nullptr);
    if (!gs.renderer) {
        SDL_Log("SDL create renderer failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Enable automatic scaling
    SDL_SetRenderLogicalPresentation(gs.renderer, 320, 180, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    // Initialize canvas
    gs.canvas = SDL_CreateTexture(gs.renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 320, 180);
    if (!gs.canvas) {
        SDL_Log("SDL create texture failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Initialize camera
    gs.camera = clayborne::init_camera(gs.registry);

    // Initialize player
    gs.player = clayborne::init_player(gs.registry);

    // Initialize timer
    gs.current_time = SDL_GetTicksNS();

    // Initialize input manager
    gs.inputs = {};

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    auto &gs{ *static_cast<gamestate*>(appstate) };

    switch (event->type) {
    case SDL_EVENT_QUIT: [[fallthrough]];
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_KEY_DOWN:
        switch (event->key.scancode) {
        case SDL_SCANCODE_F11:
            gs.is_fullscreen = !gs.is_fullscreen;
            SDL_SetWindowFullscreen(gs.window, gs.is_fullscreen);
            break;
        // ------------------------ //
        // Temporary Input Handling //
        // ------------------------ //
        case SDL_SCANCODE_J: gs.player.jump = clayborne::player::button::just_pressed; break;
        case SDL_SCANCODE_K: gs.player.head = clayborne::player::button::just_pressed; break;
        case SDL_SCANCODE_W: gs.player.up = true; break;
        case SDL_SCANCODE_A: gs.player.left = true; break;
        case SDL_SCANCODE_S: gs.player.down = true; break;
        case SDL_SCANCODE_D: gs.player.right = true; break;
        // ------------------------ //
        default:
            break;
        }
        break;
    case SDL_EVENT_KEY_UP:
        // ------------------------ //
        // Temporary Input Handling //
        // ------------------------ //
        switch (event->key.scancode) {
        case SDL_SCANCODE_J: gs.player.jump = clayborne::player::button::just_released; break;
        case SDL_SCANCODE_K: gs.player.head = clayborne::player::button::just_released; break;
        case SDL_SCANCODE_W: gs.player.up = false; break;
        case SDL_SCANCODE_A: gs.player.left = false; break;
        case SDL_SCANCODE_S: gs.player.down = false; break;
        case SDL_SCANCODE_D: gs.player.right = false; break;
        default: break;
        }
        break;
        // ------------------------ //
    // case SDL_EVENT_GAMEPAD_ADDED:
    // case SDL_EVENT_GAMEPAD_REMOVED:
    default:
        // gs.inputs.process_event(*event);
        break;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto &gs{ *static_cast<gamestate*>(appstate) };

    Uint64 frame_time = SDL_GetTicksNS() - gs.current_time;
    gs.current_time += frame_time;
    gs.accumulated_time += frame_time;

    while (gs.accumulated_time >= SDL_NS_PER_SECOND / 60) {
        clayborne::update_player(gs.player, gs.registry, gs.inputs, SDL_NS_PER_SECOND / 60);
        clayborne::update_physics(gs.registry, SDL_NS_PER_SECOND / 60);
        gs.accumulated_time -= SDL_NS_PER_SECOND / 60;
    }

    clayborne::render(gs.camera, gs.registry, gs.renderer, gs.canvas);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    auto &gs{ *static_cast<gamestate*>(appstate) };

    clayborne::deinit_player(gs.player, gs.registry);
    clayborne::deinit_camera(gs.camera, gs.registry);

    SDL_DestroyTexture(gs.canvas);
    SDL_DestroyRenderer(gs.renderer);
    SDL_DestroyWindow(gs.window);

    switch (result) {
    // case SDL_APP_SUCCESS: std::println("App Success"); break;
    // case SDL_APP_FAILURE: std::println("App Failure"); break;
    // case SDL_APP_CONTINUE: std::unreachable();
    default:
        break;
    }
}