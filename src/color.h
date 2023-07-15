#pragma once
#include <iostream>
#include <algorithm>

class Color {
public:
    uint8_t r;
    uint8_t g;
    uint8_t b;

    Color() : r(0), g(0), b(0) {}
    Color(uint8_t red, uint8_t green, uint8_t blue) : r(clamp(red)), g(clamp(green)), b(clamp(blue)) {}

    Color operator+(const Color& other) const {
        return Color(
            static_cast<uint8_t>(clamp(static_cast<int>(r) + other.r)),
            static_cast<uint8_t>(clamp(static_cast<int>(g) + other.g)),
            static_cast<uint8_t>(clamp(static_cast<int>(b) + other.b))
        );
    }

    Color operator*(float scalar) const {
        return Color(
            static_cast<uint8_t>(clamp(static_cast<int>(r * scalar))),
            static_cast<uint8_t>(clamp(static_cast<int>(g * scalar))),
            static_cast<uint8_t>(clamp(static_cast<int>(b * scalar)))
        );
    }

    friend std::ostream& operator<<(std::ostream& os, const Color& color) {
        os << "RGB: (" << static_cast<int>(color.r) << ", " << static_cast<int>(color.g) << ", " << static_cast<int>(color.b) << ")";
        return os;
    }

private:
    static uint8_t clamp(int value) {
        return static_cast<uint8_t>(std::clamp(value, 0, 255));
    }
};
