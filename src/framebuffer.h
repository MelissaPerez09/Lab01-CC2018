#include <vector>
#include "color.h"

constexpr size_t width = 800;
constexpr size_t height = 600;

std::vector<Color> framebuffer(width * height);
Color clearColor{0, 0, 0};
Color currentColor{255, 255, 255};

void clear() {
  std::fill(framebuffer.begin(), framebuffer.end(), clearColor);
}
