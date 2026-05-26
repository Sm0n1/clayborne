#include "SDL3/SDL_events.h"
#include "SDL3/SDL_gamepad.h"
#include "SDL3/SDL_init.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "audio.hpp"
#include "head.hpp"
#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <entt/entt.hpp>
#include <cstdio>
#include "camera.hpp"
#include "player.hpp"
#include "physics.hpp"
#include "interactables.hpp"
#include "level_loader.hpp"
#include "sprite.hpp"
#include "rendering.hpp"
#include "sdl.hpp"
#include "vfx.hpp"
#include "menu.hpp"

struct gamestate {
    SDL_Window *window{ nullptr };
    SDL_Renderer *renderer{ nullptr };
    SDL_Texture *canvas{ nullptr };
    SDL_Texture *vignette{ nullptr };
    MIX_Mixer *mixer{ nullptr };
    Uint64 current_time;
    Uint64 accumulated_time{ 0 };
    entt::registry registry;
    entt::entity player;
    entt::entity camera;
    bool is_fullscreen{ false };

    clayborne::texture_cache textures{};
    clayborne::animation_cache animations{};
    clayborne::audio_cache sounds{};

    SDL_Gamepad *gamepad{ nullptr };

    bool is_started{ false };
    Uint64 start_timer{ 0 };
    entt::entity start_music{ entt::null };
};

