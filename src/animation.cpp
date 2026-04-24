#include "animation.hpp"
#include "camera.hpp"

namespace clayborne {
    void animate(entt::registry &registry) {
        auto view{ registry.view<renderer, animator>() };
        for (auto [e, r, a] : view.each()) {
            if(!a.resource) {
                continue;
            }

            //SDL_Log("Total frames: %d", static_cast<int>(a.resource->frames.size()));
            //SDL_Log("Current frame: %d", static_cast<int>(a.current_frame));

            const auto &animation{ a.resource->frames };
            a.current_frame++;
            if (a.current_frame >= animation.size() && a.is_looping) {
                a.current_frame = 0;
            }
            // If a isn't a looping animation, its entity should have already been destroyed by now?

            const auto frame{ animation[a.current_frame] };

            r.srcrect.x = frame.x;
            r.srcrect.y = frame.y;
            r.srcrect.w = frame.w;
            r.srcrect.h = frame.h;
            
            if (a.current_frame++ >= animation.size()) {
                a.current_frame = !a.is_looping * animation.size();
            }
        }
    }
}