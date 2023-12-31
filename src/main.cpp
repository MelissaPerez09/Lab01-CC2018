#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "color.h"
#include "framebuffer.h"
#include "point.h"


void writeInt(std::ofstream& file, int value) {
    file.put(value & 0xFF);
    file.put((value >> 8) & 0xFF);
    file.put((value >> 16) & 0xFF);
    file.put((value >> 24) & 0xFF);
}

void renderBuffer() {
    std::ofstream file("out.bmp", std::ios::binary);
    unsigned char fileHeader[] = {
        'B', 'M',
        0, 0, 0, 0,
        0, 0,
        0, 0,
        54, 0, 0, 0
    };

    unsigned char infoHeader[] = {
        40, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        1, 0,
        24, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };

    size_t imageSize = width * height * 3;
    int fileSize = imageSize + sizeof(fileHeader) + sizeof(infoHeader);

    *(int*)&fileHeader[2] = fileSize;
    *(int*)&fileHeader[10] = sizeof(fileHeader) + sizeof(infoHeader);
    *(int*)&infoHeader[4] = width;
    *(int*)&infoHeader[8] = height;
    *(int*)&infoHeader[20] = imageSize;

    file.write(reinterpret_cast<char*>(fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char*>(infoHeader), sizeof(infoHeader));

    for (const auto& color : framebuffer) {
        file.put(color.b);
        file.put(color.g);
        file.put(color.r);
    }

    file.close();
}

void point(int x, int y, const Color& color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        int pixelIndex = y * width + x;
        framebuffer[pixelIndex] = color;
    }
}

void drawLine(const Vertex& start, const Vertex& end, const Color& color) {
    int x0 = static_cast<int>(std::round(start.x));
    int y0 = static_cast<int>(std::round(start.y));
    int x1 = static_cast<int>(std::round(end.x));
    int y1 = static_cast<int>(std::round(end.y));

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        point(x0, y0, color);

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void drawPolygon(const std::vector<Vertex>& points, const Color& color) {
    if (points.size() < 2) {
        return;
    }

    for (size_t i = 0; i < points.size() - 1; ++i) {
        drawLine(points[i], points[i + 1], color);
    }

    drawLine(points.back(), points.front(), color);
}

void fillPolygon(const std::vector<Vertex>& vertices, const Color& color) {
    if (vertices.size() < 3) {
        return;
    }

    float minX = vertices[0].x;
    float maxX = vertices[0].x;
    float minY = vertices[0].y;
    float maxY = vertices[0].y;

    for (size_t i = 1; i < vertices.size(); ++i) {
        minX = std::min(minX, vertices[i].x);
        maxX = std::max(maxX, vertices[i].x);
        minY = std::min(minY, vertices[i].y);
        maxY = std::max(maxY, vertices[i].y);
    }

    for (float y = minY; y <= maxY; ++y) {
        std::vector<float> intersections;

        for (size_t i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
            if (((vertices[i].y <= y) && (y < vertices[j].y)) || ((vertices[j].y <= y) && (y < vertices[i].y))) {
                if (vertices[i].y != vertices[j].y) {
                    float x = ((vertices[j].x - vertices[i].x) * (y - vertices[i].y)) / (vertices[j].y - vertices[i].y) + vertices[i].x;
                    intersections.push_back(x);
                }
            }
        }

        std::sort(intersections.begin(), intersections.end());

        for (size_t i = 0; i < intersections.size(); i += 2) {
            int startX = std::max(0, static_cast<int>(std::ceil(intersections[i])));
            int endX = static_cast<int>(std::min(static_cast<float>(width - 1), std::floor(intersections[i + 1])));

            for (int x = startX; x <= endX; ++x) {
                point(x, static_cast<int>(y), color);
            }
        }
    }
}

void render() {

  // first polygon
    std::vector<Vertex> polygon1 = {
        Vertex(165.0f, 380.0f),
        Vertex(185.0f, 360.0f),
        Vertex(180.0f, 330.0f),
        Vertex(207.0f, 345.0f),
        Vertex(233.0f, 330.0f),
        Vertex(230.0f, 360.0f),
        Vertex(250.0f, 380.0f),
        Vertex(220.0f, 385.0f),
        Vertex(205.0f, 410.0f),
        Vertex(193.0f, 383.0f)
    };
    Color pColor(255, 255, 255);
    Color pFill(255, 255, 0);
    fillPolygon(polygon1, pFill);
    drawPolygon(polygon1, pColor);
  
    // second polygon
    std::vector<Vertex> polygon2 = {
        Vertex(321.0f, 335.0f),
        Vertex(288.0f, 286.0f),
        Vertex(339.0f, 251.0f),
        Vertex(374.0f, 302.0f)
    };
    Color pColor2(255, 255, 255);
    Color pFill2(0,0,255);
    fillPolygon(polygon2, pFill2);
    drawPolygon(polygon2, pColor2);
  
  // third polygon
  std::vector<Vertex> polygon3 = {
    Vertex(377.0f, 249.0f),
    Vertex(411.0f, 197.0f),
    Vertex(436.0f, 249.0f)
  };
  Color pColor3(255, 255, 255);
  Color pFill3(255,0,0);
  fillPolygon(polygon3, pFill3);
  drawPolygon(polygon3, pColor3);
  
  // fourth polygon
  std::vector<Vertex> polygon4 = {
      Vertex(413.0f, 177.0f),
      Vertex(448.0f, 159.0f),
      Vertex(502.0f, 88.0f),
      Vertex(553.0f, 53.0f),
      Vertex(535.0f, 36.0f),
      Vertex(676.0f, 37.0f),
      Vertex(660.0f, 52.0f),
      Vertex(750.0f, 145.0f),
      Vertex(761.0f, 179.0f),
      Vertex(672.0f, 192.0f),
      Vertex(659.0f, 214.0f),
      Vertex(615.0f, 214.0f),
      Vertex(632.0f, 230.0f),
      Vertex(580.0f, 230.0f),
      Vertex(597.0f, 215.0f),
      Vertex(552.0f, 214.0f),
      Vertex(517.0f, 144.0f),
      Vertex(466.0f, 180.0f)
  };
  Color pColor4(255, 255, 255);
  Color pFill4(0,255,0);
  fillPolygon(polygon4, pFill4);
  drawPolygon(polygon4, pColor4);

  // fifth polygon
  std::vector<Vertex> polygon5 = {
      Vertex(682.0f, 175.0f),
      Vertex(708.0f, 120.0f),
      Vertex(735.0f, 148.0f),
      Vertex(739.0f, 170.0f)
  };
  Color pColor5(255, 255, 255);
  Color pFill5(0,0,0);
  fillPolygon(polygon5, pFill5);
  drawPolygon(polygon5, pColor5);
}

int main() {
    render();
    renderBuffer();
    return 0;
}
