#pragma once
#include <iostream>
#include "color.h"

struct Vertex {
  float x;
  float y;
  Color color;

  Vertex(float xCoord, float yCoord, const Color& c) : x(xCoord), y(yCoord), color(c) {}
};