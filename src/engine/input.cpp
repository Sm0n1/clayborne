#include <SDL3/SDL.h>
#include "input.hpp"
#include "SDL3/SDL_events.h"

namespace clayborne::input {
    input_manager init() {
        input_manager input_manager;

        // TODO: Load from json instead
        input_manager.keyboard_inputs.emplace(SDL_SCANCODE_W, gameplay_input::up);
        input_manager.keyboard_inputs.emplace(SDL_SCANCODE_A, gameplay_input::left);
        input_manager.keyboard_inputs.emplace(SDL_SCANCODE_S, gameplay_input::down);
        input_manager.keyboard_inputs.emplace(SDL_SCANCODE_D, gameplay_input::right);
        input_manager.keyboard_inputs.emplace(SDL_SCANCODE_J, gameplay_input::jump);
        input_manager.keyboard_inputs.emplace(SDL_SCANCODE_K, gameplay_input::head);

        return input_manager;
    }

    void deinit(input_manager &input_manager) {
        (void)input_manager;
    }

    void update(input_manager &input_manager) {
        auto joystick_mask = static_cast<std::uint8_t>(
            (input_manager.is_joystick_up    << 0) |
            (input_manager.is_joystick_down  << 1) |
            (input_manager.is_joystick_left  << 2) |
            (input_manager.is_joystick_right << 3)
        );
        
        // up + down == none
        if (input_manager.is_joystick_up && input_manager.is_joystick_down) {
            joystick_mask &= 0b1100;
        }

        // left + right == none
        if (input_manager.is_joystick_left && input_manager.is_joystick_right) {
            joystick_mask &= 0b0011;
        }

        // Update joystick
        input_manager.joystick = static_cast<joystick>(joystick_mask);

        // Update button_a
        if (input_manager.is_button_a_pressed) {
            input_manager.button_a = button::pressed;
        } else if (input_manager.is_button_a_released) {
            input_manager.button_a = button::released;
        } else if (input_manager.is_button_a_down) {
            input_manager.button_a = button::down;
        } else {
            input_manager.button_a = button::up;
        }

        // Update button_b
        if (input_manager.is_button_b_pressed) {
            input_manager.button_b = button::pressed;
        } else if (input_manager.is_button_b_released) {
            input_manager.button_b = button::released;
        } else if (input_manager.is_button_b_down) {
            input_manager.button_b = button::down;
        } else {
            input_manager.button_b = button::up;
        }

        // Reset flags lasting a single frame
        input_manager.is_button_a_pressed = false;
        input_manager.is_button_a_released = false;
        input_manager.is_button_b_pressed = false;
        input_manager.is_button_b_released = false;
    }

    void handle_keyboard_event(input_manager &input_manager, SDL_KeyboardEvent &event) {
        if (event.repeat) {
            return;
        }

        auto it{ input_manager.keyboard_inputs.find(event.scancode) };

        if (it == input_manager.keyboard_inputs.end()) {
            return;
        }

        switch (it->second) {
        case gameplay_input::up:
            input_manager.is_joystick_up = event.down;
            break;
        case gameplay_input::down:
            input_manager.is_joystick_down = event.down;
            break;
        case gameplay_input::left:
            input_manager.is_joystick_left = event.down;
            break;
        case gameplay_input::right:
            input_manager.is_joystick_right = event.down;
            break;
        case gameplay_input::jump:
            if (event.down) {
                input_manager.is_button_a_pressed = true;
                input_manager.is_button_a_down = true;
            } else {
                input_manager.is_button_a_released = true;
                input_manager.is_button_a_down = false;
            }
            break;
        case gameplay_input::head:
            if (event.down) {
                input_manager.is_button_b_pressed = true;
                input_manager.is_button_b_down = true;
            } else {
                input_manager.is_button_b_released = true;
                input_manager.is_button_b_down = false;
            }
            break;
        }
    }
}