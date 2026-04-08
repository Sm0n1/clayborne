#ifndef CLAYBORNE_PHYSICS_HPP
#define CLAYBORNE_PHYSICS_HPP

#include <SDL3/SDL.h>
#include <cmath>
#include <entt/entt.hpp>
#include <optional>

namespace clayborne {
    struct vector {
        float x{ 0.0f };
        float y{ 0.0f };

        constexpr vector() noexcept = default;
        constexpr vector(float x_, float y_) noexcept : x(x_), y(y_) {}

        constexpr vector operator+(const vector& other) const noexcept {
            return {x + other.x, y + other.y};
        }

        constexpr vector operator-(const vector& other) const noexcept {
            return {x - other.x, y - other.y};
        }

        constexpr vector operator*(float scalar) const noexcept {
            return {x * scalar, y * scalar};
        }

        constexpr vector operator/(float scalar) const noexcept {
            return {x / scalar, y / scalar};
        }

        vector& operator+=(const vector& other) noexcept {
            x += other.x;
            y += other.y;
            return static_cast<vector&>(*this);
        }

        vector& operator-=(const vector& other) noexcept  {
            x -= other.x;
            y -= other.y;
            return static_cast<vector&>(*this);
        }

        vector& operator*=(float scalar) noexcept {
            x *= scalar;
            y *= scalar;
            return static_cast<vector&>(*this);
        }

        vector& operator/=(float scalar) noexcept {
            x /= scalar;
            y /= scalar;
            return static_cast<vector&>(*this);
        }

        [[nodiscard]] float length() const noexcept {
            return std::sqrt(x * x + y * y);
        }

        [[nodiscard]] float length_squared() const noexcept {
            return x * x + y * y;
        }

        [[nodiscard]] vector normalized() const noexcept {
            float len = length();
            return vector{ x / len, y / len };
        }

        [[nodiscard]] float dot(const vector &other) const noexcept {
            return x * other.x + y * other.y;
        }

        [[nodiscard]] float cross(const vector &other) const noexcept {
            return x * other.y - y * other.x;
        }
    };

    struct position {
        float x;
        float y;
    };

    struct velocity {
        float x;
        float y;
        float x_subpos;
        float y_subpos;
    };

    struct collider {
        float w;
        float h;

        // TODO: make an actual collision handler
        std::optional<std::function<void (entt::entity entity, bool x, bool y)>> handler{ std::nullopt };
    };

    void update_physics(entt::registry &registry, Uint64 dt_ns);

    [[nodiscard]] constexpr bool overlap(const position &p1, const collider &c1, const position &p2, const collider &c2) noexcept {
        return (p1.x + c1.w > p2.x) & (p1.y + c1.h > p2.y) & (p2.x + c2.w > p1.x) & (p2.y + c2.h > p1.y ); // eager evaluation
    }
}

#endif // CLAYBORNE_PHYSICS_HPP