#ifndef CLAYBORNE_INPUT_HPP
#define CLAYBORNE_INPUT_HPP


#include <cstdint>
#include <unordered_map>
#include <SDL3/SDL.h>

namespace clayborne::input {
    enum class menu_input : std::uint8_t  {
        up,
        down,
        left,
        right,
        confirm,
        cancel,
    };

    enum class gameplay_input : std::uint8_t {
        up,
        down,
        left,
        right,
        jump,
        head,
        // pause,
    };

    enum class joystick : std::uint8_t  {
        neutral    = 0b0000,
        up         = 0b0001,
        down       = 0b0010,
        left       = 0b0100,
        right      = 0b1000,
        up_left    = 0b0101,
        up_right   = 0b1001,
        down_left  = 0b0110,
        down_right = 0b1010,
    };

    enum class button : std::uint8_t  {
        up       = 0b00,
        down     = 0b01,
        pressed  = 0b11,
        released = 0b10,
    };

    struct input_manager {
        std::unordered_map<SDL_Scancode, gameplay_input> keyboard_inputs;

        bool is_joystick_up{ false };
        bool is_joystick_down{ false };
        bool is_joystick_left{ false };
        bool is_joystick_right{ false };
        bool is_button_a_down{ false };
        bool is_button_a_pressed{ false };
        bool is_button_a_released{ false };
        bool is_button_b_down{ false };
        bool is_button_b_pressed{ false };
        bool is_button_b_released{ false };

        // Walking, aiming, and navigating menus.
        joystick joystick{ joystick::neutral };
        // Jumping, skipping dialogue, and selecting menu options.
        button button_a{ button::up };
        // Throwing/detonating the head, and going back in the menu.
        button button_b{ button::up };
    };

    input_manager init();
    void deinit(input_manager &input_manager);
    void update(input_manager &input_manager);
    void handle_keyboard_event(input_manager &input_manager, SDL_KeyboardEvent &event);
}

// There are three different possible states the game can be in:
// - There is only the menu being visualized and interacted with
// - There is only the gameplay being visualized and interacted with
// - Both the gameplay and menu state is stored, but the game is paused and the menu is interacted with
// 
// In terms of input, either the menu or gameplay is active.
// In terms of the simulation, every gameplay related system should be paused when the menu is open.
// Using a shared registry, this would require a menu or gameplay component and for every system to only
// simuluate the relevant entities.
// Using two registries would allow 

#endif // CLAYBORNE_INPUT_HPP