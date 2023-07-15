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
}

int main() {
    render();
    renderBuffer();
    return 0;
}