static SDL_Gamepad *find_gamepad() noexcept {
    int gamepads_len;
    auto gamepads_ptr{
        SDL_GetGamepads(&gamepads_len)
    };
    
    if (!gamepads_ptr) {
        SDL_Log("Could not get list of gamepads: %s", SDL_GetError());
        return nullptr;
    }

    for (int i{ 0 }; i < gamepads_len; i += 1) {
        auto gamepad{
            SDL_OpenGamepad(gamepads_ptr[i])
        };

        if (!gamepad) {
            SDL_Log("Could not load gamepad %d: %s", gamepads_ptr[i], SDL_GetError());
            continue;
        }

        SDL_Log("Loaded gamepad %d", gamepads_ptr[i]);
        SDL_free(gamepads_ptr);
        return gamepad;
    }

    SDL_Log("Failed to load any connected gamepad");
    SDL_free(gamepads_ptr);
    return nullptr;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
try {
    (void)argc;
    (void)argv;

    static gamestate gs;
    *appstate = &gs;

    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        SDL_Log("SDL init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Initialize window
    gs.window = SDL_CreateWindow("Clayborne", 1280, 720, SDL_WINDOW_RESIZABLE);
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
    SDL_SetRenderLogicalPresentation(gs.renderer, clayborne::canvas_width, clayborne::canvas_height, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    
    // Initialize canvas
    gs.canvas = SDL_CreateTexture(gs.renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, clayborne::canvas_width, clayborne::canvas_height);
    if (!gs.canvas) {
        SDL_Log("SDL create texture failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Scale the canvas with sharp edges
    SDL_SetTextureScaleMode(gs.canvas, SDL_SCALEMODE_NEAREST);

    // Initialize vignette
    gs.vignette = clayborne::init_vignette(gs.renderer);
    if (!gs.vignette) {
        return SDL_APP_FAILURE;
    }

    // Initialize SDL_mixer.
    if (!MIX_Init()) {
        SDL_Log("SDL_mixer init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Initialize audio mixer.
    gs.mixer = MIX_CreateMixerDevice(sdl_audio_device_default_playback, NULL);
    if (!gs.mixer) {
        SDL_Log("MIX create mixer with default device failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // TODO: remove
    if (!clayborne::load_debug_sounds(gs.sounds, gs.mixer)) {
        return SDL_APP_FAILURE;
    }

    gs.start_music = clayborne::play_sound(gs.registry, gs.sounds, gs.mixer, entt::hashed_string{ "menu_music" }, 0.7f, true);

    if (!clayborne::load_player_data(gs.textures, gs.renderer, gs.animations)) {
        return SDL_APP_FAILURE;
    }

    // Initialize camera
    gs.camera = clayborne::init_camera(gs.registry);

    // Initialize levels
    auto level_load_result{
        clayborne::load_levels(
            "data/levels",
            gs.registry,
            gs.textures,
            gs.renderer
        )
    };
    if (!level_load_result) {
        SDL_Log("%s", level_load_result.error().c_str());
        return SDL_APP_FAILURE;
    }

    // Initialize player
    gs.player = gs.registry.view<clayborne::player>().front();
    if (gs.player == entt::null) {
        SDL_Log("Level contains no player");
        return SDL_APP_FAILURE;
    }
    
    // Initialize timer
    gs.current_time = SDL_GetTicksNS();

    // Initialize gamepad if available
    gs.gamepad = find_gamepad();

    if (!clayborne::init_menu(gs.registry, gs.textures, gs.renderer)) {
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;

} catch (const std::exception& e) {
    SDL_Log("std::exception: %s", e.what());
    return SDL_APP_FAILURE;
} catch (const char* e) {
    SDL_Log("const char*: %s", e);
    return SDL_APP_FAILURE;
} catch (...) {
    SDL_Log("Unknown exception");
    return SDL_APP_FAILURE;
}
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    auto &gs{ *static_cast<gamestate*>(appstate) };
    auto &player{ gs.registry.get<clayborne::player>(gs.player) };

    switch (event->type) {
    case SDL_EVENT_QUIT: [[fallthrough]];
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_KEY_DOWN:
        if (event->key.repeat) {
            break;
        }
        switch (event->key.scancode) {
        case SDL_SCANCODE_F11:
            gs.is_fullscreen = !gs.is_fullscreen;
            SDL_SetWindowFullscreen(gs.window, gs.is_fullscreen);
            break;
        // ------------------------ //
        // Temporary Input Handling //
        // ------------------------ //
        case SDL_SCANCODE_J:
        case SDL_SCANCODE_Z:
        case SDL_SCANCODE_SPACE:
            player.jump_just_pressed = true;
            player.jump_pressed = true;
            break;
        case SDL_SCANCODE_K:
        case SDL_SCANCODE_X:
        case SDL_SCANCODE_LSHIFT:
            player.head_just_pressed = true;
            player.head_pressed = true;
            break;
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_UP:
            player.up = true;
            break;
        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT:
            player.left = true;
            break;
        case SDL_SCANCODE_S:
        case SDL_SCANCODE_DOWN:
            player.down = true;
            break;
        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT:
            player.right = true;
            break;
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
        case SDL_SCANCODE_J:
        case SDL_SCANCODE_Z:
        case SDL_SCANCODE_SPACE:
            player.jump_pressed = false;
            break;
        case SDL_SCANCODE_K:
        case SDL_SCANCODE_X:
        case SDL_SCANCODE_LSHIFT:
            player.head_pressed = false;
            break; 
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_UP:
            player.up = false;
            break;
        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT:
            player.left = false;
            break;
        case SDL_SCANCODE_S:
        case SDL_SCANCODE_DOWN:
            player.down = false;
            break;
        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT:
            player.right = false;
            break;
        default: break;
        }
        break;
        // ------------------------ //
    case SDL_EVENT_GAMEPAD_ADDED: {
        const auto gamepad{
            SDL_OpenGamepad(event->gdevice.which)
        };
        if (gamepad) {
            SDL_Log("Loaded gamepad %d", event->gdevice.which);
            SDL_CloseGamepad(gs.gamepad);
            gs.gamepad = gamepad;
        }
        else {
            SDL_Log("Could not load gamepad %d: %s", event->gdevice.which, SDL_GetError());
        }
        break;
    }
    case SDL_EVENT_GAMEPAD_REMOVED:
        if (event->gdevice.which == SDL_GetGamepadID(gs.gamepad)) {
            SDL_Log("Unloaded gamepad %d", SDL_GetGamepadID(gs.gamepad));
            SDL_CloseGamepad(gs.gamepad);
            gs.gamepad = find_gamepad();
        }
        break;
    // ------------------------ //
    // Temporary Input Handling //
    // ------------------------ //
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        switch (event->gbutton.button) {
        case SDL_GAMEPAD_BUTTON_NORTH:
        case SDL_GAMEPAD_BUTTON_SOUTH:
            player.jump_just_pressed = true;
            player.jump_pressed = true;
            break;
        case SDL_GAMEPAD_BUTTON_EAST:
        case SDL_GAMEPAD_BUTTON_WEST:
            player.head_just_pressed = true;
            player.head_pressed = true;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_UP:
            player.up = true;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
            player.left = true;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
            player.down = true;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
            player.right = true;
            break;
        }
        break;
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
        switch (event->gbutton.button) {
        case SDL_GAMEPAD_BUTTON_NORTH:
        case SDL_GAMEPAD_BUTTON_SOUTH:
            player.jump_pressed = false;
            break;
        case SDL_GAMEPAD_BUTTON_EAST:
        case SDL_GAMEPAD_BUTTON_WEST:
            player.head_pressed = false;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_UP:
            player.up = false;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
            player.left = false;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
            player.down = false;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
            player.right = false;
            break;
        }
        break;
    case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
        constexpr Sint16 dead_zone{ 8000 };
        if (event->gaxis.axis == SDL_GAMEPAD_AXIS_LEFTX) {
            player.left = event->gaxis.value < -dead_zone;
            player.right = event->gaxis.value > dead_zone;
        }
        else if (event->gaxis.axis == SDL_GAMEPAD_AXIS_LEFTY) {
            player.up = event->gaxis.value < -dead_zone;
            player.down = event->gaxis.value > dead_zone;
        }
        break;
    }
    // ------------------------ //
    default:
        // gs.inputs.process_event(*event);
        break;
    }

    switch (event->type) {
    case SDL_EVENT_KEY_DOWN:
        if (event->key.scancode == SDL_SCANCODE_F11) {
            break;
        }
        __attribute__ ((fallthrough));
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        if (!gs.is_started) {
            player.up = false;
            player.down = false;
            player.left = false;
            player.right = false;
            player.jump_pressed = false;
            player.jump_just_pressed = false;
            player.head_pressed = false;
            player.head_just_pressed = false;
            auto track{ gs.registry.get<clayborne::sound>(gs.start_music).track };
            MIX_StopTrack(track, MIX_TrackMSToFrames(track, 1000));
            clayborne::play_sound(gs.registry, gs.sounds, gs.mixer, entt::hashed_string{ "menu_start" }, 0.3f, false);
        }
        gs.is_started = true;
        break;
    default:
        break;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto &gs{ *static_cast<gamestate*>(appstate) };

    Uint64 frame_time = SDL_GetTicksNS() - gs.current_time;
    gs.current_time += frame_time;
    gs.accumulated_time += frame_time;

    constexpr auto dt_ns{ SDL_NS_PER_SECOND / 60 };

    while (gs.accumulated_time >= dt_ns) {
        if (gs.is_started && gs.start_timer < 2 * SDL_NS_PER_SECOND) {
            gs.start_timer += dt_ns;
        }

        static bool is_start_handled{ false };
        if (!is_start_handled && gs.is_started && gs.start_timer >= SDL_NS_PER_SECOND) {
            is_start_handled = true;
            clayborne::play_sound(gs.registry, gs.sounds, gs.mixer, entt::hashed_string{ "ambiance" }, 1.5f, true);
            clayborne::play_sound(gs.registry, gs.sounds, gs.mixer, entt::hashed_string{ "music" }, 0.7f, true);
        }

        clayborne::update_menu(gs.registry, dt_ns, gs.is_started, gs.start_timer);
        clayborne::update_player(gs.player, gs.registry, dt_ns, gs.sounds, gs.mixer, gs.is_started, gs.start_timer);
        clayborne::update_heads(gs.registry, dt_ns, gs.animations, gs.sounds, gs.mixer);
        clayborne::update_physics(gs.registry, dt_ns);
        clayborne::sense(gs.registry);
        clayborne::toggle_doors(gs.registry);
        clayborne::update_camera(gs.camera, gs.player, gs.registry);
        clayborne::update_audio(gs.registry, gs.camera);  
        clayborne::update_effects(gs.registry);
        clayborne::player_head_catch_sfx(gs.player, gs.registry, gs.sounds, gs.mixer);
        clayborne::animate_player(gs.player, gs.registry, gs.animations);
        clayborne::animate_sprites(gs.registry, gs.animations);
        gs.accumulated_time -= dt_ns;
    }

    // TODO: banish to the shadowrealm
    gs.registry.sort<clayborne::sprite_renderer>(
        [](
            const clayborne::sprite_renderer &lhs,
            const clayborne::sprite_renderer &rhs
        ) {
            return lhs.z < rhs.z;
        }
    );

    clayborne::render(gs.camera, gs.registry, gs.textures, gs.renderer, gs.canvas, gs.vignette, gs.is_started, gs.start_timer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    auto &gs{ *static_cast<gamestate*>(appstate) };

    SDL_CloseGamepad(gs.gamepad);
    SDL_DestroyTexture(gs.vignette);
    SDL_DestroyTexture(gs.canvas);
    SDL_DestroyRenderer(gs.renderer);
    SDL_DestroyWindow(gs.window);

    switch (result) {
    case SDL_APP_SUCCESS: SDL_Log("App Success"); break;
    case SDL_APP_FAILURE: SDL_Log("App Failure"); break;
    default:              std::unreachable();
    }
}